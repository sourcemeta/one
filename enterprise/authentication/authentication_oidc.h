#ifndef SOURCEMETA_ONE_AUTHENTICATION_OIDC_H_
#define SOURCEMETA_ONE_AUTHENTICATION_OIDC_H_

#include <sourcemeta/core/jose.h>
#include <sourcemeta/core/json.h>
#include <sourcemeta/core/oauth.h>

#include <array>       // std::array
#include <optional>    // std::optional, std::nullopt
#include <span>        // std::span
#include <string>      // std::string
#include <string_view> // std::string_view

/// @defgroup oidc OpenID Connect
/// @brief Standards-driven primitives for the OpenID Connect identity layer,
/// building on OAuth and JOSE.

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
/// What the provider asserted about the person who completed a login.
struct OIDCIdentity {
  std::string subject;
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

/// @ingroup oidc
/// Mint a fresh, high-entropy nonce that binds an identity token to the login
/// this instance started, defeating replay (OpenID Connect Core 1.0
/// Section 15.5.2). The 43 bytes are not null terminated, so pass them onward
/// as a view of `data()` and `size()` rather than as a C string.
inline auto oidc_nonce() -> std::array<char, 43> {
  return sourcemeta::core::oauth_random_token();
}

/// @ingroup oidc
/// The URL that begins an OpenID Connect authentication request, an OAuth
/// authorization code request (RFC 6749 Section 4.1.1) that always carries the
/// `openid` scope and a `nonce` bound to this login (OpenID Connect Core 1.0
/// Section 3.1.2.1). Building the request in one place keeps it and
/// @ref oidc_validate from drifting on the nonce.
inline auto oidc_authorization_url(
    const std::string_view authorization_endpoint,
    const std::string_view client_id, const std::string_view redirect_uri,
    const std::string_view state, const std::string_view code_challenge,
    const std::string_view nonce) -> std::string {
  const std::array<sourcemeta::core::OAuthParameter, 1> extra{
      {{.name = "nonce", .value = nonce}}};
  sourcemeta::core::OAuthAuthorizationRequest request;
  request.client_id = client_id;
  request.redirect_uri = redirect_uri;
  request.scope = "openid";
  request.state = state;
  request.code_challenge = code_challenge;
  request.code_challenge_method = sourcemeta::core::oauth_pkce_method_code(
      sourcemeta::core::OAuthPKCEMethod::S256);
  request.extra = extra;
  std::string result;
  sourcemeta::core::oauth_build_authorization_url(authorization_endpoint,
                                                  request, result);
  return result;
}

/// @ingroup oidc
/// Read the identity token out of a token endpoint response body (OpenID
/// Connect Core 1.0 Section 3.1.3.3), returning nothing when the response does
/// not carry one.
inline auto oidc_parse_id_token(const std::string_view token_response_body)
    -> std::optional<std::string> {
  const auto document{sourcemeta::core::try_parse_json(token_response_body)};
  if (!document.has_value() || !document.value().is_object()) {
    return std::nullopt;
  }

  const auto *id_token{document.value().try_at("id_token")};
  if (id_token == nullptr || !id_token->is_string()) {
    return std::nullopt;
  }

  return id_token->to_string();
}

/// @ingroup oidc
/// Validate an identity token against the provider keys and the login
/// transaction, returning the identity it asserts only when every check passes
/// (OpenID Connect Core 1.0 Section 3.1.3.7). The `audience` is the relying
/// party's client identifier.
inline auto
oidc_validate(sourcemeta::core::JWKSProvider &provider,
              const std::string_view id_token,
              const std::span<const sourcemeta::core::JWSAlgorithm> algorithms,
              const std::string_view issuer, const std::string_view audience,
              const std::string_view nonce) -> std::optional<OIDCIdentity> {
  // A login attempt always carries a nonce, so an identity token that cannot
  // be bound to one proves nothing
  if (nonce.empty()) {
    return std::nullopt;
  }

  const auto token{sourcemeta::core::JWT::from(id_token)};
  if (!token.has_value()) {
    return std::nullopt;
  }

  // The identity token is addressed to the relying party, so its audience is
  // the client identifier
  const auto error{
      provider.verify(token.value(), algorithms, issuer, audience)};
  if (error.has_value()) {
    return std::nullopt;
  }

  // The provider check confirms this relying party is among the audiences. A
  // token naming several audiences was minted primarily for someone else,
  // and no additional audience is trusted here
  const auto *aud{token.value().payload().try_at("aud")};
  if (aud == nullptr || (aud->is_array() && aud->size() != 1)) {
    return std::nullopt;
  }

  // An identity token always declares when it was issued
  if (!token.value().issued_at().has_value()) {
    return std::nullopt;
  }

  const auto *nonce_claim{token.value().payload().try_at("nonce")};
  if (nonce_claim == nullptr || !nonce_claim->is_string() ||
      nonce_claim->to_string() != nonce) {
    return std::nullopt;
  }

  const auto *subject{token.value().payload().try_at("sub")};
  if (subject == nullptr || !subject->is_string() ||
      subject->to_string().empty()) {
    return std::nullopt;
  }

  return OIDCIdentity{.subject = std::string{subject->to_string()}};
}

} // namespace sourcemeta::one

#endif
