#ifndef SOURCEMETA_ONE_ACTIONS_HEALTH_CHECK_H
#define SOURCEMETA_ONE_ACTIONS_HEALTH_CHECK_H

#include <sourcemeta/core/uritemplate.h>

#include <sourcemeta/one/http.h>

#include <filesystem>  // std::filesystem
#include <span>        // std::span
#include <string>      // std::string
#include <string_view> // std::string_view

class ActionHealthCheck {
public:
  ActionHealthCheck(const std::filesystem::path &,
                    const sourcemeta::core::URITemplateRouterView &router,
                    const sourcemeta::core::URITemplateRouter::Identifier)
      : base_path_{router.base_path()} {}

  auto run(const std::span<std::string_view>,
           sourcemeta::one::HTTPRequest &request,
           sourcemeta::one::HTTPResponse &response) -> void {
    if (request.method() != "get" && request.method() != "head") {
      sourcemeta::one::json_error(
          request, response, sourcemeta::one::STATUS_METHOD_NOT_ALLOWED,
          "method-not-allowed", "This HTTP method is invalid for this URL",
          std::string{this->base_path_} + "/self/v1/schemas/api/error");
      return;
    }

    response.write_status(sourcemeta::one::STATUS_OK);
    response.write_header("Access-Control-Allow-Origin", "*");
    sourcemeta::one::send_response(sourcemeta::one::STATUS_OK, request,
                                   response);
  }

private:
  std::string_view base_path_;
};

#endif
