#ifndef SOURCEMETA_ONE_ENTERPRISE_SERVER_ACTION_AUTH_LOGOUT_V1_H_
#define SOURCEMETA_ONE_ENTERPRISE_SERVER_ACTION_AUTH_LOGOUT_V1_H_

#include <sourcemeta/core/http.h>
#include <sourcemeta/core/json.h>
#include <sourcemeta/core/jsonrpc.h>
#include <sourcemeta/core/mcp.h>
#include <sourcemeta/core/uritemplate.h>

#include <sourcemeta/one/authentication.h>
#include <sourcemeta/one/http.h>
#include <sourcemeta/one/router.h>

#include <chrono>      // std::chrono::seconds
#include <filesystem>  // std::filesystem::path
#include <span>        // std::span
#include <string_view> // std::string_view

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

    response.write_status(sourcemeta::core::HTTP_STATUS_SEE_OTHER);

    // Logout is local: expire every session and login-transaction cookie the
    // request carries, and there is no server-side state to destroy. The
    // attributes must mirror the ones the cookies are minted under, scoped to
    // the instance rather than the whole host, so the browser replaces the
    // cookies rather than shadowing them
    const auto secure{this->server_uri().starts_with("https")};
    const auto base{this->server_uri_base_path()};
    const auto scope{base.empty() ? std::string_view{"/"} : base};
    sourcemeta::core::http_parse_cookies(
        request.header("cookie"),
        [&response, secure, scope](const std::string_view name,
                                   const std::string_view) -> void {
          if (!name.starts_with(sourcemeta::one::SESSION_COOKIE_PREFIX) &&
              !name.starts_with(sourcemeta::one::TRANSACTION_COOKIE_PREFIX)) {
            return;
          }

          // A name that does not serialise back was never minted here, so it
          // is left alone
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
        });

    response.write_header("Location", scope);
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
  std::string_view error_schema_;
};

#endif
