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
  ActionServeStatic(
      const std::filesystem::path &,
      const sourcemeta::core::URITemplateRouterView &router,
      const sourcemeta::core::URITemplateRouter::Identifier identifier) {
    router.arguments(identifier, [this](const auto &key, const auto &value) {
      if (key == "path") {
        this->base_path_ = std::get<std::string_view>(value);
      }
    });
  }

  auto run(const std::span<std::string_view> matches,
           sourcemeta::one::HTTPRequest &request,
           sourcemeta::one::HTTPResponse &response) -> void {
    if (this->base_path_.empty()) {
      sourcemeta::one::json_error(
          request, response, sourcemeta::one::STATUS_INTERNAL_SERVER_ERROR,
          "missing-base-path", "The base path is not configured for this route",
          "/self/v1/schemas/api/error");
      return;
    }

    ActionServeMetapackFile::serve(this->base_path_ / matches.front(),
                                   sourcemeta::one::STATUS_OK, false, {}, {},
                                   request, response);
  }

private:
  std::filesystem::path base_path_;
};

#endif
