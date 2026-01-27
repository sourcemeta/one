#include <sourcemeta/core/uritemplate.h>

#include "action_jsonschema_evaluate.h"
#include "action_jsonschema_serve.h"
#include "action_schema_search.h"
#include "action_serve_metapack_file.h"

#include "helpers.h"

#include <array>       // std::array
#include <csignal>     // std::signal, SIGINT, SIGTERM
#include <cstdint>     // std::uint32_t, std::stoul
#include <cstdlib>     // EXIT_FAILURE, std::exit
#include <filesystem>  // std::filesystem
#include <iostream>    // std::cout
#include <limits>      // std::numeric_limits
#include <span>        // std::span
#include <sstream>     // std::ostringstream
#include <string>      // std::string
#include <string_view> // std::string_view

static auto handle_self_v1_api_list(const std::filesystem::path &base,
                                    const std::span<std::string_view>,
                                    sourcemeta::one::HTTPRequest &request,
                                    sourcemeta::one::HTTPResponse &response)
    -> void {
  action_serve_metapack_file(
      request, response, base / "explorer" / SENTINEL / "directory.metapack",
      sourcemeta::one::STATUS_OK, true, std::nullopt,
      "/self/v1/schemas/api/list/response");
}

static auto
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

static auto handle_self_v1_api_schemas_dependencies(
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

static auto
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

static auto
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

static auto
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

static auto
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

static auto
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

static auto
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

static auto
handle_self_v1_api_schemas_evaluate(const std::filesystem::path &base,
                                    const std::span<std::string_view> matches,
                                    sourcemeta::one::HTTPRequest &request,
                                    sourcemeta::one::HTTPResponse &response)
    -> void {
  action_jsonschema_evaluate(base, matches.front(), request, response,
                             sourcemeta::one::EvaluateType::Standard);
}

static auto
handle_self_v1_api_schemas_trace(const std::filesystem::path &base,
                                 const std::span<std::string_view> matches,
                                 sourcemeta::one::HTTPRequest &request,
                                 sourcemeta::one::HTTPResponse &response)
    -> void {
  action_jsonschema_evaluate(base, matches.front(), request, response,
                             sourcemeta::one::EvaluateType::Trace);
}

static auto handle_self_v1_api_schemas_search(
    const std::filesystem::path &base, const std::span<std::string_view>,
    sourcemeta::one::HTTPRequest &request,
    sourcemeta::one::HTTPResponse &response) -> void {
  action_schema_search(base, request, response);
}

static auto handle_self_api_not_found(const std::filesystem::path &,
                                      const std::span<std::string_view>,
                                      sourcemeta::one::HTTPRequest &request,
                                      sourcemeta::one::HTTPResponse &response)
    -> void {
  json_error(request, response, sourcemeta::one::STATUS_NOT_FOUND, "not-found",
             "There is nothing at this URL");
}

static auto handle_self_static(const std::filesystem::path &,
                               const std::span<std::string_view> matches,
                               sourcemeta::one::HTTPRequest &request,
                               sourcemeta::one::HTTPResponse &response)
    -> void {
  std::ostringstream absolute_path;
  absolute_path << SOURCEMETA_ONE_STATIC;
  absolute_path << '/';
  absolute_path << matches.front();
  action_serve_metapack_file(request, response, absolute_path.str(),
                             sourcemeta::one::STATUS_OK);
}

static auto handle_default(const std::filesystem::path &base,
                           const std::span<std::string_view>,
                           sourcemeta::one::HTTPRequest &request,
                           sourcemeta::one::HTTPResponse &response) -> void {
  if (request.path() == "/") {
    if (request.prefers_html()) {
      action_serve_metapack_file(request, response,
                                 base / "explorer" / SENTINEL /
                                     "directory-html.metapack",
                                 sourcemeta::one::STATUS_OK);
      return;
    } else if (request.method() == "get" || request.method() == "head") {
      json_error(request, response, sourcemeta::one::STATUS_NOT_FOUND,
                 "not-found", "There is nothing at this URL");
      return;
    } else {
      json_error(request, response, sourcemeta::one::STATUS_METHOD_NOT_ALLOWED,
                 "method-not-allowed",
                 "This HTTP method is invalid for this URL");
      return;
    }
  }

  if (request.path().ends_with(".json")) {
    action_jsonschema_serve(base,
                            request.path().substr(1, request.path().size() - 6),
                            request, response);
    return;
  }

  const auto path{request.path().substr(1)};
  if (request.method() == "get" || request.method() == "head") {
    if (request.prefers_html()) {
      auto absolute_path{base / "explorer" / path / SENTINEL};
      if (std::filesystem::exists(absolute_path / "schema-html.metapack") &&
          // To distinguish between entries that are both directories and
          // schemas
          !path.ends_with("/")) {
        action_serve_metapack_file(request, response,
                                   absolute_path / "schema-html.metapack",
                                   sourcemeta::one::STATUS_OK);
      } else {
        absolute_path /= "directory-html.metapack";
        if (std::filesystem::exists(absolute_path)) {
          action_serve_metapack_file(request, response, absolute_path,
                                     sourcemeta::one::STATUS_OK);
        } else {
          action_serve_metapack_file(
              request, response, base / "explorer" / SENTINEL / "404.metapack",
              sourcemeta::one::STATUS_NOT_FOUND);
        }
      }
    } else {
      action_jsonschema_serve(base, path, request, response);
    }
  } else {
    json_error(request, response, sourcemeta::one::STATUS_NOT_FOUND,
               "not-found", "There is nothing at this URL");
  }
}

using Handler = auto (*)(const std::filesystem::path &,
                         const std::span<std::string_view>,
                         sourcemeta::one::HTTPRequest &,
                         sourcemeta::one::HTTPResponse &) -> void;

static const Handler HANDLERS[] = {handle_default,
                                   handle_self_v1_api_list,
                                   handle_self_v1_api_list_path,
                                   handle_self_v1_api_schemas_dependencies,
                                   handle_self_v1_api_schemas_dependents,
                                   handle_self_v1_api_schemas_health,
                                   handle_self_v1_api_schemas_locations,
                                   handle_self_v1_api_schemas_positions,
                                   handle_self_v1_api_schemas_stats,
                                   handle_self_v1_api_schemas_metadata,
                                   handle_self_v1_api_schemas_evaluate,
                                   handle_self_v1_api_schemas_trace,
                                   handle_self_v1_api_schemas_search,
                                   handle_self_api_not_found,
                                   handle_self_static};

static auto dispatch(const sourcemeta::core::URITemplateRouterView &router,
                     const std::filesystem::path &base,
                     uWS::HttpResponse<true> *const raw_response,
                     uWS::HttpRequest *const raw_request) noexcept -> void {
  sourcemeta::one::HTTPResponse response{raw_response};
  sourcemeta::one::HTTPRequest request{raw_request, raw_response};
  try {
    request.negotiate();
    if (request.satisfiable_encoding()) {
      thread_local std::array<
          std::string_view,
          std::numeric_limits<
              sourcemeta::core::URITemplateRouter::Index>::max()>
          matches;
      const auto handler{router.match(
          request.path(), [](const auto index, auto, const auto value) {
            matches[index] = value;
          })};

      HANDLERS[handler](base, matches, request, response);
    } else {
      json_error(request, response, sourcemeta::one::STATUS_NOT_ACCEPTABLE,
                 "cannot-satisfy-content-encoding",
                 "The server cannot satisfy the request content encoding");
    }
  } catch (const std::exception &error) {
    json_error(request, response, sourcemeta::one::STATUS_METHOD_NOT_ALLOWED,
               "uncaught-error", error.what());
  }
}

auto terminate(int signal) -> void {
  std::cerr << "Terminatting on signal: " << signal << "\n";
  // TODO: Use `us_listen_socket_close` instead
  // See https://github.com/uNetworking/uWebSockets/issues/1402
  std::exit(EXIT_SUCCESS);
}

// We try to keep this function as straight to the point as possible
// with minimal input validation (outside debug builds). The intention
// is for the server to start running and bind to the port as quickly
// as possible, so we can take better advantage of scale-to-zero.
auto main(int argc, char *argv[]) noexcept -> int {
  const auto timestamp_start{std::chrono::steady_clock::now()};

  std::cout << "Sourcemeta One v" << sourcemeta::one::version() << "\n";

  // Mainly for Docker Compose
  std::signal(SIGINT, terminate);
  std::signal(SIGTERM, terminate);

  try {
    if (argc != 3) {
      std::cout << "Usage: " << argv[0]
                << " <path/to/output/directory> <port>\n";
      return EXIT_FAILURE;
    }

    const auto port{static_cast<std::uint32_t>(std::stoul(argv[2]))};
    const auto base{std::filesystem::canonical(argv[1])};
    const sourcemeta::core::URITemplateRouterView router{base / "routes.bin"};

    uWS::LocalCluster(
        {}, [&router, &base, port, timestamp_start](uWS::SSLApp &app) -> void {
          app.any("/*",
                  [&router, &base](auto *const response,
                                   auto *const request) noexcept -> void {
                    dispatch(router, base, response, request);
                  });

          app.listen(
              static_cast<int>(port),
              [port,
               timestamp_start](us_listen_socket_t *const socket) -> void {
                const auto timestamp_end{std::chrono::steady_clock::now()};
                const auto duration{
                    std::chrono::duration_cast<std::chrono::milliseconds>(
                        timestamp_end - timestamp_start)};
                if (socket) {
                  const auto socket_port = us_socket_local_port(
                      true, reinterpret_cast<struct us_socket_t *>(socket));
                  assert(socket_port > 0);
                  assert(port == static_cast<std::uint32_t>(socket_port));
                  log("Listening on port " + std::to_string(socket_port) +
                      " in " + std::to_string(duration.count()) + " ms");
                } else {
                  log("Failed to listen on port " + std::to_string(port));
                }
              });
        });

    log("The server could not start");
    return EXIT_FAILURE;
  } catch (const std::exception &error) {
    std::cerr << "unexpected error: " << error.what() << "\n";
    return EXIT_FAILURE;
  }
}
