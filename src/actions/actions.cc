#include <sourcemeta/one/actions.h>

#include "action_default_v1.h"
#include "action_dependency_tree_v1.h"
#include "action_health_check_v1.h"
#include "action_jsonschema_evaluate_v1.h"
#include "action_jsonschema_trace_v1.h"
#include "action_list_directory_v1.h"
#include "action_mcp_v1.h"
#include "action_not_found_v1.h"
#include "action_schema_search_v1.h"
#include "action_serve_explorer_artifact_v1.h"
#include "action_serve_schema_artifact_v1.h"
#include "action_serve_static_v1.h"

namespace sourcemeta::one {

#define SOURCEMETA_ONE_MAKE_CONSTRUCTOR_ENTRY(Name, Class)                     \
  table[ACTION_TYPE_##Name] = &make_router_action<Class>;

const std::array<RouterActionConstructor, ACTION_TYPE_COUNT> CONSTRUCTORS{[] {
  std::array<RouterActionConstructor, ACTION_TYPE_COUNT> table{};
  SOURCEMETA_ONE_FOR_EACH_ACTION(SOURCEMETA_ONE_MAKE_CONSTRUCTOR_ENTRY)
  return table;
}()};

#undef SOURCEMETA_ONE_MAKE_CONSTRUCTOR_ENTRY

#define SOURCEMETA_ONE_DEFINE_DESCRIPTION(Name, Class) Class::DESCRIPTION,

const std::array<std::string_view, ACTION_TYPE_COUNT> DESCRIPTIONS{
    {SOURCEMETA_ONE_FOR_EACH_ACTION(SOURCEMETA_ONE_DEFINE_DESCRIPTION)}};

#undef SOURCEMETA_ONE_DEFINE_DESCRIPTION

auto action_description(
    const sourcemeta::core::URITemplateRouter::Identifier context) noexcept
    -> std::string_view {
  if (context >= DESCRIPTIONS.size()) {
    return {};
  }
  return DESCRIPTIONS[context];
}

} // namespace sourcemeta::one
