#include <sourcemeta/core/numeric.h>
#include <sourcemeta/core/preprocessor.h>
#include <sourcemeta/core/stacktrace.h>
#include <sourcemeta/core/uritemplate.h>

#include <sourcemeta/one/actions.h>

#include <array>       // std::array
#include <cassert>     // assert
#include <chrono>      // std::chrono::steady_clock, std::chrono::milliseconds
#include <csignal>     // std::signal, SIGINT, SIGTERM
#include <cstddef>     // std::size_t
#include <cstdint>     // std::uint16_t
#include <cstdio>      // std::setvbuf, stderr, _IOLBF
#include <cstdlib>     // EXIT_FAILURE, EXIT_SUCCESS
#include <filesystem>  // std::filesystem
#include <iostream>    // std::cerr
#include <limits>      // std::numeric_limits
#include <print>       // std::print, std::println
#include <string>      // std::string, std::to_string
#include <string_view> // std::string_view
#include <unistd.h>    // write, STDERR_FILENO

// TODO: Maybe we should merge this entire function into `Router`?
static auto dispatch(sourcemeta::one::Router &actions,
                     const sourcemeta::core::URITemplateRouterView &router,
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

      actions.dispatch(match_result.first, match_result.second,
                       std::span{matches.data(), matches_size}, request,
                       response);
    } else {
      actions.error(
          request, response, sourcemeta::core::HTTP_STATUS_NOT_ACCEPTABLE,
          "urn:sourcemeta:one:cannot-satisfy-content-encoding",
          "The server cannot satisfy the request content encoding", "*");
    }
  } catch (const std::exception &error) {
    actions.error(request, response,
                  sourcemeta::core::HTTP_STATUS_INTERNAL_SERVER_ERROR,
                  "urn:sourcemeta:one:uncaught-error", error.what(), "*");
  } catch (...) {
    actions.error(request, response,
                  sourcemeta::core::HTTP_STATUS_INTERNAL_SERVER_ERROR,
                  "urn:sourcemeta:one:uncaught-error",
                  "An unknown unexpected error occurred", "*");
  }
}

// POSIX.1-2017 §2.4.3 lists the functions guaranteed safe to call
// from a signal handler. `std::cerr <<` and `std::exit` are not on
// that list. Restrict the handler to `write(2)` plus an atomic flag
// store. The real shutdown work (closing apps, joining workers)
// happens in the watcher thread inside `HTTPServer`, which polls
// the flag from normal context.
extern "C" auto terminate(int /* signal */) noexcept -> void {
  static constexpr std::string_view message{
      "Terminating on requested signal\n"};
  // Best-effort. If the write fails (interrupted, EPIPE, etc.) we
  // still proceed with the cooperative shutdown via the flag.
  (void)::write(STDERR_FILENO, message.data(), message.size());
  sourcemeta::one::HTTPServer::request_stop();
}

SOURCEMETA_FORCEINLINE inline auto print_usage(const std::string_view program)
    -> void {
  std::println("Usage: {} <path/to/output/directory> <port>", program);
}

// We try to keep this function as straight to the point as possible
// with minimal input validation (outside debug builds). The intention
// is for the server to start running and bind to the port as quickly
// as possible, so we can take better advantage of scale-to-zero.
auto main(int argc, char *argv[]) noexcept -> int {
  const auto timestamp_start{std::chrono::steady_clock::now()};
  sourcemeta::core::stacktrace_on_crash();

  // Default stderr is unbuffered, so every `std::print` fragment turns
  // into its own `write` syscall. Line-buffer it so each request log
  // line lands on stderr as one syscall instead of several.
  std::setvbuf(stderr, nullptr, _IOLBF, 0);

  // Mainly for Docker Compose
  std::signal(SIGINT, terminate);
  std::signal(SIGTERM, terminate);

  try {
    const auto program{std::filesystem::path{argv[0]}.filename().string()};

    std::println("Sourcemeta One {} v{}", sourcemeta::one::edition(),
                 sourcemeta::one::version());

    if (argc != 3) [[unlikely]] {
      print_usage(program);
      return EXIT_FAILURE;
    }

    const std::string_view port_argument{argv[2]};
    const auto parsed_port{sourcemeta::core::to_uint16_t(port_argument)};
    if (!parsed_port.has_value() || parsed_port.value() == 0) [[unlikely]] {
      print_usage(program);
      std::println("error: The port must be a valid TCP port");
      return EXIT_FAILURE;
    }

    const auto port{parsed_port.value()};

    // Note we purposely DO NOT canonicalise in order to NOT resolve
    // symlinks in the output location
    const std::filesystem::path base{argv[1]};
    if (!base.is_absolute()) [[unlikely]] {
      print_usage(program);
      std::println("error: The output directory path must be absolute");
      return EXIT_FAILURE;
    }

    const sourcemeta::core::URITemplateRouterView router{base / "routes.bin"};
    sourcemeta::one::Router actions{base, router,
                                    sourcemeta::one::CONSTRUCTORS};

    sourcemeta::one::HTTPServer(
        port,
        [&actions, &router](sourcemeta::one::HTTPRequest &request,
                            sourcemeta::one::HTTPResponse &response) {
          dispatch(actions, router, request, response);
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

    // The constructor returns either because all workers exited the
    // event loop (cooperative shutdown via `request_stop`) or because
    // none of them ever managed to bind the requested port. Only the
    // latter is a failure.
    if (sourcemeta::one::HTTPServer::stop_requested()) {
      sourcemeta::one::HTTP_LOG("The server stopped gracefully");
      return EXIT_SUCCESS;
    }
    sourcemeta::one::HTTP_LOG("The server could not start");
    return EXIT_FAILURE;
  } catch (const std::exception &error) {
    std::cerr << "unexpected error: " << error.what() << "\n";
    return EXIT_FAILURE;
  }
}
