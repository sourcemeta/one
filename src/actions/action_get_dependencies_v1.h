#ifndef SOURCEMETA_ONE_ACTIONS_GET_DEPENDENCIES_V1_H
#define SOURCEMETA_ONE_ACTIONS_GET_DEPENDENCIES_V1_H

#include "action_dependency_tree_v1.h"

#include <filesystem>  // std::filesystem
#include <string_view> // std::string_view

class ActionGetDependenciesV1 : public ActionDependencyTreeV1 {
public:
  static constexpr std::string_view TITLE{"Get Dependencies"};
  static constexpr std::string_view DESCRIPTION{
      "List the resources of this catalog that this one directly or "
      "transitively references, the outgoing edges of the dependency "
      "graph"};
  static constexpr bool READ_ONLY{true};
  static constexpr bool DESTRUCTIVE{false};
  static constexpr bool IDEMPOTENT{true};
  static constexpr bool OPEN_WORLD{false};

  ActionGetDependenciesV1(
      const std::filesystem::path &base,
      const sourcemeta::core::URITemplateRouterView &router,
      const sourcemeta::core::URITemplateRouter::Identifier identifier,
      sourcemeta::one::Router &dispatcher)
      : ActionDependencyTreeV1{base,
                               router,
                               identifier,
                               "dependencies",
                               Tree::Schemas,
                               "uri",
                               "There is nothing at this URL",
                               dispatcher} {}
};

#endif
