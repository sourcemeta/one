#ifndef SOURCEMETA_ONE_HTTP_RESPONSE_H
#define SOURCEMETA_ONE_HTTP_RESPONSE_H

#include <sourcemeta/core/gzip.h>
#include <sourcemeta/core/http.h>
#include <sourcemeta/one/shared.h>

#include <sourcemeta/one/http_uwebsockets.h>

#include <cstddef>     // std::size_t
#include <cstdint>     // std::uint8_t
#include <optional>    // std::optional
#include <string>      // std::string
#include <string_view> // std::string_view
#include <utility>     // std::move

namespace sourcemeta::one {

class HTTPResponse {
public:
  HTTPResponse(uWS::HttpResponse<true> *response) noexcept
      : response_{response} {}

  auto write_status(const sourcemeta::core::HTTPStatus &status) -> void {
    this->response_->writeStatus(status.wire);
  }

  auto write_header(const std::string_view name, const std::string_view value)
      -> void {
    this->response_->writeHeader(name, value);
  }

  auto handle() noexcept -> uWS::HttpResponse<true> * {
    return this->response_;
  }

  auto send_without_content() -> void { this->response_->end(); }

  template <typename Request>
  auto send(const Request &request, const std::string &message,
            const Encoding current_encoding,
            const std::optional<std::size_t> precomputed_compressed_size =
                std::nullopt) -> void {
    const auto method{request.method()};
    const auto expected_encoding{request.response_encoding()};
    if (expected_encoding == Encoding::GZIP) {
      this->response_->writeHeader("Content-Encoding", "gzip");
      if (current_encoding == Encoding::Identity) {
        // RFC 9110 §9.3.2: a HEAD response must carry the same
        // representation metadata (including Content-Length) as the
        // matching GET would. When the artifact is identity-stored
        // but the wire encoding is gzip, the metapack carries the
        // precomputed compressed size, so we can answer HEAD without
        // running the compressor just to discard its output.
        if (method == "head" && precomputed_compressed_size.has_value()) {
          this->response_->endWithoutBody(precomputed_compressed_size);
          this->response_->end();
        } else {
          auto effective_message{sourcemeta::core::gzip(
              reinterpret_cast<const std::uint8_t *>(message.data()),
              message.size())};
          if (method == "head") {
            this->response_->endWithoutBody(effective_message.size());
            this->response_->end();
          } else {
            this->response_->end(std::move(effective_message));
          }
        }
      } else {
        if (method == "head") {
          this->response_->endWithoutBody(message.size());
          this->response_->end();
        } else {
          this->response_->end(message);
        }
      }
    } else if (expected_encoding == Encoding::Identity) {
      if (current_encoding == Encoding::GZIP) {
        auto effective_message{sourcemeta::core::gunzip(
            reinterpret_cast<const std::uint8_t *>(message.data()),
            message.size())};
        if (method == "head") {
          this->response_->endWithoutBody(effective_message.size());
          this->response_->end();
        } else {
          this->response_->end(effective_message);
        }
      } else {
        if (method == "head") {
          this->response_->endWithoutBody(message.size());
          this->response_->end();
        } else {
          this->response_->end(message);
        }
      }
    }
  }

private:
  uWS::HttpResponse<true> *response_;
};

} // namespace sourcemeta::one

#endif
