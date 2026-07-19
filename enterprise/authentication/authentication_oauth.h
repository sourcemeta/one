#ifndef SOURCEMETA_ONE_AUTHENTICATION_OAUTH_H_
#define SOURCEMETA_ONE_AUTHENTICATION_OAUTH_H_

#include <sourcemeta/core/crypto.h>
#include <sourcemeta/core/uri.h>

#include <cstddef>     // std::size_t
#include <span>        // std::span
#include <string>      // std::string
#include <string_view> // std::string_view

/// @defgroup oauth OAuth
/// @brief Standards-driven primitives for the OAuth 2.0 authorization code
/// flow.

namespace sourcemeta::one {

namespace detail {

// The byte length of a single-use flow secret, giving a base64url token of the
// entropy PKCE mandates for a code verifier (RFC 7636 Section 7.1)
inline constexpr std::size_t OAUTH_SECRET_LENGTH{32};

// A fresh, high-entropy, URL-safe token drawn from a cryptographically secure
// source, unpadded so it is safe as a bare query parameter and as the input to
// the PKCE challenge digest
inline auto random_token() -> std::string {
  const auto bytes{sourcemeta::core::random_bytes(OAUTH_SECRET_LENGTH)};
  // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
  return sourcemeta::core::base64url_encode(std::string_view{
      reinterpret_cast<const char *>(bytes.data()), bytes.size()});
}

inline auto append_parameter(std::string &target, const std::string_view name,
                             const std::string_view value) -> void {
  target += name;
  target += '=';
  sourcemeta::core::URI::escape(value, target);
}

} // namespace detail

/// @ingroup oauth
/// The relying party's registered credentials (RFC 6749 Section 2). The fields
/// borrow from the caller, so an instance is only valid while the strings it
/// points into remain alive.
struct OAuthClient {
  std::string_view client_id;
  std::string_view client_secret;
  std::string_view redirect_uri;
};

/// @ingroup oauth
/// One additional authorization request parameter an upper layer contributes,
/// for example an OpenID Connect `nonce`. Name and value borrow from the
/// caller.
struct OAuthParameter {
  std::string_view name;
  std::string_view value;
};

/// @ingroup oauth
/// Mint a fresh, high-entropy `state` that binds a callback to the login this
/// instance started, defeating cross-site request forgery on the redirect
/// (RFC 6749 Section 10.12).
inline auto oauth_state() -> std::string { return detail::random_token(); }

/// @ingroup oauth
/// Mint a fresh PKCE code verifier with at least 256 bits of entropy
/// (RFC 7636 Section 4.1, Section 7.1).
inline auto oauth_pkce_verifier() -> std::string {
  return detail::random_token();
}

/// @ingroup oauth
/// The S256 code challenge for a verifier, `BASE64URL(SHA-256(verifier))`
/// (RFC 7636 Section 4.2). Only S256 is offered, since a capable client must
/// use it over `plain`.
inline auto oauth_pkce_challenge(const std::string_view code_verifier)
    -> std::string {
  const auto digest{sourcemeta::core::sha256_digest(code_verifier)};
  // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
  return sourcemeta::core::base64url_encode(std::string_view{
      reinterpret_cast<const char *>(digest.data()), digest.size()});
}

/// @ingroup oauth
/// The URL that sends a browser to the provider to authenticate, an
/// authorization code request with PKCE (RFC 6749 Section 4.1.1, RFC 7636
/// Section 4.3). The response type is always `code` and the challenge method
/// always S256. Any `extra` parameters an upper layer requires, such as an
/// OpenID Connect `nonce`, are appended. Every value is percent-escaped
/// (RFC 3986 Section 2.1).
inline auto oauth_authorization_url(
    const std::string_view authorization_endpoint, const OAuthClient &client,
    const std::string_view scope, const std::string_view state,
    const std::string_view code_challenge,
    const std::span<const OAuthParameter> extra = {}) -> std::string {
  std::string result{authorization_endpoint};
  result.reserve(authorization_endpoint.size() + client.client_id.size() +
                 client.redirect_uri.size() + scope.size() + state.size() +
                 code_challenge.size() + 128);
  result +=
      authorization_endpoint.find('?') == std::string_view::npos ? '?' : '&';
  result += "response_type=code&";
  detail::append_parameter(result, "client_id", client.client_id);
  result += '&';
  detail::append_parameter(result, "redirect_uri", client.redirect_uri);
  result += '&';
  detail::append_parameter(result, "scope", scope);
  result += '&';
  detail::append_parameter(result, "state", state);
  result += '&';
  detail::append_parameter(result, "code_challenge", code_challenge);
  result += "&code_challenge_method=S256";
  for (const auto &parameter : extra) {
    result += '&';
    detail::append_parameter(result, parameter.name, parameter.value);
  }

  return result;
}

/// @ingroup oauth
/// The form body that redeems an authorization code at the provider's token
/// endpoint (RFC 6749 Section 4.1.3), carrying the PKCE code verifier
/// (RFC 7636 Section 4.5) and authenticating the client through the request
/// body (RFC 6749 Section 2.3.1).
inline auto oauth_token_request(const OAuthClient &client,
                                const std::string_view code,
                                const std::string_view code_verifier)
    -> std::string {
  std::string result{"grant_type=authorization_code&"};
  detail::append_parameter(result, "code", code);
  result += '&';
  detail::append_parameter(result, "redirect_uri", client.redirect_uri);
  result += '&';
  detail::append_parameter(result, "client_id", client.client_id);
  result += '&';
  detail::append_parameter(result, "client_secret", client.client_secret);
  result += '&';
  detail::append_parameter(result, "code_verifier", code_verifier);
  return result;
}

} // namespace sourcemeta::one

#endif
