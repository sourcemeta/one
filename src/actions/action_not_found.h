#ifndef SOURCEMETA_ONE_ACTIONS_NOT_FOUND_H
#define SOURCEMETA_ONE_ACTIONS_NOT_FOUND_H

#include <sourcemeta/core/uritemplate.h>

#include <sourcemeta/one/http.h>

#include <filesystem>  // std::filesystem
#include <span>        // std::span
#include <string>      // std::string
#include <string_view> // std::string_view

class ActionNotFound {
public:
  ActionNotFound(const std::filesystem::path &,
                 const sourcemeta::core::URITemplateRouterView &router,
                 const sourcemeta::core::URITemplateRouter::Identifier)
      : base_path_{router.base_path()} {}

  auto run(const std::span<std::string_view>,
           sourcemeta::one::HTTPRequest &request,
           sourcemeta::one::HTTPResponse &response) -> void {
    sourcemeta::one::json_error(
        request, response, sourcemeta::one::STATUS_NOT_FOUND, "not-found",
        "There is nothing at this URL",
        std::string{this->base_path_} + "/self/v1/schemas/api/error");
  }

private:
  std::string_view base_path_;
};

#endif
