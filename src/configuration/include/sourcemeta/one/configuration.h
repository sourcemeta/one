#ifndef SOURCEMETA_ONE_CONFIGURATION_H_
#define SOURCEMETA_ONE_CONFIGURATION_H_

#include <sourcemeta/blaze/configuration.h>
#include <sourcemeta/core/jose.h>
#include <sourcemeta/core/json.h>
#include <sourcemeta/core/uri.h>

#include <sourcemeta/one/configuration_error.h>

#include <algorithm>     // std::clamp
#include <cstdint>       // std::uint8_t
#include <filesystem>    // std::filesystem::path
#include <optional>      // std::optional
#include <string>        // std::string
#include <string_view>   // std::string_view
#include <unordered_map> // std::unordered_map
#include <unordered_set> // std::unordered_set
#include <variant>       // std::variant
#include <vector>        // std::vector

namespace sourcemeta::one {

struct Configuration {
  static auto read(const std::filesystem::path &configuration_path,
                   const std::filesystem::path &self_path,
                   std::unordered_set<std::string> &configuration_files)
      -> sourcemeta::core::JSON;
  static auto read(const std::filesystem::path &configuration_path,
                   const std::filesystem::path &self_path)
      -> sourcemeta::core::JSON;
  static auto parse(const sourcemeta::core::JSON &data,
                    const std::filesystem::path &configuration_path,
                    const std::filesystem::path &default_base_path)
      -> Configuration;

  sourcemeta::core::JSON::String url;
  sourcemeta::core::JSON::String origin;
  // The path of the configuration file this was parsed from
  std::filesystem::path path;

  struct HTML {
    sourcemeta::core::JSON::String name;
    sourcemeta::core::JSON::String description;
    std::optional<sourcemeta::core::JSON::String> head;
    std::optional<sourcemeta::core::JSON::String> hero;

    struct Action {
      sourcemeta::core::JSON::String url;
      sourcemeta::core::JSON::String icon;
      sourcemeta::core::JSON::String title;
    };

    std::optional<Action> action;
  };

  std::optional<HTML> html;
  bool api{true};

  struct AuthenticationEntry {
    // What a policy authenticates against
    enum class Type : std::uint8_t { ApiKey, JWT, OIDC };

    // How a presented credential is compared against the keys
    enum class Algorithm : std::uint8_t { Identity, Sha256 };

    Type type{Type::ApiKey};
    // The policy name
    sourcemeta::core::JSON::String name;
    // A human readable version of the name
    sourcemeta::core::JSON::String title;
    std::vector<sourcemeta::core::JSON::String> paths;
    Algorithm algorithm{Algorithm::Identity};
    // Environment variable names holding the keys
    std::vector<sourcemeta::core::JSON::String> keys;
    sourcemeta::core::JSON::String issuer;
    sourcemeta::core::JSON::String audience;
    std::optional<sourcemeta::core::JSON::String> jwks_uri;
    std::vector<sourcemeta::core::JWSAlgorithm> algorithms;
    // The `typ` header a presented token must carry, empty to accept any
    sourcemeta::core::JSON::String token_type;
    // The claims a credential must carry to be admitted, each named alongside
    // the values that admit it, in the shape of an OpenID Connect claims
    // request parameter. Null where a policy names no rule
    sourcemeta::core::JSON claims{nullptr};
    // The email domains that admit a person, lowercased and sorted, since a
    // domain names a host and the order says nothing about who is admitted
    std::vector<sourcemeta::core::JSON::String> email_domains;
    sourcemeta::core::JSON::String client_id;
    // The environment variable name holding the client secret
    sourcemeta::core::JSON::String client_secret_variable;
    // The environment variable names holding the session signing secrets,
    // newest first. Several coexist so that a secret can be replaced while
    // values signed under the one it replaces are still honoured
    std::vector<sourcemeta::core::JSON::String> session_secret_variables;
  };

  std::vector<AuthenticationEntry> authentication;

  struct Page {
    std::optional<sourcemeta::core::JSON::String> title;
    std::optional<sourcemeta::core::JSON::String> description;
    std::optional<sourcemeta::core::JSON::String> email;
    std::optional<sourcemeta::core::JSON::String> github;
    std::optional<sourcemeta::core::JSON::String> website;
  };

  using Collection = sourcemeta::blaze::Configuration;

  [[nodiscard]] auto resolve_schema(const sourcemeta::core::URI &input) const
      -> std::optional<std::filesystem::path>;

  [[nodiscard]] static auto should_evaluate(const Collection &collection)
      -> bool {
    const auto *value{collection.extra.try_at("x-sourcemeta-one:evaluate")};
    return value == nullptr || !value->is_boolean() || value->to_boolean();
  }

  [[nodiscard]] static auto priority(const Collection &collection)
      -> std::uint8_t {
    const auto *value{collection.extra.try_at("x-sourcemeta-one:priority")};
    if (value == nullptr || !value->is_integer()) {
      return 50;
    }
    return static_cast<std::uint8_t>(
        std::clamp<sourcemeta::core::JSON::Integer>(value->to_integer(), 0,
                                                    100));
  }

  [[nodiscard]] auto
  is_collection_base(const std::filesystem::path &relative_path) const -> bool {
    const auto match{this->entries.find(relative_path)};
    return match != this->entries.cend() &&
           std::get_if<Collection>(&match->second) != nullptr;
  }

  // Whether the registry path is at or above a declared collection or page (the
  // registry root is above all of them). A path inside a collection is not, so
  // a policy gates whole collections and pages, never a path within one. The
  // path grammar matches the route table: no trailing slash
  [[nodiscard]] auto covers_entry(std::string_view registry_path) const -> bool;

  std::unordered_map<std::filesystem::path, std::variant<Page, Collection>>
      entries;
};

// Whether a policy signs a person in through a provider. Only such a policy
// has anywhere to send somebody who carries nothing, so this is what decides
// both what the login endpoint offers and whether the bar offers a way to it
[[nodiscard]] inline auto
is_interactive(const Configuration::AuthenticationEntry &policy) -> bool {
  return policy.type == Configuration::AuthenticationEntry::Type::OIDC;
}

} // namespace sourcemeta::one

#endif
