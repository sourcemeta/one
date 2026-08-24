#include <sourcemeta/one/router_metrics.h>

#include <algorithm>  // std::ranges::lower_bound, std::max
#include <chrono>     // std::chrono::system_clock, std::chrono::duration
#include <functional> // std::hash
#include <iterator>   // std::distance
#include <mutex>      // std::scoped_lock
#include <thread>     // std::thread, std::this_thread

namespace sourcemeta::one {

RouterMetrics::RouterMetrics()
    : shards_{std::max<std::size_t>(std::thread::hardware_concurrency(), 1)},
      started_{std::chrono::duration<double>{
          std::chrono::system_clock::now().time_since_epoch()}
                   .count()} {}

auto RouterMetrics::resize(const std::size_t actions) -> void {
  this->actions_ = actions;
  for (auto &shard : this->shards_) {
    const std::scoped_lock guard{shard.mutex};
    shard.buckets.assign(actions, {});
    shard.sums.assign(actions, 0.0);
  }
}

auto RouterMetrics::shard() const noexcept -> Shard & {
  const auto index{std::hash<std::thread::id>{}(std::this_thread::get_id()) %
                   this->shards_.size()};
  return this->shards_[index];
}

auto RouterMetrics::enter() noexcept -> void {
  this->entered_.fetch_add(1, std::memory_order_relaxed);
}

auto RouterMetrics::observe(const std::uint8_t action,
                            const std::uint16_t status,
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
    const auto key{(static_cast<std::uint32_t>(action) << 16U) | status};
    target.requests[key] += 1;
    target.buckets[action][bucket] += 1;
    target.sums[action] += seconds;
  } catch (...) {
    // A gap somebody can see is worth more than one they cannot, so what could
    // not be recorded is counted and said in the answer itself
    this->dropped_.fetch_add(1, std::memory_order_relaxed);
  }
}

auto RouterMetrics::snapshot() const -> Snapshot {
  Snapshot result;
  result.dropped = this->dropped_.load(std::memory_order_relaxed);
  const auto entered{this->entered_.load(std::memory_order_relaxed)};
  const auto answered{this->answered_.load(std::memory_order_relaxed)};
  result.in_flight = entered > answered ? entered - answered : 0;
  result.buckets.assign(this->actions_, {});
  result.sums.assign(this->actions_, 0.0);

  for (const auto &shard : this->shards_) {
    const std::scoped_lock guard{shard.mutex};
    for (const auto &[key, count] : shard.requests) {
      result.requests[key] += count;
    }

    for (std::size_t action = 0; action < shard.buckets.size(); action++) {
      for (std::size_t bucket = 0; bucket <= BUCKET_COUNT; bucket++) {
        result.buckets[action][bucket] += shard.buckets[action][bucket];
      }

      result.sums[action] += shard.sums[action];
    }
  }

  return result;
}

} // namespace sourcemeta::one
