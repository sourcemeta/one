#ifndef SOURCEMETA_ONE_ACTIONS_AUTH_LOGOUT_V1_H
#define SOURCEMETA_ONE_ACTIONS_AUTH_LOGOUT_V1_H

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

class ActionAuthLogoutV1 : public sourcemeta::one::RouterAction {
public:
  static constexpr std::string_view DESCRIPTION{
      "Clear the browser sessions established through interactive login"};
  static constexpr bool READ_ONLY{false};
  static constexpr bool DESTRUCTIVE{false};
  static constexpr bool IDEMPOTENT{true};
  static constexpr bool OPEN_WORLD{false};

  ActionAuthLogoutV1(
      const std::filesystem::path &base,
      const sourcemeta::core::URITemplateRouterView &router,
      const sourcemeta::core::URITemplateRouter::Identifier identifier,
      sourcemeta::one::Router &dispatcher)
      : sourcemeta::one::RouterAction{base, router.base_url(), dispatcher} {
    router.arguments(
        identifier, [this](const auto &key, const auto &value) -> void {
          if (key == "errorSchema") {
            this->error_schema_ = std::get<std::string_view>(value);
          }
        });
  }

  auto rest(const std::span<std::string_view>,
            const sourcemeta::one::Authentication::Caller &,
            sourcemeta::one::HTTPRequest &request,
            sourcemeta::one::HTTPResponse &response) -> void override {
    if (request.method() == "options") {
      response.write_status(sourcemeta::core::HTTP_STATUS_NO_CONTENT);
      response.write_header("Cache-Control",
                            sourcemeta::one::cache_control_no_store());
      response.write_header("Allow", "POST, OPTIONS");
      sourcemeta::one::send_response(sourcemeta::core::HTTP_STATUS_NO_CONTENT,
                                     request, response);
      return;
    }

    // Which method this answers is part of the contract both editions present,
    // so anything other than a POST is refused here exactly as the edition
    // that implements signing out refuses it. Where the two differ is only in
    // what a POST then gets
    if (request.method() != "post") {
      sourcemeta::one::json_error(
          request, response, sourcemeta::core::HTTP_STATUS_METHOD_NOT_ALLOWED,
          "urn:sourcemeta:one:method-not-allowed",
          "This HTTP method is invalid for this URL", this->error_schema_, "*",
          "POST, OPTIONS");
      return;
    }

    sourcemeta::one::json_error(
        request, response, sourcemeta::core::HTTP_STATUS_FORBIDDEN,
        "urn:sourcemeta:one:enterprise-required",
        "This feature is only available in the Enterprise edition",
        this->error_schema_, "*");
  }

  auto mcp(const sourcemeta::core::MCPProtocolVersion,
           const sourcemeta::core::JSON &request_id,
           const sourcemeta::core::JSON &,
           const sourcemeta::one::Authentication::Caller &)
      -> sourcemeta::core::JSON override {
    return sourcemeta::core::jsonrpc_make_error_method_not_found(request_id);
  }

private:
  std::string_view error_schema_;
};

#endif

#endif
