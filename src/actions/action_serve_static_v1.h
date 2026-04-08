#ifndef SOURCEMETA_ONE_ACTIONS_SERVE_STATIC_V1_H
#define SOURCEMETA_ONE_ACTIONS_SERVE_STATIC_V1_H

#include <sourcemeta/core/uritemplate.h>

#include <sourcemeta/one/actions.h>
#include <sourcemeta/one/http.h>

#include "action_serve_metapack_file_v1.h"

#include <filesystem>  // std::filesystem
#include <span>        // std::span
#include <string>      // std::string
#include <string_view> // std::string_view

class ActionServeStatic_v1 : public sourcemeta::one::Action {
public:
  ActionServeStatic_v1(
      const std::filesystem::path &base,
      const sourcemeta::core::URITemplateRouterView &router,
      const sourcemeta::core::URITemplateRouter::Identifier identifier)
      : sourcemeta::one::Action{base, router.base_path()} {
    router.arguments(identifier, [this](const auto &key, const auto &value) {
      if (key == "path") {
        this->file_root_ = std::get<std::string_view>(value);
      }
    });
  }

  auto run(const std::span<std::string_view> matches,
           sourcemeta::one::HTTPRequest &request,
           sourcemeta::one::HTTPResponse &response) -> void override {
    if (this->file_root_.empty()) {
      sourcemeta::one::json_error(
          request, response, sourcemeta::one::STATUS_INTERNAL_SERVER_ERROR,
          "missing-base-path", "The base path is not configured for this route",
          std::string{this->base_path()} + "/self/v1/schemas/api/error");
      return;
    }

    ActionServeMetapackFile_v1::serve(this->file_root_ / matches.front(),
                                      sourcemeta::one::STATUS_OK, false, {}, {},
                                      request, response, this->base_path());
  }

private:
  std::filesystem::path file_root_;
};

#endif
