#ifndef SOURCEMETA_ONE_ACTIONS_GET_OPENAPI_DEPENDENCIES_V1_H
#define SOURCEMETA_ONE_ACTIONS_GET_OPENAPI_DEPENDENCIES_V1_H

#include "action_dependency_tree_v1.h"

#include <filesystem>  // std::filesystem
#include <string_view> // std::string_view

class ActionGetOpenAPIDependenciesV1 : public ActionDependencyTreeV1 {
public:
  static constexpr std::string_view DESCRIPTION{
      "List the documents this OpenAPI description directly or transitively "
      "reaches, the outgoing edges of the dependency graph"};
  static constexpr bool READ_ONLY{true};
  static constexpr bool DESTRUCTIVE{false};
  static constexpr bool IDEMPOTENT{true};
  static constexpr bool OPEN_WORLD{false};

  ActionGetOpenAPIDependenciesV1(
      const std::filesystem::path &base,
      const sourcemeta::core::URITemplateRouterView &router,
      const sourcemeta::core::URITemplateRouter::Identifier identifier,
      sourcemeta::one::Router &dispatcher)
      : ActionDependencyTreeV1{
            base,          router,
            identifier,    "dependencies",
            Tree::Schemas, sourcemeta::one::LeafKind::OpenAPI,
            "openapi",     "OpenAPI description not found",
            dispatcher} {}
};

#endif
