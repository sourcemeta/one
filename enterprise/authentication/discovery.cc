#include <sourcemeta/one/authentication_discovery.h>

#include <sourcemeta/core/json.h>

#include <optional>    // std::optional, std::nullopt
#include <string>      // std::string
#include <string_view> // std::string_view

namespace {

auto read_endpoint(const sourcemeta::core::JSON &document,
                   const std::string_view property)
    -> std::optional<std::string> {
  const auto *endpoint{document.try_at(property)};
  if (endpoint == nullptr || !endpoint->is_string()) {
    return std::nullopt;
  }

  return endpoint->to_string();
}

} // namespace

namespace sourcemeta::one {

auto discovery_url(const std::string_view issuer) -> std::string {
  std::string result{issuer};
  if (!result.empty() && result.back() == '/') {
    result.pop_back();
  }

  result += "/.well-known/openid-configuration";
  return result;
}

auto discovery_parse(const std::string_view body)
    -> std::optional<DiscoveryDocument> {
  const auto document{sourcemeta::core::try_parse_json(body)};
  if (!document.has_value() || !document.value().is_object()) {
    return std::nullopt;
  }

  return DiscoveryDocument{
      .jwks_uri = read_endpoint(document.value(), "jwks_uri"),
      .authorization_endpoint =
          read_endpoint(document.value(), "authorization_endpoint"),
      .token_endpoint = read_endpoint(document.value(), "token_endpoint"),
      .userinfo_endpoint =
          read_endpoint(document.value(), "userinfo_endpoint")};
}

} // namespace sourcemeta::one
