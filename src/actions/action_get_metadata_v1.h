#ifndef SOURCEMETA_ONE_ACTIONS_GET_METADATA_V1_H
#define SOURCEMETA_ONE_ACTIONS_GET_METADATA_V1_H

#include "action_serve_explorer_artifact_v1.h"

#include <filesystem>  // std::filesystem
#include <string_view> // std::string_view

class ActionGetMetadataV1 : public ActionServeExplorerArtifactV1 {
public:
  static constexpr std::string_view TITLE{"Get Metadata"};
  static constexpr std::string_view DESCRIPTION{
      "Return navigation metadata for a resource in the catalog, whether it "
      "is a JSON Schema or an OpenAPI description, including title, "
      "description, what it conforms to, dialect, byte size, dependency "
      "count, and breadcrumb path"};
  static constexpr bool READ_ONLY{true};
  static constexpr bool DESTRUCTIVE{false};
  static constexpr bool IDEMPOTENT{true};
  static constexpr bool OPEN_WORLD{false};

  ActionGetMetadataV1(
      const std::filesystem::path &base,
      const sourcemeta::core::URITemplateRouterView &router,
      const sourcemeta::core::URITemplateRouter::Identifier identifier,
      sourcemeta::one::Router &dispatcher)
      : ActionServeExplorerArtifactV1{base, router, identifier, "metadata",
                                      dispatcher} {}
};

#endif
