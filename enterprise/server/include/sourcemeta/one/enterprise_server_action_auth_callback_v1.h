#ifndef SOURCEMETA_ONE_ENTERPRISE_SERVER_ACTION_AUTH_CALLBACK_V1_H_
#define SOURCEMETA_ONE_ENTERPRISE_SERVER_ACTION_AUTH_CALLBACK_V1_H_

#include <sourcemeta/core/crypto.h>
#include <sourcemeta/core/http.h>
#include <sourcemeta/core/jose.h>
#include <sourcemeta/core/json.h>
#include <sourcemeta/core/jsonrpc.h>
#include <sourcemeta/core/mcp.h>
#include <sourcemeta/core/oauth.h>
#include <sourcemeta/core/oidc.h>
#include <sourcemeta/core/uritemplate.h>

#include <sourcemeta/one/authentication.h>
#include <sourcemeta/one/http.h>
#include <sourcemeta/one/router.h>

#include <array>       // std::array
#include <chrono>      // std::chrono::seconds, std::chrono::system_clock
#include <filesystem>  // std::filesystem::path
#include <optional>    // std::optional, std::nullopt
#include <span>        // std::span
#include <sstream>     // std::ostringstream
#include <string>      // std::string
#include <string_view> // std::string_view
#include <utility>     // std::move
#include <vector>      // std::vector

class ActionAuthCallback_v1 : public sourcemeta::one::RouterAction {
public:
  static constexpr std::string_view DESCRIPTION{
      "Complete an interactive login by exchanging the provider's "
      "authorization code for a session"};
  static constexpr bool READ_ONLY{false};
  static constexpr bool DESTRUCTIVE{false};
  static constexpr bool IDEMPOTENT{false};
  static constexpr bool OPEN_WORLD{false};

  // A session lasts an hour, kept short so that a lost cookie cannot outlive
  // its usefulness, with silent re-authentication as the eventual refresh
  static constexpr std::chrono::seconds SESSION_LIFETIME{3600};

  ActionAuthCallback_v1(
      const std::filesystem::path &base,
      const sourcemeta::core::URITemplateRouterView &router,
      const sourcemeta::core::URITemplateRouter::Identifier identifier,
      sourcemeta::one::Router &dispatcher)
      : sourcemeta::one::RouterAction{base, router.base_path(),
                                      router.base_url(), dispatcher} {
    router.arguments(
        identifier, [this](const auto &key, const auto &value) -> void {
          if (key == "errorSchema") {
            this->error_schema_ = std::get<std::string_view>(value);
          }
        });
  }

  [[nodiscard]] auto is_authentication_exempt() const noexcept
      -> bool override {
    return true;
  }

  auto rest(const std::span<std::string_view> matches, std::string_view,
            sourcemeta::one::HTTPRequest &request,
            sourcemeta::one::HTTPResponse &response) -> void override {
    if (request.method() == "options") {
      response.write_status(sourcemeta::core::HTTP_STATUS_NO_CONTENT);
      response.write_header("Cache-Control", "no-store");
      // RFC 9110 §9.3.7: OPTIONS responses SHOULD include Allow
      response.write_header("Allow", "GET, HEAD, OPTIONS");
      sourcemeta::one::send_response(sourcemeta::core::HTTP_STATUS_NO_CONTENT,
                                     request, response);
      return;
    }

    if (request.method() != "get" && request.method() != "head") {
      sourcemeta::one::json_error(
          request, response, sourcemeta::core::HTTP_STATUS_METHOD_NOT_ALLOWED,
          "urn:sourcemeta:one:method-not-allowed",
          "This HTTP method is invalid for this URL", this->error_schema_, "*",
          "GET, HEAD, OPTIONS");
      return;
    }

    if (matches.empty()) {
      sourcemeta::one::json_error(
          request, response,
          sourcemeta::core::HTTP_STATUS_INTERNAL_SERVER_ERROR,
          "urn:sourcemeta:one:auth-missing-policy-match",
          "This action requires a policy name match", this->error_schema_, "*");
      return;
    }
    const auto policy_name{matches.front()};
    const auto state{request.query("state")};

    // The transaction cookie is the only proof that this response belongs to
    // a login this instance started, and the state it carries must match the
    // one the provider echoes back. This gate runs before either the success
    // or the decline is honoured, so a cross-site callback cannot even
    // trigger an error on a person's behalf, per RFC 6749 section 4.1.2.1
    const auto &authentication{this->dispatcher().authentication()};
    const auto transaction{
        this->transaction(request, authentication, policy_name, state)};
    if (!transaction.has_value()) {
      this->fail(request, response, sourcemeta::core::HTTP_STATUS_BAD_REQUEST,
                 "urn:sourcemeta:one:auth-invalid-callback",
                 "The login could not be completed", policy_name);
      return;
    }

    const auto *nonce{transaction.value().try_at("nonce")};
    const auto *verifier{transaction.value().try_at("verifier")};

    // Only once the callback is proven to belong to a real login is the
    // provider's outcome honoured: a decline returns an error instead of a
    // code, and a success without a code is malformed
    if (request.has_query("error")) {
      this->fail(request, response, sourcemeta::core::HTTP_STATUS_FORBIDDEN,
                 "urn:sourcemeta:one:auth-login-declined",
                 "The identity provider declined the login", policy_name);
      return;
    }

    const auto code{request.query("code")};
    if (code.empty()) {
      this->fail(request, response, sourcemeta::core::HTTP_STATUS_BAD_REQUEST,
                 "urn:sourcemeta:one:auth-invalid-callback",
                 "The login could not be completed", policy_name);
      return;
    }

    const auto policy{authentication.interactive(policy_name)};
    if (!policy.has_value()) {
      this->fail(request, response, sourcemeta::core::HTTP_STATUS_NOT_FOUND,
                 "urn:sourcemeta:one:not-found", "There is nothing at this URL",
                 policy_name);
      return;
    }

    const auto client_secret{authentication.client_secret(policy_name)};
    if (!client_secret.has_value()) {
      this->fail(request, response,
                 sourcemeta::core::HTTP_STATUS_INTERNAL_SERVER_ERROR,
                 "urn:sourcemeta:one:auth-misconfigured",
                 "The authentication configuration is incomplete", policy_name);
      return;
    }

    const auto metadata{this->discover(policy->issuer)};
    if (!metadata.has_value() ||
        !metadata.value().token_endpoint().has_value()) {
      this->fail(request, response, sourcemeta::core::HTTP_STATUS_BAD_GATEWAY,
                 "urn:sourcemeta:one:auth-provider-unreachable",
                 "The identity provider could not be reached", policy_name);
      return;
    }

    std::string redirect_uri{this->server_uri()};
    redirect_uri += "/self/v1/auth/callback/";
    redirect_uri += policy_name;

    const auto id_token{this->exchange(
        metadata.value().token_endpoint().value(), policy->client_id,
        client_secret.value(), redirect_uri, code, verifier->to_string())};
    if (!id_token.has_value()) {
      this->fail(request, response, sourcemeta::core::HTTP_STATUS_BAD_GATEWAY,
                 "urn:sourcemeta:one:auth-exchange-failed",
                 "The authorization code could not be redeemed", policy_name);
      return;
    }

    const auto token{sourcemeta::core::JWT::from(id_token.value())};
    if (!token.has_value()) {
      this->fail(request, response, sourcemeta::core::HTTP_STATUS_BAD_GATEWAY,
                 "urn:sourcemeta:one:auth-invalid-identity",
                 "The identity token could not be validated", policy_name);
      return;
    }

    auto provider{
        this->jwks_provider(std::string{metadata.value().jwks_uri()})};
    sourcemeta::core::OIDCValidationOptions options;
    options.nonce = nonce->to_string();
    const auto identity{sourcemeta::core::oidc_validate_id_token(
        provider, token.value(), ID_TOKEN_ALGORITHMS, policy->issuer,
        policy->client_id, options)};
    if (!identity.has_value()) {
      this->fail(request, response, sourcemeta::core::HTTP_STATUS_BAD_GATEWAY,
                 "urn:sourcemeta:one:auth-invalid-identity",
                 "The identity token could not be validated", policy_name);
      return;
    }

    auto payload{sourcemeta::core::JSON::make_object()};
    payload.assign_assume_new("policy",
                              sourcemeta::core::JSON{std::string{policy_name}});
    payload.assign_assume_new("subject",
                              sourcemeta::core::JSON{identity.value().subject});
    std::ostringstream payload_text;
    sourcemeta::core::stringify(payload, payload_text);

    const auto expiry{std::chrono::time_point_cast<std::chrono::seconds>(
                          std::chrono::system_clock::now()) +
                      SESSION_LIFETIME};
    const auto session{authentication.seal(
        policy_name, sourcemeta::one::Authentication::Purpose::Session,
        payload_text.str(), expiry)};
    if (!session.has_value()) {
      this->fail(request, response,
                 sourcemeta::core::HTTP_STATUS_INTERNAL_SERVER_ERROR,
                 "urn:sourcemeta:one:auth-misconfigured",
                 "The authentication configuration is incomplete", policy_name);
      return;
    }

    const auto base{this->server_uri_base_path()};
    const auto scope{base.empty() ? std::string_view{"/"} : base};
    const auto secure{this->server_uri().starts_with("https://")};
    const auto session_cookie{sourcemeta::core::http_serialize_cookie(
        {.name = this->cookie_name(
             sourcemeta::one::Authentication::SESSION_COOKIE_PREFIX,
             policy_name),
         .value = session.value(),
         .path = scope,
         .max_age = SESSION_LIFETIME,
         .http_only = true,
         .secure = secure,
         .same_site = sourcemeta::core::HTTPCookieSameSite::Lax})};
    if (!session_cookie.has_value()) {
      this->fail(request, response,
                 sourcemeta::core::HTTP_STATUS_INTERNAL_SERVER_ERROR,
                 "urn:sourcemeta:one:auth-misconfigured",
                 "The authentication configuration is incomplete", policy_name);
      return;
    }

    // The login may have sealed a page to return to. It came through this
    // instance's own signature, yet it is re-checked as a same-origin local
    // path before being trusted as a redirect target, defaulting to the
    // instance root
    std::string destination{scope};
    const auto *sealed_destination{transaction.value().try_at("to")};
    if (sealed_destination != nullptr && sealed_destination->is_string()) {
      const auto &candidate{sealed_destination->to_string()};
      if (sourcemeta::one::is_local_path(candidate)) {
        destination = candidate;
      }
    }

    response.write_status(sourcemeta::core::HTTP_STATUS_SEE_OTHER);
    response.write_header("Set-Cookie", session_cookie.value());
    // The single-use transaction has served its purpose, so it is expired
    // alongside minting the session
    this->expire_transaction(response, policy_name, scope, secure);
    response.write_header("Location", destination);
    response.write_header("Cache-Control", "no-store");
    sourcemeta::one::send_response(sourcemeta::core::HTTP_STATUS_SEE_OTHER,
                                   request, response);
  }

  auto mcp(const sourcemeta::core::MCPProtocolVersion,
           const sourcemeta::core::JSON &id, const sourcemeta::core::JSON &,
           std::string_view) -> sourcemeta::core::JSON override {
    return sourcemeta::core::jsonrpc_make_error_method_not_found(id);
  }

private:
  // The signature algorithms an identity token may be signed with. The major
  // providers sign with one of these, and the policy does not yet let an
  // operator narrow the set
  static constexpr std::array<sourcemeta::core::JWSAlgorithm, 2>
      ID_TOKEN_ALGORITHMS{{sourcemeta::core::JWSAlgorithm::RS256,
                           sourcemeta::core::JWSAlgorithm::ES256}};

  [[nodiscard]] auto cookie_name(const std::string_view prefix,
                                 const std::string_view policy_name) const
      -> std::string {
    std::string result;
    result.reserve(prefix.size() + policy_name.size());
    result += prefix;
    result += policy_name;
    return result;
  }

  // The transaction a callback belongs to, if the request carries one. A
  // request can present several cookies under one name, since a parent
  // domain and the host itself can each set one and neither the header nor
  // the order says which is which, so every value is tried. Letting whoever
  // controls a neighbouring host decide which transaction this instance
  // reads is what turns the cookie from a defence against a forged callback
  // into the way to mount one
  [[nodiscard]] auto
  transaction(sourcemeta::one::HTTPRequest &request,
              const sourcemeta::one::Authentication &authentication,
              const std::string_view policy_name,
              const std::string_view state) const
      -> std::optional<sourcemeta::core::JSON> {
    if (state.empty()) {
      return std::nullopt;
    }

    const auto name{this->cookie_name(
        sourcemeta::one::Authentication::TRANSACTION_COOKIE_PREFIX,
        policy_name)};
    std::vector<std::string_view> candidates;
    sourcemeta::core::http_cookie_values(request.header("cookie"), name,
                                         candidates);
    for (const auto sealed : candidates) {
      auto opened{authentication.open(
          policy_name, sourcemeta::one::Authentication::Purpose::Transaction,
          sealed)};
      if (!opened.has_value()) {
        continue;
      }

      auto document{sourcemeta::core::try_parse_json(opened.value())};
      if (!document.has_value() || !document.value().is_object()) {
        continue;
      }

      const auto *sealed_policy{document.value().try_at("policy")};
      const auto *sealed_state{document.value().try_at("state")};
      const auto *nonce{document.value().try_at("nonce")};
      const auto *verifier{document.value().try_at("verifier")};
      if (sealed_policy == nullptr || !sealed_policy->is_string() ||
          sealed_policy->to_string() != policy_name ||
          sealed_state == nullptr || !sealed_state->is_string() ||
          sealed_state->to_string() != state || nonce == nullptr ||
          !nonce->is_string() || nonce->to_string().empty() ||
          verifier == nullptr || !verifier->is_string() ||
          verifier->to_string().empty()) {
        continue;
      }

      return document;
    }

    return std::nullopt;
  }

  auto expire_transaction(sourcemeta::one::HTTPResponse &response,
                          const std::string_view policy_name,
                          const std::string_view scope, const bool secure) const
      -> void {
    const auto cookie{sourcemeta::core::http_serialize_cookie(
        {.name = this->cookie_name(
             sourcemeta::one::Authentication::TRANSACTION_COOKIE_PREFIX,
             policy_name),
         .value = "",
         .path = scope,
         .max_age = std::chrono::seconds{0},
         .http_only = true,
         .secure = secure,
         .same_site = sourcemeta::core::HTTPCookieSameSite::Lax})};
    if (cookie.has_value()) {
      response.write_header("Set-Cookie", cookie.value());
    }
  }

  [[nodiscard]] auto discover(const std::string_view issuer) const
      -> std::optional<sourcemeta::core::OIDCProviderMetadata> {
    try {
      const auto url{sourcemeta::core::oidc_discovery_url(issuer)};
      if (!url.has_value()) {
        return std::nullopt;
      }

      sourcemeta::core::HTTPSystemRequest fetch{url.value()};
      fetch.connect_timeout(std::chrono::seconds{2});
      fetch.timeout(std::chrono::seconds{5});
      fetch.maximum_response_size(1024UL * 1024UL);
      fetch.follow_redirects(false);
      const auto result{fetch.send()};
      if (result.status.code < 200 || result.status.code >= 300) {
        return std::nullopt;
      }

      auto parsed{sourcemeta::core::try_parse_json(result.body)};
      if (!parsed.has_value()) {
        return std::nullopt;
      }

      return sourcemeta::core::OIDCProviderMetadata::from(
          std::move(parsed).value(), issuer);
    } catch (...) {
      return std::nullopt;
    }
  }

  [[nodiscard]] auto exchange(const std::string_view token_endpoint,
                              const std::string_view client_id,
                              const std::string_view client_secret,
                              const std::string_view redirect_uri,
                              const std::string_view code,
                              const std::string_view code_verifier) const
      -> std::optional<std::string> {
    try {
      sourcemeta::core::HTTPSystemRequest fetch{
          std::string{token_endpoint}, sourcemeta::core::HTTPMethod::POST};
      fetch.connect_timeout(std::chrono::seconds{2});
      fetch.timeout(std::chrono::seconds{5});
      fetch.maximum_response_size(1024UL * 1024UL);
      fetch.follow_redirects(false);
      sourcemeta::core::SecureString body;
      sourcemeta::core::oauth_build_token_request_code(code, redirect_uri,
                                                       code_verifier, {}, body);
      sourcemeta::core::oauth_client_secret_post(client_id, client_secret,
                                                 body);
      fetch.body(std::move(body), "application/x-www-form-urlencoded");
      const auto result{fetch.send()};
      if (result.status.code < 200 || result.status.code >= 300) {
        return std::nullopt;
      }

      const auto document{sourcemeta::core::try_parse_json(result.body)};
      if (!document.has_value()) {
        return std::nullopt;
      }

      return sourcemeta::core::oidc_parse_id_token(document.value());
    } catch (...) {
      return std::nullopt;
    }
  }

  [[nodiscard]] static auto jwks_provider(std::string location)
      -> sourcemeta::core::JWKSProvider {
    return sourcemeta::core::JWKSProvider{
        std::move(location),
        [](const std::string_view url)
            -> std::optional<sourcemeta::core::JWKSProvider::FetchResult> {
          try {
            sourcemeta::core::HTTPSystemRequest fetch{std::string{url}};
            fetch.connect_timeout(std::chrono::seconds{2});
            fetch.timeout(std::chrono::seconds{5});
            fetch.maximum_response_size(1024UL * 1024UL);
            fetch.follow_redirects(false);
            const auto result{fetch.send()};
            if (result.status.code < 200 || result.status.code >= 300) {
              return std::nullopt;
            }

            return sourcemeta::core::JWKSProvider::FetchResult{
                .body = result.body, .max_age = std::nullopt};
          } catch (...) {
            return std::nullopt;
          }
        },
        {}};
  }

  // A failed login leaves its transaction cookie in place, sealed and bound
  // to a state the provider would have to echo, expiring on its own within
  // minutes, so no cookie is cleared here and the error response owns the
  // status line uncontested
  auto fail(sourcemeta::one::HTTPRequest &request,
            sourcemeta::one::HTTPResponse &response,
            const sourcemeta::core::HTTPStatus &status,
            const std::string_view type, const std::string_view detail,
            const std::string_view) const -> void {
    sourcemeta::one::json_error(request, response, status, type, detail,
                                this->error_schema_, "*");
  }

  std::string_view error_schema_;
};

#endif
