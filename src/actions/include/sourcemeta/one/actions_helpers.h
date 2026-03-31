#ifndef SOURCEMETA_ONE_ACTIONS_HELPERS_H
#define SOURCEMETA_ONE_ACTIONS_HELPERS_H

#include <sourcemeta/core/json.h>
#include <sourcemeta/core/time.h>

#include <sourcemeta/one/http.h>
#include <sourcemeta/one/shared.h>

#include <cassert>     // assert
#include <chrono>      // std::chrono::system_clock
#include <format>      // std::format
#include <mutex>       // std::mutex, std::lock_guard
#include <print>       // std::print
#include <sstream>     // std::ostringstream
#include <string>      // std::string
#include <string_view> // std::string_view
#include <thread>      // std::this_thread
#include <utility>     // std::move

constexpr auto SENTINEL{"%"};

inline auto write_link_header(sourcemeta::one::HTTPResponse &response,
                              const std::string_view schema_path) -> void {
  response.write_header("Link",
                        std::format("<{}>; rel=\"describedby\"", schema_path));
}

inline auto log(std::string_view message) -> void {
  // Otherwise we can get messed up output interleaved from multiple threads
  static std::mutex log_mutex;
  std::lock_guard<std::mutex> guard{log_mutex};
  std::print(stderr, "[{}] {} {}\n",
             sourcemeta::core::to_gmt(std::chrono::system_clock::now()),
             std::this_thread::get_id(), message);
}

inline auto send_response(const char *const code,
                          const sourcemeta::one::HTTPRequest &request,
                          sourcemeta::one::HTTPResponse &response) -> void {
  response.send_without_content();
  assert(code);
  log(std::format("{} {} {}", code, request.method(), request.path()));
}

inline auto send_response(const char *const code,
                          const sourcemeta::one::HTTPRequest &request,
                          sourcemeta::one::HTTPResponse &response,
                          const std::string &message,
                          const sourcemeta::one::Encoding current_encoding)
    -> void {
  response.send(request, message, current_encoding);
  assert(code);
  log(std::format("{} {} {}", code, request.method(), request.path()));
}

// See https://www.rfc-editor.org/rfc/rfc7807
inline auto json_error(const sourcemeta::one::HTTPRequest &request,
                       sourcemeta::one::HTTPResponse &response,
                       const char *const code, std::string &&id,
                       std::string &&message) -> void {
  auto object{sourcemeta::core::JSON::make_object()};
  object.assign("title",
                // A URI with a custom scheme
                sourcemeta::core::JSON{"sourcemeta:one/" + std::move(id)});
  object.assign("status", sourcemeta::core::JSON{std::atoi(code)});
  object.assign("detail", sourcemeta::core::JSON{std::move(message)});
  response.write_status(code);
  response.write_header("Content-Type", "application/problem+json");
  response.write_header("Access-Control-Allow-Origin", "*");
  write_link_header(response, "/self/v1/schemas/api/error");

  std::ostringstream output;
  sourcemeta::core::prettify(object, output);
  send_response(code, request, response, output.str(),
                sourcemeta::one::Encoding::Identity);
}

#endif
