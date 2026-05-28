#ifndef SOURCEMETA_ONE_ACTIONS_GET_SCHEMA_METADATA_V1_H
#define SOURCEMETA_ONE_ACTIONS_GET_SCHEMA_METADATA_V1_H

#include "action_serve_explorer_artifact_v1.h"

#include <filesystem>  // std::filesystem
#include <string_view> // std::string_view

class ActionGetSchemaMetadata_v1 : public ActionServeExplorerArtifact_v1 {
public:
  static constexpr std::string_view DESCRIPTION{
      "Return navigation metadata for a schema in the catalog, including "
      "title, description, dialect, byte size, dependency count, and "
      "breadcrumb path"};
  static constexpr bool READ_ONLY{true};
  static constexpr bool DESTRUCTIVE{false};
  static constexpr bool IDEMPOTENT{true};
  static constexpr bool OPEN_WORLD{false};

  ActionGetSchemaMetadata_v1(
      const std::filesystem::path &base,
      const sourcemeta::core::URITemplateRouterView &router,
      const sourcemeta::core::URITemplateRouter::Identifier identifier,
      sourcemeta::one::Router &)
      : ActionServeExplorerArtifact_v1{base, router, identifier, "schema"} {}
};

#endif
