#ifndef SOURCEMETA_ONE_ACTIONS_GET_SCHEMA_LOCATIONS_V1_H
#define SOURCEMETA_ONE_ACTIONS_GET_SCHEMA_LOCATIONS_V1_H

#include "action_serve_schema_artifact_v1.h"

#include <filesystem>  // std::filesystem
#include <string_view> // std::string_view

class ActionGetSchemaLocations_v1 : public ActionServeSchemaArtifact_v1 {
public:
  static constexpr std::string_view DESCRIPTION{
      "Return every URI exposed by a schema, including subschemas and "
      "anchors, with their parent and base URI relationships. The response "
      "carries per-pointer metadata and grows with the schema's pointer "
      "count, so substantial payloads are expected for large schemas"};
  static constexpr bool READ_ONLY{true};
  static constexpr bool DESTRUCTIVE{false};
  static constexpr bool IDEMPOTENT{true};
  static constexpr bool OPEN_WORLD{false};

  ActionGetSchemaLocations_v1(
      const std::filesystem::path &base,
      const sourcemeta::core::URITemplateRouterView &router,
      const sourcemeta::core::URITemplateRouter::Identifier identifier,
      sourcemeta::one::Router &dispatcher)
      : ActionServeSchemaArtifact_v1{base, router, identifier, dispatcher} {
    this->artifact_ = "locations";
  }
};

#endif
