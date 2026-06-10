#ifndef SOURCEMETA_ONE_HTTP_SERVER_H
#define SOURCEMETA_ONE_HTTP_SERVER_H

#include <sourcemeta/one/http_request.h>
#include <sourcemeta/one/http_response.h>
#include <sourcemeta/one/http_uwebsockets.h>

#include <algorithm>   // std::max
#include <atomic>      // std::atomic
#include <cassert>     // assert
#include <chrono>      // std::chrono::milliseconds
#include <csignal>     // std::signal, SIGINT, SIGTERM
#include <cstdint>     // std::uint16_t
#include <latch>       // std::latch
#include <memory>      // std::unique_ptr, std::make_unique
#include <mutex>       // std::mutex, std::lock_guard
#include <string_view> // std::string_view
#include <thread>      // std::thread, std::thread::hardware_concurrency
#include <unistd.h>    // write, STDERR_FILENO
#include <vector>      // std::vector

namespace sourcemeta::one {

// POSIX.1-2017 §2.4.3 limits signal handlers to a fixed function set,
// and C++ only allows `std::atomic<T>::store` from one when the type
// is lock-free. The vast majority of targets we care about make
// `std::atomic<bool>` lock-free, but assert it at compile time so
// porting to anything exotic fails loud instead of silently.
static_assert(std::atomic<bool>::is_always_lock_free,
              "HTTPServer's signal handler stores into "
              "std::atomic<bool>, which must be lock-free for the "
              "store to be async-signal-safe on this target");

class HTTPServer {
public:
  template <typename RequestHandler, typename ListenCallback,
            typename ErrorCallback>
  HTTPServer(const std::uint16_t port, RequestHandler on_request,
             ListenCallback on_listen, ErrorCallback on_error) {
    HTTPServer::concurrency_ =
        std::max(1u, std::thread::hardware_concurrency());
    HTTPServer::round_robin_.store(0, std::memory_order_relaxed);
    HTTPServer::should_stop_.store(false, std::memory_order_relaxed);
    HTTPServer::workers_alive_.store(HTTPServer::concurrency_,
                                     std::memory_order_relaxed);
    // std::atomic is neither copyable nor movable, so std::vector
    // operations on it are awkward. A heap-allocated array sized
    // once at construction is the cleanest owning shape.
    HTTPServer::apps_ =
        // NOLINTNEXTLINE(modernize-avoid-c-arrays)
        std::make_unique<std::atomic<uWS::SSLApp *>[]>(
            HTTPServer::concurrency_);
    for (unsigned int index{0}; index < HTTPServer::concurrency_; ++index) {
      HTTPServer::apps_[index].store(nullptr, std::memory_order_relaxed);
    }

    // Take ownership of SIGINT and SIGTERM for the lifetime of this
    // constructor. RAII keeps the previous dispositions restored on
    // any exit from the scope, including an exception thrown later
    // during thread creation.
    class SignalGuard {
    public:
      SignalGuard()
          : previous_int_{std::signal(SIGINT, HTTPServer::on_signal)},
            previous_term_{std::signal(SIGTERM, HTTPServer::on_signal)} {}
      ~SignalGuard() {
        std::signal(SIGINT, this->previous_int_);
        std::signal(SIGTERM, this->previous_term_);
      }
      SignalGuard(const SignalGuard &) = delete;
      SignalGuard(SignalGuard &&) = delete;
      auto operator=(const SignalGuard &) -> SignalGuard & = delete;
      auto operator=(SignalGuard &&) -> SignalGuard & = delete;

    private:
      void (*previous_int_)(int);
      void (*previous_term_)(int);
    };
    const SignalGuard signal_guard;

    // The latch starts at `concurrency_ + 1`: each worker arrives
    // once after publishing its `apps_[index]` pointer, and this
    // thread also arrives so it waits until every worker has reached
    // that point. Polling for shutdown only starts after that wait,
    // which guarantees the close sweep below sees every app.
    std::latch setup_barrier{
        static_cast<std::ptrdiff_t>(HTTPServer::concurrency_ + 1)};
    std::mutex setup_mutex;

    std::vector<std::unique_ptr<std::thread>> threads;
    threads.reserve(HTTPServer::concurrency_);
    for (unsigned int index{0}; index < HTTPServer::concurrency_; ++index) {
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
            HTTPServer::workers_alive_.fetch_sub(1, std::memory_order_release);
          }));
    }

    // Arrive on the setup barrier and wait until every worker has
    // published its app pointer before polling for shutdown. Without
    // this gate, a signal arriving during setup could let the close
    // sweep run while some `apps_` slots are still nullptr, leaving
    // the corresponding `app->run()` loops without a deferred close
    // and the `thread->join()` below hanging forever.
    setup_barrier.arrive_and_wait();

    // Poll on this thread so there is no separate watcher thread to
    // schedule. Exits when a stop was requested or every worker has
    // already exited on its own. The predicate is checked before
    // each sleep so the failure path does not pay a poll tick.
    while (true) {
      if (HTTPServer::should_stop_.load(std::memory_order_acquire)) {
        break;
      }
      if (HTTPServer::workers_alive_.load(std::memory_order_acquire) == 0) {
        break;
      }
      std::this_thread::sleep_for(std::chrono::milliseconds{50});
    }

    if (HTTPServer::should_stop_.load(std::memory_order_acquire)) {
      this->stopped_gracefully_ = true;
      // Give any preOpen call already in flight time to finish
      // against the old listen sockets before they disappear.
      std::this_thread::sleep_for(std::chrono::milliseconds{50});
      for (unsigned int index{0}; index < HTTPServer::concurrency_; ++index) {
        auto *app{HTTPServer::apps_[index].load(std::memory_order_acquire)};
        if (app != nullptr) {
          // `us_socket_context_close` must run on the loop's thread.
          app->getLoop()->defer([app]() { app->close(); });
        }
      }
    }

    for (auto &thread : threads) {
      thread->join();
    }
  }

  // Returns true when the server exited because SIGINT or SIGTERM
  // arrived. False means every worker exited on its own (e.g. nothing
  // bound the port).
  [[nodiscard]] auto stopped_gracefully() const noexcept -> bool {
    return this->stopped_gracefully_;
  }

private:
  // POSIX.1-2017 §2.4.3: limited to the async-signal-safe set.
  // `write(2)` is on it, and an atomic store on a lock-free type is
  // safe in practice. No formatting, no allocation, no locks.
  static auto on_signal(int /* signal */) noexcept -> void {
    constexpr std::string_view message{"Terminating on requested signal\n"};
    (void)::write(STDERR_FILENO, message.data(), message.size());
    HTTPServer::should_stop_.store(true, std::memory_order_release);
  }

  static auto load_balance(struct us_socket_context_t *,
                           LIBUS_SOCKET_DESCRIPTOR fd, char *, int)
      -> LIBUS_SOCKET_DESCRIPTOR {
    // Drop new accepts once shutdown has been requested. The kernel
    // can hand us connections for a short window before listen
    // sockets actually close, and dispatching them to apps that are
    // about to disappear is wasted work.
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

  bool stopped_gracefully_{false};
  // NOLINTNEXTLINE(modernize-avoid-c-arrays)
  static inline std::unique_ptr<std::atomic<uWS::SSLApp *>[]> apps_;
  static inline std::atomic<unsigned int> round_robin_{0};
  static inline std::atomic<unsigned int> workers_alive_{0};
  static inline std::atomic<bool> should_stop_{false};
  static inline unsigned int concurrency_{0};
};

} // namespace sourcemeta::one

#endif
