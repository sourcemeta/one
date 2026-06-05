#ifndef SOURCEMETA_ONE_HTTP_REQUEST_H
#define SOURCEMETA_ONE_HTTP_REQUEST_H

#include <sourcemeta/core/time.h>
#include <sourcemeta/core/uri.h>

#include <sourcemeta/one/http_response.h>
#include <sourcemeta/one/http_uwebsockets.h>

#include <algorithm>   // std::ranges::sort
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

  // RFC 9110 §12.5.3:
  // - Identity is acceptable by default unless explicitly excluded by
  //   `identity;q=0` or by `*;q=0` without a specific identity entry.
  // - The server may return 406 Not Acceptable when no representation is
  //   acceptable. See
  //   https://developer.mozilla.org/en-US/docs/Web/HTTP/Headers/Accept-Encoding
  //
  // We prefer gzip by design when both gzip and identity are acceptable at
  // equal effective q-value as the indexer gzip's artifacts by default.
  // For "x-gzip" interop see
  // https://www.w3.org/Protocols/rfc2616/rfc2616-sec3.html#sec3.5
  auto negotiate() -> void {
    // TODO: Upstream this complex logic into Core
    bool identity_listed{false};
    bool identity_excluded{false};
    bool gzip_listed{false};
    bool wildcard_listed{false};
    bool wildcard_excluded{false};
    float gzip_best_q{0.0f};
    float identity_best_q{0.0f};
    float wildcard_best_q{0.0f};

    for (const auto &rule : this->header_list("accept-encoding")) {
      if (rule.first == "identity") {
        identity_listed = true;
        if (rule.second == 0.0f) {
          identity_excluded = true;
        } else {
          identity_best_q = std::max(identity_best_q, rule.second);
        }
      } else if (rule.first == "gzip" || rule.first == "x-gzip") {
        gzip_listed = true;
        if (rule.second > 0.0f) {
          gzip_best_q = std::max(gzip_best_q, rule.second);
        }
      } else if (rule.first == "*") {
        wildcard_listed = true;
        if (rule.second == 0.0f) {
          wildcard_excluded = true;
        } else {
          wildcard_best_q = std::max(wildcard_best_q, rule.second);
        }
      }
    }

    // Wildcard supplies q for codings not explicitly listed
    if (!gzip_listed && wildcard_best_q > 0.0f) {
      gzip_best_q = wildcard_best_q;
    }
    if (!identity_listed) {
      if (wildcard_excluded) {
        identity_excluded = true;
      } else if (wildcard_best_q > 0.0f) {
        identity_best_q = wildcard_best_q;
      } else if (!wildcard_listed) {
        // No Accept-Encoding info applies so identity acceptable by default
        identity_best_q = 1.0f;
      }
    }
    if (identity_excluded) {
      identity_best_q = 0.0f;
    }

    if (gzip_best_q > 0.0f && gzip_best_q >= identity_best_q) {
      this->response_encoding_ = sourcemeta::one::Encoding::GZIP;
    } else if (identity_best_q == 0.0f) {
      this->satisfiable_encoding_ = false;
    }
  }

  [[nodiscard]] auto method() const noexcept -> std::string_view {
    return this->request_ ? this->request_->getMethod() : this->method_;
  }

  [[nodiscard]] auto path() const noexcept -> std::string_view {
    return this->request_ ? this->request_->getUrl() : this->path_;
  }

  [[nodiscard]] auto header(const std::string_view name) const noexcept
      -> std::string_view {
    return this->request_->getHeader(name);
  }

  [[nodiscard]] auto header_exists(const std::string_view name) const noexcept
      -> bool {
    for (const auto [key, value] : *this->request_) {
      if (key == name) {
        return true;
      }
    }
    return false;
  }

  [[nodiscard]] auto query(const std::string_view name) const
      -> std::string_view {
    return this->request_->getQuery(name);
  }

  [[nodiscard]] auto has_query(const std::string_view name) const -> bool {
    const sourcemeta::core::URI::Query query{this->request_->getQuery()};
    return query.at(name).has_value();
  }

  [[nodiscard]] auto header_gmt(const std::string_view name) const noexcept
      -> std::optional<std::chrono::system_clock::time_point> {
    const auto value{this->header(name)};
    if (value.empty()) {
      return std::nullopt;
    }

    try {
      return sourcemeta::core::from_gmt(value);
    } catch (const std::invalid_argument &) {
      return std::nullopt;
    }
  }

  // A header list element consists of the element value and its quality value
  // See https://developer.mozilla.org/en-US/docs/Glossary/Quality_values
  [[nodiscard]] auto header_list(const std::string_view name) const
      -> std::vector<std::pair<std::string, float>> {
    const auto value{this->header(name)};
    if (value.empty()) {
      return {};
    }

    std::istringstream stream{std::string{value}};
    std::string token;
    std::vector<std::pair<std::string, float>> result;

    while (std::getline(stream, token, ',')) {
      const std::size_t start{token.find_first_not_of(' ')};
      const std::size_t end{token.find_last_not_of(' ')};
      if (start == std::string::npos || end == std::string::npos) {
        continue;
      }

      // See https://developer.mozilla.org/en-US/docs/Glossary/Quality_values
      const std::size_t value_start{token.find_first_of(';')};
      if (value_start != std::string::npos && value_start + 3 < token.size() &&
          token[value_start + 1] == 'q' && token[value_start + 2] == '=') {
        // Garbage q values (e.g. `q=abc`) would otherwise escape as an
        // exception out of `header_list` -> `negotiate()` -> dispatch's catch
        // -> 500. Treat unparsable as the default 1.0 to keep negotiation
        // defensive against malformed client input.
        float quality{1.0f};
        try {
          quality = std::stof(token.substr(value_start + 3));
        } catch (const std::exception &) {
          quality = 1.0f;
        }
        result.emplace_back(token.substr(start, value_start - start), quality);
      } else if (value_start != std::string::npos) {
        // Malformed quality value, treat as default 1.0
        result.emplace_back(token.substr(start, value_start - start), 1.0f);
      } else {
        // No quality value is 1.0 by default
        result.emplace_back(token.substr(start, end - start + 1), 1.0f);
      }
    }

    // For convenience, automatically sort by the quality value
    std::ranges::sort(result, [](const auto &left, const auto &right) {
      return left.second > right.second;
    });

    return result;
  }

  [[nodiscard]] auto prefers_html() const noexcept -> bool {
    // TODO: We probably want to take Accept sequences and q= into account
    return this->header("accept").find("text/html") != std::string_view::npos;
  }

  [[nodiscard]] auto satisfiable_encoding() const noexcept -> bool {
    return this->satisfiable_encoding_;
  }

  [[nodiscard]] auto response_encoding() const noexcept
      -> sourcemeta::one::Encoding {
    return this->response_encoding_;
  }

  // Read the entire request body asynchronously.
  // - callback: Invoked with (response, body, too_big) on completion
  // - on_error: Invoked with (response, exception_ptr) on any exception,
  //   including exceptions thrown by the main callback
  // - max_size: Maximum body size in bytes (default 1MB)
  // Note: If the request is aborted, the callback is not invoked
  template <typename Callback, typename ErrorCallback>
  // NOLINTNEXTLINE(performance-unnecessary-value-param)
  auto body(Callback callback, ErrorCallback on_error,
            std::size_t max_size = static_cast<std::size_t>(1024) * 1024)
      -> void {
    auto raw_response = this->response_;
    auto snapshot = std::make_shared<HTTPRequest>(
        std::string{this->method()}, std::string{this->path()},
        this->response_encoding_, raw_response);
    auto buffer = std::make_shared<std::string>();
    auto completed = std::make_shared<bool>(false);

    raw_response->onAborted([completed]() mutable { *completed = true; });

    raw_response->onData(
        // NOLINTNEXTLINE(bugprone-exception-escape)
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
