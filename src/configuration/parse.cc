#include <sourcemeta/one/configuration.h>

#include <sourcemeta/blaze/evaluator.h>
#include <sourcemeta/blaze/output.h>
#include <sourcemeta/core/jose.h>
#include <sourcemeta/core/text.h>
#include <sourcemeta/core/uri.h>

#include "template.h"

#include <algorithm>   // std::ranges::sort, std::ranges::unique
#include <cassert>     // assert
#include <set>         // std::set
#include <string_view> // std::string_view
#include <utility>     // std::move
#include <vector>      // std::vector

namespace {

// A session cookie is only marked secure on an https instance, so an
// interactive login anywhere else hands the browser a credential that travels
// in the clear. A loopback address and the special-use localhost name are the
// exception, since a browser already treats either as a trustworthy origin and
// honours the attribute there
auto serves_securely(const std::string_view url) -> bool {
  const sourcemeta::core::URI parsed{std::string{url}};
  // A scheme on its own names no origin, so there is nowhere for a browser to
  // hold a cookie against in the first place
  if (!parsed.host().has_value() || parsed.host().value().empty()) {
    return false;
  }

  return parsed.is_https() ||
         (parsed.is_http() && (parsed.is_loopback() || parsed.is_localhost()));
}

// A rule naming a claim and the values it may carry is exactly an individual
// claim request (OpenID Connect Core 1.0 Section 5.5.1), so it is stored as
// one rather than in a shape of our own. Both the names and their values are
// sorted and deduplicated here, so that two policies admitting the same
// callers serialise identically, which is what decides whether one may
// reference the other
auto claims_from_json(const sourcemeta::core::JSON &input)
    -> sourcemeta::core::JSON {
  std::vector<sourcemeta::core::JSON::String> names;
  names.reserve(input.size());
  for (const auto &claim : input.as_object()) {
    names.push_back(claim.first);
  }

  std::ranges::sort(names);
  auto result{sourcemeta::core::JSON::make_object()};
  for (const auto &name : names) {
    std::vector<sourcemeta::core::JSON::String> values;
    values.reserve(input.at(name).size());
    for (const auto &value : input.at(name).as_array()) {
      values.push_back(value.to_string());
    }

    std::ranges::sort(values);
    auto accepted{sourcemeta::core::JSON::make_array()};
    for (auto &&value : values) {
      accepted.push_back(sourcemeta::core::JSON{std::move(value)});
    }

    auto request{sourcemeta::core::JSON::make_object()};
    request.assign_assume_new("essential", sourcemeta::core::JSON{true});
    request.assign_assume_new("values", std::move(accepted));
    result.assign_assume_new(name, std::move(request));
  }

  return result;
}

auto page_from_json(const sourcemeta::core::JSON &input)
    -> sourcemeta::one::Configuration::Page {
  sourcemeta::one::Configuration::Page result;
  using namespace sourcemeta::core;
  const JSON null{nullptr};
  result.title =
      from_json<decltype(result.title)::value_type>(input.at_or("title", null));
  result.description = from_json<decltype(result.description)::value_type>(
      input.at_or("description", null));
  result.email =
      from_json<decltype(result.email)::value_type>(input.at_or("email", null));
  result.github = from_json<decltype(result.github)::value_type>(
      input.at_or("github", null));
  result.website = from_json<decltype(result.website)::value_type>(
      input.at_or("website", null));
  return result;
}

template <typename T>
auto entries_from_json(T &result, const std::filesystem::path &location,
                       const sourcemeta::core::JSON &input,
                       const std::filesystem::path &default_base_path) -> void {
  // A heuristic to check if we are at the root or not
  if (input.defines("url")) {
    if (input.defines("contents")) {
      for (const auto &entry : input.at("contents").as_object()) {
        entries_from_json<T>(result, location / entry.first, entry.second,
                             default_base_path);
      }
    }
  } else {
    assert(!result.contains(location));
    if (input.defines("path")) {
      const auto base_path{
          input.defines("x-sourcemeta-one:path")
              ? std::filesystem::path{input.at("x-sourcemeta-one:path")
                                          .to_string()}
                    .parent_path()
              : default_base_path};
      auto collection{
          sourcemeta::blaze::Configuration::from_json(input, base_path)};
      // Filesystems behave differently with regards to casing. To unify
      // them, assume they are case-insensitive and just go for lowercase
      sourcemeta::core::to_lowercase(collection.base);
      // This URI is guaranteed to be canonicalised by the collection parser
      assert(collection.base ==
             sourcemeta::core::URI::canonicalize(collection.base));
      result.emplace(location, std::move(collection));
    } else {
      result.emplace(location, page_from_json(input));
      // Only pages may have children
      if (input.defines("contents")) {
        for (const auto &entry : input.at("contents").as_object()) {
          entries_from_json<T>(result, location / entry.first, entry.second,
                               default_base_path);
        }
      }
    }
  }
}

} // namespace

namespace sourcemeta::one {

auto Configuration::parse(const sourcemeta::core::JSON &data,
                          const std::filesystem::path &configuration_path,
                          const std::filesystem::path &default_base_path)
    -> Configuration {
  const auto compiled_schema{sourcemeta::blaze::from_json(
      sourcemeta::core::parse_json(CONFIGURATION))};
  assert(compiled_schema.has_value());
  sourcemeta::blaze::Evaluator evaluator;
  sourcemeta::blaze::SimpleOutput output{data};
  if (!evaluator.validate(compiled_schema.value(), data, std::ref(output))) {
    throw ConfigurationValidationError(configuration_path, output);
  }

  Configuration result;
  assert(configuration_path.is_absolute());
  result.path = configuration_path;
  sourcemeta::core::URI server_url{data.at("url").to_string()};
  server_url.canonicalize();
  result.url = server_url.recompose();

  // A well-known URI is rooted at the top of a path hierarchy and is not
  // well-known anywhere else (RFC 8615 Section 3), so an instance below the
  // origin cannot serve the locations standards derive from its own
  // identifiers. One spelling is admitted rather than several normalised into
  // one, since there is then nothing to normalise and nothing left untested
  const auto server_path{server_url.path()};
  if (server_path.has_value() && !server_path.value().empty()) {
    throw ConfigurationInstancePathError(configuration_path,
                                         data.at("url").to_string());
  }

  // Every schema identifier is composed against this URL, so anything here
  // that does not say where the instance is served travels into identifiers
  // that name no resource, credentials included
  if (server_url.userinfo().has_value() || server_url.query().has_value() ||
      server_url.fragment().has_value()) {
    throw ConfigurationInstanceOriginError(configuration_path,
                                           data.at("url").to_string());
  }

  server_url.path("");
  server_url.userinfo("");
  server_url.query("");
  server_url.fragment("");
  server_url.canonicalize();
  result.origin = server_url.recompose();

  if (data.defines("html")) {
    if (data.at("html").is_boolean() && !data.at("html").to_boolean()) {
      result.html = std::nullopt;
    } else {
      result.html = Configuration::HTML{};
      result.html->name = data.at("html").at("name").to_string();
      result.html->description = data.at("html").at("description").to_string();
      if (data.at("html").defines("hero")) {
        result.html->hero = data.at("html").at("hero").to_string();
      }

      if (data.at("html").defines("head")) {
        result.html->head = data.at("html").at("head").to_string();
      }

      if (data.at("html").defines("action")) {
        result.html->action = {
            .url = data.at("html").at("action").at("url").to_string(),
            .icon = data.at("html").at("action").at("icon").to_string(),
            .title = data.at("html").at("action").at("title").to_string()};
      }
    }
  }

  if (data.defines("api")) {
    if (data.at("api").is_boolean() && !data.at("api").to_boolean()) {
      result.api = false;
    }
  }

  if (data.defines("authentication")) {
    for (const auto &entry : data.at("authentication").as_array()) {
      Configuration::AuthenticationEntry parsed;
      parsed.name = entry.at("name").to_string();
      parsed.title = parsed.name;
      for (const auto &path : entry.at("paths").as_array()) {
        parsed.paths.push_back(path.to_string());
      }

      if (entry.at("type").to_string() == "jwt") {
        parsed.type = Configuration::AuthenticationEntry::Type::JWT;
        parsed.issuer = entry.at("issuer").to_string();
        parsed.audience = entry.at("audience").to_string();
        if (entry.defines("jwksUri")) {
          parsed.jwks_uri = entry.at("jwksUri").to_string();
        }

        if (entry.defines("tokenType")) {
          parsed.token_type = entry.at("tokenType").to_string();
        }

        for (const auto &algorithm : entry.at("algorithms").as_array()) {
          parsed.algorithms.push_back(
              sourcemeta::core::to_jws_algorithm(algorithm.to_string())
                  .value());
        }

        const auto *claims{entry.try_at("claims")};
        if (claims != nullptr) {
          parsed.claims = claims_from_json(*claims);
        }
      } else if (entry.at("type").to_string() == "oidc") {
        parsed.type = Configuration::AuthenticationEntry::Type::OIDC;
        parsed.issuer = entry.at("issuer").to_string();
        parsed.client_id = entry.at("clientId").to_string();
        parsed.client_secret_variable =
            entry.at("clientSecret").at("environmentVariable").to_string();
        for (const auto &secret : entry.at("sessionSecrets").as_array()) {
          parsed.session_secret_variables.push_back(
              secret.at("environmentVariable").to_string());
        }
        const auto *title{entry.try_at("title")};
        if (title != nullptr) {
          parsed.title = title->to_string();
        }

        const auto *claims{entry.try_at("claims")};
        if (claims != nullptr) {
          parsed.claims = claims_from_json(*claims);
        }

        const auto *domains{entry.try_at("emailDomains")};
        if (domains != nullptr) {
          for (const auto &domain : domains->as_array()) {
            parsed.email_domains.push_back(domain.to_string());
            sourcemeta::core::to_lowercase(parsed.email_domains.back());
          }

          std::ranges::sort(parsed.email_domains);
        }
      } else {
        parsed.type = Configuration::AuthenticationEntry::Type::ApiKey;
        parsed.algorithm =
            entry.at("algorithm").to_string() == "sha256"
                ? Configuration::AuthenticationEntry::Algorithm::Sha256
                : Configuration::AuthenticationEntry::Algorithm::Identity;
        for (const auto &key : entry.at("keys").as_array()) {
          parsed.keys.push_back(key.at("environmentVariable").to_string());
        }
      }

      result.authentication.push_back(std::move(parsed));
    }
  }

  // An issuer this instance fetches a discovery document from must be one it
  // could complete that exchange against. A policy naming its key set directly
  // never makes the exchange, so its issuer is only an identifier compared
  // against a token's claim, and nothing here applies to it
  for (auto &entry : result.authentication) {
    const auto discovers{
        entry.type == Configuration::AuthenticationEntry::Type::OIDC ||
        (entry.type == Configuration::AuthenticationEntry::Type::JWT &&
         !entry.jwks_uri.has_value())};
    if (!discovers) {
      continue;
    }

    // OpenID Connect Discovery 1.0 Section 4.1 removes a trailing slash before
    // appending the well-known suffix, and Section 4.3 requires the document to
    // declare the prefix that was actually used. So the slash is not part of
    // the identifier and is dropped here, once, rather than left to disagree
    // with itself between building the URL and comparing what comes back
    if (entry.issuer.ends_with("/")) {
      entry.issuer.pop_back();
    }

    // OpenID Connect Discovery 1.0 Section 3: the issuer is an https URL
    if (!sourcemeta::core::URI{entry.issuer}.is_https()) {
      throw ConfigurationInvalidAuthenticationIssuerError(
          configuration_path, entry.name, entry.issuer);
    }

    if (entry.type == Configuration::AuthenticationEntry::Type::OIDC &&
        !serves_securely(result.url)) {
      throw ConfigurationInsecureAuthenticationURLError(configuration_path,
                                                        entry.name, result.url);
    }
  }

  // Policy names are unique, and "public" is reserved because an empty policy
  // array on a listing already means public
  std::set<std::string_view> authentication_names;
  std::set<std::string_view> authentication_keys;
  std::set<std::string_view> authentication_secrets;
  for (const auto &entry : result.authentication) {
    if (entry.name == "public") {
      throw ConfigurationReservedAuthenticationNameError(configuration_path,
                                                         entry.name);
    }

    if (!authentication_names.emplace(entry.name).second) {
      throw ConfigurationDuplicateAuthenticationNameError(configuration_path,
                                                          entry.name);
    }

    // A key opening more than one policy makes a caller belong to several at
    // once, which no single view names.
    //
    // A variable naming both a credential callers present and a secret the
    // instance signs with is worse than that: whoever holds the credential can
    // derive the key sessions are sealed under, and mint one naming whichever
    // policy they please. Secrets alone may repeat, as several policies
    // fronting the same provider is ordinary
    for (const auto &key : entry.keys) {
      if (authentication_secrets.contains(key) ||
          !authentication_keys.emplace(key).second) {
        throw ConfigurationSharedAuthenticationKeyError(configuration_path,
                                                        entry.name, key);
      }
    }

    if (!entry.client_secret_variable.empty()) {
      if (authentication_keys.contains(entry.client_secret_variable)) {
        throw ConfigurationSharedAuthenticationKeyError(
            configuration_path, entry.name, entry.client_secret_variable);
      }

      authentication_secrets.emplace(entry.client_secret_variable);
    }

    for (const auto &secret : entry.session_secret_variables) {
      if (authentication_keys.contains(secret)) {
        throw ConfigurationSharedAuthenticationKeyError(configuration_path,
                                                        entry.name, secret);
      }

      authentication_secrets.emplace(secret);
    }
  }

  entries_from_json(result.entries, "", data, default_base_path);

  return result;
}

} // namespace sourcemeta::one
