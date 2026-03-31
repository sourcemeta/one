#include <sourcemeta/core/uritemplate.h>

#include <sourcemeta/one/actions.h>

#include <array>       // std::array
#include <cassert>     // assert
#include <chrono>      // std::chrono::steady_clock, std::chrono::milliseconds
#include <csignal>     // std::signal, SIGINT, SIGTERM
#include <cstdint>     // std::uint16_t, std::uint32_t, std::stoul
#include <cstdlib>     // EXIT_FAILURE, std::exit
#include <filesystem>  // std::filesystem
#include <iostream>    // std::cout, std::cerr
#include <limits>      // std::numeric_limits
#include <string>      // std::string, std::to_string
#include <string_view> // std::string_view

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
                                   handle_self_static,
                                   handle_self_v1_health};

static auto dispatch(const sourcemeta::core::URITemplateRouterView &router,
                     const std::filesystem::path &base,
                     sourcemeta::one::HTTPRequest &request,
                     sourcemeta::one::HTTPResponse &response) noexcept -> void {
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

      // For backwards compatibility in case the generated routes
      // don't match this server version
      if (handler >= std::size(HANDLERS)) [[unlikely]] {
        json_error(
            request, response, sourcemeta::one::STATUS_NOT_IMPLEMENTED,
            "unknown-handler-code",
            "This server version does not implement the handler for this URL");
      } else {
        HANDLERS[handler](base, matches, request, response);
      }
    } else {
      json_error(request, response, sourcemeta::one::STATUS_NOT_ACCEPTABLE,
                 "cannot-satisfy-content-encoding",
                 "The server cannot satisfy the request content encoding");
    }
  } catch (const std::exception &error) {
    json_error(request, response, sourcemeta::one::STATUS_INTERNAL_SERVER_ERROR,
               "uncaught-error", error.what());
  } catch (...) {
    json_error(request, response, sourcemeta::one::STATUS_INTERNAL_SERVER_ERROR,
               "uncaught-error", "An unknown unexpected error occurred");
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

  std::cout << "Sourcemeta One " << sourcemeta::one::edition() << " v"
            << sourcemeta::one::version() << "\n";

  // Mainly for Docker Compose
  std::signal(SIGINT, terminate);
  std::signal(SIGTERM, terminate);

  try {
    if (argc != 3) [[unlikely]] {
      std::cout << "Usage: " << argv[0]
                << " <path/to/output/directory> <port>\n";
      return EXIT_FAILURE;
    }

    const auto port{static_cast<std::uint32_t>(std::stoul(argv[2]))};

    // Note we purposely DO NOT canonicalise in order to NOT resolve
    // symlinks in the output location
    const std::filesystem::path base{argv[1]};
    if (!base.is_absolute()) [[unlikely]] {
      std::cout << "The output directory path must be absolute\n";
      return EXIT_FAILURE;
    }

    sourcemeta::one::static_asset_path = SOURCEMETA_ONE_STATIC;

    const sourcemeta::core::URITemplateRouterView router{base / "routes.bin"};

    sourcemeta::one::HTTPServer([&router, &base, port, timestamp_start](
                                    sourcemeta::one::HTTPApp &app) -> void {
      app.on_request([&router, &base](sourcemeta::one::HTTPRequest &request,
                                      sourcemeta::one::HTTPResponse &response) {
        dispatch(router, base, request, response);
      });

      app.listen(
          static_cast<std::uint16_t>(port),
          [port, timestamp_start](const std::uint16_t bound_port) -> void {
            const auto timestamp_end{std::chrono::steady_clock::now()};
            const auto duration{
                std::chrono::duration_cast<std::chrono::milliseconds>(
                    timestamp_end - timestamp_start)};
            if (bound_port > 0) {
              assert(port == static_cast<std::uint32_t>(bound_port));
              log("Listening on port " + std::to_string(bound_port) + " in " +
                  std::to_string(duration.count()) + " ms");
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
