#ifndef SOURCEMETA_ONE_ENTERPRISE_SERVER_ACTION_AUTH_LOGOUT_V1_H_
#define SOURCEMETA_ONE_ENTERPRISE_SERVER_ACTION_AUTH_LOGOUT_V1_H_

#include <sourcemeta/core/http.h>
#include <sourcemeta/core/json.h>
#include <sourcemeta/core/jsonrpc.h>
#include <sourcemeta/core/mcp.h>
#include <sourcemeta/core/oidc.h>
#include <sourcemeta/core/uri.h>
#include <sourcemeta/core/uritemplate.h>

#include <sourcemeta/one/authentication.h>
#include <sourcemeta/one/http.h>
#include <sourcemeta/one/router.h>

#include <chrono>      // std::chrono::seconds
#include <filesystem>  // std::filesystem::path
#include <optional>    // std::optional, std::nullopt
#include <span>        // std::span
#include <string>      // std::string
#include <string_view> // std::string_view
#include <vector>      // std::vector

class ActionAuthLogout_v1 : public sourcemeta::one::RouterAction {
public:
  static constexpr std::string_view DESCRIPTION{
      "Clear the browser sessions established through interactive login"};
  static constexpr bool READ_ONLY{false};
  static constexpr bool DESTRUCTIVE{false};
  static constexpr bool IDEMPOTENT{true};
  static constexpr bool OPEN_WORLD{false};

  ActionAuthLogout_v1(
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

  auto rest(const std::span<std::string_view>, std::string_view,
            sourcemeta::one::HTTPRequest &request,
            sourcemeta::one::HTTPResponse &response) -> void override {
    if (request.method() == "options") {
      response.write_status(sourcemeta::core::HTTP_STATUS_NO_CONTENT);
      response.write_header("Cache-Control", "no-store");
      // RFC 9110 §9.3.7: OPTIONS responses SHOULD include Allow
      response.write_header("Allow", "POST, OPTIONS");
      sourcemeta::one::send_response(sourcemeta::core::HTTP_STATUS_NO_CONTENT,
                                     request, response);
      return;
    }

    // Signing out ends a session at the provider, which RFC 9110 Section 9.2.1
    // puts outside what a safe method may do, since the guarantee it gives is
    // what lets a user agent "prefetch, follow links" freely. A link to this
    // would be followed by anything that warms URLs or unfurls them in a chat
    // window, and the person would be signed out by something that was only
    // looking. So the control that reaches it is a form rather than a link.
    // A confirmation page answering the same URL over GET would suit an
    // instance that renders HTML, and is worth adding when there is one
    if (request.method() != "post") {
      sourcemeta::one::json_error(
          request, response, sourcemeta::core::HTTP_STATUS_METHOD_NOT_ALLOWED,
          "urn:sourcemeta:one:method-not-allowed",
          "This HTTP method is invalid for this URL", this->error_schema_, "*",
          "POST, OPTIONS");
      return;
    }

    response.write_status(sourcemeta::core::HTTP_STATUS_SEE_OTHER);

    const auto secure{sourcemeta::core::URI{this->server_uri()}.is_https()};
    const auto base{this->server_uri_base_path()};
    const auto scope{base.empty() ? std::string_view{"/"} : base};

    // Both cookies are expired whether or not the request carried them. A
    // cookie is withheld on plenty of navigations while the browser still
    // holds it, so clearing only what arrived leaves a session behind and
    // tells the person they are signed out. This runs before anything that
    // could fail, so no outcome below can end with the session surviving
    this->expire(response, sourcemeta::one::Authentication::SESSION_COOKIE,
                 scope, secure);
    this->expire(response, sourcemeta::one::Authentication::TRANSACTION_COOKIE,
                 scope, secure);

    // Ending the session here leaves the provider's own untouched, so signing
    // in again would not ask who you are. Where the session names a policy
    // whose provider offers to end it, the browser is sent there to finish the
    // job, carrying the identity token as the proof of whose session it is
    const auto &authentication{this->dispatcher().authentication()};
    const auto elsewhere{this->provider_logout(request, authentication)};
    response.write_header("Location", elsewhere.value_or(std::string{scope}));
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
  auto expire(sourcemeta::one::HTTPResponse &response,
              const std::string_view name, const std::string_view scope,
              const bool secure) const -> void {
    // The attributes mirror the ones the cookie is minted under, scoped to the
    // instance rather than the whole host, so the browser replaces the cookie
    // rather than keeping it alongside a second one of the same name
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

  // Where to send the browser so the provider ends its own session, if the
  // request carried a session this instance minted and the provider offers to
  // end it. Every step that cannot be completed simply yields nothing, since
  // the local session is already gone by the time this runs
  [[nodiscard]] auto
  provider_logout(sourcemeta::one::HTTPRequest &request,
                  const sourcemeta::one::Authentication &authentication) const
      -> std::optional<std::string> {
    std::vector<std::string_view> candidates;
    request.header_values(
        "cookie", [&candidates](const std::string_view field) -> void {
          sourcemeta::core::http_cookie_values(
              field, sourcemeta::one::Authentication::SESSION_COOKIE,
              candidates);
        });
    for (const auto sealed : candidates) {
      const auto payload{authentication.open_session(sealed)};
      if (!payload.has_value()) {
        continue;
      }

      const auto document{sourcemeta::core::try_parse_json(payload.value())};
      if (!document.has_value() || !document.value().is_object()) {
        continue;
      }

      const auto *policy{document.value().try_at("policy")};
      const auto *token{document.value().try_at("id_token")};
      if (policy == nullptr || !policy->is_string()) {
        continue;
      }

      const auto endpoints{authentication.endpoints(policy->to_string())};
      if (!endpoints.has_value() || endpoints.value().end_session.empty()) {
        continue;
      }

      sourcemeta::core::OIDCLogoutRequest logout;
      if (token != nullptr && token->is_string()) {
        logout.id_token_hint = token->to_string();
      }

      // The instance URL already carries whatever base path it is served
      // under, so this is where the provider sends the browser back to
      logout.post_logout_redirect_uri = this->server_uri();
      std::string url;
      sourcemeta::core::oidc_build_logout_url(endpoints.value().end_session,
                                              logout, url);
      return url;
    }

    return std::nullopt;
  }

  std::string_view error_schema_;
};

#endif
