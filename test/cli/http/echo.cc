#include <sourcemeta/core/http.h>

#include <sourcemeta/one/http.h>

#include <cstdint>   // std::uint16_t
#include <exception> // std::exception_ptr
#include <iostream>  // std::cout, std::endl
#include <string>    // std::string

auto main() -> int {
  using namespace sourcemeta::one;
  const HTTPServer server{
      0,
      [](HTTPRequest &request, HTTPResponse &response) {
        if (request.method() == "get") {
          response.send(request, request.path(), Encoding::Identity);
        } else {
          request.body(
              [](HTTPRequest &snapshot, HTTPResponse &reply, std::string body,
                 const bool too_big) {
                if (too_big) {
                  reply.write_status(
                      sourcemeta::core::HTTP_STATUS_CONTENT_TOO_LARGE);
                  reply.send_without_content();
                } else {
                  reply.send(snapshot, body, Encoding::Identity);
                }
              },
              [](HTTPRequest &, HTTPResponse &reply, std::exception_ptr) {
                reply.write_status(
                    sourcemeta::core::HTTP_STATUS_INTERNAL_SERVER_ERROR);
                reply.send_without_content();
              },
              64);
        }
      },
      [](const std::uint16_t port) { std::cout << port << std::endl; },
      [](const std::uint16_t port) {
        std::cerr << "Failed to listen on 127.0.0.1:" << port << '\n';
      },
      "127.0.0.1"};
  return server.stopped_gracefully() ? 0 : 1;
}
