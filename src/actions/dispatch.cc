#include <sourcemeta/one/actions.h>

#include "action_default.h"
#include "action_health_check.h"
#include "action_jsonschema_evaluate.h"
#include "action_not_found.h"
#include "action_schema_search.h"
#include "action_serve_metapack_file.h"

#include <array> // std::array

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
  static ActionServeMetapackFile instance{base};
  const auto path{(base / "explorer" / "%" / "directory.metapack").string()};
  const std::array<sourcemeta::core::URITemplateRouter::ArgumentValue, 5>
      arguments{{sourcemeta::core::URITemplateRouter::ArgumentValue{
                     std::string_view{path}},
                 sourcemeta::core::URITemplateRouter::ArgumentValue{
                     std::string_view{sourcemeta::one::STATUS_OK}},
                 sourcemeta::core::URITemplateRouter::ArgumentValue{true},
                 sourcemeta::core::URITemplateRouter::ArgumentValue{
                     std::string_view{}},
                 sourcemeta::core::URITemplateRouter::ArgumentValue{
                     std::string_view{"/self/v1/schemas/api/list/response"}}}};
  instance.run({}, {}, request, response, arguments);
}

static auto
handle_self_v1_api_list_path(const std::uint16_t,
                             const sourcemeta::core::URITemplateRouterView &,
                             const std::filesystem::path &base,
                             const std::span<std::string_view> matches,
                             sourcemeta::one::HTTPRequest &request,
                             sourcemeta::one::HTTPResponse &response) -> void {
  static ActionServeMetapackFile instance{base};
  const auto path{
      (base / "explorer" / matches.front() / "%" / "directory.metapack")
          .string()};
  const std::array<sourcemeta::core::URITemplateRouter::ArgumentValue, 5>
      arguments{{sourcemeta::core::URITemplateRouter::ArgumentValue{
                     std::string_view{path}},
                 sourcemeta::core::URITemplateRouter::ArgumentValue{
                     std::string_view{sourcemeta::one::STATUS_OK}},
                 sourcemeta::core::URITemplateRouter::ArgumentValue{true},
                 sourcemeta::core::URITemplateRouter::ArgumentValue{
                     std::string_view{}},
                 sourcemeta::core::URITemplateRouter::ArgumentValue{
                     std::string_view{"/self/v1/schemas/api/list/response"}}}};
  instance.run({}, {}, request, response, arguments);
}

static auto handle_self_v1_api_schemas_dependencies(
    const std::uint16_t, const sourcemeta::core::URITemplateRouterView &,
    const std::filesystem::path &base,
    const std::span<std::string_view> matches,
    sourcemeta::one::HTTPRequest &request,
    sourcemeta::one::HTTPResponse &response) -> void {
  static ActionServeMetapackFile instance{base};
  const auto path{
      (base / "schemas" / matches.front() / "%" / "dependencies.metapack")
          .string()};
  const std::array<sourcemeta::core::URITemplateRouter::ArgumentValue, 5>
      arguments{
          {sourcemeta::core::URITemplateRouter::ArgumentValue{
               std::string_view{path}},
           sourcemeta::core::URITemplateRouter::ArgumentValue{
               std::string_view{sourcemeta::one::STATUS_OK}},
           sourcemeta::core::URITemplateRouter::ArgumentValue{true},
           sourcemeta::core::URITemplateRouter::ArgumentValue{
               std::string_view{}},
           sourcemeta::core::URITemplateRouter::ArgumentValue{std::string_view{
               "/self/v1/schemas/api/schemas/dependencies/response"}}}};
  instance.run({}, {}, request, response, arguments);
}

static auto handle_self_v1_api_schemas_dependents(
    const std::uint16_t, const sourcemeta::core::URITemplateRouterView &,
    const std::filesystem::path &base,
    const std::span<std::string_view> matches,
    sourcemeta::one::HTTPRequest &request,
    sourcemeta::one::HTTPResponse &response) -> void {
  static ActionServeMetapackFile instance{base};
  const auto path{
      (base / "schemas" / matches.front() / "%" / "dependents.metapack")
          .string()};
  const std::array<sourcemeta::core::URITemplateRouter::ArgumentValue, 5>
      arguments{
          {sourcemeta::core::URITemplateRouter::ArgumentValue{
               std::string_view{path}},
           sourcemeta::core::URITemplateRouter::ArgumentValue{
               std::string_view{sourcemeta::one::STATUS_OK}},
           sourcemeta::core::URITemplateRouter::ArgumentValue{true},
           sourcemeta::core::URITemplateRouter::ArgumentValue{
               std::string_view{}},
           sourcemeta::core::URITemplateRouter::ArgumentValue{std::string_view{
               "/self/v1/schemas/api/schemas/dependents/response"}}}};
  instance.run({}, {}, request, response, arguments);
}

static auto handle_self_v1_api_schemas_health(
    const std::uint16_t, const sourcemeta::core::URITemplateRouterView &,
    const std::filesystem::path &base,
    const std::span<std::string_view> matches,
    sourcemeta::one::HTTPRequest &request,
    sourcemeta::one::HTTPResponse &response) -> void {
  static ActionServeMetapackFile instance{base};
  const auto path{
      (base / "schemas" / matches.front() / "%" / "health.metapack").string()};
  const std::array<sourcemeta::core::URITemplateRouter::ArgumentValue, 5>
      arguments{
          {sourcemeta::core::URITemplateRouter::ArgumentValue{
               std::string_view{path}},
           sourcemeta::core::URITemplateRouter::ArgumentValue{
               std::string_view{sourcemeta::one::STATUS_OK}},
           sourcemeta::core::URITemplateRouter::ArgumentValue{true},
           sourcemeta::core::URITemplateRouter::ArgumentValue{
               std::string_view{}},
           sourcemeta::core::URITemplateRouter::ArgumentValue{std::string_view{
               "/self/v1/schemas/api/schemas/health/response"}}}};
  instance.run({}, {}, request, response, arguments);
}

static auto handle_self_v1_api_schemas_locations(
    const std::uint16_t, const sourcemeta::core::URITemplateRouterView &,
    const std::filesystem::path &base,
    const std::span<std::string_view> matches,
    sourcemeta::one::HTTPRequest &request,
    sourcemeta::one::HTTPResponse &response) -> void {
  static ActionServeMetapackFile instance{base};
  const auto path{
      (base / "schemas" / matches.front() / "%" / "locations.metapack")
          .string()};
  const std::array<sourcemeta::core::URITemplateRouter::ArgumentValue, 5>
      arguments{
          {sourcemeta::core::URITemplateRouter::ArgumentValue{
               std::string_view{path}},
           sourcemeta::core::URITemplateRouter::ArgumentValue{
               std::string_view{sourcemeta::one::STATUS_OK}},
           sourcemeta::core::URITemplateRouter::ArgumentValue{true},
           sourcemeta::core::URITemplateRouter::ArgumentValue{
               std::string_view{}},
           sourcemeta::core::URITemplateRouter::ArgumentValue{std::string_view{
               "/self/v1/schemas/api/schemas/locations/response"}}}};
  instance.run({}, {}, request, response, arguments);
}

static auto handle_self_v1_api_schemas_positions(
    const std::uint16_t, const sourcemeta::core::URITemplateRouterView &,
    const std::filesystem::path &base,
    const std::span<std::string_view> matches,
    sourcemeta::one::HTTPRequest &request,
    sourcemeta::one::HTTPResponse &response) -> void {
  static ActionServeMetapackFile instance{base};
  const auto path{
      (base / "schemas" / matches.front() / "%" / "positions.metapack")
          .string()};
  const std::array<sourcemeta::core::URITemplateRouter::ArgumentValue, 5>
      arguments{
          {sourcemeta::core::URITemplateRouter::ArgumentValue{
               std::string_view{path}},
           sourcemeta::core::URITemplateRouter::ArgumentValue{
               std::string_view{sourcemeta::one::STATUS_OK}},
           sourcemeta::core::URITemplateRouter::ArgumentValue{true},
           sourcemeta::core::URITemplateRouter::ArgumentValue{
               std::string_view{}},
           sourcemeta::core::URITemplateRouter::ArgumentValue{std::string_view{
               "/self/v1/schemas/api/schemas/positions/response"}}}};
  instance.run({}, {}, request, response, arguments);
}

static auto handle_self_v1_api_schemas_stats(
    const std::uint16_t, const sourcemeta::core::URITemplateRouterView &,
    const std::filesystem::path &base,
    const std::span<std::string_view> matches,
    sourcemeta::one::HTTPRequest &request,
    sourcemeta::one::HTTPResponse &response) -> void {
  static ActionServeMetapackFile instance{base};
  const auto path{
      (base / "schemas" / matches.front() / "%" / "stats.metapack").string()};
  const std::array<sourcemeta::core::URITemplateRouter::ArgumentValue, 5>
      arguments{
          {sourcemeta::core::URITemplateRouter::ArgumentValue{
               std::string_view{path}},
           sourcemeta::core::URITemplateRouter::ArgumentValue{
               std::string_view{sourcemeta::one::STATUS_OK}},
           sourcemeta::core::URITemplateRouter::ArgumentValue{true},
           sourcemeta::core::URITemplateRouter::ArgumentValue{
               std::string_view{}},
           sourcemeta::core::URITemplateRouter::ArgumentValue{std::string_view{
               "/self/v1/schemas/api/schemas/stats/response"}}}};
  instance.run({}, {}, request, response, arguments);
}

static auto handle_self_v1_api_schemas_metadata(
    const std::uint16_t, const sourcemeta::core::URITemplateRouterView &,
    const std::filesystem::path &base,
    const std::span<std::string_view> matches,
    sourcemeta::one::HTTPRequest &request,
    sourcemeta::one::HTTPResponse &response) -> void {
  static ActionServeMetapackFile instance{base};
  const auto path{
      (base / "explorer" / matches.front() / "%" / "schema.metapack").string()};
  const std::array<sourcemeta::core::URITemplateRouter::ArgumentValue, 5>
      arguments{
          {sourcemeta::core::URITemplateRouter::ArgumentValue{
               std::string_view{path}},
           sourcemeta::core::URITemplateRouter::ArgumentValue{
               std::string_view{sourcemeta::one::STATUS_OK}},
           sourcemeta::core::URITemplateRouter::ArgumentValue{true},
           sourcemeta::core::URITemplateRouter::ArgumentValue{
               std::string_view{}},
           sourcemeta::core::URITemplateRouter::ArgumentValue{std::string_view{
               "/self/v1/schemas/api/schemas/metadata/response"}}}};
  instance.run({}, {}, request, response, arguments);
}

static auto handle_self_v1_api_schemas_evaluate(
    const std::uint16_t, const sourcemeta::core::URITemplateRouterView &,
    const std::filesystem::path &base,
    const std::span<std::string_view> matches,
    sourcemeta::one::HTTPRequest &request,
    sourcemeta::one::HTTPResponse &response) -> void {
  static ActionJSONSchemaEvaluate instance{base};
  const std::array<sourcemeta::core::URITemplateRouter::ArgumentValue, 1>
      arguments{{sourcemeta::core::URITemplateRouter::ArgumentValue{
          std::int64_t{0}}}};
  instance.run(base, matches, request, response, arguments);
}

static auto handle_self_v1_api_schemas_trace(
    const std::uint16_t, const sourcemeta::core::URITemplateRouterView &,
    const std::filesystem::path &base,
    const std::span<std::string_view> matches,
    sourcemeta::one::HTTPRequest &request,
    sourcemeta::one::HTTPResponse &response) -> void {
  static ActionJSONSchemaEvaluate instance{base};
  const std::array<sourcemeta::core::URITemplateRouter::ArgumentValue, 1>
      arguments{{sourcemeta::core::URITemplateRouter::ArgumentValue{
          std::int64_t{1}}}};
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
  static ActionServeMetapackFile instance{base};
  std::filesystem::path base_path;
  router.arguments(identifier,
                   [&base_path](const auto &key, const auto &value) {
                     if (key == "path") {
                       base_path = std::get<std::string_view>(value);
                     }
                   });

  if (base_path.empty()) {
    sourcemeta::one::json_error(
        request, response, sourcemeta::one::STATUS_INTERNAL_SERVER_ERROR,
        "missing-base-path", "The base path is not configured for this route",
        "/self/v1/schemas/api/error");
    return;
  }

  const auto &relative_path{matches.front()};
  const auto resolved_path{(base_path / relative_path).string()};
  const std::array<sourcemeta::core::URITemplateRouter::ArgumentValue, 5>
      arguments{{sourcemeta::core::URITemplateRouter::ArgumentValue{
                     std::string_view{resolved_path}},
                 sourcemeta::core::URITemplateRouter::ArgumentValue{
                     std::string_view{sourcemeta::one::STATUS_OK}},
                 sourcemeta::core::URITemplateRouter::ArgumentValue{false},
                 sourcemeta::core::URITemplateRouter::ArgumentValue{
                     std::string_view{}},
                 sourcemeta::core::URITemplateRouter::ArgumentValue{
                     std::string_view{}}}};
  instance.run({}, {}, request, response, arguments);
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
