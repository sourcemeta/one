#ifndef SOURCEMETA_ONE_SERVER_REQUEST_H
#define SOURCEMETA_ONE_SERVER_REQUEST_H

#include <sourcemeta/core/time.h>

#include "uwebsockets.h"

#include <algorithm>   // std::sort
#include <chrono>      // std::chrono::system_clock
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
  enum class Encoding { Identity, GZIP };

  HTTPRequest(uWS::HttpRequest *request) noexcept : request_{request} {}

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
        this->response_encoding_ = Encoding::GZIP;
        break;
      }
    }
  }

  auto method() const noexcept -> std::string_view {
    return this->request_->getMethod();
  }

  auto path() const noexcept -> std::string_view {
    return this->request_->getUrl();
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

  auto response_encoding() const noexcept -> Encoding {
    return this->response_encoding_;
  }

private:
  uWS::HttpRequest *request_;
  bool satisfiable_encoding_{true};
  Encoding response_encoding_{Encoding::Identity};
};

} // namespace sourcemeta::one

#endif
