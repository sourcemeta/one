#ifndef SOURCEMETA_ONE_ACTIONS_H
#define SOURCEMETA_ONE_ACTIONS_H

#include <sourcemeta/one/dispatcher.h>

#include <array>       // std::array
#include <cstdint>     // std::uint8_t
#include <string_view> // std::string_view

namespace sourcemeta::one {

#define SOURCEMETA_ONE_FOR_EACH_ACTION(X)                                      \
  X(DEFAULT_V1, ActionDefault_v1)                                              \
  X(HEALTH_CHECK_V1, ActionHealthCheck_v1)                                     \
  X(NOT_FOUND_V1, ActionNotFound_v1)                                           \
  X(SCHEMA_ARTIFACT_V1, ActionServeSchemaArtifact_v1)                          \
  X(EXPLORER_ARTIFACT_V1, ActionServeExplorerArtifact_v1)                      \
  X(JSONSCHEMA_EVALUATE_V1, ActionJSONSchemaEvaluate_v1)                       \
  X(JSONSCHEMA_TRACE_V1, ActionJSONSchemaTrace_v1)                             \
  X(SCHEMA_SEARCH_V1, ActionSchemaSearch_v1)                                   \
  X(SERVE_STATIC_V1, ActionServeStatic_v1)                                     \
  X(MCP_V1, ActionMCP_v1)

#define SOURCEMETA_ONE_DEFINE_ACTION_TYPE(Name, Class) ACTION_TYPE_##Name,

enum : std::uint8_t {
  SOURCEMETA_ONE_FOR_EACH_ACTION(SOURCEMETA_ONE_DEFINE_ACTION_TYPE)
      ACTION_TYPE_COUNT
};

#undef SOURCEMETA_ONE_DEFINE_ACTION_TYPE

extern const std::array<ActionConstructor, ACTION_TYPE_COUNT> CONSTRUCTORS;
extern const std::array<std::string_view, ACTION_TYPE_COUNT> DESCRIPTIONS;

[[nodiscard]] auto action_description(
    sourcemeta::core::URITemplateRouter::Identifier context) noexcept
    -> std::string_view;

} // namespace sourcemeta::one

#endif
