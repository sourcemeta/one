#include <sourcemeta/core/test.h>
#include <sourcemeta/one/router_lru.h>

#include <atomic>     // std::atomic
#include <cstddef>    // std::size_t
#include <filesystem> // std::filesystem
#include <latch>      // std::latch
#include <memory>     // std::shared_ptr
#include <stdexcept>  // std::runtime_error
#include <string>     // std::string
#include <thread>     // std::thread
#include <vector>     // std::vector

TEST(construction_capacity_and_size) {
  sourcemeta::one::RouterLRU<int, int> cache{3};
  EXPECT_EQ(cache.capacity(), 3u);
  EXPECT_EQ(cache.size(), 0u);
}

TEST(get_or_compute_inserts_on_miss) {
  sourcemeta::one::RouterLRU<int, int> cache{3};
  const auto handle{cache.get_or_compute(1, [] { return 100; })};
  EXPECT_NE(handle, nullptr);
  EXPECT_EQ(*handle, 100);
  EXPECT_EQ(cache.size(), 1u);
}

TEST(get_or_compute_returns_existing_on_hit) {
  sourcemeta::one::RouterLRU<int, int> cache{3};
  [[maybe_unused]] const auto seeded{
      cache.get_or_compute(1, [] { return 100; })};
  int factory_calls{0};
  const auto handle{cache.get_or_compute(1, [&factory_calls] {
    ++factory_calls;
    return 200;
  })};
  EXPECT_NE(handle, nullptr);
  EXPECT_EQ(*handle, 100);
  EXPECT_EQ(factory_calls, 0);
  EXPECT_EQ(cache.size(), 1u);
}

TEST(try_get_misses_when_empty) {
  sourcemeta::one::RouterLRU<int, int> cache{3};
  EXPECT_EQ(cache.try_get(1), nullptr);
}

TEST(try_get_hits_existing_entry) {
  sourcemeta::one::RouterLRU<int, int> cache{3};
  [[maybe_unused]] const auto seeded{
      cache.get_or_compute(1, [] { return 100; })};
  const auto handle{cache.try_get(1)};
  EXPECT_NE(handle, nullptr);
  EXPECT_EQ(*handle, 100);
}

TEST(capacity_bound_evicts_oldest) {
  sourcemeta::one::RouterLRU<int, int> cache{3};
  [[maybe_unused]] const auto entry_one{
      cache.get_or_compute(1, [] { return 10; })};
  [[maybe_unused]] const auto entry_two{
      cache.get_or_compute(2, [] { return 20; })};
  [[maybe_unused]] const auto entry_three{
      cache.get_or_compute(3, [] { return 30; })};
  [[maybe_unused]] const auto entry_four{
      cache.get_or_compute(4, [] { return 40; })};
  EXPECT_EQ(cache.size(), 3u);
  EXPECT_EQ(cache.try_get(1), nullptr);
  EXPECT_NE(cache.try_get(2), nullptr);
  EXPECT_NE(cache.try_get(3), nullptr);
  EXPECT_NE(cache.try_get(4), nullptr);
}

TEST(try_get_promotes_to_mru) {
  sourcemeta::one::RouterLRU<int, int> cache{3};
  [[maybe_unused]] const auto entry_one{
      cache.get_or_compute(1, [] { return 10; })};
  [[maybe_unused]] const auto entry_two{
      cache.get_or_compute(2, [] { return 20; })};
  [[maybe_unused]] const auto entry_three{
      cache.get_or_compute(3, [] { return 30; })};
  // Promote 1 to MRU. 2 is now the LRU back.
  EXPECT_NE(cache.try_get(1), nullptr);
  [[maybe_unused]] const auto entry_four{
      cache.get_or_compute(4, [] { return 40; })};
  EXPECT_NE(cache.try_get(1), nullptr);
  EXPECT_EQ(cache.try_get(2), nullptr);
  EXPECT_NE(cache.try_get(3), nullptr);
  EXPECT_NE(cache.try_get(4), nullptr);
}

TEST(get_or_compute_promotes_to_mru_on_hit) {
  sourcemeta::one::RouterLRU<int, int> cache{3};
  [[maybe_unused]] const auto entry_one{
      cache.get_or_compute(1, [] { return 10; })};
  [[maybe_unused]] const auto entry_two{
      cache.get_or_compute(2, [] { return 20; })};
  [[maybe_unused]] const auto entry_three{
      cache.get_or_compute(3, [] { return 30; })};
  // Hit on 1 promotes it. 2 is now the LRU back.
  [[maybe_unused]] const auto promoted_one{
      cache.get_or_compute(1, [] { return 999; })};
  [[maybe_unused]] const auto entry_four{
      cache.get_or_compute(4, [] { return 40; })};
  EXPECT_NE(cache.try_get(1), nullptr);
  EXPECT_EQ(cache.try_get(2), nullptr);
  EXPECT_NE(cache.try_get(3), nullptr);
  EXPECT_NE(cache.try_get(4), nullptr);
}

TEST(clear_empties_cache) {
  sourcemeta::one::RouterLRU<int, int> cache{3};
  [[maybe_unused]] const auto entry_one{
      cache.get_or_compute(1, [] { return 10; })};
  [[maybe_unused]] const auto entry_two{
      cache.get_or_compute(2, [] { return 20; })};
  cache.clear();
  EXPECT_EQ(cache.size(), 0u);
  EXPECT_EQ(cache.try_get(1), nullptr);
  EXPECT_EQ(cache.try_get(2), nullptr);
}

TEST(evicted_handle_remains_valid) {
  sourcemeta::one::RouterLRU<int, std::string> cache{2};
  const auto handle{
      cache.get_or_compute(1, [] { return std::string{"hello"}; })};
  EXPECT_NE(handle, nullptr);
  EXPECT_EQ(*handle, "hello");
  [[maybe_unused]] const auto evicting_two{
      cache.get_or_compute(2, [] { return std::string{"two"}; })};
  [[maybe_unused]] const auto evicting_three{
      cache.get_or_compute(3, [] { return std::string{"three"}; })};
  EXPECT_EQ(cache.try_get(1), nullptr);
  EXPECT_EQ(*handle, "hello");
}

TEST(clear_does_not_invalidate_outstanding_handle) {
  sourcemeta::one::RouterLRU<int, std::string> cache{2};
  const auto handle{cache.get_or_compute(1, [] { return std::string{"foo"}; })};
  cache.clear();
  EXPECT_NE(handle, nullptr);
  EXPECT_EQ(*handle, "foo");
  EXPECT_EQ(cache.size(), 0u);
}

TEST(factory_exception_does_not_insert) {
  sourcemeta::one::RouterLRU<int, int> cache{3};
  try {
    [[maybe_unused]] const auto handle{cache.get_or_compute(
        1, []() -> int { throw std::runtime_error{"boom"}; })};
    FAIL();
  } catch (const std::runtime_error &error) {
    EXPECT_STREQ(error.what(), "boom");
  }
  EXPECT_EQ(cache.size(), 0u);
  EXPECT_EQ(cache.try_get(1), nullptr);
}

TEST(filesystem_path_key) {
  sourcemeta::one::RouterLRU<std::filesystem::path, std::string> cache{3};
  const std::filesystem::path key{"/tmp/foo/bar.metapack"};
  const auto handle{cache.get_or_compute(key, [] { return std::string{"v"}; })};
  EXPECT_NE(handle, nullptr);
  EXPECT_EQ(*handle, "v");
  const auto hit{cache.try_get(key)};
  EXPECT_NE(hit, nullptr);
  EXPECT_EQ(*hit, "v");
}

TEST(concurrent_hits_on_prewarmed_cache) {
  constexpr std::size_t capacity{32};
  constexpr std::size_t thread_count{8};
  constexpr std::size_t iterations{10000};

  sourcemeta::one::RouterLRU<int, int> cache{capacity};
  for (int key = 0; key < static_cast<int>(capacity); ++key) {
    [[maybe_unused]] const auto prewarmed{
        cache.get_or_compute(key, [key] { return key * 100; })};
  }

  std::latch start_gate{static_cast<std::ptrdiff_t>(thread_count)};
  std::atomic<bool> error{false};
  std::vector<std::thread> threads;
  threads.reserve(thread_count);
  for (std::size_t thread_index = 0; thread_index < thread_count;
       ++thread_index) {
    threads.emplace_back([&cache, &error, &start_gate] {
      start_gate.arrive_and_wait();
      for (std::size_t iteration = 0; iteration < iterations; ++iteration) {
        const int key{static_cast<int>(iteration % capacity)};
        const auto handle{cache.try_get(key)};
        if (!handle || *handle != key * 100) {
          error.store(true, std::memory_order_relaxed);
          return;
        }
      }
    });
  }
  for (auto &thread : threads) {
    thread.join();
  }
  EXPECT_FALSE(error.load());
}

TEST(concurrent_misses_for_same_key_converge) {
  constexpr std::size_t thread_count{8};
  sourcemeta::one::RouterLRU<int, int> cache{8};

  std::latch start_gate{static_cast<std::ptrdiff_t>(thread_count)};
  std::atomic<int> factory_calls{0};
  std::vector<std::shared_ptr<const int>> results(thread_count);

  std::vector<std::thread> threads;
  threads.reserve(thread_count);
  for (std::size_t thread_index = 0; thread_index < thread_count;
       ++thread_index) {
    threads.emplace_back(
        [&cache, &factory_calls, &results, &start_gate, thread_index] {
          start_gate.arrive_and_wait();
          results[thread_index] = cache.get_or_compute(42, [&factory_calls] {
            factory_calls.fetch_add(1, std::memory_order_relaxed);
            return 12345;
          });
        });
  }
  for (auto &thread : threads) {
    thread.join();
  }

  for (const auto &handle : results) {
    EXPECT_NE(handle, nullptr);
    EXPECT_EQ(*handle, 12345);
  }
  EXPECT_GE(factory_calls.load(), 1);
  EXPECT_EQ(cache.size(), 1u);

  const auto cached{cache.try_get(42)};
  EXPECT_NE(cached, nullptr);
  bool found{false};
  for (const auto &handle : results) {
    if (handle.get() == cached.get()) {
      found = true;
      break;
    }
  }
  EXPECT_TRUE(found);
}

TEST(concurrent_mixed_traffic_bounded) {
  constexpr std::size_t capacity{16};
  constexpr int key_range{64};
  constexpr std::size_t thread_count{16};
  constexpr std::size_t iterations{5000};

  sourcemeta::one::RouterLRU<int, int> cache{capacity};

  std::latch start_gate{static_cast<std::ptrdiff_t>(thread_count)};
  std::atomic<bool> error{false};
  std::vector<std::thread> threads;
  threads.reserve(thread_count);
  for (std::size_t thread_index = 0; thread_index < thread_count;
       ++thread_index) {
    threads.emplace_back([&cache, &error, &start_gate, thread_index] {
      start_gate.arrive_and_wait();
      for (std::size_t iteration = 0; iteration < iterations; ++iteration) {
        const int key{
            static_cast<int>((thread_index * 31 + iteration) % key_range)};
        const auto handle{cache.get_or_compute(key, [key] { return key * 7; })};
        if (!handle || *handle != key * 7) {
          error.store(true, std::memory_order_relaxed);
          return;
        }
      }
    });
  }
  for (auto &thread : threads) {
    thread.join();
  }
  EXPECT_FALSE(error.load());
  EXPECT_LE(cache.size(), capacity);
}
