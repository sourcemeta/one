#include <sourcemeta/one/actions.h>

#include "action_default.h"
#include "action_health_check.h"
#include "action_jsonschema_evaluate.h"
#include "action_jsonschema_serve.h"
#include "action_not_found.h"
#include "action_schema_search.h"
#include "action_serve_metapack_file.h"

#include <array>   // std::array
#include <cassert> // assert
#include <variant> // std::get

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
  action_default(base, request, response);
}

static auto handle_self_v1_api_list(
    const std::uint16_t, const sourcemeta::core::URITemplateRouterView &,
    const std::filesystem::path &base, const std::span<std::string_view>,
    sourcemeta::one::HTTPRequest &request,
    sourcemeta::one::HTTPResponse &response) -> void {
  action_serve_metapack_file(request, response,
                             base / "explorer" / "%" / "directory.metapack",
                             sourcemeta::one::STATUS_OK, true, std::nullopt,
                             "/self/v1/schemas/api/list/response");
}

static auto
handle_self_v1_api_list_path(const std::uint16_t,
                             const sourcemeta::core::URITemplateRouterView &,
                             const std::filesystem::path &base,
                             const std::span<std::string_view> matches,
                             sourcemeta::one::HTTPRequest &request,
                             sourcemeta::one::HTTPResponse &response) -> void {
  const auto absolute_path{base / "explorer" / matches.front() / "%" /
                           "directory.metapack"};
  action_serve_metapack_file(request, response, absolute_path,
                             sourcemeta::one::STATUS_OK, true, std::nullopt,
                             "/self/v1/schemas/api/list/response");
}

static auto handle_self_v1_api_schemas_dependencies(
    const std::uint16_t, const sourcemeta::core::URITemplateRouterView &,
    const std::filesystem::path &base,
    const std::span<std::string_view> matches,
    sourcemeta::one::HTTPRequest &request,
    sourcemeta::one::HTTPResponse &response) -> void {
  const auto absolute_path{base / "schemas" / matches.front() / "%" /
                           "dependencies.metapack"};
  action_serve_metapack_file(
      request, response, absolute_path, sourcemeta::one::STATUS_OK, true,
      std::nullopt, "/self/v1/schemas/api/schemas/dependencies/response");
}

static auto handle_self_v1_api_schemas_dependents(
    const std::uint16_t, const sourcemeta::core::URITemplateRouterView &,
    const std::filesystem::path &base,
    const std::span<std::string_view> matches,
    sourcemeta::one::HTTPRequest &request,
    sourcemeta::one::HTTPResponse &response) -> void {
  const auto absolute_path{base / "schemas" / matches.front() / "%" /
                           "dependents.metapack"};
  action_serve_metapack_file(
      request, response, absolute_path, sourcemeta::one::STATUS_OK, true,
      std::nullopt, "/self/v1/schemas/api/schemas/dependents/response");
}

static auto handle_self_v1_api_schemas_health(
    const std::uint16_t, const sourcemeta::core::URITemplateRouterView &,
    const std::filesystem::path &base,
    const std::span<std::string_view> matches,
    sourcemeta::one::HTTPRequest &request,
    sourcemeta::one::HTTPResponse &response) -> void {
  const auto absolute_path{base / "schemas" / matches.front() / "%" /
                           "health.metapack"};
  action_serve_metapack_file(request, response, absolute_path,
                             sourcemeta::one::STATUS_OK, true, std::nullopt,
                             "/self/v1/schemas/api/schemas/health/response");
}

static auto handle_self_v1_api_schemas_locations(
    const std::uint16_t, const sourcemeta::core::URITemplateRouterView &,
    const std::filesystem::path &base,
    const std::span<std::string_view> matches,
    sourcemeta::one::HTTPRequest &request,
    sourcemeta::one::HTTPResponse &response) -> void {
  const auto absolute_path{base / "schemas" / matches.front() / "%" /
                           "locations.metapack"};
  action_serve_metapack_file(request, response, absolute_path,
                             sourcemeta::one::STATUS_OK, true, std::nullopt,
                             "/self/v1/schemas/api/schemas/locations/response");
}

static auto handle_self_v1_api_schemas_positions(
    const std::uint16_t, const sourcemeta::core::URITemplateRouterView &,
    const std::filesystem::path &base,
    const std::span<std::string_view> matches,
    sourcemeta::one::HTTPRequest &request,
    sourcemeta::one::HTTPResponse &response) -> void {
  const auto absolute_path{base / "schemas" / matches.front() / "%" /
                           "positions.metapack"};
  action_serve_metapack_file(request, response, absolute_path,
                             sourcemeta::one::STATUS_OK, true, std::nullopt,
                             "/self/v1/schemas/api/schemas/positions/response");
}

static auto handle_self_v1_api_schemas_stats(
    const std::uint16_t, const sourcemeta::core::URITemplateRouterView &,
    const std::filesystem::path &base,
    const std::span<std::string_view> matches,
    sourcemeta::one::HTTPRequest &request,
    sourcemeta::one::HTTPResponse &response) -> void {
  const auto absolute_path{base / "schemas" / matches.front() / "%" /
                           "stats.metapack"};
  action_serve_metapack_file(request, response, absolute_path,
                             sourcemeta::one::STATUS_OK, true, std::nullopt,
                             "/self/v1/schemas/api/schemas/stats/response");
}

static auto handle_self_v1_api_schemas_metadata(
    const std::uint16_t, const sourcemeta::core::URITemplateRouterView &,
    const std::filesystem::path &base,
    const std::span<std::string_view> matches,
    sourcemeta::one::HTTPRequest &request,
    sourcemeta::one::HTTPResponse &response) -> void {
  const auto absolute_path{base / "explorer" / matches.front() / "%" /
                           "schema.metapack"};
  action_serve_metapack_file(request, response, absolute_path,
                             sourcemeta::one::STATUS_OK, true, std::nullopt,
                             "/self/v1/schemas/api/schemas/metadata/response");
}

static auto handle_self_v1_api_schemas_evaluate(
    const std::uint16_t, const sourcemeta::core::URITemplateRouterView &,
    const std::filesystem::path &base,
    const std::span<std::string_view> matches,
    sourcemeta::one::HTTPRequest &request,
    sourcemeta::one::HTTPResponse &response) -> void {
  action_jsonschema_evaluate(base, matches.front(), request, response,
                             sourcemeta::one::EvaluateType::Standard);
}

static auto handle_self_v1_api_schemas_trace(
    const std::uint16_t, const sourcemeta::core::URITemplateRouterView &,
    const std::filesystem::path &base,
    const std::span<std::string_view> matches,
    sourcemeta::one::HTTPRequest &request,
    sourcemeta::one::HTTPResponse &response) -> void {
  action_jsonschema_evaluate(base, matches.front(), request, response,
                             sourcemeta::one::EvaluateType::Trace);
}

static auto handle_self_v1_api_schemas_search(
    const std::uint16_t, const sourcemeta::core::URITemplateRouterView &,
    const std::filesystem::path &base, const std::span<std::string_view>,
    sourcemeta::one::HTTPRequest &request,
    sourcemeta::one::HTTPResponse &response) -> void {
  static sourcemeta::one::SearchView search_view{base / "explorer" / "%" /
                                                 "search.metapack"};
  action_schema_search(search_view, request, response);
}

static auto handle_self_api_not_found(
    const std::uint16_t, const sourcemeta::core::URITemplateRouterView &,
    const std::filesystem::path &, const std::span<std::string_view>,
    sourcemeta::one::HTTPRequest &request,
    sourcemeta::one::HTTPResponse &response) -> void {
  action_not_found(request, response);
}

static auto handle_self_v1_health(
    const std::uint16_t, const sourcemeta::core::URITemplateRouterView &,
    const std::filesystem::path &, const std::span<std::string_view>,
    sourcemeta::one::HTTPRequest &request,
    sourcemeta::one::HTTPResponse &response) -> void {
  action_health_check(request, response);
}

static auto
handle_self_static(const std::uint16_t identifier,
                   const sourcemeta::core::URITemplateRouterView &router,
                   const std::filesystem::path &,
                   const std::span<std::string_view> matches,
                   sourcemeta::one::HTTPRequest &request,
                   sourcemeta::one::HTTPResponse &response) -> void {
  std::filesystem::path static_path;
  router.arguments(identifier,
                   [&static_path](const auto &key, const auto &value) {
                     if (key == "path") {
                       static_path = std::get<std::string_view>(value);
                     }
                   });

  action_serve_metapack_file_relative(request, response, static_path,
                                      matches.front(),
                                      sourcemeta::one::STATUS_OK);
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
