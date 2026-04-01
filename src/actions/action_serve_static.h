#ifndef SOURCEMETA_ONE_ACTIONS_SERVE_STATIC_H
#define SOURCEMETA_ONE_ACTIONS_SERVE_STATIC_H

#include <sourcemeta/core/uritemplate.h>

#include <sourcemeta/one/http.h>

#include "action_serve_metapack_file.h"

#include <filesystem>  // std::filesystem
#include <span>        // std::span
#include <string_view> // std::string_view

class ActionServeStatic {
public:
  explicit ActionServeStatic(const std::filesystem::path &) {}

  auto
  run(const std::filesystem::path &, const std::span<std::string_view> matches,
      sourcemeta::one::HTTPRequest &request,
      sourcemeta::one::HTTPResponse &response,
      const std::span<const sourcemeta::core::URITemplateRouter::ArgumentValue>
          arguments) -> void {
    const std::filesystem::path base_path{
        std::get<std::string_view>(arguments[0])};
    if (base_path.empty()) {
      sourcemeta::one::json_error(
          request, response, sourcemeta::one::STATUS_INTERNAL_SERVER_ERROR,
          "missing-base-path", "The base path is not configured for this route",
          "/self/v1/schemas/api/error");
      return;
    }

    ActionServeMetapackFile::serve(base_path / matches.front(),
                                   sourcemeta::one::STATUS_OK, false, {}, {},
                                   request, response);
  }
};

#endif
