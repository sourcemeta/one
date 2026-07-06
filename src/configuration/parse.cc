#include <sourcemeta/one/configuration.h>

#include <sourcemeta/blaze/evaluator.h>
#include <sourcemeta/blaze/output.h>
#include <sourcemeta/core/jose.h>
#include <sourcemeta/core/text.h>
#include <sourcemeta/core/uri.h>

#include "template.h"

#include <algorithm>   // std::ranges::sort
#include <cassert>     // assert
#include <set>         // std::set
#include <string_view> // std::string_view

namespace {

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
      auto collection_input{input};
      const auto base_path{
          collection_input.defines("x-sourcemeta-one:path")
              ? std::filesystem::path{collection_input
                                          .at("x-sourcemeta-one:path")
                                          .to_string()}
                    .parent_path()
              : default_base_path};
      auto collection{sourcemeta::blaze::Configuration::from_json(
          collection_input, base_path)};
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
  result.base_path = server_url.path().value_or("");
  while (result.base_path.ends_with('/')) {
    result.base_path.pop_back();
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

        for (const auto &algorithm : entry.at("algorithms").as_array()) {
          parsed.algorithms.push_back(
              sourcemeta::core::to_jws_algorithm(algorithm.to_string())
                  .value());
        }

        // Canonicalise the allow-list so that policies with the same set of
        // algorithms serialise identically regardless of declaration order
        std::ranges::sort(parsed.algorithms);
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

  // Policy names are unique, and "public" is reserved because an empty policy
  // array on a listing already means public
  std::set<std::string_view> authentication_names;
  for (const auto &entry : result.authentication) {
    if (entry.name == "public") {
      throw ConfigurationReservedAuthenticationNameError(configuration_path,
                                                         entry.name);
    }

    if (!authentication_names.emplace(entry.name).second) {
      throw ConfigurationDuplicateAuthenticationNameError(configuration_path,
                                                          entry.name);
    }
  }

  entries_from_json(result.entries, "", data, default_base_path);

  return result;
}

} // namespace sourcemeta::one
