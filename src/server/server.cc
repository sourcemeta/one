#include <sourcemeta/core/json.h>
#include <sourcemeta/core/uritemplate.h>

#include "evaluate.h"
#include "helpers.h"
#include "request.h"
#include "search.h"

#include <array>       // std::array
#include <cctype>      // std::tolower
#include <csignal>     // std::signal, SIGINT, SIGTERM
#include <cstdint>     // std::uint32_t, std::stoul
#include <cstdlib>     // EXIT_FAILURE, std::exit
#include <filesystem>  // std::filesystem
#include <iostream>    // std::cout
#include <limits>      // std::numeric_limits
#include <memory>      // std::unique_ptr
#include <span>        // std::span
#include <sstream>     // std::ostringstream
#include <string>      // std::string
#include <string_view> // std::string_view

static auto on_evaluate(const std::filesystem::path &base,
                        const std::string_view &path,
                        const sourcemeta::one::HTTPRequest &request,
                        uWS::HttpResponse<true> *response,
                        const sourcemeta::one::EvaluateType mode) -> void {
  // A CORS pre-flight request
  if (request.method() == "options") {
    response->writeStatus(sourcemeta::one::STATUS_NO_CONTENT);
    response->writeHeader("Access-Control-Allow-Origin", "*");
    response->writeHeader("Access-Control-Allow-Methods", "POST, OPTIONS");
    response->writeHeader("Access-Control-Allow-Headers", "Content-Type");
    response->writeHeader("Access-Control-Max-Age", "3600");
    send_response(sourcemeta::one::STATUS_NO_CONTENT, request.method(),
                  request.path(), response);
  } else if (request.method() == "post") {
    auto template_path{base / "schemas"};
    template_path /= path;
    template_path /= SENTINEL;
    template_path /= "blaze-exhaustive.metapack";
    if (!std::filesystem::exists(template_path)) {
      const auto schema_path{template_path.parent_path() / "schema.metapack"};
      if (std::filesystem::exists(schema_path)) {
        json_error(request.method(), request.path(), response,
                   request.response_encoding(),
                   sourcemeta::one::STATUS_METHOD_NOT_ALLOWED, "no-template",
                   "This schema was not precompiled for schema evaluation");
      } else {
        json_error(request.method(), request.path(), response,
                   request.response_encoding(),
                   sourcemeta::one::STATUS_NOT_FOUND, "not-found",
                   "There is nothing at this URL");
      }

      return;
    }

    response->onAborted([]() {});
    std::unique_ptr<std::string> buffer;
    // Because `request` gets de-allocated
    std::string url{request.path()};
    const auto encoding{request.response_encoding()};
    response->onData([response, encoding, mode, buffer = std::move(buffer),
                      template_path = std::move(template_path),
                      url = std::move(url)](const std::string_view chunk,
                                            const bool is_last) mutable {
      try {
        if (!buffer.get()) {
          buffer = std::make_unique<std::string>(chunk);
        } else {
          buffer->append(chunk);
        }

        if (is_last) {
          if (buffer->empty()) {
            json_error("post", url, response, encoding,
                       sourcemeta::one::STATUS_BAD_REQUEST, "no-instance",
                       "You must pass an instance to validate against");
          } else {
            const auto result{
                sourcemeta::one::evaluate(template_path, *buffer, mode)};
            response->writeStatus(sourcemeta::one::STATUS_OK);
            response->writeHeader("Content-Type", "application/json");
            response->writeHeader("Access-Control-Allow-Origin", "*");
            if (mode == sourcemeta::one::EvaluateType::Trace) {
              write_link_header(response,
                                "/self/v1/schemas/api/schemas/trace/response");
            } else {
              write_link_header(
                  response, "/self/v1/schemas/api/schemas/evaluate/response");
            }
            std::ostringstream payload;
            sourcemeta::core::prettify(result, payload);
            send_response(sourcemeta::one::STATUS_OK, "post", url, response,
                          payload.str(), encoding,
                          sourcemeta::one::HTTPRequest::Encoding::Identity);
          }
        }
      } catch (const std::exception &error) {
        json_error("post", url, response, encoding,
                   sourcemeta::one::STATUS_METHOD_NOT_ALLOWED, "uncaught-error",
                   error.what());
      }
    });
  } else {
    json_error(request.method(), request.path(), response,
               request.response_encoding(),
               sourcemeta::one::STATUS_METHOD_NOT_ALLOWED, "method-not-allowed",
               "This HTTP method is invalid for this URL");
  }
}

static auto on_schema(const std::filesystem::path &base,
                      const std::string_view &path,
                      const sourcemeta::one::HTTPRequest &request,
                      uWS::HttpResponse<true> *response) -> void {
  // Otherwise we may get unexpected results in case-sensitive file-systems
  std::string lowercase_path{path};
  std::transform(
      lowercase_path.begin(), lowercase_path.end(), lowercase_path.begin(),
      [](const unsigned char character) { return std::tolower(character); });

  // Because Visual Studio Code famously does not support `$id` or `id`
  // See
  // https://github.com/microsoft/vscode-json-languageservice/issues/224
  const auto &user_agent{request.header("user-agent")};
  const auto is_vscode{user_agent.starts_with("Visual Studio Code") ||
                       user_agent.starts_with("VSCodium")};
  const auto is_deno{user_agent.starts_with("Deno/")};
  const auto bundle{!request.query("bundle").empty()};
  auto absolute_path{base / "schemas" / lowercase_path / SENTINEL};
  if (is_vscode) {
    absolute_path /= "editor.metapack";
  } else if (bundle || is_deno) {
    absolute_path /= "bundle.metapack";
  } else {
    absolute_path /= "schema.metapack";
  }

  if (is_deno) {
    serve_static_file(request, response, absolute_path,
                      sourcemeta::one::STATUS_OK, true,
                      // For HTTP imports, as Deno won't like the
                      // `application/schema+json` one
                      "application/json");
  } else {
    serve_static_file(request, response, absolute_path,
                      sourcemeta::one::STATUS_OK, true);
  }
}

static auto handle_root(const std::filesystem::path &base,
                        const std::span<std::string_view>,
                        const sourcemeta::one::HTTPRequest &request,
                        uWS::HttpResponse<true> *response) -> void {
  if (request.prefers_html()) {
    serve_static_file(request, response,
                      base / "explorer" / SENTINEL / "directory-html.metapack",
                      sourcemeta::one::STATUS_OK);
  } else if (request.method() == "get" || request.method() == "head") {
    json_error(request.method(), request.path(), response,
               request.response_encoding(), sourcemeta::one::STATUS_NOT_FOUND,
               "not-found", "There is nothing at this URL");
  } else {
    json_error(request.method(), request.path(), response,
               request.response_encoding(),
               sourcemeta::one::STATUS_METHOD_NOT_ALLOWED, "method-not-allowed",
               "This HTTP method is invalid for this URL");
  }
}

static auto handle_self_v1_api_list(const std::filesystem::path &base,
                                    const std::span<std::string_view>,
                                    const sourcemeta::one::HTTPRequest &request,
                                    uWS::HttpResponse<true> *response) -> void {
  serve_static_file(request, response,
                    base / "explorer" / SENTINEL / "directory.metapack",
                    sourcemeta::one::STATUS_OK, true, std::nullopt,
                    "/self/v1/schemas/api/list/response");
}

static auto
handle_self_v1_api_list_path(const std::filesystem::path &base,
                             const std::span<std::string_view> matches,
                             const sourcemeta::one::HTTPRequest &request,
                             uWS::HttpResponse<true> *response) -> void {
  const auto absolute_path{base / "explorer" / matches.front() / SENTINEL /
                           "directory.metapack"};
  serve_static_file(request, response, absolute_path,
                    sourcemeta::one::STATUS_OK, true, std::nullopt,
                    "/self/v1/schemas/api/list/response");
}

static auto handle_self_v1_api_schemas_dependencies(
    const std::filesystem::path &base,
    const std::span<std::string_view> matches,
    const sourcemeta::one::HTTPRequest &request,
    uWS::HttpResponse<true> *response) -> void {
  if (request.method() == "get" || request.method() == "head") {
    const auto absolute_path{base / "schemas" / matches.front() / SENTINEL /
                             "dependencies.metapack"};
    serve_static_file(request, response, absolute_path,
                      sourcemeta::one::STATUS_OK, true, std::nullopt,
                      "/self/v1/schemas/api/schemas/dependencies/response");
  } else {
    json_error(request.method(), request.path(), response,
               request.response_encoding(),
               sourcemeta::one::STATUS_METHOD_NOT_ALLOWED, "method-not-allowed",
               "This HTTP method is invalid for this URL");
  }
}

static auto
handle_self_v1_api_schemas_health(const std::filesystem::path &base,
                                  const std::span<std::string_view> matches,
                                  const sourcemeta::one::HTTPRequest &request,
                                  uWS::HttpResponse<true> *response) -> void {
  if (request.method() == "get" || request.method() == "head") {
    const auto absolute_path{base / "schemas" / matches.front() / SENTINEL /
                             "health.metapack"};
    serve_static_file(request, response, absolute_path,
                      sourcemeta::one::STATUS_OK, true, std::nullopt,
                      "/self/v1/schemas/api/schemas/health/response");
  } else {
    json_error(request.method(), request.path(), response,
               request.response_encoding(),
               sourcemeta::one::STATUS_METHOD_NOT_ALLOWED, "method-not-allowed",
               "This HTTP method is invalid for this URL");
  }
}

static auto handle_self_v1_api_schemas_locations(
    const std::filesystem::path &base,
    const std::span<std::string_view> matches,
    const sourcemeta::one::HTTPRequest &request,
    uWS::HttpResponse<true> *response) -> void {
  if (request.method() == "get" || request.method() == "head") {
    const auto absolute_path{base / "schemas" / matches.front() / SENTINEL /
                             "locations.metapack"};
    serve_static_file(request, response, absolute_path,
                      sourcemeta::one::STATUS_OK, true, std::nullopt,
                      "/self/v1/schemas/api/schemas/locations/response");
  } else {
    json_error(request.method(), request.path(), response,
               request.response_encoding(),
               sourcemeta::one::STATUS_METHOD_NOT_ALLOWED, "method-not-allowed",
               "This HTTP method is invalid for this URL");
  }
}

static auto handle_self_v1_api_schemas_positions(
    const std::filesystem::path &base,
    const std::span<std::string_view> matches,
    const sourcemeta::one::HTTPRequest &request,
    uWS::HttpResponse<true> *response) -> void {
  if (request.method() == "get" || request.method() == "head") {
    const auto absolute_path{base / "schemas" / matches.front() / SENTINEL /
                             "positions.metapack"};
    serve_static_file(request, response, absolute_path,
                      sourcemeta::one::STATUS_OK, true, std::nullopt,
                      "/self/v1/schemas/api/schemas/positions/response");
  } else {
    json_error(request.method(), request.path(), response,
               request.response_encoding(),
               sourcemeta::one::STATUS_METHOD_NOT_ALLOWED, "method-not-allowed",
               "This HTTP method is invalid for this URL");
  }
}

static auto
handle_self_v1_api_schemas_stats(const std::filesystem::path &base,
                                 const std::span<std::string_view> matches,
                                 const sourcemeta::one::HTTPRequest &request,
                                 uWS::HttpResponse<true> *response) -> void {
  if (request.method() == "get" || request.method() == "head") {
    const auto absolute_path{base / "schemas" / matches.front() / SENTINEL /
                             "stats.metapack"};
    serve_static_file(request, response, absolute_path,
                      sourcemeta::one::STATUS_OK, true, std::nullopt,
                      "/self/v1/schemas/api/schemas/stats/response");
  } else {
    json_error(request.method(), request.path(), response,
               request.response_encoding(),
               sourcemeta::one::STATUS_METHOD_NOT_ALLOWED, "method-not-allowed",
               "This HTTP method is invalid for this URL");
  }
}

static auto
handle_self_v1_api_schemas_metadata(const std::filesystem::path &base,
                                    const std::span<std::string_view> matches,
                                    const sourcemeta::one::HTTPRequest &request,
                                    uWS::HttpResponse<true> *response) -> void {
  if (request.method() == "get" || request.method() == "head") {
    const auto absolute_path{base / "explorer" / matches.front() / SENTINEL /
                             "schema.metapack"};
    serve_static_file(request, response, absolute_path,
                      sourcemeta::one::STATUS_OK, true, std::nullopt,
                      "/self/v1/schemas/api/schemas/metadata/response");
  } else {
    json_error(request.method(), request.path(), response,
               request.response_encoding(),
               sourcemeta::one::STATUS_METHOD_NOT_ALLOWED, "method-not-allowed",
               "This HTTP method is invalid for this URL");
  }
}

static auto
handle_self_v1_api_schemas_evaluate(const std::filesystem::path &base,
                                    const std::span<std::string_view> matches,
                                    const sourcemeta::one::HTTPRequest &request,
                                    uWS::HttpResponse<true> *response) -> void {
  on_evaluate(base, matches.front(), request, response,
              sourcemeta::one::EvaluateType::Standard);
}

static auto
handle_self_v1_api_schemas_trace(const std::filesystem::path &base,
                                 const std::span<std::string_view> matches,
                                 const sourcemeta::one::HTTPRequest &request,
                                 uWS::HttpResponse<true> *response) -> void {
  on_evaluate(base, matches.front(), request, response,
              sourcemeta::one::EvaluateType::Trace);
}

static auto
handle_self_v1_api_schemas_search(const std::filesystem::path &base,
                                  const std::span<std::string_view>,
                                  const sourcemeta::one::HTTPRequest &request,
                                  uWS::HttpResponse<true> *response) -> void {
  if (request.method() == "get") {
    const auto query{request.query("q")};
    if (query.empty()) {
      json_error(request.method(), request.path(), response,
                 request.response_encoding(),
                 sourcemeta::one::STATUS_BAD_REQUEST, "missing-query",
                 "You must provide a query parameter to search for");
    } else {
      auto result{sourcemeta::one::search(
          base / "explorer" / SENTINEL / "search.metapack", query)};
      response->writeStatus(sourcemeta::one::STATUS_OK);
      response->writeHeader("Access-Control-Allow-Origin", "*");
      response->writeHeader("Content-Type", "application/json");
      write_link_header(response,
                        "/self/v1/schemas/api/schemas/search/response");
      std::ostringstream output;
      sourcemeta::core::prettify(result, output);
      send_response(sourcemeta::one::STATUS_OK, request.method(),
                    request.path(), response, output.str(),
                    request.response_encoding(),
                    sourcemeta::one::HTTPRequest::Encoding::Identity);
    }
  } else {
    json_error(request.method(), request.path(), response,
               request.response_encoding(),
               sourcemeta::one::STATUS_METHOD_NOT_ALLOWED, "method-not-allowed",
               "This HTTP method is invalid for this URL");
  }
}

static auto
handle_self_api_not_found(const std::filesystem::path &,
                          const std::span<std::string_view>,
                          const sourcemeta::one::HTTPRequest &request,
                          uWS::HttpResponse<true> *response) -> void {
  json_error(request.method(), request.path(), response,
             request.response_encoding(), sourcemeta::one::STATUS_NOT_FOUND,
             "not-found", "There is nothing at this URL");
}

static auto handle_self_static(const std::filesystem::path &,
                               const std::span<std::string_view> matches,
                               const sourcemeta::one::HTTPRequest &request,
                               uWS::HttpResponse<true> *response) -> void {
  std::ostringstream absolute_path;
  absolute_path << SOURCEMETA_ONE_STATIC;
  absolute_path << '/';
  absolute_path << matches.front();
  serve_static_file(request, response, absolute_path.str(),
                    sourcemeta::one::STATUS_OK);
}

static auto handle_default(const std::filesystem::path &base,
                           const std::span<std::string_view>,
                           const sourcemeta::one::HTTPRequest &request,
                           uWS::HttpResponse<true> *response) -> void {
  if (request.path().ends_with(".json")) {
    on_schema(base, request.path().substr(1, request.path().size() - 6),
              request, response);
    return;
  }

  const auto path{request.path().substr(1)};
  if (request.method() == "get" || request.method() == "head") {
    if (request.prefers_html()) {
      auto absolute_path{base / "explorer" / path / SENTINEL};
      if (std::filesystem::exists(absolute_path / "schema-html.metapack")) {
        serve_static_file(request, response,
                          absolute_path / "schema-html.metapack",
                          sourcemeta::one::STATUS_OK);
      } else {
        absolute_path /= "directory-html.metapack";
        if (std::filesystem::exists(absolute_path)) {
          serve_static_file(request, response, absolute_path,
                            sourcemeta::one::STATUS_OK);
        } else {
          serve_static_file(request, response,
                            base / "explorer" / SENTINEL / "404.metapack",
                            sourcemeta::one::STATUS_NOT_FOUND);
        }
      }
    } else {
      on_schema(base, path, request, response);
    }
  } else {
    json_error(request.method(), request.path(), response,
               request.response_encoding(), sourcemeta::one::STATUS_NOT_FOUND,
               "not-found", "There is nothing at this URL");
  }
}

using Handler = auto (*)(const std::filesystem::path &,
                         const std::span<std::string_view>,
                         const sourcemeta::one::HTTPRequest &,
                         uWS::HttpResponse<true> *) -> void;

static const Handler HANDLERS[] = {handle_default,
                                   handle_root,
                                   handle_self_v1_api_list,
                                   handle_self_v1_api_list_path,
                                   handle_self_v1_api_schemas_dependencies,
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

static auto dispatch(const sourcemeta::core::URITemplateRouter &router,
                     const std::filesystem::path &base,
                     uWS::HttpResponse<true> *const response,
                     uWS::HttpRequest *const raw_request) noexcept -> void {
  sourcemeta::one::HTTPRequest request{raw_request};
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
      json_error(request.method(), request.path(), response,
                 sourcemeta::one::HTTPRequest::Encoding::Identity,
                 sourcemeta::one::STATUS_NOT_ACCEPTABLE,
                 "cannot-satisfy-content-encoding",
                 "The server cannot satisfy the request content encoding");
    }
  } catch (const std::exception &error) {
    json_error(request.method(), request.path(), response,
               sourcemeta::one::HTTPRequest::Encoding::Identity,
               sourcemeta::one::STATUS_METHOD_NOT_ALLOWED, "uncaught-error",
               error.what());
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
    const auto is_headless{!std::filesystem::exists(
        base / "explorer" / SENTINEL / "directory-html.metapack")};

    sourcemeta::core::URITemplateRouter router;
    router.add("/", sourcemeta::one::HANDLER_ROOT);
    router.add("/self/v1/api/list", sourcemeta::one::HANDLER_SELF_V1_API_LIST);
    router.add("/self/v1/api/list/{+path}",
               sourcemeta::one::HANDLER_SELF_V1_API_LIST_PATH);
    router.add("/self/v1/api/schemas/dependencies/{+schema}",
               sourcemeta::one::HANDLER_SELF_V1_API_SCHEMAS_DEPENDENCIES);
    router.add("/self/v1/api/schemas/health/{+schema}",
               sourcemeta::one::HANDLER_SELF_V1_API_SCHEMAS_HEALTH);
    router.add("/self/v1/api/schemas/locations/{+schema}",
               sourcemeta::one::HANDLER_SELF_V1_API_SCHEMAS_LOCATIONS);
    router.add("/self/v1/api/schemas/positions/{+schema}",
               sourcemeta::one::HANDLER_SELF_V1_API_SCHEMAS_POSITIONS);
    router.add("/self/v1/api/schemas/stats/{+schema}",
               sourcemeta::one::HANDLER_SELF_V1_API_SCHEMAS_STATS);
    router.add("/self/v1/api/schemas/metadata/{+schema}",
               sourcemeta::one::HANDLER_SELF_V1_API_SCHEMAS_METADATA);
    router.add("/self/v1/api/schemas/evaluate/{+schema}",
               sourcemeta::one::HANDLER_SELF_V1_API_SCHEMAS_EVALUATE);
    router.add("/self/v1/api/schemas/trace/{+schema}",
               sourcemeta::one::HANDLER_SELF_V1_API_SCHEMAS_TRACE);
    router.add("/self/v1/api/schemas/search",
               sourcemeta::one::HANDLER_SELF_V1_API_SCHEMAS_SEARCH);
    router.add("/self/v1/api/{+any}",
               sourcemeta::one::HANDLER_SELF_V1_API_DEFAULT);

    if (!is_headless) {
      router.add("/self/static/{+path}", sourcemeta::one::HANDLER_SELF_STATIC);
    }

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
