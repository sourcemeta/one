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
  explicit ActionServeSchemaArtifact(const std::filesystem::path &) {}

  auto
  run(const std::filesystem::path &base,
      const std::span<std::string_view> matches,
      sourcemeta::one::HTTPRequest &request,
      sourcemeta::one::HTTPResponse &response,
      const std::span<const sourcemeta::core::URITemplateRouter::ArgumentValue>
          arguments) -> void {
    if (matches.empty()) {
      sourcemeta::one::json_error(
          request, response, sourcemeta::one::STATUS_INTERNAL_SERVER_ERROR,
          "missing-schema-match", "This action requires a schema path match",
          "/self/v1/schemas/api/error");
      return;
    }

    const auto artifact{std::get<std::string_view>(arguments[0])};
    const auto link{std::get<std::string_view>(arguments[1])};
    auto absolute_path{base / "schemas" / matches.front() / "%"};
    absolute_path /= std::string{artifact} + ".metapack";
    ActionServeMetapackFile::serve(absolute_path, sourcemeta::one::STATUS_OK,
                                   true, {}, link, request, response);
  }
};

#endif
