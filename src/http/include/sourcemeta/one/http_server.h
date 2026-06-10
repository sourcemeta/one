#ifndef SOURCEMETA_ONE_HTTP_SERVER_H
#define SOURCEMETA_ONE_HTTP_SERVER_H

#include <sourcemeta/one/http_request.h>
#include <sourcemeta/one/http_response.h>
#include <sourcemeta/one/http_uwebsockets.h>

#include <algorithm> // std::max
#include <atomic>    // std::atomic
#include <cassert>   // assert
#include <chrono>    // std::chrono::milliseconds
#include <cstdint>   // std::uint16_t
#include <latch>     // std::latch
#include <memory>    // std::unique_ptr, std::make_unique
#include <mutex>     // std::mutex, std::lock_guard
#include <thread>    // std::thread, std::thread::hardware_concurrency
#include <vector>    // std::vector

namespace sourcemeta::one {

class HTTPServer {
public:
  // POSIX.1-2017 §2.4.3: only the listed async-signal-safe functions
  // may run from a signal handler. This setter is callable from one
  // because it does nothing but an atomic store. Callers from non-
  // signal context (tests, watchdogs) may also use it.
  static auto request_stop() noexcept -> void {
    HTTPServer::should_stop_.store(true, std::memory_order_release);
  }

  // Returns true when a cooperative shutdown has been requested.
  // Lets `main` distinguish "constructor returned because we asked
  // the server to stop" from "constructor returned because nothing
  // bound the port".
  static auto stop_requested() noexcept -> bool {
    return HTTPServer::should_stop_.load(std::memory_order_acquire);
  }

  template <typename RequestHandler, typename ListenCallback,
            typename ErrorCallback>
  HTTPServer(const std::uint16_t port, RequestHandler on_request,
             ListenCallback on_listen, ErrorCallback on_error) {
    this->concurrency_ = std::max(1u, std::thread::hardware_concurrency());
    this->round_robin_.store(0, std::memory_order_relaxed);
    // Each worker stores its app pointer through an atomic so that
    // `load_balance`, which runs on whatever thread the kernel hands
    // an accept to, never reads a half-published value or a freed
    // pointer through the published value.
    // std::atomic is neither copyable nor movable, so std::vector
    // operations on it are awkward. A heap-allocated array sized
    // once at construction is the cleanest owning shape.
    HTTPServer::apps_ =
        // NOLINTNEXTLINE(modernize-avoid-c-arrays)
        std::make_unique<std::atomic<uWS::SSLApp *>[]>(this->concurrency_);
    for (unsigned int index{0}; index < this->concurrency_; ++index) {
      HTTPServer::apps_[index].store(nullptr, std::memory_order_relaxed);
    }

    std::latch setup_barrier{static_cast<std::ptrdiff_t>(this->concurrency_)};
    std::mutex setup_mutex;

    std::vector<std::unique_ptr<std::thread>> threads;
    threads.reserve(this->concurrency_);
    for (unsigned int index{0}; index < this->concurrency_; ++index) {
      threads.emplace_back(std::make_unique<std::thread>(
          [index, port, &on_request, &on_listen, &on_error, &setup_barrier,
           &setup_mutex]() {
            // uWS provides slow-loris protection out of the box, so
            // we don't have to configure any of it ourselves.
            uWS::SocketContextOptions options{};
            auto *app{new uWS::SSLApp(options)};

            app->any(
                "/*",
                // NOLINTNEXTLINE(bugprone-exception-escape)
                [&on_request](auto *const raw_response,
                              auto *const raw_request) noexcept -> void {
                  HTTPResponse response{raw_response};
                  HTTPRequest request{raw_request, raw_response};
                  try {
                    on_request(request, response);
                  } catch (...) {
                    response.write_status(
                        sourcemeta::core::HTTP_STATUS_INTERNAL_SERVER_ERROR);
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
            HTTPServer::apps_[index].store(app, std::memory_order_release);
            setup_barrier.arrive_and_wait();

            app->run();
            // Null out before deleting so preOpen never hits a freed
            // pointer. The acquire/release pair pairs with the load
            // in `load_balance`.
            HTTPServer::apps_[index].store(nullptr, std::memory_order_release);
            delete app;
          }));
    }

    // Watcher thread: parks on `should_stop_` and, once requested,
    // defers `app->close()` onto each worker's event loop. The defer
    // hop is required because `us_socket_context_close` must run on
    // the thread that owns the loop. The short sleep after observing
    // the flag lets any preOpen call already in flight finish before
    // we start tearing down apps.
    std::thread watcher{[]() {
      while (!HTTPServer::should_stop_.load(std::memory_order_acquire)) {
        std::this_thread::sleep_for(std::chrono::milliseconds{50});
      }
      std::this_thread::sleep_for(std::chrono::milliseconds{50});
      for (unsigned int index{0}; index < HTTPServer::concurrency_; ++index) {
        auto *app{HTTPServer::apps_[index].load(std::memory_order_acquire)};
        if (app != nullptr) {
          app->getLoop()->defer([app]() { app->close(); });
        }
      }
    }};

    for (auto &thread : threads) {
      thread->join();
    }
    watcher.join();
  }

private:
  static auto load_balance(struct us_socket_context_t *,
                           LIBUS_SOCKET_DESCRIPTOR fd, char *, int)
      -> LIBUS_SOCKET_DESCRIPTOR {
    // Drop new accepts once shutdown has been requested. By the time
    // the watcher thread defers `app->close()`, listen sockets are
    // still alive for a short window, and the kernel may keep handing
    // us connections during that window. Refusing them here avoids
    // dispatching work to apps that are about to disappear.
    if (HTTPServer::should_stop_.load(std::memory_order_acquire)) {
      return static_cast<LIBUS_SOCKET_DESCRIPTOR>(-1);
    }
    const auto target{
        HTTPServer::round_robin_.fetch_add(1, std::memory_order_relaxed) %
        HTTPServer::concurrency_};
    auto *receiving_app{
        HTTPServer::apps_[target].load(std::memory_order_acquire)};
    if (receiving_app) {
      receiving_app->getLoop()->defer(
          [fd, receiving_app]() { receiving_app->adoptSocket(fd); });
    }
    return static_cast<LIBUS_SOCKET_DESCRIPTOR>(-1);
  }

  // NOLINTNEXTLINE(modernize-avoid-c-arrays)
  static inline std::unique_ptr<std::atomic<uWS::SSLApp *>[]> apps_;
  static inline std::atomic<unsigned int> round_robin_{0};
  static inline std::atomic<bool> should_stop_{false};
  static inline unsigned int concurrency_{0};
};

} // namespace sourcemeta::one

#endif
