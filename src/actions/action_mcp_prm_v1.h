#ifndef SOURCEMETA_ONE_ACTIONS_MCP_PRM_V1_H
#define SOURCEMETA_ONE_ACTIONS_MCP_PRM_V1_H

#if defined(SOURCEMETA_ONE_ENTERPRISE)

#include <sourcemeta/one/enterprise_server.h>

#else

#include <sourcemeta/core/http.h>
#include <sourcemeta/core/json.h>
#include <sourcemeta/core/jsonrpc.h>
#include <sourcemeta/core/mcp.h>
#include <sourcemeta/core/uritemplate.h>

#include <sourcemeta/one/http.h>
#include <sourcemeta/one/router.h>

#include <filesystem>  // std::filesystem::path
#include <span>        // std::span
#include <string_view> // std::string_view

class ActionMCPProtectedResourceMetadata_v1
    : public sourcemeta::one::RouterAction {
public:
  static constexpr std::string_view DESCRIPTION{
      "Report where a token for the Model Context Protocol endpoint comes "
      "from"};
  static constexpr bool READ_ONLY{true};
  static constexpr bool DESTRUCTIVE{false};
  static constexpr bool IDEMPOTENT{true};
  static constexpr bool OPEN_WORLD{false};

  ActionMCPProtectedResourceMetadata_v1(
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
  }

  [[nodiscard]] auto is_authentication_exempt() const noexcept
      -> bool override {
    return true;
  }

  auto rest(const std::span<std::string_view>, std::string_view,
            std::string_view, sourcemeta::one::HTTPRequest &request,
            sourcemeta::one::HTTPResponse &response) -> void override {
    if (request.method() == "options") {
      sourcemeta::one::cors_preflight(request, response, "GET, HEAD, OPTIONS",
                                      "Accept, Accept-Encoding");
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

    sourcemeta::one::json_error(
        request, response, sourcemeta::core::HTTP_STATUS_FORBIDDEN,
        "urn:sourcemeta:one:enterprise-required",
        "This feature is only available in the Enterprise edition",
        this->error_schema_, "*");
  }

  auto mcp(const sourcemeta::core::MCPProtocolVersion,
           const sourcemeta::core::JSON &id, const sourcemeta::core::JSON &,
           const sourcemeta::one::Credentials &)
      -> sourcemeta::core::JSON override {
    return sourcemeta::core::jsonrpc_make_error_method_not_found(id);
  }

private:
  std::string_view error_schema_;
  std::string_view response_schema_;
};

#endif

#endif
