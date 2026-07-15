#include <sourcemeta/one/authentication_oidc.h>

#include <sourcemeta/core/crypto.h>
#include <sourcemeta/core/json.h>
#include <sourcemeta/core/uri.h>

#include <cstddef>     // std::size_t
#include <optional>    // std::optional, std::nullopt
#include <span>        // std::span
#include <string>      // std::string
#include <string_view> // std::string_view

namespace {

constexpr std::size_t TRANSACTION_SECRET_LENGTH{32};

auto random_token() -> std::string {
  const auto bytes{sourcemeta::core::random_bytes(TRANSACTION_SECRET_LENGTH)};
  // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
  return sourcemeta::core::base64url_encode(std::string_view{
      reinterpret_cast<const char *>(bytes.data()), bytes.size()});
}

auto append_parameter(std::string &target, const std::string_view name,
                      const std::string_view value) -> void {
  target += name;
  target += '=';
  sourcemeta::core::URI::escape(value, target);
}

// The challenge is the URL-safe digest of the verifier, so the provider can
// prove the party redeeming the code is the one that started the login
auto code_challenge(const std::string_view code_verifier) -> std::string {
  const auto digest{sourcemeta::core::sha256_digest(code_verifier)};
  // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
  return sourcemeta::core::base64url_encode(std::string_view{
      reinterpret_cast<const char *>(digest.data()), digest.size()});
}

} // namespace

namespace sourcemeta::one {

auto oidc_transaction() -> OIDCTransaction {
  return {.state = random_token(),
          .nonce = random_token(),
          .code_verifier = random_token()};
}

auto oidc_authorization_url(const std::string_view authorization_endpoint,
                            const OIDCClient &client,
                            const std::string_view scope,
                            const OIDCTransaction &transaction) -> std::string {
  std::string result{authorization_endpoint};
  result +=
      authorization_endpoint.find('?') == std::string_view::npos ? '?' : '&';
  result += "response_type=code&";
  append_parameter(result, "client_id", client.client_id);
  result += '&';
  append_parameter(result, "redirect_uri", client.redirect_uri);
  result += '&';
  append_parameter(result, "scope", scope);
  result += '&';
  append_parameter(result, "state", transaction.state);
  result += '&';
  append_parameter(result, "nonce", transaction.nonce);
  result += '&';
  append_parameter(result, "code_challenge",
                   code_challenge(transaction.code_verifier));
  result += "&code_challenge_method=S256";
  return result;
}

auto oidc_token_request(const OIDCClient &client, const std::string_view code,
                        const std::string_view code_verifier) -> std::string {
  std::string result{"grant_type=authorization_code&"};
  append_parameter(result, "code", code);
  result += '&';
  append_parameter(result, "redirect_uri", client.redirect_uri);
  result += '&';
  append_parameter(result, "client_id", client.client_id);
  result += '&';
  append_parameter(result, "client_secret", client.client_secret);
  result += '&';
  append_parameter(result, "code_verifier", code_verifier);
  return result;
}

auto oidc_parse_token_response(const std::string_view body)
    -> std::optional<std::string> {
  const auto document{sourcemeta::core::try_parse_json(body)};
  if (!document.has_value() || !document.value().is_object()) {
    return std::nullopt;
  }

  const auto *id_token{document.value().try_at("id_token")};
  if (id_token == nullptr || !id_token->is_string()) {
    return std::nullopt;
  }

  return id_token->to_string();
}

auto oidc_validate(
    sourcemeta::core::JWKSProvider &provider, const std::string_view id_token,
    const std::span<const sourcemeta::core::JWSAlgorithm> algorithms,
    const std::string_view issuer, const OIDCClient &client,
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
      provider.verify(token.value(), algorithms, issuer, client.client_id)};
  if (error.has_value()) {
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
