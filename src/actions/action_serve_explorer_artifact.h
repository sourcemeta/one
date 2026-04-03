#ifndef SOURCEMETA_ONE_ACTIONS_SERVE_EXPLORER_ARTIFACT_H
#define SOURCEMETA_ONE_ACTIONS_SERVE_EXPLORER_ARTIFACT_H

#include <sourcemeta/core/uritemplate.h>

#include <sourcemeta/one/http.h>

#include "action_serve_metapack_file.h"

#include <filesystem>  // std::filesystem
#include <span>        // std::span
#include <string>      // std::string
#include <string_view> // std::string_view

class ActionServeExplorerArtifact {
public:
  ActionServeExplorerArtifact(
      const std::filesystem::path &base,
      const sourcemeta::core::URITemplateRouterView &router,
      const sourcemeta::core::URITemplateRouter::Identifier identifier)
      : base_{base}, base_path_{router.base_path()} {
    router.arguments(identifier, [this](const auto &key, const auto &value) {
      if (key == "artifact") {
        this->artifact_ = std::get<std::string_view>(value);
      } else if (key == "responseSchema") {
        this->response_schema_ = std::get<std::string_view>(value);
      }
    });
  }

  auto run(const std::span<std::string_view> matches,
           sourcemeta::one::HTTPRequest &request,
           sourcemeta::one::HTTPResponse &response) -> void {
    auto absolute_path{this->base_ / "explorer"};
    if (!matches.empty()) {
      absolute_path /= matches.front();
    }
    absolute_path /= "%";
    absolute_path /= std::string{this->artifact_} + ".metapack";
    ActionServeMetapackFile::serve(absolute_path, sourcemeta::one::STATUS_OK,
                                   true, {}, this->response_schema_, request,
                                   response, this->base_path_);
  }

private:
  // NOLINTNEXTLINE(cppcoreguidelines-avoid-const-or-ref-data-members)
  const std::filesystem::path &base_;
  std::string_view base_path_;
  std::string_view artifact_;
  std::string_view response_schema_;
};

#endif
