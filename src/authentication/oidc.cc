#include <sourcemeta/one/authentication_oidc.h>

#include <optional>    // std::optional, std::nullopt
#include <span>        // std::span
#include <string>      // std::string
#include <string_view> // std::string_view

namespace sourcemeta::one {

// Interactive login is an enterprise feature, so this edition never starts a
// login, never redeems a code, and never accepts an identity token
auto oidc_transaction() -> OIDCTransaction { return {}; }

auto oidc_authorization_url(const std::string_view, const OIDCClient &,
                            const std::string_view, const OIDCTransaction &)
    -> std::string {
  return {};
}

auto oidc_token_request(const OIDCClient &, const std::string_view,
                        const std::string_view) -> std::string {
  return {};
}

auto oidc_parse_token_response(const std::string_view)
    -> std::optional<std::string> {
  return std::nullopt;
}

auto oidc_validate(sourcemeta::core::JWKSProvider &, const std::string_view,
                   const std::span<const sourcemeta::core::JWSAlgorithm>,
                   const std::string_view, const OIDCClient &,
                   const std::string_view) -> std::optional<OIDCIdentity> {
  return std::nullopt;
}

} // namespace sourcemeta::one
