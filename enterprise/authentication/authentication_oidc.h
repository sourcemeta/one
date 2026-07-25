#ifndef SOURCEMETA_ONE_AUTHENTICATION_OIDC_H_
#define SOURCEMETA_ONE_AUTHENTICATION_OIDC_H_

#include <sourcemeta/core/json.h>

#include <optional>    // std::optional, std::nullopt
#include <string>      // std::string
#include <string_view> // std::string_view

/// @defgroup oidc OpenID Connect
/// @brief Standards-driven primitives for OpenID Connect provider discovery,
/// kept tolerant of providers reachable over plain HTTP.

namespace sourcemeta::one {

namespace detail {

inline auto read_endpoint(const sourcemeta::core::JSON &document,
                          const std::string_view property)
    -> std::optional<std::string> {
  const auto *endpoint{document.try_at(property)};
  if (endpoint == nullptr || !endpoint->is_string()) {
    return std::nullopt;
  }

  return endpoint->to_string();
}

} // namespace detail

/// @ingroup oidc
/// The provider endpoints an OpenID Provider metadata document advertises
/// (OpenID Connect Discovery 1.0 Section 3), each present only when the
/// document declares it as a string. The `authorization_endpoint` and
/// `token_endpoint` are OAuth concepts (RFC 8414), while `userinfo_endpoint`
/// is OpenID Connect.
struct OIDCProviderMetadata {
  std::optional<std::string> jwks_uri;
  std::optional<std::string> authorization_endpoint;
  std::optional<std::string> token_endpoint;
  std::optional<std::string> userinfo_endpoint;
};

/// @ingroup oidc
/// The well-known location of an issuer's provider metadata,
/// `{issuer}/.well-known/openid-configuration` (OpenID Connect Discovery 1.0
/// Section 4.1).
inline auto oidc_discovery_url(const std::string_view issuer) -> std::string {
  std::string result{issuer};
  if (!result.empty() && result.back() == '/') {
    result.pop_back();
  }

  result += "/.well-known/openid-configuration";
  return result;
}

/// @ingroup oidc
/// Read the provider endpoints out of an OpenID Provider metadata document
/// (OpenID Connect Discovery 1.0 Section 3), returning nothing for a body that
/// is not a JSON object, or whose `issuer` is not identical to the issuer the
/// document was requested from (OpenID Connect Discovery 1.0 Section 4.3), so a
/// valid response cannot bind the flow to a different provider's endpoints.
inline auto oidc_parse_provider_metadata(const std::string_view body,
                                         const std::string_view issuer)
    -> std::optional<OIDCProviderMetadata> {
  const auto document{sourcemeta::core::try_parse_json(body)};
  if (!document.has_value() || !document.value().is_object()) {
    return std::nullopt;
  }

  const auto returned_issuer{detail::read_endpoint(document.value(), "issuer")};
  if (!returned_issuer.has_value() || returned_issuer.value() != issuer) {
    return std::nullopt;
  }

  return OIDCProviderMetadata{
      .jwks_uri = detail::read_endpoint(document.value(), "jwks_uri"),
      .authorization_endpoint =
          detail::read_endpoint(document.value(), "authorization_endpoint"),
      .token_endpoint =
          detail::read_endpoint(document.value(), "token_endpoint"),
      .userinfo_endpoint =
          detail::read_endpoint(document.value(), "userinfo_endpoint")};
}

} // namespace sourcemeta::one

#endif
