#include <sourcemeta/one/actions.h>

#include "action_default.h"
#include "action_health_check.h"
#include "action_jsonschema_evaluate.h"
#include "action_not_found.h"
#include "action_schema_search.h"
#include "action_serve_explorer_artifact.h"
#include "action_serve_schema_artifact.h"
#include "action_serve_static.h"

#include <array> // std::array

using Handler = auto (*)(const sourcemeta::core::URITemplateRouter::Identifier,
                         const sourcemeta::core::URITemplateRouterView &,
                         const std::filesystem::path &,
                         const std::span<std::string_view>,
                         sourcemeta::one::HTTPRequest &,
                         sourcemeta::one::HTTPResponse &) -> void;

static auto
handle_default(const sourcemeta::core::URITemplateRouter::Identifier identifier,
               const sourcemeta::core::URITemplateRouterView &router,
               const std::filesystem::path &base,
               const std::span<std::string_view> matches,
               sourcemeta::one::HTTPRequest &request,
               sourcemeta::one::HTTPResponse &response) -> void {
  static ActionDefault instance{base, router, identifier};
  instance.run(matches, request, response);
}

static auto handle_self_v1_api_list(
    const sourcemeta::core::URITemplateRouter::Identifier identifier,
    const sourcemeta::core::URITemplateRouterView &router,
    const std::filesystem::path &base,
    const std::span<std::string_view> matches,
    sourcemeta::one::HTTPRequest &request,
    sourcemeta::one::HTTPResponse &response) -> void {
  static ActionServeExplorerArtifact instance{base, router, identifier};
  instance.run(matches, request, response);
}

static auto handle_self_v1_api_list_path(
    const sourcemeta::core::URITemplateRouter::Identifier identifier,
    const sourcemeta::core::URITemplateRouterView &router,
    const std::filesystem::path &base,
    const std::span<std::string_view> matches,
    sourcemeta::one::HTTPRequest &request,
    sourcemeta::one::HTTPResponse &response) -> void {
  static ActionServeExplorerArtifact instance{base, router, identifier};
  instance.run(matches, request, response);
}

static auto handle_self_v1_api_schemas_dependencies(
    const sourcemeta::core::URITemplateRouter::Identifier identifier,
    const sourcemeta::core::URITemplateRouterView &router,
    const std::filesystem::path &base,
    const std::span<std::string_view> matches,
    sourcemeta::one::HTTPRequest &request,
    sourcemeta::one::HTTPResponse &response) -> void {
  static ActionServeSchemaArtifact instance{base, router, identifier};
  instance.run(matches, request, response);
}

static auto handle_self_v1_api_schemas_dependents(
    const sourcemeta::core::URITemplateRouter::Identifier identifier,
    const sourcemeta::core::URITemplateRouterView &router,
    const std::filesystem::path &base,
    const std::span<std::string_view> matches,
    sourcemeta::one::HTTPRequest &request,
    sourcemeta::one::HTTPResponse &response) -> void {
  static ActionServeSchemaArtifact instance{base, router, identifier};
  instance.run(matches, request, response);
}

static auto handle_self_v1_api_schemas_health(
    const sourcemeta::core::URITemplateRouter::Identifier identifier,
    const sourcemeta::core::URITemplateRouterView &router,
    const std::filesystem::path &base,
    const std::span<std::string_view> matches,
    sourcemeta::one::HTTPRequest &request,
    sourcemeta::one::HTTPResponse &response) -> void {
  static ActionServeSchemaArtifact instance{base, router, identifier};
  instance.run(matches, request, response);
}

static auto handle_self_v1_api_schemas_locations(
    const sourcemeta::core::URITemplateRouter::Identifier identifier,
    const sourcemeta::core::URITemplateRouterView &router,
    const std::filesystem::path &base,
    const std::span<std::string_view> matches,
    sourcemeta::one::HTTPRequest &request,
    sourcemeta::one::HTTPResponse &response) -> void {
  static ActionServeSchemaArtifact instance{base, router, identifier};
  instance.run(matches, request, response);
}

static auto handle_self_v1_api_schemas_positions(
    const sourcemeta::core::URITemplateRouter::Identifier identifier,
    const sourcemeta::core::URITemplateRouterView &router,
    const std::filesystem::path &base,
    const std::span<std::string_view> matches,
    sourcemeta::one::HTTPRequest &request,
    sourcemeta::one::HTTPResponse &response) -> void {
  static ActionServeSchemaArtifact instance{base, router, identifier};
  instance.run(matches, request, response);
}

static auto handle_self_v1_api_schemas_stats(
    const sourcemeta::core::URITemplateRouter::Identifier identifier,
    const sourcemeta::core::URITemplateRouterView &router,
    const std::filesystem::path &base,
    const std::span<std::string_view> matches,
    sourcemeta::one::HTTPRequest &request,
    sourcemeta::one::HTTPResponse &response) -> void {
  static ActionServeSchemaArtifact instance{base, router, identifier};
  instance.run(matches, request, response);
}

static auto handle_self_v1_api_schemas_metadata(
    const sourcemeta::core::URITemplateRouter::Identifier identifier,
    const sourcemeta::core::URITemplateRouterView &router,
    const std::filesystem::path &base,
    const std::span<std::string_view> matches,
    sourcemeta::one::HTTPRequest &request,
    sourcemeta::one::HTTPResponse &response) -> void {
  static ActionServeExplorerArtifact instance{base, router, identifier};
  instance.run(matches, request, response);
}

static auto handle_self_v1_api_schemas_evaluate(
    const sourcemeta::core::URITemplateRouter::Identifier identifier,
    const sourcemeta::core::URITemplateRouterView &router,
    const std::filesystem::path &base,
    const std::span<std::string_view> matches,
    sourcemeta::one::HTTPRequest &request,
    sourcemeta::one::HTTPResponse &response) -> void {
  static ActionJSONSchemaEvaluate instance{base, router, identifier};
  instance.run(matches, request, response);
}

static auto handle_self_v1_api_schemas_trace(
    const sourcemeta::core::URITemplateRouter::Identifier identifier,
    const sourcemeta::core::URITemplateRouterView &router,
    const std::filesystem::path &base,
    const std::span<std::string_view> matches,
    sourcemeta::one::HTTPRequest &request,
    sourcemeta::one::HTTPResponse &response) -> void {
  static ActionJSONSchemaEvaluate instance{base, router, identifier};
  instance.run(matches, request, response);
}

static auto handle_self_v1_api_schemas_search(
    const sourcemeta::core::URITemplateRouter::Identifier identifier,
    const sourcemeta::core::URITemplateRouterView &router,
    const std::filesystem::path &base,
    const std::span<std::string_view> matches,
    sourcemeta::one::HTTPRequest &request,
    sourcemeta::one::HTTPResponse &response) -> void {
  static ActionSchemaSearch instance{base, router, identifier};
  instance.run(matches, request, response);
}

static auto handle_self_api_not_found(
    const sourcemeta::core::URITemplateRouter::Identifier identifier,
    const sourcemeta::core::URITemplateRouterView &router,
    const std::filesystem::path &base,
    const std::span<std::string_view> matches,
    sourcemeta::one::HTTPRequest &request,
    sourcemeta::one::HTTPResponse &response) -> void {
  static ActionNotFound instance{base, router, identifier};
  instance.run(matches, request, response);
}

static auto handle_self_v1_health(
    const sourcemeta::core::URITemplateRouter::Identifier identifier,
    const sourcemeta::core::URITemplateRouterView &router,
    const std::filesystem::path &base,
    const std::span<std::string_view> matches,
    sourcemeta::one::HTTPRequest &request,
    sourcemeta::one::HTTPResponse &response) -> void {
  static ActionHealthCheck instance{base, router, identifier};
  instance.run(matches, request, response);
}

static auto handle_self_static(
    const sourcemeta::core::URITemplateRouter::Identifier identifier,
    const sourcemeta::core::URITemplateRouterView &router,
    const std::filesystem::path &base,
    const std::span<std::string_view> matches,
    sourcemeta::one::HTTPRequest &request,
    sourcemeta::one::HTTPResponse &response) -> void {
  static ActionServeStatic instance{base, router, identifier};
  instance.run(matches, request, response);
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
    const sourcemeta::core::URITemplateRouter::Identifier identifier,
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
