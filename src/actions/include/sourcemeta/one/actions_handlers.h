#ifndef SOURCEMETA_ONE_ACTIONS_HANDLERS_H
#define SOURCEMETA_ONE_ACTIONS_HANDLERS_H

#include <sourcemeta/one/http.h>
#include <sourcemeta/one/shared.h>

#include <sourcemeta/one/actions_helpers.h>
#include <sourcemeta/one/actions_jsonschema_evaluate.h>
#include <sourcemeta/one/actions_jsonschema_serve.h>
#include <sourcemeta/one/actions_schema_search.h>
#include <sourcemeta/one/actions_serve_metapack_file.h>

#include <filesystem>  // std::filesystem
#include <span>        // std::span
#include <sstream>     // std::ostringstream
#include <string_view> // std::string_view

namespace sourcemeta::one {
inline std::filesystem::path static_asset_path;
} // namespace sourcemeta::one

inline auto handle_self_v1_api_list(const std::filesystem::path &base,
                                    const std::span<std::string_view>,
                                    sourcemeta::one::HTTPRequest &request,
                                    sourcemeta::one::HTTPResponse &response)
    -> void {
  action_serve_metapack_file(
      request, response, base / "explorer" / SENTINEL / "directory.metapack",
      sourcemeta::one::STATUS_OK, true, std::nullopt,
      "/self/v1/schemas/api/list/response");
}

inline auto
handle_self_v1_api_list_path(const std::filesystem::path &base,
                             const std::span<std::string_view> matches,
                             sourcemeta::one::HTTPRequest &request,
                             sourcemeta::one::HTTPResponse &response) -> void {
  const auto absolute_path{base / "explorer" / matches.front() / SENTINEL /
                           "directory.metapack"};
  action_serve_metapack_file(request, response, absolute_path,
                             sourcemeta::one::STATUS_OK, true, std::nullopt,
                             "/self/v1/schemas/api/list/response");
}

inline auto handle_self_v1_api_schemas_dependencies(
    const std::filesystem::path &base,
    const std::span<std::string_view> matches,
    sourcemeta::one::HTTPRequest &request,
    sourcemeta::one::HTTPResponse &response) -> void {
  const auto absolute_path{base / "schemas" / matches.front() / SENTINEL /
                           "dependencies.metapack"};
  action_serve_metapack_file(
      request, response, absolute_path, sourcemeta::one::STATUS_OK, true,
      std::nullopt, "/self/v1/schemas/api/schemas/dependencies/response");
}

inline auto
handle_self_v1_api_schemas_dependents(const std::filesystem::path &base,
                                      const std::span<std::string_view> matches,
                                      sourcemeta::one::HTTPRequest &request,
                                      sourcemeta::one::HTTPResponse &response)
    -> void {
  const auto absolute_path{base / "schemas" / matches.front() / SENTINEL /
                           "dependents.metapack"};
  action_serve_metapack_file(
      request, response, absolute_path, sourcemeta::one::STATUS_OK, true,
      std::nullopt, "/self/v1/schemas/api/schemas/dependents/response");
}

inline auto
handle_self_v1_api_schemas_health(const std::filesystem::path &base,
                                  const std::span<std::string_view> matches,
                                  sourcemeta::one::HTTPRequest &request,
                                  sourcemeta::one::HTTPResponse &response)
    -> void {
  const auto absolute_path{base / "schemas" / matches.front() / SENTINEL /
                           "health.metapack"};
  action_serve_metapack_file(request, response, absolute_path,
                             sourcemeta::one::STATUS_OK, true, std::nullopt,
                             "/self/v1/schemas/api/schemas/health/response");
}

inline auto
handle_self_v1_api_schemas_locations(const std::filesystem::path &base,
                                     const std::span<std::string_view> matches,
                                     sourcemeta::one::HTTPRequest &request,
                                     sourcemeta::one::HTTPResponse &response)
    -> void {
  const auto absolute_path{base / "schemas" / matches.front() / SENTINEL /
                           "locations.metapack"};
  action_serve_metapack_file(request, response, absolute_path,
                             sourcemeta::one::STATUS_OK, true, std::nullopt,
                             "/self/v1/schemas/api/schemas/locations/response");
}

inline auto
handle_self_v1_api_schemas_positions(const std::filesystem::path &base,
                                     const std::span<std::string_view> matches,
                                     sourcemeta::one::HTTPRequest &request,
                                     sourcemeta::one::HTTPResponse &response)
    -> void {
  const auto absolute_path{base / "schemas" / matches.front() / SENTINEL /
                           "positions.metapack"};
  action_serve_metapack_file(request, response, absolute_path,
                             sourcemeta::one::STATUS_OK, true, std::nullopt,
                             "/self/v1/schemas/api/schemas/positions/response");
}

inline auto
handle_self_v1_api_schemas_stats(const std::filesystem::path &base,
                                 const std::span<std::string_view> matches,
                                 sourcemeta::one::HTTPRequest &request,
                                 sourcemeta::one::HTTPResponse &response)
    -> void {
  const auto absolute_path{base / "schemas" / matches.front() / SENTINEL /
                           "stats.metapack"};
  action_serve_metapack_file(request, response, absolute_path,
                             sourcemeta::one::STATUS_OK, true, std::nullopt,
                             "/self/v1/schemas/api/schemas/stats/response");
}

inline auto
handle_self_v1_api_schemas_metadata(const std::filesystem::path &base,
                                    const std::span<std::string_view> matches,
                                    sourcemeta::one::HTTPRequest &request,
                                    sourcemeta::one::HTTPResponse &response)
    -> void {
  const auto absolute_path{base / "explorer" / matches.front() / SENTINEL /
                           "schema.metapack"};
  action_serve_metapack_file(request, response, absolute_path,
                             sourcemeta::one::STATUS_OK, true, std::nullopt,
                             "/self/v1/schemas/api/schemas/metadata/response");
}

inline auto
handle_self_v1_api_schemas_evaluate(const std::filesystem::path &base,
                                    const std::span<std::string_view> matches,
                                    sourcemeta::one::HTTPRequest &request,
                                    sourcemeta::one::HTTPResponse &response)
    -> void {
  action_jsonschema_evaluate(base, matches.front(), request, response,
                             sourcemeta::one::EvaluateType::Standard);
}

inline auto
handle_self_v1_api_schemas_trace(const std::filesystem::path &base,
                                 const std::span<std::string_view> matches,
                                 sourcemeta::one::HTTPRequest &request,
                                 sourcemeta::one::HTTPResponse &response)
    -> void {
  action_jsonschema_evaluate(base, matches.front(), request, response,
                             sourcemeta::one::EvaluateType::Trace);
}

inline auto handle_self_v1_api_schemas_search(
    const std::filesystem::path &base, const std::span<std::string_view>,
    sourcemeta::one::HTTPRequest &request,
    sourcemeta::one::HTTPResponse &response) -> void {
  static sourcemeta::one::SearchView search_view{base / "explorer" / SENTINEL /
                                                 "search.metapack"};
  action_schema_search(search_view, request, response);
}

inline auto handle_self_api_not_found(const std::filesystem::path &,
                                      const std::span<std::string_view>,
                                      sourcemeta::one::HTTPRequest &request,
                                      sourcemeta::one::HTTPResponse &response)
    -> void {
  json_error(request, response, sourcemeta::one::STATUS_NOT_FOUND, "not-found",
             "There is nothing at this URL");
}

inline auto handle_self_v1_health(const std::filesystem::path &,
                                  const std::span<std::string_view>,
                                  sourcemeta::one::HTTPRequest &request,
                                  sourcemeta::one::HTTPResponse &response)
    -> void {
  if (request.method() != "get" && request.method() != "head") {
    json_error(request, response, sourcemeta::one::STATUS_METHOD_NOT_ALLOWED,
               "method-not-allowed",
               "This HTTP method is invalid for this URL");
    return;
  }

  response.write_status(sourcemeta::one::STATUS_OK);
  response.write_header("Access-Control-Allow-Origin", "*");
  send_response(sourcemeta::one::STATUS_OK, request, response);
}

inline auto handle_self_static(const std::filesystem::path &,
                               const std::span<std::string_view> matches,
                               sourcemeta::one::HTTPRequest &request,
                               sourcemeta::one::HTTPResponse &response)
    -> void {
  action_serve_metapack_file(
      request, response, sourcemeta::one::static_asset_path / matches.front(),
      sourcemeta::one::STATUS_OK);
}

using Handler = auto (*)(const std::filesystem::path &,
                         const std::span<std::string_view>,
                         sourcemeta::one::HTTPRequest &,
                         sourcemeta::one::HTTPResponse &) -> void;

#endif
