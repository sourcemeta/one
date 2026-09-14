#ifndef SOURCEMETA_ONE_ENTERPRISE_SERVER_SCHEMA_AS_H_
#define SOURCEMETA_ONE_ENTERPRISE_SERVER_SCHEMA_AS_H_

#include <sourcemeta/core/http.h>

#include <sourcemeta/one/http.h>
#include <sourcemeta/one/router.h>

#include <algorithm>   // std::ranges::find
#include <array>       // std::array
#include <optional>    // std::optional
#include <string>      // std::string
#include <string_view> // std::string_view

namespace sourcemeta::one {

// The dialects a schema may be asked for in, as the query parameter spells them
inline constexpr std::array<std::string_view, 5> SCHEMA_CONVERSIONS{
    "draft4", "draft6", "draft7", "2019-09", "2020-12"};

// The artifact holding a schema converted into the given dialect, if the
// dialect is one a schema may be asked for in at all
[[nodiscard]] inline auto
schema_conversion_artifact(const std::string_view dialect)
    -> std::optional<std::string> {
  if (std::ranges::find(SCHEMA_CONVERSIONS, dialect) ==
      SCHEMA_CONVERSIONS.cend()) {
    return std::nullopt;
  }

  std::string artifact{"schema-"};
  artifact.append(dialect);
  return artifact;
}

inline auto serve_schema_as(const RouterAction &self,
                            const Authentication::Caller &caller,
                            const std::string_view schema_path,
                            HTTPRequest &request, HTTPResponse &response,
                            const std::string_view error_schema) -> void {
  const auto schema{self.artifact_resolve_path(
      caller, schema_path, RouterAction::Tree::Schemas, "schema")};
  if (!schema.path.has_value()) {
    json_error(request, response, sourcemeta::core::HTTP_STATUS_NOT_FOUND,
               "urn:sourcemeta:one:not-found", "There is nothing at this URL",
               error_schema, "*");
    return;
  }

  if (request.has_query("bundle")) {
    json_error(request, response, sourcemeta::core::HTTP_STATUS_BAD_REQUEST,
               "urn:sourcemeta:one:incompatible-query-parameters",
               "The as and bundle query parameters cannot be combined",
               error_schema, "*");
    return;
  }

  const auto artifact{schema_conversion_artifact(request.query("as"))};
  if (artifact.has_value()) {
    const auto resolution{self.artifact_resolve_path(
        caller, schema_path, RouterAction::Tree::Schemas, artifact.value())};
    if (resolution.path.has_value()) {
      self.artifact_serve(resolution.path.value(),
                          sourcemeta::core::HTTP_STATUS_OK, true, {}, {}, {},
                          request, response, error_schema,
                          cache_control_content(resolution.is_public),
                          vary_client_and_encoding());
      return;
    }
  }

  json_error(request, response, sourcemeta::core::HTTP_STATUS_BAD_REQUEST,
             "urn:sourcemeta:one:invalid-conversion",
             "The schema cannot be converted into this dialect", error_schema,
             "*");
}

} // namespace sourcemeta::one

#endif
