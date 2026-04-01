#ifndef SOURCEMETA_ONE_ACTIONS_NOT_FOUND_H
#define SOURCEMETA_ONE_ACTIONS_NOT_FOUND_H

#include <sourcemeta/core/uritemplate.h>

#include <sourcemeta/one/http.h>

#include <filesystem>  // std::filesystem
#include <span>        // std::span
#include <string_view> // std::string_view

class ActionNotFound {
public:
  explicit ActionNotFound(const std::filesystem::path &) {}

  auto
  run(const std::filesystem::path &, const std::span<std::string_view>,
      sourcemeta::one::HTTPRequest &request,
      sourcemeta::one::HTTPResponse &response,
      const std::span<const sourcemeta::core::URITemplateRouter::ArgumentValue>)
      -> void {
    sourcemeta::one::json_error(
        request, response, sourcemeta::one::STATUS_NOT_FOUND, "not-found",
        "There is nothing at this URL", "/self/v1/schemas/api/error");
  }
};

#endif
