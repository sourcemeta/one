#ifndef SOURCEMETA_ONE_ACTIONS_HEALTH_CHECK_V1_H
#define SOURCEMETA_ONE_ACTIONS_HEALTH_CHECK_V1_H

#include <sourcemeta/core/uritemplate.h>

#include <sourcemeta/one/actions.h>
#include <sourcemeta/one/http.h>

#include "mcp.h"

#include <filesystem>  // std::filesystem
#include <span>        // std::span
#include <string>      // std::string
#include <string_view> // std::string_view

class ActionHealthCheck_v1 : public sourcemeta::one::Action {
public:
  ActionHealthCheck_v1(
      const std::filesystem::path &base,
      const sourcemeta::core::URITemplateRouterView &router,
      const sourcemeta::core::URITemplateRouter::Identifier identifier)
      : sourcemeta::one::Action{base, router.base_path()} {
    router.arguments(identifier, [this](const auto &key, const auto &value) {
      if (key == "errorSchema") {
        this->error_schema_ = std::get<std::string_view>(value);
      }
    });
  }

  auto run(const std::span<std::string_view>,
           sourcemeta::one::HTTPRequest &request,
           sourcemeta::one::HTTPResponse &response) -> void override {
    if (request.method() != "get" && request.method() != "head") {
      sourcemeta::one::json_error(
          request, response, sourcemeta::one::STATUS_METHOD_NOT_ALLOWED,
          "method-not-allowed", "This HTTP method is invalid for this URL",
          this->error_schema_);
      return;
    }

    response.write_status(sourcemeta::one::STATUS_OK);
    response.write_header("Access-Control-Allow-Origin", "*");
    sourcemeta::one::send_response(sourcemeta::one::STATUS_OK, request,
                                   response);
  }

  auto mcp(const sourcemeta::core::JSON &) -> sourcemeta::core::JSON override {
    return sourcemeta::one::mcp_empty();
  }

private:
  std::string_view error_schema_;
};

#endif
