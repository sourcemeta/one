#ifndef SOURCEMETA_ONE_ACTIONS_SERVE_STATIC_V1_H
#define SOURCEMETA_ONE_ACTIONS_SERVE_STATIC_V1_H

#include <sourcemeta/core/json.h>
#include <sourcemeta/core/jsonrpc.h>
#include <sourcemeta/core/mcp.h>
#include <sourcemeta/core/uritemplate.h>

#include <sourcemeta/one/http.h>
#include <sourcemeta/one/router.h>

#include <filesystem>  // std::filesystem
#include <span>        // std::span
#include <string>      // std::string
#include <string_view> // std::string_view

class ActionServeStaticV1 : public sourcemeta::one::RouterAction {
public:
  static constexpr std::string_view DESCRIPTION{
      "Serve a static asset bundled with the server"};
  static constexpr bool READ_ONLY{true};
  static constexpr bool DESTRUCTIVE{false};
  static constexpr bool IDEMPOTENT{true};
  static constexpr bool OPEN_WORLD{false};

  ActionServeStaticV1(
      const std::filesystem::path &base,
      const sourcemeta::core::URITemplateRouterView &router,
      const sourcemeta::core::URITemplateRouter::Identifier identifier,
      sourcemeta::one::Router &dispatcher)
      : sourcemeta::one::RouterAction{base, router.base_url(), dispatcher} {
    router.arguments(
        identifier, [this](const auto &key, const auto &value) -> void {
          if (key == "path") {
            this->file_root_ = std::get<std::string_view>(value);
          } else if (key == "errorSchema") {
            this->error_schema_ = std::get<std::string_view>(value);
          }
        });
  }

  auto rest(const std::span<std::string_view> matches,
            const sourcemeta::one::Authentication::Caller &,
            sourcemeta::one::HTTPRequest &request,
            sourcemeta::one::HTTPResponse &response) -> void override {
    if (request.method() == "options") {
      sourcemeta::one::cors_preflight(request, response, "GET, HEAD, OPTIONS",
                                      "Accept-Encoding, If-None-Match, "
                                      "If-Modified-Since");
      return;
    }
    if (this->file_root_.empty()) {
      if (request.method() != "get" && request.method() != "head") {
        sourcemeta::one::json_error(
            request, response, sourcemeta::core::HTTP_STATUS_METHOD_NOT_ALLOWED,
            "urn:sourcemeta:one:method-not-allowed",
            "This HTTP method is invalid for this URL", this->error_schema_,
            "*", "GET, HEAD, OPTIONS");
        return;
      }

      sourcemeta::one::json_error(
          request, response, sourcemeta::core::HTTP_STATUS_NOT_FOUND,
          "urn:sourcemeta:one:not-found", "There is nothing at this URL",
          this->error_schema_, "*");
      return;
    }

    const auto resolution{
        this->artifact_resolve_static(this->file_root_, matches.front())};
    if (!resolution.path.has_value()) {
      // A path that escapes the static asset tree behaves exactly like
      // a missing file, including the method check coming first
      if (request.method() != "get" && request.method() != "head") {
        sourcemeta::one::json_error(
            request, response, sourcemeta::core::HTTP_STATUS_METHOD_NOT_ALLOWED,
            "urn:sourcemeta:one:method-not-allowed",
            "This HTTP method is invalid for this URL", this->error_schema_, "",
            "GET, HEAD, OPTIONS");
        return;
      }
      sourcemeta::one::json_error(
          request, response, sourcemeta::core::HTTP_STATUS_NOT_FOUND,
          "urn:sourcemeta:one:not-found", "There is nothing at this URL",
          this->error_schema_, "");
      return;
    }

    // RFC 8246 §2: static assets are deploy-pinned (the build emits
    // them under a versioned URL), so a year-long `max-age` with the
    // `immutable` extension lets browsers skip the conditional GET
    // entirely.
    //
    // A policy may gate this tree like any other, and a gated answer is one
    // caller's rather than everybody's, so the year only applies where the
    // location is open to all. That is asked of the route as the router
    // matched it, and of nobody in particular, since what a shared cache may
    // hand to the next caller is what anybody would have been given
    const auto &authentication{this->dispatcher().authentication()};
    const auto is_public{authentication.permits(
        sourcemeta::one::Authentication::RouteTarget{request.path()},
        authentication.caller({}))};
    this->artifact_serve(resolution.path.value(),
                         sourcemeta::core::HTTP_STATUS_OK, false, {}, {}, {},
                         request, response, this->error_schema_,
                         sourcemeta::one::cache_control_immutable(is_public),
                         sourcemeta::one::vary_encoding());
  }

  auto mcp(const sourcemeta::core::MCPProtocolVersion,
           const sourcemeta::core::JSON &request_id,
           const sourcemeta::core::JSON &,
           const sourcemeta::one::Authentication::Caller &)
      -> sourcemeta::core::JSON override {
    return sourcemeta::core::jsonrpc_make_error_method_not_found(request_id);
  }

private:
  std::filesystem::path file_root_;
  std::string_view error_schema_;
};

#endif
