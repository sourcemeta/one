#ifndef SOURCEMETA_ONE_ACTIONS_SERVE_STATIC_FILE_V1_H
#define SOURCEMETA_ONE_ACTIONS_SERVE_STATIC_FILE_V1_H

#include <sourcemeta/core/uritemplate.h>

#include <sourcemeta/one/actions.h>
#include <sourcemeta/one/http.h>

#include "action_serve_metapack_file_v1.h"

#include <filesystem>  // std::filesystem
#include <span>        // std::span
#include <string>      // std::string
#include <string_view> // std::string_view

class ActionServeStaticFile_v1 : public sourcemeta::one::Action {
public:
  ActionServeStaticFile_v1(
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

  auto run(const std::span<std::string_view> matches,
           sourcemeta::one::HTTPRequest &request,
           sourcemeta::one::HTTPResponse &response) -> void override {
    if (matches.empty()) {
      sourcemeta::one::json_error(
          request, response, sourcemeta::one::STATUS_NOT_FOUND, "missing-hash",
          "No hash was provided in the URL", this->error_schema_);
      return;
    }

    const auto absolute_path{this->base() / "static" /
                             (std::string{matches.front()} + ".metapack")};
    ActionServeMetapackFile_v1::serve(absolute_path, sourcemeta::one::STATUS_OK,
                                      true, {}, {}, request, response,
                                      this->error_schema_);
  }

private:
  std::string_view error_schema_;
};

#endif
