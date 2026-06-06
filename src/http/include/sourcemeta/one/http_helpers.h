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

// True when every media type in `media_types` is individually acceptable
// under the Accept header per RFC 9110 §12.5.1 (q-aware, wildcard-aware).
// An empty Accept header means "accept any" per the spec, so this returns
// true. Callers that need to reject an absent Accept header should check
// `header.empty()` separately. Allocation-free.
// TODO: Upstream to `core/http` and replace the per-element `http_match_accept`
// call with a single-pass parse of the Accept header. The current impl
// re-parses the header once per required type; the upstream version would parse
// once and answer membership for all types in one walk.
inline auto http_accept_includes_all(
    const std::string_view header,
    std::initializer_list<std::string_view> media_types) noexcept -> bool {
  for (const auto media_type : media_types) {
    if (sourcemeta::core::http_match_accept(header, {media_type}).empty()) {
      return false;
    }
  }
  return true;
}

// True if the Content-Type header denotes the given media type. Strips any
// parameters (RFC 9110 §5.6.6 permits OWS on either side of the `;`) and
// compares the bare media-type case-insensitively per RFC 9110 §8.3.1.
// Allocation-free.
// TODO: Upstream to `core/http`.
inline auto
http_content_type_matches(std::string_view header,
                          const std::string_view media_type) noexcept -> bool {
  if (const auto semicolon{header.find(';')};
      semicolon != std::string_view::npos) {
    header = header.substr(0, semicolon);
  }
  // RFC 9110 §5.6.3: OWS = *( SP / HTAB )
  while (!header.empty() && (header.back() == ' ' || header.back() == '\t')) {
    header.remove_suffix(1);
  }
  while (!header.empty() && (header.front() == ' ' || header.front() == '\t')) {
    header.remove_prefix(1);
  }
  if (header.size() != media_type.size()) {
    return false;
  }
  for (std::size_t index{0}; index < header.size(); ++index) {
    const auto left{static_cast<unsigned char>(header[index])};
    const auto right{static_cast<unsigned char>(media_type[index])};
    const auto left_lower{(left >= 'A' && left <= 'Z')
                              ? static_cast<unsigned char>(left + 32)
                              : left};
    const auto right_lower{(right >= 'A' && right <= 'Z')
                               ? static_cast<unsigned char>(right + 32)
                               : right};
    if (left_lower != right_lower) {
      return false;
    }
  }
  return true;
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
