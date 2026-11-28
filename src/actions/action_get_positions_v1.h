#ifndef SOURCEMETA_ONE_ACTIONS_GET_POSITIONS_V1_H
#define SOURCEMETA_ONE_ACTIONS_GET_POSITIONS_V1_H

#include "action_serve_schema_artifact_v1.h"

#include <filesystem>  // std::filesystem
#include <string_view> // std::string_view

class ActionGetPositionsV1 : public ActionServeSchemaArtifactV1 {
public:
  static constexpr std::string_view TITLE{"Get Positions"};
  static constexpr std::string_view DESCRIPTION{
      "Return the source line and column position of every JSON Pointer "
      "inside a resource, whether it is a JSON Schema or an OpenAPI "
      "description"};
  static constexpr bool READ_ONLY{true};
  static constexpr bool DESTRUCTIVE{false};
  static constexpr bool IDEMPOTENT{true};
  static constexpr bool OPEN_WORLD{false};

  ActionGetPositionsV1(
      const std::filesystem::path &base,
      const sourcemeta::core::URITemplateRouterView &router,
      const sourcemeta::core::URITemplateRouter::Identifier identifier,
      sourcemeta::one::Router &dispatcher)
      : ActionServeSchemaArtifactV1{base,        router,     identifier,
                                    "positions", dispatcher, "uri"} {}
};

#endif
