#ifndef SOURCEMETA_ONE_ENTERPRISE_SERVER_ACTION_AUTH_LOGIN_PAGE_V1_H_
#define SOURCEMETA_ONE_ENTERPRISE_SERVER_ACTION_AUTH_LOGIN_PAGE_V1_H_

#include <sourcemeta/core/http.h>
#include <sourcemeta/core/json.h>
#include <sourcemeta/core/jsonrpc.h>
#include <sourcemeta/core/mcp.h>
#include <sourcemeta/core/uritemplate.h>

#include <sourcemeta/one/http.h>
#include <sourcemeta/one/router.h>

#include <filesystem>  // std::filesystem::path
#include <optional>    // std::optional
#include <span>        // std::span
#include <string_view> // std::string_view
#include <utility>     // std::move

class ActionAuthLoginPage_v1 : public sourcemeta::one::RouterAction {
public:
  static constexpr std::string_view DESCRIPTION{
      "Show the ways of signing in to this instance"};
  static constexpr bool READ_ONLY{true};
  static constexpr bool DESTRUCTIVE{false};
  static constexpr bool IDEMPOTENT{true};
  static constexpr bool OPEN_WORLD{false};

  ActionAuthLoginPage_v1(
      const std::filesystem::path &base,
      const sourcemeta::core::URITemplateRouterView &router,
      const sourcemeta::core::URITemplateRouter::Identifier identifier,
      sourcemeta::one::Router &dispatcher)
      : sourcemeta::one::RouterAction{base, router.base_url(), dispatcher} {
    router.arguments(
        identifier, [this](const auto &key, const auto &value) -> void {
          if (key == "errorSchema") {
            this->error_schema_ = std::get<std::string_view>(value);
          } else if (key == "responseSchema") {
            this->response_schema_ = std::get<std::string_view>(value);
          }
        });

    // Signing in is what a caller without a session does, so what is offered is
    // the anonymous answer for everybody, and it says the same thing for as
    // long as the instance stands. The data is what an instance always holds
    // and the page is what it holds when it renders any at all
    this->document_ = this->artifact_resolve_path_unauthenticated(
        sourcemeta::one::VIEW_PUBLIC, "", Tree::Explorer, "login");
    this->page_ = this->artifact_resolve_path_unauthenticated(
        sourcemeta::one::VIEW_PUBLIC, "", Tree::Explorer, "login-html");
  }

  [[nodiscard]] auto is_authentication_exempt() const noexcept
      -> bool override {
    return true;
  }

  auto rest(const std::span<std::string_view>,
            const sourcemeta::one::Authentication::Caller &,
            sourcemeta::one::HTTPRequest &request,
            sourcemeta::one::HTTPResponse &response) -> void override {
    // The data representation is cross-origin readable, so the preflight has to
    // grant what the request that follows it needs. A page is a navigation and
    // never asks, but an interface fetching the data from elsewhere does
    if (request.method() == "options") {
      sourcemeta::one::cors_preflight(request, response, "GET, HEAD, OPTIONS",
                                      "Accept-Encoding, If-None-Match, "
                                      "If-Modified-Since");
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

    // A browser is shown the page and everything else is handed the same answer
    // as data. An instance that renders nothing has only the data, so asking
    // for a page there is answered with what there is rather than with nothing
    if (sourcemeta::one::prefers_html(request.header("accept")) &&
        this->page_.has_value()) {
      this->artifact_serve(
          this->page_.value(), sourcemeta::core::HTTP_STATUS_OK, false, {}, {},
          HTML_BROWSER_SECURITY, request, response, this->error_schema_,
          "public, max-age=0, must-revalidate", "Accept, Accept-Encoding");
      return;
    }

    if (!this->document_.has_value()) {
      sourcemeta::one::json_error(
          request, response, sourcemeta::core::HTTP_STATUS_NOT_FOUND,
          "urn:sourcemeta:one:not-found", "There is nothing at this URL",
          this->error_schema_, "*");
      return;
    }

    // The one answer every caller is given, so a shared cache holding it hands
    // the next caller what they would have been given anyway
    this->artifact_serve(
        this->document_.value(), sourcemeta::core::HTTP_STATUS_OK, true, {},
        this->response_schema_, {}, request, response, this->error_schema_,
        "public, max-age=0, must-revalidate", "Accept, Accept-Encoding");
  }

  auto mcp(const sourcemeta::core::MCPProtocolVersion,
           const sourcemeta::core::JSON &id, const sourcemeta::core::JSON &,
           const sourcemeta::one::Authentication::Caller &)
      -> sourcemeta::core::JSON override {
    return sourcemeta::core::jsonrpc_make_error_method_not_found(id);
  }

private:
  std::optional<sourcemeta::one::ResolvedArtifact> document_;
  std::optional<sourcemeta::one::ResolvedArtifact> page_;
  std::string_view error_schema_;
  std::string_view response_schema_;
};

#endif
