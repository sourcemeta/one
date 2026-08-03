#ifndef SOURCEMETA_ONE_ENTERPRISE_SERVER_ACTION_AUTH_LOGIN_V1_H_
#define SOURCEMETA_ONE_ENTERPRISE_SERVER_ACTION_AUTH_LOGIN_V1_H_

#include <sourcemeta/core/http.h>
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

#include <chrono>      // std::chrono::seconds, std::chrono::system_clock
#include <filesystem>  // std::filesystem::path
#include <optional>    // std::optional, std::nullopt
#include <span>        // std::span
#include <sstream>     // std::ostringstream
#include <string>      // std::string
#include <string_view> // std::string_view
#include <utility>     // std::move

class ActionAuthLogin_v1 : public sourcemeta::one::RouterAction {
public:
  static constexpr std::string_view DESCRIPTION{
      "Start an interactive login by redirecting the browser to the identity "
      "provider"};
  static constexpr bool READ_ONLY{false};
  static constexpr bool DESTRUCTIVE{false};
  static constexpr bool IDEMPOTENT{true};
  static constexpr bool OPEN_WORLD{false};

  // How long a browser has to complete a login at the provider before the
  // transaction expires
  static constexpr std::chrono::seconds TRANSACTION_LIFETIME{600};

  ActionAuthLogin_v1(
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

    // An unknown or non-interactive policy name reveals nothing
    const auto policy_name{matches.front()};
    const auto &authentication{this->dispatcher().authentication()};
    const auto policy{authentication.interactive(policy_name)};
    if (!policy.has_value()) {
      sourcemeta::one::json_error(
          request, response, sourcemeta::core::HTTP_STATUS_NOT_FOUND,
          "urn:sourcemeta:one:not-found", "There is nothing at this URL",
          this->error_schema_, "*");
      return;
    }

    // Starting a login that cannot be completed only strands the person at the
    // provider, so the secret the exchange will need is required up front
    if (!authentication.client_secret(policy_name).has_value()) {
      sourcemeta::one::HTTP_LOG("No client secret is set for the policy",
                                policy_name);
      this->unavailable(request, response);
      return;
    }

    const auto endpoints{authentication.endpoints(policy_name)};
    if (!endpoints.has_value() || endpoints.value().authorization.empty()) {
      sourcemeta::one::HTTP_LOG("The provider named no authorization endpoint, "
                                "or could not be reached, for the policy",
                                policy_name);
      this->unavailable(request, response);
      return;
    }

    const auto secrets{sourcemeta::core::oauth_transaction_mint()};
    const std::string_view state{secrets.state.data(), secrets.state.size()};
    const std::string_view verifier{secrets.code_verifier.data(),
                                    secrets.code_verifier.size()};
    const auto nonce_token{sourcemeta::core::oidc_nonce()};
    const std::string_view nonce{nonce_token.data(), nonce_token.size()};

    // A silent attempt asks the provider whether an existing sign-in still
    // stands, and is answered either way without the person seeing anything.
    // The callback has to know which kind it is completing, since a provider
    // refusing to answer without interaction is an ordinary outcome here and a
    // failure anywhere else
    const auto silent{!request.query("silent").empty()};

    auto payload{sourcemeta::core::JSON::make_object()};
    payload.assign_assume_new("policy",
                              sourcemeta::core::JSON{std::string{policy_name}});
    if (silent) {
      payload.assign_assume_new("silent", sourcemeta::core::JSON{true});
    }
    payload.assign_assume_new("state", sourcemeta::core::JSON{state});
    payload.assign_assume_new("nonce", sourcemeta::core::JSON{nonce});
    payload.assign_assume_new("verifier", sourcemeta::core::JSON{verifier});
    // The return target lets the login send the browser back to the page it
    // was denied. An explicit `to` query wins, then the referring page, which
    // for a login served in place is exactly that denied page and so needs no
    // query, then what the policy governs. Only a same-origin local path is
    // ever honoured, so the login cannot be turned into an open redirect
    std::optional<std::string> target;
    const auto destination{request.query("to")};
    if (!destination.empty() && sourcemeta::one::is_local_path(destination)) {
      target = std::string{destination};
    } else if (const auto referer{request.header("referer")};
               referer.starts_with(this->server_uri())) {
      std::string candidate{this->server_uri_base_path()};
      candidate += referer.substr(this->server_uri().size());
      if (sourcemeta::one::is_local_path(candidate)) {
        target = std::move(candidate);
      }
    }
    if (!target.has_value() && !policy->default_path.empty()) {
      std::string fallback{this->server_uri_base_path()};
      fallback += policy->default_path;
      target = std::move(fallback);
    }
    if (target.has_value()) {
      payload.assign_assume_new("to", sourcemeta::core::JSON{target.value()});
    }
    std::ostringstream payload_text;
    sourcemeta::core::stringify(payload, payload_text);

    const auto expiry{std::chrono::time_point_cast<std::chrono::seconds>(
                          std::chrono::system_clock::now()) +
                      TRANSACTION_LIFETIME};
    const auto sealed{authentication.seal(
        policy_name, sourcemeta::one::Authentication::Purpose::Transaction,
        payload_text.str(), expiry)};
    if (!sealed.has_value()) {
      sourcemeta::one::HTTP_LOG("No session secret is set for the policy",
                                policy_name);
      this->unavailable(request, response);
      return;
    }

    // The callback URL is pinned from the configured public URL, never
    // inferred from the incoming request
    constexpr std::string_view CALLBACK_PATH{"/self/v1/auth/callback/"};
    std::string redirect_uri;
    redirect_uri.reserve(this->server_uri().size() + CALLBACK_PATH.size() +
                         policy_name.size());
    redirect_uri += this->server_uri();
    redirect_uri += CALLBACK_PATH;
    redirect_uri += policy_name;

    // A provider sends only the claims a request asks for, so a policy whose
    // rules name any is asked for them here. The standard way is the claims
    // request parameter, and where a provider does not honour that, the scopes
    // that carry the standard claims are the fallback. A claim no standard
    // scope carries is then arranged at the provider instead, since inventing
    // a scope name risks a request refused outright
    // The rules outlive every request built from them, since a claim request
    // names its claim by pointing into them rather than copying
    const auto rules{policy->claims.empty()
                         ? std::optional<sourcemeta::core::JSON>{std::nullopt}
                         : sourcemeta::core::try_parse_json(policy->claims)};
    const auto wanted{this->wanted_claims(policy.value(), rules)};
    std::string scope_request;
    std::string claims_parameter;
    if (endpoints.value().claims_parameter_supported && !wanted.empty()) {
      std::ostringstream text;
      sourcemeta::core::stringify(
          sourcemeta::core::oidc_build_claims_parameter({}, wanted), text);
      claims_parameter = text.str();
    }

    this->requested_scope(wanted, scope_request);

    const auto challenge{sourcemeta::core::oauth_pkce_challenge(verifier)};
    sourcemeta::core::OIDCAuthenticationRequest authentication_request{};
    authentication_request.client_id = policy->client_id;
    authentication_request.redirect_uri = redirect_uri;
    authentication_request.scope = scope_request;
    authentication_request.claims = claims_parameter;
    authentication_request.response_type = "code";
    authentication_request.state = state;
    authentication_request.code_challenge = {challenge.data(),
                                             challenge.size()};
    authentication_request.code_challenge_method = "S256";
    authentication_request.nonce = nonce;
    if (silent) {
      authentication_request.prompt = "none";
    }

    std::string authorization_url;
    const auto url{sourcemeta::core::oidc_build_authentication_url(
                       endpoints.value().authorization, authentication_request,
                       authorization_url)
                       ? std::optional<std::string>{authorization_url}
                       : std::nullopt};
    if (!url.has_value()) {
      sourcemeta::one::HTTP_LOG("The authorization endpoint is not a URL a "
                                "request can be built against, for the policy",
                                policy_name);
      this->unavailable(request, response);
      return;
    }

    const auto base{this->server_uri_base_path()};
    const auto scope{base.empty() ? std::string_view{"/"} : base};
    const auto cookie{sourcemeta::core::http_serialize_cookie(
        {.name = sourcemeta::one::Authentication::TRANSACTION_COOKIE,
         .value = sealed.value(),
         .path = scope,
         .max_age = TRANSACTION_LIFETIME,
         .http_only = true,
         .secure = sourcemeta::core::URI{this->server_uri()}.is_https(),
         .same_site = sourcemeta::core::HTTPCookieSameSite::Lax})};
    // A redirect without the transaction cookie could never complete at the
    // callback, so it is not worth sending
    if (!cookie.has_value()) {
      sourcemeta::one::HTTP_LOG(
          "The login transaction could not be put in a cookie, for the policy",
          policy_name);
      this->unavailable(request, response);
      return;
    }

    response.write_status(sourcemeta::core::HTTP_STATUS_SEE_OTHER);
    response.write_header("Set-Cookie", cookie.value());
    response.write_header("Location", url.value());
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
  // The claims a policy's rules speak about, each asked for as essential, so
  // that a provider is told what is actually needed rather than being left to
  // guess from a scope. A domain rule reads an address, so it asks for the
  // pair OpenID Connect Core Section 5.1 defines for one
  [[nodiscard]] static auto wanted_claims(
      const sourcemeta::one::Authentication::InteractivePolicy &policy,
      const std::optional<sourcemeta::core::JSON> &rules)
      -> std::vector<sourcemeta::core::OIDCClaimRequest> {
    std::vector<sourcemeta::core::OIDCClaimRequest> result;
    if (!policy.email_domains.empty()) {
      result.push_back({.name = "email", .essential = true});
      result.push_back({.name = "email_verified", .essential = true});
    }

    if (!rules.has_value() || !rules.value().is_object()) {
      return result;
    }

    for (const auto &rule : rules.value().as_object()) {
      result.push_back({.name = rule.first, .essential = true});
    }

    return result;
  }

  // The scope a login asks for. Every request carries `openid`, and a claim
  // one of the standard scopes carries adds that scope, which is the only
  // mapping a specification defines. A claim outside them adds nothing, since
  // a scope this invented could be refused outright by the provider
  static auto
  requested_scope(const std::vector<sourcemeta::core::OIDCClaimRequest> &wanted,
                  std::string &sink) -> void {
    sink += "openid";
    std::vector<std::string_view> scopes;
    for (const auto &claim : wanted) {
      const auto scope{sourcemeta::core::oidc_claim_to_scope(claim.name)};
      if (scope.has_value() && scope.value() != "openid" &&
          std::ranges::find(scopes, scope.value()) == scopes.cend()) {
        scopes.push_back(scope.value());
      }
    }

    std::ranges::sort(scopes);
    for (const auto scope : scopes) {
      sink += " ";
      sink += scope;
    }
  }

  // Every reason a login cannot start answers identically. The login page names
  // its policies to anybody who reaches a gated path, so which policies exist
  // is published rather than secret, but whether one is misconfigured and
  // whether its provider is answering are neither, and telling those apart
  // hands a caller who has authenticated to nothing a view of how this
  // deployment is doing. The cause goes to the log, where an operator looks and
  // a caller cannot
  auto unavailable(sourcemeta::one::HTTPRequest &request,
                   sourcemeta::one::HTTPResponse &response) const -> void {
    sourcemeta::one::json_error(
        request, response, sourcemeta::core::HTTP_STATUS_INTERNAL_SERVER_ERROR,
        "urn:sourcemeta:one:auth-unavailable", "This login cannot be started",
        this->error_schema_, "*");
  }

  std::string_view error_schema_;
};

#endif
