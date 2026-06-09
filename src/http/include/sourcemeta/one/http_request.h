#ifndef SOURCEMETA_ONE_HTTP_REQUEST_H
#define SOURCEMETA_ONE_HTTP_REQUEST_H

#include <sourcemeta/core/http.h>
#include <sourcemeta/core/uri.h>

#include <sourcemeta/one/http_response.h>
#include <sourcemeta/one/http_uwebsockets.h>

#include <chrono>      // std::chrono::system_clock
#include <cstddef>     // std::size_t
#include <exception>   // std::exception_ptr, std::current_exception
#include <memory>      // std::shared_ptr, std::make_shared
#include <optional>    // std::optional
#include <string>      // std::string
#include <string_view> // std::string_view
#include <utility>     // std::move

namespace sourcemeta::one {

// 4 MB inbound body cap. Accommodates realistic JSON Schema instances
// and evaluate bodies while still rejecting pathological inputs. RFC
// 9110 §15.5.14 maps oversize bodies to 413 Content Too Large.
inline constexpr std::size_t MAX_REQUEST_BODY_BYTES{
    static_cast<std::size_t>(4) * 1024 * 1024};

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

  auto negotiate() -> void {
    const auto chosen{sourcemeta::core::http_negotiate_encoding(
        this->header("accept-encoding"),
        sourcemeta::core::HTTPContentEncoding::GZIP)};
    if (!chosen.has_value()) {
      this->satisfiable_encoding_ = false;
      return;
    }
    this->response_encoding_ =
        chosen.value() == sourcemeta::core::HTTPContentEncoding::GZIP
            ? sourcemeta::one::Encoding::GZIP
            : sourcemeta::one::Encoding::Identity;
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

  // uWebSockets stores header field-names lowercased, so `name` must be
  // lowercase too
  [[nodiscard]] auto header_exists(const std::string_view name) const noexcept
      -> bool {
    if (this->request_ == nullptr) {
      return false;
    }

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
    return sourcemeta::core::http_from_date(this->header(name));
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
  // - max_size: Maximum body size in bytes. Defaults to the universal
  //   cap. See `MAX_REQUEST_BODY_BYTES` above for the rationale.
  // Note: If the request is aborted, the callback is not invoked
  template <typename Callback, typename ErrorCallback>
  // NOLINTNEXTLINE(performance-unnecessary-value-param)
  auto body(Callback callback, ErrorCallback on_error,
            std::size_t max_size = MAX_REQUEST_BODY_BYTES) -> void {
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
