#ifndef SOURCEMETA_ONE_ROUTER_METRICS_H
#define SOURCEMETA_ONE_ROUTER_METRICS_H

#include <array>   // std::array
#include <atomic>  // std::atomic
#include <cstddef> // std::size_t
#include <cstdint> // std::uint8_t, std::uint16_t, std::uint32_t, std::uint64_t
#include <map>     // std::map
#include <mutex>   // std::mutex
#include <vector>  // std::vector

namespace sourcemeta::one {

// What a request cost, kept as numbers and nothing else. What any of it is
// called, and whether an instance says any of it out loud, is decided by
// whoever reads this rather than here
class RouterMetrics {
public:
  static constexpr std::size_t BUCKET_COUNT{10};

  // An order of magnitude finer at the low end than the usual boundaries,
  // since answering out of a metapack takes well under a millisecond and the
  // defaults would put nearly every request in the first bucket
  static constexpr std::array<double, BUCKET_COUNT> BUCKETS{
      {0.0001, 0.00025, 0.0005, 0.001, 0.0025, 0.005, 0.01, 0.05, 0.25, 1.0}};

  // Everything counted so far, taken in one pass so that what is read cannot
  // disagree with itself. Requests are ordered, since two readings of an
  // unchanged instance should say the same thing in the same order
  struct Snapshot {
    std::uint64_t in_flight{0};
    std::uint64_t dropped{0};
    std::map<std::uint32_t, std::uint64_t> requests;
    std::vector<std::array<std::uint64_t, BUCKET_COUNT + 1>> buckets;
    std::vector<double> sums;
  };

  RouterMetrics();

  // To avoid mistakes
  RouterMetrics(const RouterMetrics &) = delete;
  RouterMetrics(RouterMetrics &&) = delete;
  auto operator=(const RouterMetrics &) -> RouterMetrics & = delete;
  auto operator=(RouterMetrics &&) -> RouterMetrics & = delete;

  // How many handlers this instance has, which is every index that may ever be
  // counted against
  auto resize(std::size_t actions) -> void;

  // A request has reached a handler, which is what the in-flight count is the
  // difference between
  auto enter() noexcept -> void;

  // A request has been answered, which is both what is counted and what brings
  // the in-flight count back down
  auto observe(std::uint8_t action, std::uint16_t status,
               double seconds) noexcept -> void;

  [[nodiscard]] auto snapshot() const -> Snapshot;

  // When this instance began, as seconds since the Unix epoch
  [[nodiscard]] auto started() const noexcept -> double {
    return this->started_;
  }

  // How a request and what it was answered with are held together, since a
  // series is named by both and neither alone identifies one
  [[nodiscard]] static constexpr auto
  action_of(const std::uint32_t key) noexcept -> std::size_t {
    return static_cast<std::size_t>(key >> 16U);
  }

  [[nodiscard]] static constexpr auto
  status_of(const std::uint32_t key) noexcept -> std::uint16_t {
    return static_cast<std::uint16_t>(key & 0xFFFFU);
  }

private:
  // One of these per hardware thread, so that answering a request touches a
  // line no other thread is writing to. Reading them is a scrape, which
  // happens once every several seconds and can afford to visit each in turn
  struct Shard {
    mutable std::mutex mutex;
    std::map<std::uint32_t, std::uint64_t> requests;
    std::vector<std::array<std::uint64_t, BUCKET_COUNT + 1>> buckets;
    std::vector<double> sums;
  };

  [[nodiscard]] auto shard() const noexcept -> Shard &;

  std::size_t actions_{0};
  mutable std::vector<Shard> shards_;
  std::atomic<std::uint64_t> entered_{0};
  std::atomic<std::uint64_t> answered_{0};
  std::atomic<std::uint64_t> dropped_{0};
  double started_;
};

} // namespace sourcemeta::one

#endif
