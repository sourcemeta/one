#ifndef SOURCEMETA_ONE_ACTIONS_GET_SCHEMA_STATS_V1_H
#define SOURCEMETA_ONE_ACTIONS_GET_SCHEMA_STATS_V1_H

#include "action_serve_schema_artifact_v1.h"

#include <filesystem>  // std::filesystem
#include <string_view> // std::string_view

class ActionGetSchemaStats_v1 : public ActionServeSchemaArtifact_v1 {
public:
  static constexpr std::string_view DESCRIPTION{
      "Return per-vocabulary keyword usage statistics for a schema in the "
      "catalog, identifying which JSON Schema keywords the schema relies "
      "on and how often each appears"};
  static constexpr bool READ_ONLY{true};
  static constexpr bool DESTRUCTIVE{false};
  static constexpr bool IDEMPOTENT{true};
  static constexpr bool OPEN_WORLD{false};

  ActionGetSchemaStats_v1(
      const std::filesystem::path &base,
      const sourcemeta::core::URITemplateRouterView &router,
      const sourcemeta::core::URITemplateRouter::Identifier identifier,
      sourcemeta::one::Router &dispatcher)
      : ActionServeSchemaArtifact_v1{base, router, identifier, "stats",
                                     dispatcher} {}
};

#endif
