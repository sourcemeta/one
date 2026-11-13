#ifndef SOURCEMETA_ONE_ACTIONS_GET_OPENAPI_DEPENDENTS_V1_H
#define SOURCEMETA_ONE_ACTIONS_GET_OPENAPI_DEPENDENTS_V1_H

#include "action_dependency_tree_v1.h"

#include <filesystem>  // std::filesystem
#include <string_view> // std::string_view

class ActionGetOpenAPIDependentsV1 : public ActionDependencyTreeV1 {
public:
  static constexpr std::string_view TITLE{"Get OpenAPI Dependents"};
  static constexpr std::string_view DESCRIPTION{
      "List the OpenAPI descriptions that directly or transitively reference "
      "this one, the incoming edges of the dependency graph"};
  static constexpr bool READ_ONLY{true};
  static constexpr bool DESTRUCTIVE{false};
  static constexpr bool IDEMPOTENT{true};
  static constexpr bool OPEN_WORLD{false};

  ActionGetOpenAPIDependentsV1(
      const std::filesystem::path &base,
      const sourcemeta::core::URITemplateRouterView &router,
      const sourcemeta::core::URITemplateRouter::Identifier identifier,
      sourcemeta::one::Router &dispatcher)
      : ActionDependencyTreeV1{
            base,           router,
            identifier,     "dependents",
            Tree::Explorer, sourcemeta::one::LeafKind::OpenAPI,
            "openapi",      "OpenAPI description not found",
            dispatcher} {}
};

#endif
