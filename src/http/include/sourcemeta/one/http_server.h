#ifndef SOURCEMETA_ONE_HTTP_SERVER_H
#define SOURCEMETA_ONE_HTTP_SERVER_H

#include <sourcemeta/one/http_app.h>
#include <sourcemeta/one/http_uwebsockets.h>

#include <functional> // std::ignore
#include <memory>     // std::unique_ptr, std::make_unique
#include <mutex>      // std::mutex
#include <thread>     // std::thread, std::thread::hardware_concurrency
#include <vector>     // std::vector

namespace sourcemeta::one {

class HTTPServer {
public:
  template <typename Callback> explicit HTTPServer(Callback callback) {
    concurrency_ = std::thread::hardware_concurrency();
    round_robin_ = 0;
    apps_.clear();

    std::mutex mutex;
    std::vector<std::unique_ptr<std::thread>> threads;
    threads.reserve(concurrency_);
    for (unsigned int index{0}; index < concurrency_; ++index) {
      threads.emplace_back(std::make_unique<std::thread>([&callback, &mutex]() {
        mutex.lock();
        uWS::SocketContextOptions options{};
        apps_.emplace_back(new uWS::SSLApp(options));
        auto *raw_app{apps_.back()};

        HTTPApp app{raw_app};
        callback(app);

        raw_app->preOpen(
            [](struct us_socket_context_t *context,
               LIBUS_SOCKET_DESCRIPTOR fd) -> LIBUS_SOCKET_DESCRIPTOR {
              std::ignore = context;
              auto *receiving_app{apps_[round_robin_]};
              receiving_app->getLoop()->defer(
                  [fd, receiving_app]() { receiving_app->adoptSocket(fd); });
              round_robin_ = (round_robin_ + 1) % concurrency_;
              return static_cast<LIBUS_SOCKET_DESCRIPTOR>(-1);
            });

        mutex.unlock();
        raw_app->run();
        delete raw_app;
      }));
    }

    for (auto &thread : threads) {
      thread->join();
    }
  }

private:
  static inline std::vector<uWS::SSLApp *> apps_;
  static inline unsigned int round_robin_{0};
  static inline unsigned int concurrency_{0};
};

} // namespace sourcemeta::one

#endif
