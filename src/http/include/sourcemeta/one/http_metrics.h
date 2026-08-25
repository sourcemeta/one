#ifndef SOURCEMETA_ONE_HTTP_METRICS_H
#define SOURCEMETA_ONE_HTTP_METRICS_H

#include <algorithm> // std::ranges::lower_bound, std::max
#include <array>     // std::array
#include <atomic>    // std::atomic
#include <chrono>    // std::chrono::system_clock, std::chrono::duration
#include <cstddef>   // std::size_t
#include <cstdint>  // std::uint8_t, std::uint16_t, std::uint32_t, std::uint64_t
#include <iterator> // std::distance
#include <map>      // std::map
#include <mutex>    // std::mutex, std::scoped_lock
#include <thread>   // std::thread
#include <vector>   // std::vector

namespace sourcemeta::one {

// What answering a request cost, kept as numbers and nothing else. What any of
// it is called, and whether an instance says any of it out loud, is decided by
// whoever reads this rather than here
class HTTPMetrics {
public:
  static constexpr std::size_t BUCKET_COUNT{10};

  // An order of magnitude finer at the low end than the usual boundaries,
  // since answering out of a metapack takes well under a millisecond and the
  // defaults would put nearly every request in the first bucket
  static constexpr std::array<double, BUCKET_COUNT> BUCKETS{
      {0.0001, 0.00025, 0.0005, 0.001, 0.0025, 0.005, 0.01, 0.05, 0.25, 1.0}};

  // How many requests were answered a given way, with what they were answered
  // about and what they were answered with kept apart, since holding them
  // together is this class's own business
  struct Entry {
    std::uint8_t action{0};
    std::uint16_t status{0};
    std::uint64_t count{0};
  };

  // Everything counted so far, taken in one pass so that what is read cannot
  // disagree with itself. Entries are ordered, since two readings of an
  // unchanged instance should say the same thing in the same order
  struct Snapshot {
    std::uint64_t in_flight{0};
    std::uint64_t dropped{0};
    std::vector<Entry> requests;
    std::vector<std::array<std::uint64_t, BUCKET_COUNT + 1>> buckets;
    std::vector<double> sums;
  };

  explicit HTTPMetrics(const std::size_t actions)
      : actions_{actions},
        shards_{std::max<std::size_t>(std::thread::hardware_concurrency(), 1)},
        started_{std::chrono::duration<double>{
            std::chrono::system_clock::now().time_since_epoch()}
                     .count()} {
    for (auto &shard : this->shards_) {
      shard.buckets.assign(actions, {});
      shard.sums.assign(actions, 0.0);
    }
  }

  // To avoid mistakes
  HTTPMetrics(const HTTPMetrics &) = delete;
  HTTPMetrics(HTTPMetrics &&) = delete;
  auto operator=(const HTTPMetrics &) -> HTTPMetrics & = delete;
  auto operator=(HTTPMetrics &&) -> HTTPMetrics & = delete;

  // A request has reached a handler, which is what the in-flight count is the
  // difference between
  auto enter() noexcept -> void {
    this->entered_.fetch_add(1, std::memory_order_relaxed);
  }

  // A request has been answered, which is both what is counted and what brings
  // the in-flight count back down
  auto observe(const std::uint8_t action, const std::uint16_t status,
               const double seconds) noexcept -> void {
    this->answered_.fetch_add(1, std::memory_order_relaxed);
    if (action >= this->actions_) [[unlikely]] {
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
      target.requests[key(action, status)] += 1;
      target.buckets[action][bucket] += 1;
      target.sums[action] += seconds;
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
    result.buckets.assign(this->actions_, {});
    result.sums.assign(this->actions_, 0.0);

    std::map<std::uint32_t, std::uint64_t> totals;
    for (const auto &shard : this->shards_) {
      const std::scoped_lock guard{shard.mutex};
      for (const auto &[entry, count] : shard.requests) {
        totals[entry] += count;
      }

      for (std::size_t action = 0; action < shard.buckets.size(); action++) {
        for (std::size_t bucket = 0; bucket <= BUCKET_COUNT; bucket++) {
          result.buckets[action][bucket] += shard.buckets[action][bucket];
        }

        result.sums[action] += shard.sums[action];
      }
    }

    result.requests.reserve(totals.size());
    for (const auto &[entry, count] : totals) {
      result.requests.push_back(
          {.action = static_cast<std::uint8_t>(entry >> 16U),
           .status = static_cast<std::uint16_t>(entry & 0xFFFFU),
           .count = count});
    }

    return result;
  }

  // When this instance began, as seconds since the Unix epoch
  [[nodiscard]] auto started() const noexcept -> double {
    return this->started_;
  }

private:
  // A series is named by what was asked and what was answered together, so
  // neither alone identifies one
  [[nodiscard]] static constexpr auto key(const std::uint8_t action,
                                          const std::uint16_t status) noexcept
      -> std::uint32_t {
    return (static_cast<std::uint32_t>(action) << 16U) | status;
  }

  // One of these per hardware thread, so that answering a request touches a
  // line no other thread is writing to. Reading them is a scrape, which
  // happens once every several seconds and can afford to visit each in turn
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
    return this->shards_[assigned % this->shards_.size()];
  }

  std::size_t actions_;
  mutable std::vector<Shard> shards_;
  std::atomic<std::uint64_t> entered_{0};
  std::atomic<std::uint64_t> answered_{0};
  std::atomic<std::uint64_t> dropped_{0};
  double started_;
};

} // namespace sourcemeta::one

#endif
