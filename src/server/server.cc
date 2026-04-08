#include <sourcemeta/core/uritemplate.h>

#include <sourcemeta/one/actions.h>

#include <array>       // std::array
#include <cassert>     // assert
#include <chrono>      // std::chrono::steady_clock, std::chrono::milliseconds
#include <csignal>     // std::signal, SIGINT, SIGTERM
#include <cstdint>     // std::uint16_t
#include <cstdlib>     // EXIT_FAILURE, std::exit
#include <filesystem>  // std::filesystem
#include <iostream>    // std::cout, std::cerr
#include <limits>      // std::numeric_limits
#include <string>      // std::string, std::to_string
#include <string_view> // std::string_view

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
      std::size_t matches_size{0};
      const auto match_result{
          router.match(request.path(), [&matches_size](const auto index, auto,
                                                       const auto value) {
            matches[index] = value;
            matches_size = static_cast<std::size_t>(index) + 1;
          })};

      sourcemeta::one::dispatch_action(match_result.first, router, base,
                                       std::span{matches.data(), matches_size},
                                       request, response);
    } else {
      sourcemeta::one::json_error(
          request, response, sourcemeta::one::STATUS_NOT_ACCEPTABLE,
          "cannot-satisfy-content-encoding",
          "The server cannot satisfy the request content encoding",
          // TODO: This implies the API is mounted
          "/self/v1/schemas/api/error");
    }
  } catch (const std::exception &error) {
    sourcemeta::one::json_error(request, response,
                                sourcemeta::one::STATUS_INTERNAL_SERVER_ERROR,
                                "uncaught-error", error.what(),
                                // TODO: This implies the API is mounted
                                "/self/v1/schemas/api/error");
  } catch (...) {
    sourcemeta::one::json_error(
        request, response, sourcemeta::one::STATUS_INTERNAL_SERVER_ERROR,
        "uncaught-error", "An unknown unexpected error occurred",
        // TODO: This implies the API is mounted
        "/self/v1/schemas/api/error");
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

    const auto port{static_cast<std::uint16_t>(std::stoul(argv[2]))};
    assert(port > 0);

    // Note we purposely DO NOT canonicalise in order to NOT resolve
    // symlinks in the output location
    const std::filesystem::path base{argv[1]};
    if (!base.is_absolute()) [[unlikely]] {
      std::cout << "The output directory path must be absolute\n";
      return EXIT_FAILURE;
    }

    const sourcemeta::core::URITemplateRouterView router{base / "routes.bin"};

    sourcemeta::one::HTTPServer(
        port,
        [&router, &base](sourcemeta::one::HTTPRequest &request,
                         sourcemeta::one::HTTPResponse &response) {
          dispatch(router, base, request, response);
        },
        [timestamp_start](const std::uint16_t bound_port) {
          const auto duration{
              std::chrono::duration_cast<std::chrono::milliseconds>(
                  std::chrono::steady_clock::now() - timestamp_start)};
          sourcemeta::one::HTTP_LOG("Listening on port " +
                                    std::to_string(bound_port) + " in " +
                                    std::to_string(duration.count()) + " ms");
        },
        [](const std::uint16_t requested_port) {
          sourcemeta::one::HTTP_LOG("Failed to listen on port " +
                                    std::to_string(requested_port));
        });

    sourcemeta::one::HTTP_LOG("The server could not start");
    return EXIT_FAILURE;
  } catch (const std::exception &error) {
    std::cerr << "unexpected error: " << error.what() << "\n";
    return EXIT_FAILURE;
  }
}
