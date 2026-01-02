#ifndef SOURCEMETA_ONE_SERVER_REQUEST_H
#define SOURCEMETA_ONE_SERVER_REQUEST_H

#include <sourcemeta/core/time.h>

#include "response.h"
#include "uwebsockets.h"

#include <algorithm>   // std::sort
#include <chrono>      // std::chrono::system_clock
#include <exception>   // std::exception_ptr, std::current_exception
#include <memory>      // std::shared_ptr, std::make_shared
#include <optional>    // std::optional
#include <sstream>     // std::istringstream
#include <stdexcept>   // std::invalid_argument
#include <string>      // std::string, std::getline
#include <string_view> // std::string_view
#include <utility>     // std::pair
#include <vector>      // std::vector

namespace sourcemeta::one {

class HTTPRequest {
public:
  // Primary constructor from raw uWebSockets pointers
  HTTPRequest(uWS::HttpRequest *request,
              uWS::HttpResponse<true> *response) noexcept
      : request_{request}, response_{response} {}

  // Snapshot constructor for async contexts where uWS::HttpRequest is gone
  HTTPRequest(std::string method, std::string path,
              sourcemeta::one::Encoding encoding,
              uWS::HttpResponse<true> *response) noexcept
      : request_{nullptr}, response_{response}, method_{std::move(method)},
        path_{std::move(path)}, response_encoding_{encoding} {}

  // If the identity;q=0 or *;q=0 directives explicitly forbid the identity
  // encoding, the server should return a 406 Not Acceptable error. See
  // https://developer.mozilla.org/en-US/docs/Web/HTTP/Headers/Accept-Encoding
  auto negotiate() -> void {
    for (const auto &rule : this->header_list("accept-encoding")) {
      // Skip any encoding explicitly disallowed by the client (q=0)
      if (rule.second == 0.0f) {
        // If the client explicitly prohibited identity or *, we can't satisfy
        if (rule.first == "*" || rule.first == "identity") {
          this->satisfiable_encoding_ = false;
          break;
        }
        continue;
      }

      if (rule.first == "identity") {
        break;
      } else if (
          rule.first == "*" || rule.first == "gzip" ||
          // For compatibility with previous implementations of HTTP,
          // applications SHOULD consider "x-gzip" [...] to be
          // equivalent to "gzip". See
          // https://www.w3.org/Protocols/rfc2616/rfc2616-sec3.html#sec3.5
          rule.first == "x-gzip") {
        this->response_encoding_ = sourcemeta::one::Encoding::GZIP;
        break;
      }
    }
  }

  auto method() const noexcept -> std::string_view {
    return this->request_ ? this->request_->getMethod() : this->method_;
  }

  auto path() const noexcept -> std::string_view {
    return this->request_ ? this->request_->getUrl() : this->path_;
  }

  auto header(const std::string_view name) const noexcept -> std::string_view {
    return this->request_->getHeader(name);
  }

  auto query(const std::string_view name) const noexcept -> std::string_view {
    return this->request_->getQuery(name);
  }

  auto header_gmt(const std::string_view name) const noexcept
      -> std::optional<std::chrono::system_clock::time_point> {
    const auto value{this->header(name)};
    if (value.empty()) {
      return std::nullopt;
    }

    try {
      return sourcemeta::core::from_gmt(std::string{value});
    } catch (const std::invalid_argument &) {
      return std::nullopt;
    }
  }

  // A header list element consists of the element value and its quality value
  // See https://developer.mozilla.org/en-US/docs/Glossary/Quality_values
  auto header_list(const std::string_view name) const
      -> std::vector<std::pair<std::string, float>> {
    const auto value{this->header(name)};
    if (value.empty()) {
      return {};
    }

    std::istringstream stream{std::string{value}};
    std::string token;
    std::vector<std::pair<std::string, float>> result;

    while (std::getline(stream, token, ',')) {
      const std::size_t start{token.find_first_not_of(" ")};
      const std::size_t end{token.find_last_not_of(" ")};
      if (start == std::string::npos || end == std::string::npos) {
        continue;
      }

      // See https://developer.mozilla.org/en-US/docs/Glossary/Quality_values
      const std::size_t value_start{token.find_first_of(";")};
      if (value_start != std::string::npos && value_start + 3 < token.size() &&
          token[value_start + 1] == 'q' && token[value_start + 2] == '=') {
        result.emplace_back(token.substr(start, value_start - start),
                            std::stof(token.substr(value_start + 3)));
      } else if (value_start != std::string::npos) {
        // Malformed quality value, treat as default 1.0
        result.emplace_back(token.substr(start, value_start - start), 1.0f);
      } else {
        // No quality value is 1.0 by default
        result.emplace_back(token.substr(start, end - start + 1), 1.0f);
      }
    }

    // For convenience, automatically sort by the quality value
    std::sort(result.begin(), result.end(),
              [](const auto &left, const auto &right) {
                return left.second > right.second;
              });

    return result;
  }

  auto prefers_html() const noexcept -> bool {
    // TODO: We probably want to take Accept sequences and q= into account
    return this->header("accept").find("text/html") != std::string_view::npos;
  }

  auto satisfiable_encoding() const noexcept -> bool {
    return this->satisfiable_encoding_;
  }

  auto response_encoding() const noexcept -> sourcemeta::one::Encoding {
    return this->response_encoding_;
  }

  // Read the entire request body asynchronously.
  // - callback: Invoked with (response, body, too_big) on completion
  // - on_error: Invoked with (response, exception_ptr) on any exception,
  //   including exceptions thrown by the main callback
  // - max_size: Maximum body size in bytes (default 1MB)
  // Note: If the request is aborted, the callback is not invoked
  template <typename Callback, typename ErrorCallback>
  auto body(Callback callback, ErrorCallback on_error,
            std::size_t max_size = 1024 * 1024) -> void {
    auto raw_response = this->response_;
    auto snapshot = std::make_shared<HTTPRequest>(
        std::string{this->method()}, std::string{this->path()},
        this->response_encoding_, raw_response);
    auto buffer = std::make_shared<std::string>();
    auto completed = std::make_shared<bool>(false);

    raw_response->onAborted([completed]() mutable { *completed = true; });

    raw_response->onData(
        [raw_response, snapshot, buffer, completed, max_size, callback,
         on_error](std::string_view chunk, bool is_last) mutable {
          if (*completed) {
            return;
          }

          try {
            if (buffer->size() + chunk.size() > max_size) {
              *completed = true;
              HTTPResponse response{raw_response};
              try {
                callback(*snapshot, response, std::move(*buffer), true);
              } catch (...) {
                on_error(*snapshot, response, std::current_exception());
              }

              return;
            }

            buffer->append(chunk);

            if (is_last) {
              *completed = true;
              HTTPResponse response{raw_response};
              try {
                callback(*snapshot, response, std::move(*buffer), false);
              } catch (...) {
                on_error(*snapshot, response, std::current_exception());
              }
            }
          } catch (...) {
            *completed = true;
            HTTPResponse response{raw_response};
            on_error(*snapshot, response, std::current_exception());
          }
        });
  }

private:
  uWS::HttpRequest *request_;
  uWS::HttpResponse<true> *response_;
  std::string method_;
  std::string path_;
  bool satisfiable_encoding_{true};
  sourcemeta::one::Encoding response_encoding_{
      sourcemeta::one::Encoding::Identity};
};

} // namespace sourcemeta::one

#endif
