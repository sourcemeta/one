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
#include <sourcemeta/core/uri.h>
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

  // How long a browser stays eligible for a silent renewal after signing in.
  // Long enough to outlast a provider session, since the provider is the one
  // that decides whether a renewal succeeds, and losing it early only costs a
  // sign-in page that would otherwise have been skipped
  static constexpr std::chrono::seconds RENEWAL_LIFETIME{43200};

  // RFC 6265 Section 6.1 asks a user agent to support "at least 4096 bytes per
  // cookie (as measured by the sum of the length of the cookie's name, value,
  // and attributes)". That is a floor they should honour rather than a ceiling
  // they must enforce, and what happens above it is left unsaid, so the whole
  // serialised cookie is kept under it with room to spare
  static constexpr std::size_t MAXIMUM_COOKIE_LENGTH{4000};

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
                 "The login could not be completed");
      return;
    }

    // Nobody asked for a silent attempt, so from here on nothing it does is
    // shown to them: every way this can end without a session puts the browser
    // back where it started instead
    const auto silent{transaction.value().try_at("silent") != nullptr};
    const auto *nonce{transaction.value().try_at("nonce")};
    const auto *verifier{transaction.value().try_at("verifier")};

    // Which policy a callback belongs to is settled by opening its
    // transaction, so nothing reaching here names one this instance does not
    // serve. Answering as though the callback were unproven keeps that the
    // only thing this URL ever says about a name
    const auto policy{authentication.interactive(policy_name)};
    if (!policy.has_value()) {
      this->abandon(silent, transaction.value(), request, response,
                    sourcemeta::core::HTTP_STATUS_BAD_REQUEST,
                    "urn:sourcemeta:one:auth-invalid-callback",
                    "The login could not be completed");
      return;
    }

    // RFC 9207 Section 2.4 has a client compare the issuer an answer names
    // against the one it addressed the request to, which is what catches an
    // answer relayed from somewhere else. A provider naming none cannot be
    // checked that way, so this runs only when one arrives, and it runs ahead
    // of the outcome so that no answer is acted on before it is placed
    if (request.has_query("iss") && request.query("iss") != policy->issuer) {
      this->abandon(silent, transaction.value(), request, response,
                    sourcemeta::core::HTTP_STATUS_BAD_REQUEST,
                    "urn:sourcemeta:one:auth-invalid-callback",
                    "The login could not be completed");
      return;
    }

    // Only once the callback is proven to belong to a real login is the
    // provider's outcome honoured: a decline returns an error instead of a
    // code, and a success without a code is malformed. RFC 6749 Section
    // 4.1.2.1 has a decline name itself with an error code, so one that names
    // nothing is not a decision this can report as the provider's. It is no
    // grant either, and a code arriving beside it is left unredeemed
    if (request.has_query("error")) {
      if (request.query("error").empty()) {
        this->abandon(silent, transaction.value(), request, response,
                      sourcemeta::core::HTTP_STATUS_BAD_REQUEST,
                      "urn:sourcemeta:one:auth-invalid-callback",
                      "The login could not be completed");
      } else {
        this->abandon(silent, transaction.value(), request, response,
                      sourcemeta::core::HTTP_STATUS_FORBIDDEN,
                      "urn:sourcemeta:one:auth-login-declined",
                      "The identity provider declined the login");
      }

      return;
    }

    const auto code{request.query("code")};
    if (code.empty()) {
      this->abandon(silent, transaction.value(), request, response,
                    sourcemeta::core::HTTP_STATUS_BAD_REQUEST,
                    "urn:sourcemeta:one:auth-invalid-callback",
                    "The login could not be completed");
      return;
    }

    const auto client_secret{authentication.client_secret(policy_name)};
    if (!client_secret.has_value()) {
      sourcemeta::one::HTTP_LOG("No client secret is set for the policy",
                                policy_name);
      this->incomplete(silent, transaction.value(), request, response);
      return;
    }

    const auto endpoints{authentication.endpoints(policy_name)};
    if (!endpoints.has_value() || endpoints.value().token.empty()) {
      sourcemeta::one::HTTP_LOG("The provider named no token endpoint, or "
                                "could not be reached, for the policy",
                                policy_name);
      this->incomplete(silent, transaction.value(), request, response);
      return;
    }

    std::string redirect_uri{this->server_uri()};
    redirect_uri += "/self/v1/auth/callback/";
    redirect_uri += policy_name;

    const auto id_token{this->exchange(
        endpoints.value().token, policy->client_id, client_secret.value(),
        redirect_uri, code, verifier->to_string(),
        endpoints.value().token_endpoint_basic_auth)};
    if (!id_token.has_value()) {
      sourcemeta::one::HTTP_LOG(
          "The authorization code could not be redeemed for the policy",
          policy_name);
      this->incomplete(silent, transaction.value(), request, response);
      return;
    }

    const auto token{sourcemeta::core::JWT::from(id_token.value())};
    if (!token.has_value()) {
      sourcemeta::one::HTTP_LOG(
          "The provider returned an identity token that could not be read, "
          "for the policy",
          policy_name);
      this->incomplete(silent, transaction.value(), request, response);
      return;
    }

    auto provider{this->jwks_provider(endpoints.value().jwks_uri)};
    sourcemeta::core::OIDCValidationOptions options;
    options.nonce = nonce->to_string();
    const auto identity{sourcemeta::core::oidc_validate_id_token(
        provider, token.value(), ID_TOKEN_ALGORITHMS, policy->issuer,
        policy->client_id, options)};
    if (!identity.has_value()) {
      sourcemeta::one::HTTP_LOG(
          "The identity token did not validate for the policy", policy_name);
      this->incomplete(silent, transaction.value(), request, response);
      return;
    }

    const auto expiry{std::chrono::time_point_cast<std::chrono::seconds>(
                          std::chrono::system_clock::now()) +
                      SESSION_LIFETIME};
    const auto base{this->server_uri_base_path()};
    const auto scope{base.empty() ? std::string_view{"/"} : base};
    const auto secure{sourcemeta::core::URI{this->server_uri()}.is_https()};

    // The identity token is kept so that logging out can prove whose session
    // it is asking the provider to end, which is what spares the person a
    // confirmation page there. A provider that mints a large one can push the
    // cookie past what a browser will store, and a browser drops such a cookie
    // without saying so, which would look like signing in and then not being
    // signed in. So the whole cookie is measured, and the token is left out
    // when it does not fit, which costs the confirmation page and nothing else
    auto session_cookie{this->session_cookie(
        authentication, policy_name, identity.value().subject, id_token.value(),
        expiry, scope, secure)};
    if (session_cookie.has_value() &&
        session_cookie.value().size() > MAXIMUM_COOKIE_LENGTH) {
      session_cookie = this->session_cookie(authentication, policy_name,
                                            identity.value().subject, "",
                                            expiry, scope, secure);
    }

    // Without the token there is very little left, so exceeding the limit here
    // takes something extraordinary, such as a provider that identifies people
    // by something enormous. Answering plainly beats handing over a cookie the
    // browser discards, which would look like signing in and then not being
    // signed in, with nothing anywhere to explain it
    if (session_cookie.has_value() &&
        session_cookie.value().size() > MAXIMUM_COOKIE_LENGTH) {
      sourcemeta::one::HTTP_LOG(
          "The provider returned more than a session can hold, for the policy",
          policy_name);
      this->incomplete(silent, transaction.value(), request, response);
      return;
    }

    if (!session_cookie.has_value()) {
      this->incomplete(silent, transaction.value(), request, response);
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
    // Signing in is what earns a browser a silent renewal later, and the
    // marker outlives the session it accompanies because it is only of use
    // once that session has expired
    this->remember_renewal(response, policy_name, scope, secure);
    // The single-use transaction has served its purpose, so it is expired
    // alongside minting the session
    this->expire(response, sourcemeta::one::Authentication::TRANSACTION_COOKIE,
                 scope, secure);
    response.write_header("Location", destination);
    response.write_header("Cache-Control", "no-store");
    sourcemeta::one::send_response(sourcemeta::core::HTTP_STATUS_SEE_OTHER,
                                   request, response);
  }

  auto mcp(const sourcemeta::core::MCPProtocolVersion,
           const sourcemeta::core::JSON &id, const sourcemeta::core::JSON &,
           const sourcemeta::one::Credentials &)
      -> sourcemeta::core::JSON override {
    return sourcemeta::core::jsonrpc_make_error_method_not_found(id);
  }

private:
  // The signature algorithms an identity token may be signed with: every
  // asymmetric one, since a provider picks from these and an instance that
  // named a narrower set would refuse a provider it could otherwise serve,
  // after the person had already signed in. The symmetric ones are left out
  // deliberately. They sign with the client secret rather than a key from the
  // provider's published set, so admitting them alongside the rest is the
  // shape that lets one algorithm be verified as though it were another
  static constexpr std::array<sourcemeta::core::JWSAlgorithm, 10>
      ID_TOKEN_ALGORITHMS{{sourcemeta::core::JWSAlgorithm::RS256,
                           sourcemeta::core::JWSAlgorithm::RS384,
                           sourcemeta::core::JWSAlgorithm::RS512,
                           sourcemeta::core::JWSAlgorithm::PS256,
                           sourcemeta::core::JWSAlgorithm::PS384,
                           sourcemeta::core::JWSAlgorithm::PS512,
                           sourcemeta::core::JWSAlgorithm::ES256,
                           sourcemeta::core::JWSAlgorithm::ES384,
                           sourcemeta::core::JWSAlgorithm::ES512,
                           sourcemeta::core::JWSAlgorithm::EdDSA}};

  // The tolerance allowed on an identity token's time-based claims, matching
  // what a presented access token is already given. A provider whose clock
  // runs a little fast otherwise mints a token this refuses the instant it
  // arrives, which ends a login that did everything right
  static constexpr std::chrono::seconds ID_TOKEN_CLOCK_SKEW{60};

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

    std::vector<std::string_view> candidates;
    request.header_values(
        "cookie", [&candidates](const std::string_view field) -> void {
          sourcemeta::core::http_cookie_values(
              field, sourcemeta::one::Authentication::TRANSACTION_COOKIE,
              candidates);
        });
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

  // The session cookie for a login, carrying the identity token when one is
  // given. Building it is separated out so the caller can measure the result
  // and ask for a smaller one
  [[nodiscard]] auto session_cookie(
      const sourcemeta::one::Authentication &authentication,
      const std::string_view policy_name, const std::string_view subject,
      const std::string_view id_token, const std::chrono::sys_seconds expiry,
      const std::string_view scope, const bool secure) const
      -> std::optional<std::string> {
    auto payload{sourcemeta::core::JSON::make_object()};
    payload.assign_assume_new("policy",
                              sourcemeta::core::JSON{std::string{policy_name}});
    payload.assign_assume_new("subject",
                              sourcemeta::core::JSON{std::string{subject}});
    if (!id_token.empty()) {
      payload.assign_assume_new("id_token",
                                sourcemeta::core::JSON{std::string{id_token}});
    }

    std::ostringstream payload_text;
    sourcemeta::core::stringify(payload, payload_text);
    const auto sealed{authentication.seal(
        policy_name, sourcemeta::one::Authentication::Purpose::Session,
        payload_text.str(), expiry)};
    if (!sealed.has_value()) {
      sourcemeta::one::HTTP_LOG("No session secret is set for the policy",
                                policy_name);
      return std::nullopt;
    }

    auto cookie{sourcemeta::core::http_serialize_cookie(
        {.name = sourcemeta::one::Authentication::SESSION_COOKIE,
         .value = sealed.value(),
         .path = scope,
         .max_age = SESSION_LIFETIME,
         .http_only = true,
         .secure = secure,
         .same_site = sourcemeta::core::HTTPCookieSameSite::Lax})};
    if (!cookie.has_value()) {
      sourcemeta::one::HTTP_LOG("The session could not be put in a cookie, "
                                "for the policy",
                                policy_name);
    }

    return cookie;
  }

  // Every way a callback that belongs to a real login can end without a
  // session. A silent attempt is put back where it started rather than shown
  // any of this, since an error page would be the first anybody knew a renewal
  // had been tried at all. The marker goes with it whatever the reason, so an
  // attempt that did not come back with a grant is not made again on the next
  // navigation, and every navigation after that
  auto abandon(const bool silent, const sourcemeta::core::JSON &transaction,
               sourcemeta::one::HTTPRequest &request,
               sourcemeta::one::HTTPResponse &response,
               const sourcemeta::core::HTTPStatus &status,
               const std::string_view type, const std::string_view detail) const
      -> void {
    if (silent) {
      sourcemeta::one::HTTP_LOG("A silent renewal did not end in a session",
                                detail);
      this->forget_renewal_and_redirect_back(transaction, request, response);
      return;
    }

    this->fail(request, response, status, type, detail);
  }

  auto remember_renewal(sourcemeta::one::HTTPResponse &response,
                        const std::string_view policy_name,
                        const std::string_view scope, const bool secure) const
      -> void {
    const auto cookie{sourcemeta::core::http_serialize_cookie(
        {.name = sourcemeta::one::Authentication::RENEWAL_COOKIE,
         .value = policy_name,
         .path = scope,
         .max_age = RENEWAL_LIFETIME,
         .http_only = true,
         .secure = secure,
         .same_site = sourcemeta::core::HTTPCookieSameSite::Lax})};
    if (cookie.has_value()) {
      response.write_header("Set-Cookie", cookie.value());
    }
  }

  // Where a silent attempt leaves the browser when it did not come back with a
  // grant: back where it was denied, carrying neither a session nor the marker
  // that would send it here again
  auto forget_renewal_and_redirect_back(
      const sourcemeta::core::JSON &transaction,
      sourcemeta::one::HTTPRequest &request,
      sourcemeta::one::HTTPResponse &response) const -> void {
    const auto base{this->server_uri_base_path()};
    const auto scope{base.empty() ? std::string_view{"/"} : base};
    const auto secure{sourcemeta::core::URI{this->server_uri()}.is_https()};
    std::string destination{scope};
    const auto *sealed_destination{transaction.try_at("to")};
    if (sealed_destination != nullptr && sealed_destination->is_string() &&
        sourcemeta::one::is_local_path(sealed_destination->to_string())) {
      destination = sealed_destination->to_string();
    }

    response.write_status(sourcemeta::core::HTTP_STATUS_SEE_OTHER);
    this->expire(response, sourcemeta::one::Authentication::RENEWAL_COOKIE,
                 scope, secure);
    this->expire(response, sourcemeta::one::Authentication::TRANSACTION_COOKIE,
                 scope, secure);
    response.write_header("Location", destination);
    response.write_header("Cache-Control", "no-store");
    sourcemeta::one::send_response(sourcemeta::core::HTTP_STATUS_SEE_OTHER,
                                   request, response);
  }

  auto expire(sourcemeta::one::HTTPResponse &response,
              const std::string_view name, const std::string_view scope,
              const bool secure) const -> void {
    const auto cookie{sourcemeta::core::http_serialize_cookie(
        {.name = name,
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

  // RFC 6749 Section 2.3.1 has every server accept the client secret in an
  // authorization header, and asks that carrying it in the request body be
  // limited to clients that cannot send one. A body is the part of a request
  // that logging and proxies keep, while an authorization header is the part
  // they already know to redact, so the header is used wherever the provider
  // takes it
  [[nodiscard]] auto exchange(
      const std::string_view token_endpoint, const std::string_view client_id,
      const std::string_view client_secret, const std::string_view redirect_uri,
      const std::string_view code, const std::string_view code_verifier,
      const bool basic_auth) const -> std::optional<std::string> {
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
      if (basic_auth) {
        sourcemeta::core::SecureString authorization;
        sourcemeta::core::oauth_client_secret_basic(client_id, client_secret,
                                                    authorization);
        fetch.header("authorization", std::move(authorization));
      } else {
        sourcemeta::core::oauth_client_secret_post(client_id, client_secret,
                                                   body);
      }

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
        {.clock_skew = ID_TOKEN_CLOCK_SKEW}};
  }

  // A failed login leaves its transaction cookie in place, sealed and bound
  // to a state the provider would have to echo, expiring on its own within
  // minutes, so no cookie is cleared here and the error response owns the
  // status line uncontested
  auto fail(sourcemeta::one::HTTPRequest &request,
            sourcemeta::one::HTTPResponse &response,
            const sourcemeta::core::HTTPStatus &status,
            const std::string_view type, const std::string_view detail) const
      -> void {
    sourcemeta::one::json_error(request, response, status, type, detail,
                                this->error_schema_, "*");
  }

  // Every reason a proven callback cannot end in a session answers
  // identically. Anybody may start a login and bring back a code of their own
  // invention, so reaching here proves nothing about who is asking, while
  // telling a secret apart from an unanswering provider apart from a token
  // that would not validate reports how this deployment and its provider are
  // faring. The cause goes to the log, where an operator looks and a caller
  // cannot
  auto incomplete(const bool silent, const sourcemeta::core::JSON &transaction,
                  sourcemeta::one::HTTPRequest &request,
                  sourcemeta::one::HTTPResponse &response) const -> void {
    this->abandon(silent, transaction, request, response,
                  sourcemeta::core::HTTP_STATUS_INTERNAL_SERVER_ERROR,
                  "urn:sourcemeta:one:auth-incomplete",
                  "The session could not be established");
  }

  std::string_view error_schema_;
};

#endif
