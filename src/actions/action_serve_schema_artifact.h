#ifndef SOURCEMETA_ONE_ACTIONS_SERVE_SCHEMA_ARTIFACT_H
#define SOURCEMETA_ONE_ACTIONS_SERVE_SCHEMA_ARTIFACT_H

#include <sourcemeta/core/uritemplate.h>

#include <sourcemeta/one/http.h>

#include "action_serve_metapack_file.h"

#include <filesystem>  // std::filesystem
#include <span>        // std::span
#include <string>      // std::string
#include <string_view> // std::string_view

class ActionServeSchemaArtifact {
public:
  ActionServeSchemaArtifact(
      const std::filesystem::path &base,
      const sourcemeta::core::URITemplateRouterView &router,
      const sourcemeta::core::URITemplateRouter::Identifier identifier)
      : base_{base} {
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
    if (matches.empty()) {
      sourcemeta::one::json_error(
          request, response, sourcemeta::one::STATUS_INTERNAL_SERVER_ERROR,
          "missing-schema-match", "This action requires a schema path match",
          "/self/v1/schemas/api/error");
      return;
    }

    auto absolute_path{this->base_ / "schemas" / matches.front() / "%"};
    absolute_path /= std::string{this->artifact_} + ".metapack";
    ActionServeMetapackFile::serve(absolute_path, sourcemeta::one::STATUS_OK,
                                   true, {}, this->response_schema_, request,
                                   response);
  }

private:
  // NOLINTNEXTLINE(cppcoreguidelines-avoid-const-or-ref-data-members)
  const std::filesystem::path &base_;
  std::string_view artifact_;
  std::string_view response_schema_;
};

#endif
