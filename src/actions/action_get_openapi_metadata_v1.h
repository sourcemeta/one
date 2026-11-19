#ifndef SOURCEMETA_ONE_ACTIONS_GET_OPENAPI_METADATA_V1_H
#define SOURCEMETA_ONE_ACTIONS_GET_OPENAPI_METADATA_V1_H

#include "action_serve_explorer_artifact_v1.h"

#include <filesystem>  // std::filesystem
#include <string_view> // std::string_view

class ActionGetOpenAPIMetadataV1 : public ActionServeExplorerArtifactV1 {
public:
  static constexpr std::string_view TITLE{"Get OpenAPI Metadata"};
  static constexpr std::string_view DESCRIPTION{
      "Return navigation metadata for an OpenAPI description in the catalog, "
      "including title, description, revision, API version, byte size, "
      "operation count, dependency count, and breadcrumb path"};
  static constexpr bool READ_ONLY{true};
  static constexpr bool DESTRUCTIVE{false};
  static constexpr bool IDEMPOTENT{true};
  static constexpr bool OPEN_WORLD{false};

  ActionGetOpenAPIMetadataV1(
      const std::filesystem::path &base,
      const sourcemeta::core::URITemplateRouterView &router,
      const sourcemeta::core::URITemplateRouter::Identifier identifier,
      sourcemeta::one::Router &dispatcher)
      : ActionServeExplorerArtifactV1{base,
                                      router,
                                      identifier,
                                      "metadata",
                                      sourcemeta::one::LeafKind::OpenAPI,
                                      "openapi",
                                      "OpenAPI description not found",
                                      dispatcher} {}
};

#endif
