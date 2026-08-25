#ifndef SOURCEMETA_ONE_HTTP_METRICS_H
#define SOURCEMETA_ONE_HTTP_METRICS_H

#include <algorithm> // std::ranges::lower_bound
#include <array>     // std::array
#include <atomic>    // std::atomic
#include <cstddef>   // std::size_t
#include <cstdint>  // std::uint8_t, std::uint16_t, std::uint32_t, std::uint64_t
#include <iterator> // std::distance
#include <map>      // std::map
#include <mutex>    // std::mutex, std::scoped_lock
#include <vector>   // std::vector

namespace sourcemeta::one {

// What serving requests cost, kept as numbers and nothing else.
//
// A request is counted against a handler, which is a number this class never
// interprets. What a handler is, what it is called, and whether any of this is
// ever said out loud are all decided elsewhere, which is what lets this stay
// true of any server rather than of this one
class HTTPMetrics {
public:
  static constexpr std::size_t BUCKET_COUNT{10};

  // An order of magnitude finer at the low end than the usual boundaries,
  // since answering out of a file takes well under a millisecond and the
  // defaults would put nearly every request in the first bucket
  static constexpr std::array<double, BUCKET_COUNT> BUCKETS{
      {0.0001, 0.00025, 0.0005, 0.001, 0.0025, 0.005, 0.01, 0.05, 0.25, 1.0}};

  // How many requests one handler answered one way
  struct Entry {
    std::uint8_t handler{0};
    std::uint16_t status{0};
    std::uint64_t count{0};
  };

  // Everything counted so far, taken in one pass so that what is read cannot
  // disagree with itself. Entries are ordered, since two readings of an
  // unchanged server should say the same thing in the same order
  struct Snapshot {
    std::uint64_t in_flight{0};
    std::uint64_t dropped{0};
    std::vector<Entry> requests;
    std::vector<std::array<std::uint64_t, BUCKET_COUNT + 1>> buckets;
    std::vector<double> sums;
  };

  HTTPMetrics() = default;

  // To avoid mistakes
  HTTPMetrics(const HTTPMetrics &) = delete;
  HTTPMetrics(HTTPMetrics &&) = delete;
  auto operator=(const HTTPMetrics &) -> HTTPMetrics & = delete;
  auto operator=(HTTPMetrics &&) -> HTTPMetrics & = delete;

  // The server is coming up, with this many handlers it may ever count
  // against, which whoever owns them is the only one to know. Nothing is
  // counted before this is said
  auto start(const std::size_t handlers) -> void {
    this->handlers_ = handlers;
    for (auto &shard : this->shards_) {
      const std::scoped_lock guard{shard.mutex};
      shard.buckets.assign(handlers, {});
      shard.sums.assign(handlers, 0.0);
    }
  }

  // A request has arrived, which is what the in-flight count is the difference
  // between
  auto enter() noexcept -> void {
    this->entered_.fetch_add(1, std::memory_order_relaxed);
  }

  // A request has gone without ever being answered, which a caller that hung
  // up mid-upload does. Nothing is counted against a handler, since nothing
  // was served, but the in-flight count comes back down all the same
  auto abandon() noexcept -> void {
    this->answered_.fetch_add(1, std::memory_order_relaxed);
  }

  // A request has been answered, which is both what is counted and what brings
  // the in-flight count back down. A request answered before any handler was
  // chosen carries none, and is counted as served without being attributed
  auto observe(const std::uint8_t handler, const std::uint16_t status,
               const double seconds) noexcept -> void {
    this->answered_.fetch_add(1, std::memory_order_relaxed);
    if (handler >= this->handlers_) [[unlikely]] {
      return;
    }

    const auto boundary{std::ranges::lower_bound(BUCKETS, seconds)};
    const auto bucket{
        static_cast<std::size_t>(std::distance(BUCKETS.begin(), boundary))};

    // Nothing said about a request may change how it was answered, and the
    // answer has already gone out by the time this runs
    try {
      auto &target{this->shard()};
      const std::scoped_lock guard{target.mutex};
      target.requests[key(handler, status)] += 1;
      target.buckets[handler][bucket] += 1;
      target.sums[handler] += seconds;
    } catch (...) {
      // A gap somebody can see is worth more than one they cannot, so what
      // could not be recorded is counted and said in the answer itself
      this->dropped_.fetch_add(1, std::memory_order_relaxed);
    }
  }

  [[nodiscard]] auto snapshot() const -> Snapshot {
    Snapshot result;
    result.dropped = this->dropped_.load(std::memory_order_relaxed);
    const auto entered{this->entered_.load(std::memory_order_relaxed)};
    const auto answered{this->answered_.load(std::memory_order_relaxed)};
    result.in_flight = entered > answered ? entered - answered : 0;
    result.buckets.assign(this->handlers_, {});
    result.sums.assign(this->handlers_, 0.0);

    std::map<std::uint32_t, std::uint64_t> totals;
    for (const auto &shard : this->shards_) {
      const std::scoped_lock guard{shard.mutex};
      for (const auto &[entry, count] : shard.requests) {
        totals[entry] += count;
      }

      for (std::size_t handler = 0; handler < shard.buckets.size(); handler++) {
        for (std::size_t bucket = 0; bucket <= BUCKET_COUNT; bucket++) {
          result.buckets[handler][bucket] += shard.buckets[handler][bucket];
        }

        result.sums[handler] += shard.sums[handler];
      }
    }

    result.requests.reserve(totals.size());
    for (const auto &[entry, count] : totals) {
      result.requests.push_back(
          {.handler = static_cast<std::uint8_t>(entry >> 16U),
           .status = static_cast<std::uint16_t>(entry & 0xFFFFU),
           .count = count});
    }

    return result;
  }

private:
  // A series is named by what answered and what it answered together, so
  // neither alone identifies one
  [[nodiscard]] static constexpr auto key(const std::uint8_t handler,
                                          const std::uint16_t status) noexcept
      -> std::uint32_t {
    return (static_cast<std::uint32_t>(handler) << 16U) | status;
  }

  // Enough that threads rarely share one, and a fixed number rather than one
  // per hardware thread so that building this allocates nothing and can
  // therefore happen before anything else does. Threads beyond this many
  // share, which the lock already accounts for
  static constexpr std::size_t SHARD_COUNT{16};

  // Answering a request touches a line no other thread is writing to. Reading
  // them is a scrape, which happens once every several seconds and can afford
  // to visit each in turn
  struct Shard {
    mutable std::mutex mutex;
    std::map<std::uint32_t, std::uint64_t> requests;
    std::vector<std::array<std::uint64_t, BUCKET_COUNT + 1>> buckets;
    std::vector<double> sums;
  };

  // A thread takes the next place in line the first time it answers anything,
  // and keeps it, so which line to touch costs nothing to work out afterwards
  [[nodiscard]] auto shard() const noexcept -> Shard & {
    static std::atomic<std::size_t> next{0};
    thread_local const std::size_t assigned{
        next.fetch_add(1, std::memory_order_relaxed)};
    return this->shards_[assigned % SHARD_COUNT];
  }

  std::size_t handlers_{0};
  mutable std::array<Shard, SHARD_COUNT> shards_;
  std::atomic<std::uint64_t> entered_{0};
  std::atomic<std::uint64_t> answered_{0};
  std::atomic<std::uint64_t> dropped_{0};
};

// What this process has served, which there is one of because there is one
// server. It lives here rather than on the server itself, since what a request
// is worth is read where a request is answered and that is not there. It is
// built before anything runs, so reaching it never costs a check
// NOLINTNEXTLINE(cppcoreguidelines-avoid-non-const-global-variables)
inline HTTPMetrics HTTP_METRICS;

[[nodiscard]] inline auto http_metrics() noexcept -> HTTPMetrics & {
  return HTTP_METRICS;
}

} // namespace sourcemeta::one

#endif
