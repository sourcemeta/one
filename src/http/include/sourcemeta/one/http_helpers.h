#ifndef SOURCEMETA_ONE_HTTP_HELPERS_H
#define SOURCEMETA_ONE_HTTP_HELPERS_H

#include <sourcemeta/core/http.h>
#include <sourcemeta/core/json.h>
#include <sourcemeta/core/numeric.h>
#include <sourcemeta/core/text.h>
#include <sourcemeta/core/time.h>

#include <sourcemeta/one/http_request.h>
#include <sourcemeta/one/http_response.h>

#include <algorithm>   // std::ranges::equal
#include <cassert>     // assert
#include <chrono>      // std::chrono::system_clock
#include <cstddef>     // std::size_t
#include <format>      // std::format
#include <mutex>       // std::mutex, std::lock_guard
#include <optional>    // std::optional
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

// RFC 9110 §10.1.1: any expectation other than `100-continue` is
// unsupported. uWS auto-acknowledges `100-continue` via router
// middleware before our handler runs, so by the time we read the
// `Expect` header here, the only values that can still appear are
// either empty or something we cannot honour. The expectation
// token is case-insensitive per the same section, so case-fold
// the inbound value before the compare.
inline auto expect_header_unrecognised(const HTTPRequest &request) -> bool {
  const auto expect{request.header("expect")};
  return !expect.empty() &&
         !std::ranges::equal(expect, std::string_view{"100-continue"},
                             [](const char left, const char right) {
                               return sourcemeta::core::to_lowercase(left) ==
                                      right;
                             });
}

// RFC 9110 §8.6 + §15.5.14: if the client declares a `Content-Length`
// that already exceeds the inbound body cap, refuse before reading
// the body. uWS has already sent its automatic `100 Continue` for
// `Expect: 100-continue` requests, but well-behaved clients abort
// their upload on a mid-stream 4xx, so the fast-fail still saves
// both sides bandwidth versus reading bytes until the cap trips.
inline auto request_body_too_large(const HTTPRequest &request) -> bool {
  const auto declared{
      sourcemeta::core::to_uint64_t(request.header("content-length"))};
  return declared.has_value() && declared.value() > MAX_REQUEST_BODY_BYTES;
}

inline auto send_response(const sourcemeta::core::HTTPStatus &status,
                          const HTTPRequest &request, HTTPResponse &response)
    -> void {
  response.send_without_content();
  HTTP_LOG(
      std::format("{} {} {}", status.wire, request.method(), request.path()));
}

inline auto send_response(
    const sourcemeta::core::HTTPStatus &status, const HTTPRequest &request,
    HTTPResponse &response, const std::string &message,
    const Encoding current_encoding,
    const std::optional<std::size_t> precomputed_compressed_size = std::nullopt)
    -> void {
  response.send(request, message, current_encoding,
                precomputed_compressed_size);
  HTTP_LOG(
      std::format("{} {} {}", status.wire, request.method(), request.path()));
}

// RFC 9110 §9.3.7: OPTIONS responses describe communication options
// for the target resource. Fetch §3.2.2 (CORS preflight): non-simple
// cross-origin requests issue an OPTIONS preflight whose ACK shape
// (status 204 + Access-Control-Allow-*) governs whether the actual
// request fires. The per-surface `allow_methods` and `allow_headers`
// are required so each action declares its own contract explicitly,
// matching the K and L disciplines.
// https://datatracker.ietf.org/doc/html/rfc9110#section-9.3.7
// https://fetch.spec.whatwg.org/#cors-preflight-fetch
inline auto cors_preflight(const HTTPRequest &request, HTTPResponse &response,
                           const std::string_view allow_methods,
                           const std::string_view allow_headers) -> void {
  assert(!allow_methods.empty());
  assert(!allow_headers.empty());
  assert(allow_methods.find_first_of("\r\n") == std::string_view::npos);
  assert(allow_headers.find_first_of("\r\n") == std::string_view::npos);
  response.write_status(sourcemeta::core::HTTP_STATUS_NO_CONTENT);
  response.write_header("Access-Control-Allow-Origin", "*");
  response.write_header("Access-Control-Expose-Headers", "Link, ETag");
  response.write_header("Access-Control-Allow-Methods", allow_methods);
  response.write_header("Access-Control-Allow-Headers", allow_headers);
  response.write_header("Access-Control-Max-Age", "3600");
  // Browser preflight cache is governed by `Access-Control-Max-Age`;
  // `no-store` keeps shared HTTP caches from storing this response.
  response.write_header("Cache-Control", "no-store");
  // RFC 9110 §9.3.7: OPTIONS responses SHOULD include Allow. Different
  // audience than Access-Control-Allow-Methods (HTTP vs CORS preflight).
  response.write_header("Allow", allow_methods);
  send_response(sourcemeta::core::HTTP_STATUS_NO_CONTENT, request, response);
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
  // RFC 9111 §5.2.2.5: a stale error response is a footgun for both
  // shared caches and the client. The error condition is dynamic
  // (the request shape, server state, the moment) and a 500 cached
  // even briefly turns a transient hiccup into a sticky outage.
  // Apply uniformly across every error envelope.
  response.write_header("Cache-Control", "no-store");
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
