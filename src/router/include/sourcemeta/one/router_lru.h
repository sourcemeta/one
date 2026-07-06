#ifndef SOURCEMETA_ONE_ROUTER_LRU_H
#define SOURCEMETA_ONE_ROUTER_LRU_H

#include <cstddef>       // std::size_t
#include <functional>    // std::equal_to, std::hash
#include <list>          // std::list
#include <memory>        // std::make_shared, std::shared_ptr
#include <mutex>         // std::scoped_lock, std::mutex
#include <unordered_map> // std::unordered_map
#include <utility>       // std::pair

namespace sourcemeta::one {

template <typename Key, typename Value, typename Hash = std::hash<Key>,
          typename KeyEqual = std::equal_to<Key>>
class RouterLRU {
public:
  using value_handle = std::shared_ptr<const Value>;

  explicit RouterLRU(const std::size_t capacity) : capacity_{capacity} {}

  // To avoid mistakes
  RouterLRU(const RouterLRU &) = delete;
  RouterLRU(RouterLRU &&) = delete;
  auto operator=(const RouterLRU &) -> RouterLRU & = delete;
  auto operator=(RouterLRU &&) -> RouterLRU & = delete;

  template <typename Factory>
  [[nodiscard]] auto get_or_compute(const Key &key, Factory factory)
      -> value_handle {
    {
      auto hit{this->try_get(key)};
      if (hit) {
        return hit;
      }
    }

    auto computed{std::make_shared<const Value>(factory())};

    const std::scoped_lock guard{this->mutex_};
    const auto found{this->index_.find(key)};
    if (found != this->index_.end()) {
      this->entries_.splice(this->entries_.begin(), this->entries_,
                            found->second);
      return found->second->second;
    }

    this->entries_.emplace_front(key, computed);
    try {
      this->index_.emplace(key, this->entries_.begin());
    } catch (...) {
      this->entries_.pop_front();
      throw;
    }

    if (this->entries_.size() > this->capacity_) {
      this->index_.erase(this->entries_.back().first);
      this->entries_.pop_back();
    }

    return computed;
  }

  [[nodiscard]] auto try_get(const Key &key) -> value_handle {
    const std::scoped_lock guard{this->mutex_};
    const auto found{this->index_.find(key)};
    if (found == this->index_.end()) {
      return nullptr;
    }
    this->entries_.splice(this->entries_.begin(), this->entries_,
                          found->second);
    return found->second->second;
  }

  [[nodiscard]] auto size() const -> std::size_t {
    const std::scoped_lock guard{this->mutex_};
    return this->entries_.size();
  }

  [[nodiscard]] auto capacity() const noexcept -> std::size_t {
    return this->capacity_;
  }

  auto clear() -> void {
    const std::scoped_lock guard{this->mutex_};
    this->index_.clear();
    this->entries_.clear();
  }

private:
  using entry_list = std::list<std::pair<Key, value_handle>>;
  using entry_iterator = typename entry_list::iterator;

  std::size_t capacity_;
  entry_list entries_;
  std::unordered_map<Key, entry_iterator, Hash, KeyEqual> index_;
  mutable std::mutex mutex_;
};

} // namespace sourcemeta::one

#endif
