#ifndef SOURCEMETA_ONE_HTTP_HELPERS_H
#define SOURCEMETA_ONE_HTTP_HELPERS_H

#include <sourcemeta/core/http.h>
#include <sourcemeta/core/json.h>
#include <sourcemeta/core/time.h>

#include <sourcemeta/one/http_request.h>
#include <sourcemeta/one/http_response.h>

#include <cassert>     // assert
#include <chrono>      // std::chrono::system_clock
#include <format>      // std::format
#include <mutex>       // std::mutex, std::lock_guard
#include <print>       // std::print
#include <sstream>     // std::ostringstream
#include <string>      // std::string
#include <string_view> // std::string_view
#include <thread>      // std::this_thread

namespace sourcemeta::one {

inline auto HTTP_LOG(const std::string_view message) -> void {
  static std::mutex log_mutex;
  std::lock_guard<std::mutex> guard{log_mutex};
  std::print(stderr, "[{}] {} {}\n",
             sourcemeta::core::to_imf_fixdate(std::chrono::system_clock::now()),
             std::this_thread::get_id(), message);
}

inline auto write_link_header(HTTPResponse &response,
                              const std::string_view schema_path) -> void {
  response.write_header("Link",
                        sourcemeta::core::http_format_link(
                            {.target = schema_path, .rel = "describedby"}));
}

inline auto send_response(const sourcemeta::core::HTTPStatus &status,
                          const HTTPRequest &request, HTTPResponse &response)
    -> void {
  response.send_without_content();
  HTTP_LOG(
      std::format("{} {} {}", status.wire, request.method(), request.path()));
}

inline auto send_response(const sourcemeta::core::HTTPStatus &status,
                          const HTTPRequest &request, HTTPResponse &response,
                          const std::string &message,
                          const Encoding current_encoding) -> void {
  response.send(request, message, current_encoding);
  HTTP_LOG(
      std::format("{} {} {}", status.wire, request.method(), request.path()));
}

// CORS scope is required at every error site. No default for `origin` so a
// caller cannot silently widen a restricted-origin handler to wildcard. An
// empty origin means the route is CORS-disabled and no Allow-Origin or
// Expose-Headers should appear on the error response.
inline auto json_error(const HTTPRequest &request, HTTPResponse &response,
                       const sourcemeta::core::HTTPStatus &status,
                       const std::string_view type,
                       const std::string_view detail,
                       const std::string_view schema,
                       const std::string_view origin,
                       const std::string_view allow = {}) -> void {
  // Header values are written to the wire verbatim. CR/LF would split
  // headers, enabling response header injection or CORS widening. Today's
  // callers pass string literals, but the asserts catch future untrusted
  // forwards.
  assert(origin.find_first_of("\r\n") == std::string_view::npos);
  assert(allow.find_first_of("\r\n") == std::string_view::npos);
  const auto body{sourcemeta::core::http_make_problem_details(
      {.status = status, .type = type, .detail = detail})};

  response.write_status(status);
  response.write_header("Content-Type", "application/problem+json");
  if (!origin.empty()) {
    response.write_header("Access-Control-Allow-Origin", origin);
    response.write_header("Access-Control-Expose-Headers", "Link, ETag");
    // RFC 9110 §12.5.5: when the response origin is anything other than
    // the wildcard, CORS-aware caches must key on the request's Origin
    // header. Otherwise origin A's cached response can be served to
    // origin B.
    // https://datatracker.ietf.org/doc/html/rfc9110#section-12.5.5
    if (origin != "*") {
      response.write_header("Vary", "Origin");
    }
  }
  // RFC 9110 §15.5.6: 405 responses MUST carry Allow listing supported methods.
  // https://datatracker.ietf.org/doc/html/rfc9110#section-15.5.6
  if (!allow.empty() &&
      status == sourcemeta::core::HTTP_STATUS_METHOD_NOT_ALLOWED) {
    response.write_header("Allow", allow);
  }
  if (!schema.empty()) {
    write_link_header(response, schema);
  }

  std::ostringstream output;
  sourcemeta::core::prettify(body, output);
  send_response(status, request, response, output.str(), Encoding::Identity);
}

} // namespace sourcemeta::one

#endif
