#ifndef SOURCEMETA_ONE_HTTP_APP_H
#define SOURCEMETA_ONE_HTTP_APP_H

#include <sourcemeta/one/http_request.h>
#include <sourcemeta/one/http_response.h>
#include <sourcemeta/one/http_uwebsockets.h>

#include <cstdint> // std::uint16_t
#include <string>  // std::string

namespace sourcemeta::one {

class HTTPApp {
public:
  explicit HTTPApp(uWS::SSLApp *app) : app_{app} {}

  template <typename Handler>
  auto on_request(const std::string_view pattern, Handler handler) -> void {
    this->app_->any(std::string{pattern},
                    [handler = std::move(handler)](
                        auto *const raw_response,
                        auto *const raw_request) noexcept -> void {
                      HTTPResponse response{raw_response};
                      HTTPRequest request{raw_request, raw_response};
                      handler(request, response);
                    });
  }

  template <typename Callback>
  auto listen(const std::uint16_t port, Callback callback) -> void {
    this->app_->listen(
        static_cast<int>(port),
        [callback =
             std::move(callback)](us_listen_socket_t *const socket) -> void {
          if (socket) {
            callback(static_cast<std::uint16_t>(us_socket_local_port(
                true, reinterpret_cast<struct us_socket_t *>(socket))));
          } else {
            callback(static_cast<std::uint16_t>(0));
          }
        });
  }

private:
  uWS::SSLApp *app_;
};

} // namespace sourcemeta::one

#endif
