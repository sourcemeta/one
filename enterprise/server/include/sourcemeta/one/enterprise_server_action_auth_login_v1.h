#ifndef SOURCEMETA_ONE_ENTERPRISE_SERVER_ACTION_AUTH_LOGIN_V1_H_
#define SOURCEMETA_ONE_ENTERPRISE_SERVER_ACTION_AUTH_LOGIN_V1_H_

#include <sourcemeta/core/http.h>
#include <sourcemeta/core/json.h>
#include <sourcemeta/core/jsonrpc.h>
#include <sourcemeta/core/mcp.h>
#include <sourcemeta/core/uritemplate.h>

#include <sourcemeta/one/authentication.h>
#include <sourcemeta/one/http.h>
#include <sourcemeta/one/router.h>

#include <chrono>      // std::chrono::seconds, std::chrono::system_clock
#include <cstdlib>     // std::getenv
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

    // NOLINTNEXTLINE(concurrency-mt-unsafe)
    const char *client_secret{
        std::getenv(std::string{policy->client_secret_variable}.c_str())};
    if (client_secret == nullptr) {
      sourcemeta::one::json_error(
          request, response,
          sourcemeta::core::HTTP_STATUS_INTERNAL_SERVER_ERROR,
          "urn:sourcemeta:one:auth-misconfigured",
          "The authentication configuration is incomplete", this->error_schema_,
          "*");
      return;
    }

    const auto authorization_endpoint{
        this->discover_authorization_endpoint(policy->issuer)};
    if (!authorization_endpoint.has_value()) {
      sourcemeta::one::json_error(
          request, response, sourcemeta::core::HTTP_STATUS_BAD_GATEWAY,
          "urn:sourcemeta:one:auth-provider-unreachable",
          "The identity provider could not be reached", this->error_schema_,
          "*");
      return;
    }

    const auto transaction{sourcemeta::one::oidc_transaction()};

    auto payload{sourcemeta::core::JSON::make_object()};
    payload.assign_assume_new("policy",
                              sourcemeta::core::JSON{std::string{policy_name}});
    payload.assign_assume_new("state",
                              sourcemeta::core::JSON{transaction.state});
    payload.assign_assume_new("nonce",
                              sourcemeta::core::JSON{transaction.nonce});
    payload.assign_assume_new(
        "verifier", sourcemeta::core::JSON{transaction.code_verifier});
    // The return target lets the login send the browser back to the page it
    // was denied. The query value arrives already percent-decoded, so it is
    // taken as-is, and only a same-origin local path is honoured, so the login
    // cannot be turned into an open redirect. Anything else falls back to what
    // the policy governs. It is sealed into the transaction so that the
    // callback trusts it
    const auto destination{request.query("to")};
    if (!destination.empty() && sourcemeta::one::is_local_path(destination)) {
      payload.assign_assume_new(
          "to", sourcemeta::core::JSON{std::string{destination}});
    } else if (!policy->default_path.empty()) {
      std::string fallback{this->server_uri_base_path()};
      fallback += policy->default_path;
      payload.assign_assume_new("to", sourcemeta::core::JSON{fallback});
    }
    std::ostringstream payload_text;
    sourcemeta::core::stringify(payload, payload_text);

    const auto expiry{std::chrono::time_point_cast<std::chrono::seconds>(
                          std::chrono::system_clock::now()) +
                      TRANSACTION_LIFETIME};
    const auto sealed{
        authentication.seal(policy_name, payload_text.str(), expiry)};
    if (!sealed.has_value()) {
      sourcemeta::one::json_error(
          request, response,
          sourcemeta::core::HTTP_STATUS_INTERNAL_SERVER_ERROR,
          "urn:sourcemeta:one:auth-misconfigured",
          "The authentication configuration is incomplete", this->error_schema_,
          "*");
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

    const auto url{
        sourcemeta::one::oidc_authorization_url(authorization_endpoint.value(),
                                                {.client_id = policy->client_id,
                                                 .client_secret = client_secret,
                                                 .redirect_uri = redirect_uri},
                                                "openid", transaction)};

    std::string cookie_name;
    cookie_name.reserve(sourcemeta::one::TRANSACTION_COOKIE_PREFIX.size() +
                        policy_name.size());
    cookie_name += sourcemeta::one::TRANSACTION_COOKIE_PREFIX;
    cookie_name += policy_name;
    const auto base{this->server_uri_base_path()};
    const auto scope{base.empty() ? std::string_view{"/"} : base};
    const auto cookie{sourcemeta::core::http_serialize_cookie(
        {.name = cookie_name,
         .value = sealed.value(),
         .path = scope,
         .max_age = TRANSACTION_LIFETIME,
         .http_only = true,
         .secure = this->server_uri().starts_with("https"),
         .same_site = sourcemeta::core::HTTPCookieSameSite::Lax})};
    // A redirect without the transaction cookie could never complete at the
    // callback, so it is not worth sending
    if (!cookie.has_value()) {
      sourcemeta::one::json_error(
          request, response,
          sourcemeta::core::HTTP_STATUS_INTERNAL_SERVER_ERROR,
          "urn:sourcemeta:one:auth-misconfigured",
          "The authentication configuration is incomplete", this->error_schema_,
          "*");
      return;
    }

    response.write_status(sourcemeta::core::HTTP_STATUS_SEE_OTHER);
    response.write_header("Set-Cookie", cookie.value());
    response.write_header("Location", url);
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
  [[nodiscard]] auto
  discover_authorization_endpoint(const std::string_view issuer) const
      -> std::optional<std::string> {
    try {
      sourcemeta::core::HTTPSystemRequest fetch{
          sourcemeta::one::discovery_url(issuer)};
      fetch.connect_timeout(std::chrono::seconds{2});
      fetch.timeout(std::chrono::seconds{5});
      fetch.maximum_response_size(1024UL * 1024UL);
      fetch.follow_redirects(false);
      const auto result{fetch.send()};
      if (result.status.code < 200 || result.status.code >= 300) {
        return std::nullopt;
      }

      auto document{sourcemeta::one::discovery_parse(result.body)};
      if (!document.has_value() ||
          !document.value().authorization_endpoint.has_value()) {
        return std::nullopt;
      }

      return std::move(document.value().authorization_endpoint);
    } catch (...) {
      return std::nullopt;
    }
  }

  std::string_view error_schema_;
};

#endif
