#include <sourcemeta/one/actions.h>

#include "action_default.h"
#include "action_health_check.h"
#include "action_jsonschema_evaluate.h"
#include "action_not_found.h"
#include "action_schema_search.h"
#include "action_serve_explorer_artifact.h"
#include "action_serve_schema_artifact.h"
#include "action_serve_static.h"

#include <array>   // std::array
#include <utility> // std::to_underlying

using Handler = auto (*)(const std::uint16_t,
                         const sourcemeta::core::URITemplateRouterView &,
                         const std::filesystem::path &,
                         const std::span<std::string_view>,
                         sourcemeta::one::HTTPRequest &,
                         sourcemeta::one::HTTPResponse &) -> void;

static auto handle_default(const std::uint16_t,
                           const sourcemeta::core::URITemplateRouterView &,
                           const std::filesystem::path &base,
                           const std::span<std::string_view>,
                           sourcemeta::one::HTTPRequest &request,
                           sourcemeta::one::HTTPResponse &response) -> void {
  static ActionDefault instance{base};
  instance.run(base, {}, request, response, {});
}

static auto handle_self_v1_api_list(
    const std::uint16_t, const sourcemeta::core::URITemplateRouterView &,
    const std::filesystem::path &base, const std::span<std::string_view>,
    sourcemeta::one::HTTPRequest &request,
    sourcemeta::one::HTTPResponse &response) -> void {
  static ActionServeExplorerArtifact instance{base};
  static const std::array<sourcemeta::core::URITemplateRouter::ArgumentValue, 2>
      arguments{{sourcemeta::core::URITemplateRouter::ArgumentValue{
                     std::string_view{"directory"}},
                 sourcemeta::core::URITemplateRouter::ArgumentValue{
                     std::string_view{"/self/v1/schemas/api/list/response"}}}};
  instance.run(base, {}, request, response, arguments);
}

static auto
handle_self_v1_api_list_path(const std::uint16_t,
                             const sourcemeta::core::URITemplateRouterView &,
                             const std::filesystem::path &base,
                             const std::span<std::string_view> matches,
                             sourcemeta::one::HTTPRequest &request,
                             sourcemeta::one::HTTPResponse &response) -> void {
  static ActionServeExplorerArtifact instance{base};
  static const std::array<sourcemeta::core::URITemplateRouter::ArgumentValue, 2>
      arguments{{sourcemeta::core::URITemplateRouter::ArgumentValue{
                     std::string_view{"directory"}},
                 sourcemeta::core::URITemplateRouter::ArgumentValue{
                     std::string_view{"/self/v1/schemas/api/list/response"}}}};
  instance.run(base, matches, request, response, arguments);
}

static auto handle_self_v1_api_schemas_dependencies(
    const std::uint16_t, const sourcemeta::core::URITemplateRouterView &,
    const std::filesystem::path &base,
    const std::span<std::string_view> matches,
    sourcemeta::one::HTTPRequest &request,
    sourcemeta::one::HTTPResponse &response) -> void {
  static ActionServeSchemaArtifact instance{base};
  static const std::array<sourcemeta::core::URITemplateRouter::ArgumentValue, 2>
      arguments{
          {sourcemeta::core::URITemplateRouter::ArgumentValue{
               std::string_view{"dependencies"}},
           sourcemeta::core::URITemplateRouter::ArgumentValue{std::string_view{
               "/self/v1/schemas/api/schemas/dependencies/response"}}}};
  instance.run(base, matches, request, response, arguments);
}

static auto handle_self_v1_api_schemas_dependents(
    const std::uint16_t, const sourcemeta::core::URITemplateRouterView &,
    const std::filesystem::path &base,
    const std::span<std::string_view> matches,
    sourcemeta::one::HTTPRequest &request,
    sourcemeta::one::HTTPResponse &response) -> void {
  static ActionServeSchemaArtifact instance{base};
  static const std::array<sourcemeta::core::URITemplateRouter::ArgumentValue, 2>
      arguments{
          {sourcemeta::core::URITemplateRouter::ArgumentValue{
               std::string_view{"dependents"}},
           sourcemeta::core::URITemplateRouter::ArgumentValue{std::string_view{
               "/self/v1/schemas/api/schemas/dependents/response"}}}};
  instance.run(base, matches, request, response, arguments);
}

static auto handle_self_v1_api_schemas_health(
    const std::uint16_t, const sourcemeta::core::URITemplateRouterView &,
    const std::filesystem::path &base,
    const std::span<std::string_view> matches,
    sourcemeta::one::HTTPRequest &request,
    sourcemeta::one::HTTPResponse &response) -> void {
  static ActionServeSchemaArtifact instance{base};
  static const std::array<sourcemeta::core::URITemplateRouter::ArgumentValue, 2>
      arguments{
          {sourcemeta::core::URITemplateRouter::ArgumentValue{
               std::string_view{"health"}},
           sourcemeta::core::URITemplateRouter::ArgumentValue{std::string_view{
               "/self/v1/schemas/api/schemas/health/response"}}}};
  instance.run(base, matches, request, response, arguments);
}

static auto handle_self_v1_api_schemas_locations(
    const std::uint16_t, const sourcemeta::core::URITemplateRouterView &,
    const std::filesystem::path &base,
    const std::span<std::string_view> matches,
    sourcemeta::one::HTTPRequest &request,
    sourcemeta::one::HTTPResponse &response) -> void {
  static ActionServeSchemaArtifact instance{base};
  static const std::array<sourcemeta::core::URITemplateRouter::ArgumentValue, 2>
      arguments{
          {sourcemeta::core::URITemplateRouter::ArgumentValue{
               std::string_view{"locations"}},
           sourcemeta::core::URITemplateRouter::ArgumentValue{std::string_view{
               "/self/v1/schemas/api/schemas/locations/response"}}}};
  instance.run(base, matches, request, response, arguments);
}

static auto handle_self_v1_api_schemas_positions(
    const std::uint16_t, const sourcemeta::core::URITemplateRouterView &,
    const std::filesystem::path &base,
    const std::span<std::string_view> matches,
    sourcemeta::one::HTTPRequest &request,
    sourcemeta::one::HTTPResponse &response) -> void {
  static ActionServeSchemaArtifact instance{base};
  static const std::array<sourcemeta::core::URITemplateRouter::ArgumentValue, 2>
      arguments{
          {sourcemeta::core::URITemplateRouter::ArgumentValue{
               std::string_view{"positions"}},
           sourcemeta::core::URITemplateRouter::ArgumentValue{std::string_view{
               "/self/v1/schemas/api/schemas/positions/response"}}}};
  instance.run(base, matches, request, response, arguments);
}

static auto handle_self_v1_api_schemas_stats(
    const std::uint16_t, const sourcemeta::core::URITemplateRouterView &,
    const std::filesystem::path &base,
    const std::span<std::string_view> matches,
    sourcemeta::one::HTTPRequest &request,
    sourcemeta::one::HTTPResponse &response) -> void {
  static ActionServeSchemaArtifact instance{base};
  static const std::array<sourcemeta::core::URITemplateRouter::ArgumentValue, 2>
      arguments{
          {sourcemeta::core::URITemplateRouter::ArgumentValue{
               std::string_view{"stats"}},
           sourcemeta::core::URITemplateRouter::ArgumentValue{std::string_view{
               "/self/v1/schemas/api/schemas/stats/response"}}}};
  instance.run(base, matches, request, response, arguments);
}

static auto handle_self_v1_api_schemas_metadata(
    const std::uint16_t, const sourcemeta::core::URITemplateRouterView &,
    const std::filesystem::path &base,
    const std::span<std::string_view> matches,
    sourcemeta::one::HTTPRequest &request,
    sourcemeta::one::HTTPResponse &response) -> void {
  static ActionServeExplorerArtifact instance{base};
  static const std::array<sourcemeta::core::URITemplateRouter::ArgumentValue, 2>
      arguments{
          {sourcemeta::core::URITemplateRouter::ArgumentValue{
               std::string_view{"schema"}},
           sourcemeta::core::URITemplateRouter::ArgumentValue{std::string_view{
               "/self/v1/schemas/api/schemas/metadata/response"}}}};
  instance.run(base, matches, request, response, arguments);
}

static auto handle_self_v1_api_schemas_evaluate(
    const std::uint16_t, const sourcemeta::core::URITemplateRouterView &,
    const std::filesystem::path &base,
    const std::span<std::string_view> matches,
    sourcemeta::one::HTTPRequest &request,
    sourcemeta::one::HTTPResponse &response) -> void {
  static ActionJSONSchemaEvaluate instance{base};
  static const std::array<sourcemeta::core::URITemplateRouter::ArgumentValue, 1>
      arguments{{sourcemeta::core::URITemplateRouter::ArgumentValue{
          static_cast<std::int64_t>(std::to_underlying(
              ActionJSONSchemaEvaluate::EvaluateMode::Standard))}}};
  instance.run(base, matches, request, response, arguments);
}

static auto handle_self_v1_api_schemas_trace(
    const std::uint16_t, const sourcemeta::core::URITemplateRouterView &,
    const std::filesystem::path &base,
    const std::span<std::string_view> matches,
    sourcemeta::one::HTTPRequest &request,
    sourcemeta::one::HTTPResponse &response) -> void {
  static ActionJSONSchemaEvaluate instance{base};
  static const std::array<sourcemeta::core::URITemplateRouter::ArgumentValue, 1>
      arguments{{sourcemeta::core::URITemplateRouter::ArgumentValue{
          static_cast<std::int64_t>(std::to_underlying(
              ActionJSONSchemaEvaluate::EvaluateMode::Trace))}}};
  instance.run(base, matches, request, response, arguments);
}

static auto handle_self_v1_api_schemas_search(
    const std::uint16_t, const sourcemeta::core::URITemplateRouterView &,
    const std::filesystem::path &base, const std::span<std::string_view>,
    sourcemeta::one::HTTPRequest &request,
    sourcemeta::one::HTTPResponse &response) -> void {
  static ActionSchemaSearch instance{base};
  instance.run(base, {}, request, response, {});
}

static auto handle_self_api_not_found(
    const std::uint16_t, const sourcemeta::core::URITemplateRouterView &,
    const std::filesystem::path &base, const std::span<std::string_view>,
    sourcemeta::one::HTTPRequest &request,
    sourcemeta::one::HTTPResponse &response) -> void {
  static ActionNotFound instance{base};
  instance.run({}, {}, request, response, {});
}

static auto handle_self_v1_health(
    const std::uint16_t, const sourcemeta::core::URITemplateRouterView &,
    const std::filesystem::path &base, const std::span<std::string_view>,
    sourcemeta::one::HTTPRequest &request,
    sourcemeta::one::HTTPResponse &response) -> void {
  static ActionHealthCheck instance{base};
  instance.run({}, {}, request, response, {});
}

static auto
handle_self_static(const std::uint16_t identifier,
                   const sourcemeta::core::URITemplateRouterView &router,
                   const std::filesystem::path &base,
                   const std::span<std::string_view> matches,
                   sourcemeta::one::HTTPRequest &request,
                   sourcemeta::one::HTTPResponse &response) -> void {
  static ActionServeStatic instance{base};
  std::string_view static_path;
  router.arguments(identifier,
                   [&static_path](const auto &key, const auto &value) {
                     if (key == "path") {
                       static_path = std::get<std::string_view>(value);
                     }
                   });

  const std::array<sourcemeta::core::URITemplateRouter::ArgumentValue, 1>
      arguments{
          {sourcemeta::core::URITemplateRouter::ArgumentValue{static_path}}};
  instance.run(base, matches, request, response, arguments);
}

static const std::array<Handler, 16> ACTION_HANDLERS = {
    {handle_default, handle_self_v1_api_list, handle_self_v1_api_list_path,
     handle_self_v1_api_schemas_dependencies,
     handle_self_v1_api_schemas_dependents, handle_self_v1_api_schemas_health,
     handle_self_v1_api_schemas_locations, handle_self_v1_api_schemas_positions,
     handle_self_v1_api_schemas_stats, handle_self_v1_api_schemas_metadata,
     handle_self_v1_api_schemas_evaluate, handle_self_v1_api_schemas_trace,
     handle_self_v1_api_schemas_search, handle_self_api_not_found,
     handle_self_static, handle_self_v1_health}};

auto sourcemeta::one::dispatch_action(
    const std::uint16_t identifier,
    const sourcemeta::core::URITemplateRouterView &router,
    const std::filesystem::path &base,
    const std::span<std::string_view> matches,
    sourcemeta::one::HTTPRequest &request,
    sourcemeta::one::HTTPResponse &response) -> void {
  if (identifier >= std::size(ACTION_HANDLERS)) [[unlikely]] {
    sourcemeta::one::json_error(
        request, response, sourcemeta::one::STATUS_NOT_IMPLEMENTED,
        "unknown-handler-code",
        "This server version does not implement the handler for "
        "this URL",
        // TODO: This implies the API is mounted
        "/self/v1/schemas/api/error");
    return;
  }

  ACTION_HANDLERS[identifier](identifier, router, base, matches, request,
                              response);
}
