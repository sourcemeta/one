#ifndef SOURCEMETA_ONE_AUTHENTICATION_OIDC_H
#define SOURCEMETA_ONE_AUTHENTICATION_OIDC_H

#ifndef SOURCEMETA_ONE_AUTHENTICATION_EXPORT
#include <sourcemeta/one/authentication_export.h>
#endif

#include <sourcemeta/core/jose.h>

#include <optional>    // std::optional
#include <span>        // std::span
#include <string>      // std::string
#include <string_view> // std::string_view

namespace sourcemeta::one {

// The single-use secrets that bind one login attempt across the redirect to
// the provider and back
struct OIDCTransaction {
  std::string state;
  std::string nonce;
  std::string code_verifier;
};

// The relying party half of an interactive policy, as registered at the
// provider. The fields borrow from the caller, so an instance is only valid
// while the strings it points into remain alive
struct OIDCClient {
  std::string_view client_id{};
  std::string_view client_secret{};
  std::string_view redirect_uri{};
};

// What the provider asserted about the person who completed a login
struct OIDCIdentity {
  std::string subject;
};

// Mint the fresh single-use secrets for one login attempt from a
// cryptographically secure source
auto SOURCEMETA_ONE_AUTHENTICATION_EXPORT oidc_transaction() -> OIDCTransaction;

// The URL that sends a browser to the provider to authenticate, carrying the
// transaction so the callback can prove the response belongs to this attempt
auto SOURCEMETA_ONE_AUTHENTICATION_EXPORT oidc_authorization_url(
    std::string_view authorization_endpoint, const OIDCClient &client,
    std::string_view scope, const OIDCTransaction &transaction) -> std::string;

// The form body that redeems an authorization code at the provider's token
// endpoint
auto SOURCEMETA_ONE_AUTHENTICATION_EXPORT
oidc_token_request(const OIDCClient &client, std::string_view code,
                   std::string_view code_verifier) -> std::string;

// Read the identity token out of a token endpoint response, returning nothing
// when the response does not carry one
auto SOURCEMETA_ONE_AUTHENTICATION_EXPORT
oidc_parse_token_response(std::string_view body) -> std::optional<std::string>;

// Validate an identity token against the provider keys and the login
// transaction, returning the identity it asserts only when every check passes
auto SOURCEMETA_ONE_AUTHENTICATION_EXPORT oidc_validate(
    sourcemeta::core::JWKSProvider &provider, std::string_view id_token,
    std::span<const sourcemeta::core::JWSAlgorithm> algorithms,
    std::string_view issuer, const OIDCClient &client, std::string_view nonce)
    -> std::optional<OIDCIdentity>;

} // namespace sourcemeta::one

#endif
