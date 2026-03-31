#ifndef SOURCEMETA_ONE_HTTP_SERVER_H
#define SOURCEMETA_ONE_HTTP_SERVER_H

#include <sourcemeta/one/http_request.h>
#include <sourcemeta/one/http_response.h>
#include <sourcemeta/one/http_uwebsockets.h>

#include <algorithm> // std::max
#include <atomic>    // std::atomic
#include <cassert>   // assert
#include <cstdint>   // std::uint16_t
#include <latch>     // std::latch
#include <memory>    // std::unique_ptr, std::make_unique
#include <mutex>     // std::mutex, std::lock_guard
#include <thread>    // std::thread, std::thread::hardware_concurrency
#include <vector>    // std::vector

namespace sourcemeta::one {

class HTTPServer {
public:
  template <typename RequestHandler, typename ListenCallback,
            typename ErrorCallback>
  HTTPServer(const std::uint16_t port, RequestHandler on_request,
             ListenCallback on_listen, ErrorCallback on_error) {
    this->concurrency_ = std::max(1u, std::thread::hardware_concurrency());
    this->round_robin_.store(0, std::memory_order_relaxed);
    this->apps_.assign(this->concurrency_, nullptr);

    std::latch setup_barrier{static_cast<std::ptrdiff_t>(this->concurrency_)};
    std::mutex setup_mutex;

    std::vector<std::unique_ptr<std::thread>> threads;
    threads.reserve(this->concurrency_);
    for (unsigned int index{0}; index < this->concurrency_; ++index) {
      threads.emplace_back(std::make_unique<std::thread>(
          [index, port, &on_request, &on_listen, &on_error, &setup_barrier,
           &setup_mutex]() {
            uWS::SocketContextOptions options{};
            auto *app{new uWS::SSLApp(options)};

            app->any("/*",
                     // NOLINTNEXTLINE(bugprone-exception-escape)
                     [&on_request](auto *const raw_response,
                                   auto *const raw_request) noexcept -> void {
                       HTTPResponse response{raw_response};
                       HTTPRequest request{raw_request, raw_response};
                       try {
                         on_request(request, response);
                       } catch (...) {
                         response.write_status(STATUS_INTERNAL_SERVER_ERROR);
                         response.send_without_content();
                       }
                     });

            {
              std::lock_guard<std::mutex> guard{setup_mutex};
              app->listen(
                  static_cast<int>(port),
                  [port, &on_listen,
                   &on_error](us_listen_socket_t *const socket) -> void {
                    if (socket) {
                      const auto bound_port{
                          static_cast<std::uint16_t>(us_socket_local_port(
                              true,
                              reinterpret_cast<struct us_socket_t *>(socket)))};
                      assert(bound_port > 0);
                      assert(port == bound_port);
                      on_listen(bound_port);
                    } else {
                      on_error(port);
                    }
                  });
            }

            // Install preOpen for round-robin load balancing before
            // signaling readiness, so no connections arrive without
            // the hook active
            app->preOpen(HTTPServer::load_balance);

            // Publish this app and wait for all threads to finish
            // setup before entering the event loop
            HTTPServer::apps_[index] = app;
            setup_barrier.arrive_and_wait();

            app->run();
            // Null out before deleting so preOpen never hits a freed pointer
            HTTPServer::apps_[index] = nullptr;
            delete app;
          }));
    }

    for (auto &thread : threads) {
      thread->join();
    }
  }

private:
  static auto load_balance(struct us_socket_context_t *,
                           LIBUS_SOCKET_DESCRIPTOR fd)
      -> LIBUS_SOCKET_DESCRIPTOR {
    const auto target{
        HTTPServer::round_robin_.fetch_add(1, std::memory_order_relaxed) %
        HTTPServer::concurrency_};
    auto *receiving_app{HTTPServer::apps_[target]};
    if (receiving_app) {
      receiving_app->getLoop()->defer(
          [fd, receiving_app]() { receiving_app->adoptSocket(fd); });
    }
    return static_cast<LIBUS_SOCKET_DESCRIPTOR>(-1);
  }

  static inline std::vector<uWS::SSLApp *> apps_;
  static inline std::atomic<unsigned int> round_robin_{0};
  static inline unsigned int concurrency_{0};
};

} // namespace sourcemeta::one

#endif
