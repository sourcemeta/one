#include <gtest/gtest.h>

#include <sourcemeta/one/build.h>

#include <cassert>
#include <chrono> // std::chrono::seconds
#include <string>

#include "build_test_utils.h"

auto read_uint64_t(const std::filesystem::path &path) -> std::uint64_t {
  return std::stoull(READ_FILE(path));
}

auto write_uint64_t(const std::filesystem::path &path,
                    const std::uint64_t input) -> void {
  WRITE_FILE(path, std::to_string(input));
}

static auto
handler_multiply(const std::filesystem::path &destination,
                 const sourcemeta::one::Build::Dependencies &dependencies,
                 const sourcemeta::one::Build::DynamicCallback &,
                 const std::uint64_t &value) -> void {
  assert(dependencies.size() == 1);
  write_uint64_t(destination,
                 read_uint64_t(dependencies.front().second) * value);
}

static auto
handler_sum(const std::filesystem::path &destination,
            const sourcemeta::one::Build::Dependencies &dependencies,
            const sourcemeta::one::Build::DynamicCallback &,
            const std::nullptr_t &) -> void {
  std::uint64_t total{0};
  for (const auto &dependency : dependencies) {
    total += read_uint64_t(dependency.second);
  }

  write_uint64_t(destination, total);
}

static auto handler_mirror_context_node(
    const std::filesystem::path &destination,
    const sourcemeta::one::Build::Dependencies &,
    const sourcemeta::one::Build::DynamicCallback &callback,
    const std::filesystem::path &context) -> void {
  const auto input{read_uint64_t(context)};
  callback(context);
  write_uint64_t(destination, input);
}

static auto handler_mirror_context_node_without_callback(
    const std::filesystem::path &destination,
    const sourcemeta::one::Build::Dependencies &,
    const sourcemeta::one::Build::DynamicCallback &,
    const std::filesystem::path &context) -> void {
  const auto input{read_uint64_t(context)};
  write_uint64_t(destination, input);
}

static auto handler_sum_with_dynamic(
    const std::filesystem::path &destination,
    const sourcemeta::one::Build::Dependencies &dependencies,
    const sourcemeta::one::Build::DynamicCallback &callback,
    const std::filesystem::path &context) -> void {
  std::uint64_t total{0};
  for (const auto &dependency : dependencies) {
    total += read_uint64_t(dependency.second);
  }

  total += read_uint64_t(context);
  callback(context);
  write_uint64_t(destination, total);
}

TEST(Build_e2e, simple_cache_miss_hit) {
  const auto base_path{std::filesystem::path{BINARY_DIRECTORY} /
                       "simple_cache_miss_hit"};
  std::filesystem::remove_all(base_path);

  sourcemeta::one::Build build{BINARY_DIRECTORY};
  write_uint64_t(base_path / "initial.txt", 8);
  EXPECT_EQ(read_uint64_t(base_path / "initial.txt"), 8);
  build.refresh(base_path / "initial.txt");

  // Create first.txt from initial.txt (cache miss)
  const auto result_1 = build.dispatch<std::uint64_t>(
      handler_multiply, base_path / "first.txt",
      {sourcemeta::one::Build::dependency(base_path / "initial.txt")}, 2);
  EXPECT_TRUE(result_1);
  EXPECT_EQ(read_uint64_t(base_path / "first.txt"), 16);

  // Create second.txt from first.txt (cache miss)
  const auto result_2 = build.dispatch<std::uint64_t>(
      handler_multiply, base_path / "second.txt",
      {sourcemeta::one::Build::dependency(base_path / "first.txt")}, 3);
  EXPECT_TRUE(result_2);
  EXPECT_EQ(read_uint64_t(base_path / "second.txt"), 48);

  // Create first.txt from initial.txt (cache hit)
  const auto result_3 = build.dispatch<std::uint64_t>(
      handler_multiply, base_path / "first.txt",
      {sourcemeta::one::Build::dependency(base_path / "initial.txt")}, 2);
  EXPECT_FALSE(result_3);
  EXPECT_EQ(read_uint64_t(base_path / "first.txt"), 16);

  // Create second.txt from first.txt (cache hit)
  const auto result_4 = build.dispatch<std::uint64_t>(
      handler_multiply, base_path / "second.txt",
      {sourcemeta::one::Build::dependency(base_path / "first.txt")}, 3);
  EXPECT_FALSE(result_4);
  EXPECT_EQ(read_uint64_t(base_path / "second.txt"), 48);

  // Update initial.txt
  write_uint64_t(base_path / "initial.txt", 7);
  EXPECT_EQ(read_uint64_t(base_path / "initial.txt"), 7);
  build.refresh(base_path / "initial.txt");

  // Create first.txt from initial.txt (cache miss)
  const auto result_5 = build.dispatch<std::uint64_t>(
      handler_multiply, base_path / "first.txt",
      {sourcemeta::one::Build::dependency(base_path / "initial.txt")}, 2);
  EXPECT_TRUE(result_5);
  EXPECT_EQ(read_uint64_t(base_path / "first.txt"), 14);

  // Create second.txt from first.txt (cache miss)
  const auto result_6 = build.dispatch<std::uint64_t>(
      handler_multiply, base_path / "second.txt",
      {sourcemeta::one::Build::dependency(base_path / "first.txt")}, 3);
  EXPECT_TRUE(result_6);
  EXPECT_EQ(read_uint64_t(base_path / "second.txt"), 42);
}

TEST(Build_e2e, dynamic_dependency) {
  const auto base_path{std::filesystem::path{BINARY_DIRECTORY} /
                       "dynamic_dependency"};
  std::filesystem::remove_all(base_path);

  sourcemeta::one::Build build{BINARY_DIRECTORY};
  write_uint64_t(base_path / "initial.txt", 8);
  EXPECT_EQ(read_uint64_t(base_path / "initial.txt"), 8);
  build.refresh(base_path / "initial.txt");

  // Create copy.txt from initial.txt (cache miss)
  const auto result_1 = build.dispatch<std::filesystem::path>(
      handler_mirror_context_node, base_path / "copy.txt", {},
      base_path / "initial.txt");
  EXPECT_TRUE(result_1);
  EXPECT_EQ(read_uint64_t(base_path / "copy.txt"), 8);

  // Create copy.txt from initial.txt (cache hit)
  const auto result_2 = build.dispatch<std::filesystem::path>(
      handler_mirror_context_node, base_path / "copy.txt", {},
      base_path / "initial.txt");
  // As the context was dynamically registered
  EXPECT_FALSE(result_2);
  EXPECT_EQ(read_uint64_t(base_path / "copy.txt"), 8);
}

TEST(Build_e2e, missing_dynamic_dependency) {
  const auto base_path{std::filesystem::path{BINARY_DIRECTORY} /
                       "missing_dynamic_dependency"};
  std::filesystem::remove_all(base_path);

  sourcemeta::one::Build build{BINARY_DIRECTORY};
  write_uint64_t(base_path / "initial.txt", 8);
  EXPECT_EQ(read_uint64_t(base_path / "initial.txt"), 8);
  build.refresh(base_path / "initial.txt");

  // Create copy.txt from initial.txt (cache miss)
  const auto result_1 = build.dispatch<std::filesystem::path>(
      handler_mirror_context_node_without_callback, base_path / "copy.txt", {},
      base_path / "initial.txt");
  EXPECT_TRUE(result_1);
  EXPECT_EQ(read_uint64_t(base_path / "copy.txt"), 8);

  // Create copy.txt from initial.txt (cache miss)
  const auto result_2 = build.dispatch<std::filesystem::path>(
      handler_mirror_context_node_without_callback, base_path / "copy.txt", {},
      base_path / "initial.txt");
  // As the context was NOT dynamically registered
  EXPECT_TRUE(result_2);
  EXPECT_EQ(read_uint64_t(base_path / "copy.txt"), 8);
}

TEST(Build_e2e, new_dependency_invalidates) {
  const auto base_path{std::filesystem::path{BINARY_DIRECTORY} /
                       "new_dependency_invalidates"};
  std::filesystem::remove_all(base_path);

  sourcemeta::one::Build build{BINARY_DIRECTORY};
  write_uint64_t(base_path / "dep_a.txt", 10);
  build.refresh(base_path / "dep_a.txt");

  // Build with a single dependency (cache miss)
  const auto result_1 = build.dispatch<std::nullptr_t>(
      handler_sum, base_path / "target.txt",
      {sourcemeta::one::Build::dependency(base_path / "dep_a.txt")}, nullptr);
  EXPECT_TRUE(result_1);
  EXPECT_EQ(read_uint64_t(base_path / "target.txt"), 10);

  // Rebuild with the same single dependency (cache hit)
  const auto result_2 = build.dispatch<std::nullptr_t>(
      handler_sum, base_path / "target.txt",
      {sourcemeta::one::Build::dependency(base_path / "dep_a.txt")}, nullptr);
  EXPECT_FALSE(result_2);
  EXPECT_EQ(read_uint64_t(base_path / "target.txt"), 10);

  // Add a new dependency and rebuild (should be cache miss)
  write_uint64_t(base_path / "dep_b.txt", 20);
  build.refresh(base_path / "dep_b.txt");
  const auto result_3 = build.dispatch<std::nullptr_t>(
      handler_sum, base_path / "target.txt",
      {sourcemeta::one::Build::dependency(base_path / "dep_a.txt"),
       sourcemeta::one::Build::dependency(base_path / "dep_b.txt")},
      nullptr);
  EXPECT_TRUE(result_3);
  EXPECT_EQ(read_uint64_t(base_path / "target.txt"), 30);
}

TEST(Build_e2e, removed_make_dependency_invalidates) {
  const auto base_path{std::filesystem::path{BINARY_DIRECTORY} /
                       "removed_make_dependency_invalidates"};
  std::filesystem::remove_all(base_path);

  sourcemeta::one::Build build{BINARY_DIRECTORY};
  write_uint64_t(base_path / "dep_a.txt", 10);
  build.refresh(base_path / "dep_a.txt");
  write_uint64_t(base_path / "dep_b.txt", 20);
  build.refresh(base_path / "dep_b.txt");

  // Build with two dependencies (cache miss)
  const auto result_1 = build.dispatch<std::nullptr_t>(
      handler_sum, base_path / "target.txt",
      {sourcemeta::one::Build::dependency(base_path / "dep_a.txt"),
       sourcemeta::one::Build::dependency(base_path / "dep_b.txt")},
      nullptr);
  EXPECT_TRUE(result_1);
  EXPECT_EQ(read_uint64_t(base_path / "target.txt"), 30);

  // Rebuild with same two dependencies (cache hit)
  const auto result_2 = build.dispatch<std::nullptr_t>(
      handler_sum, base_path / "target.txt",
      {sourcemeta::one::Build::dependency(base_path / "dep_a.txt"),
       sourcemeta::one::Build::dependency(base_path / "dep_b.txt")},
      nullptr);
  EXPECT_FALSE(result_2);
  EXPECT_EQ(read_uint64_t(base_path / "target.txt"), 30);

  // Remove dep_b and rebuild (should be cache miss)
  const auto result_3 = build.dispatch<std::nullptr_t>(
      handler_sum, base_path / "target.txt",
      {sourcemeta::one::Build::dependency(base_path / "dep_a.txt")}, nullptr);
  EXPECT_TRUE(result_3);
  EXPECT_EQ(read_uint64_t(base_path / "target.txt"), 10);
}

TEST(Build_e2e, remove_all_static_dependencies_invalidates) {
  const auto base_path{std::filesystem::path{BINARY_DIRECTORY} /
                       "remove_all_static_deps_invalidates"};
  std::filesystem::remove_all(base_path);

  sourcemeta::one::Build build{BINARY_DIRECTORY};
  write_uint64_t(base_path / "dep_a.txt", 10);
  build.refresh(base_path / "dep_a.txt");

  // Build with one dependency (cache miss)
  const auto result_1 = build.dispatch<std::nullptr_t>(
      handler_sum, base_path / "target.txt",
      {sourcemeta::one::Build::dependency(base_path / "dep_a.txt")}, nullptr);
  EXPECT_TRUE(result_1);
  EXPECT_EQ(read_uint64_t(base_path / "target.txt"), 10);

  // Rebuild with same dependency (cache hit)
  const auto result_2 = build.dispatch<std::nullptr_t>(
      handler_sum, base_path / "target.txt",
      {sourcemeta::one::Build::dependency(base_path / "dep_a.txt")}, nullptr);
  EXPECT_FALSE(result_2);
  EXPECT_EQ(read_uint64_t(base_path / "target.txt"), 10);

  // Remove all static deps and rebuild (should be cache miss)
  const auto result_3 = build.dispatch<std::nullptr_t>(
      handler_sum, base_path / "target.txt", {}, nullptr);
  EXPECT_TRUE(result_3);
  EXPECT_EQ(read_uint64_t(base_path / "target.txt"), 0);
}

TEST(Build_e2e, replaced_make_dependency_invalidates) {
  const auto base_path{std::filesystem::path{BINARY_DIRECTORY} /
                       "replaced_make_dependency_invalidates"};
  std::filesystem::remove_all(base_path);

  sourcemeta::one::Build build{BINARY_DIRECTORY};
  write_uint64_t(base_path / "dep_a.txt", 10);
  build.refresh(base_path / "dep_a.txt");
  write_uint64_t(base_path / "dep_b.txt", 20);
  build.refresh(base_path / "dep_b.txt");

  // Build with dep_a (cache miss)
  const auto result_1 = build.dispatch<std::nullptr_t>(
      handler_sum, base_path / "target.txt",
      {sourcemeta::one::Build::dependency(base_path / "dep_a.txt")}, nullptr);
  EXPECT_TRUE(result_1);
  EXPECT_EQ(read_uint64_t(base_path / "target.txt"), 10);

  // Rebuild with dep_a (cache hit)
  const auto result_2 = build.dispatch<std::nullptr_t>(
      handler_sum, base_path / "target.txt",
      {sourcemeta::one::Build::dependency(base_path / "dep_a.txt")}, nullptr);
  EXPECT_FALSE(result_2);
  EXPECT_EQ(read_uint64_t(base_path / "target.txt"), 10);

  // Replace dep_a with dep_b (should be cache miss)
  const auto result_3 = build.dispatch<std::nullptr_t>(
      handler_sum, base_path / "target.txt",
      {sourcemeta::one::Build::dependency(base_path / "dep_b.txt")}, nullptr);
  EXPECT_TRUE(result_3);
  EXPECT_EQ(read_uint64_t(base_path / "target.txt"), 20);
}

TEST(Build_e2e, reordered_static_dependencies_invalidates) {
  const auto base_path{std::filesystem::path{BINARY_DIRECTORY} /
                       "reordered_static_deps_invalidates"};
  std::filesystem::remove_all(base_path);

  sourcemeta::one::Build build{BINARY_DIRECTORY};
  write_uint64_t(base_path / "dep_a.txt", 10);
  build.refresh(base_path / "dep_a.txt");
  write_uint64_t(base_path / "dep_b.txt", 20);
  build.refresh(base_path / "dep_b.txt");

  // Build with {a, b} (cache miss)
  const auto result_1 = build.dispatch<std::nullptr_t>(
      handler_sum, base_path / "target.txt",
      {sourcemeta::one::Build::dependency(base_path / "dep_a.txt"),
       sourcemeta::one::Build::dependency(base_path / "dep_b.txt")},
      nullptr);
  EXPECT_TRUE(result_1);
  EXPECT_EQ(read_uint64_t(base_path / "target.txt"), 30);

  // Rebuild with {a, b} (cache hit)
  const auto result_2 = build.dispatch<std::nullptr_t>(
      handler_sum, base_path / "target.txt",
      {sourcemeta::one::Build::dependency(base_path / "dep_a.txt"),
       sourcemeta::one::Build::dependency(base_path / "dep_b.txt")},
      nullptr);
  EXPECT_FALSE(result_2);
  EXPECT_EQ(read_uint64_t(base_path / "target.txt"), 30);

  // Reorder to {b, a} (should be cache miss)
  const auto result_3 = build.dispatch<std::nullptr_t>(
      handler_sum, base_path / "target.txt",
      {sourcemeta::one::Build::dependency(base_path / "dep_b.txt"),
       sourcemeta::one::Build::dependency(base_path / "dep_a.txt")},
      nullptr);
  EXPECT_TRUE(result_3);
  EXPECT_EQ(read_uint64_t(base_path / "target.txt"), 30);
}

TEST(Build_e2e, dynamic_deps_do_not_interfere_with_static_comparison) {
  const auto base_path{std::filesystem::path{BINARY_DIRECTORY} /
                       "dynamic_no_interfere_static"};
  std::filesystem::remove_all(base_path);

  sourcemeta::one::Build build{BINARY_DIRECTORY};
  write_uint64_t(base_path / "dep_static.txt", 10);
  build.refresh(base_path / "dep_static.txt");
  write_uint64_t(base_path / "dep_dynamic.txt", 5);
  build.refresh(base_path / "dep_dynamic.txt");

  // Build with one static dep and one dynamic dep (cache miss)
  const auto result_1 = build.dispatch<std::filesystem::path>(
      handler_sum_with_dynamic, base_path / "target.txt",
      {sourcemeta::one::Build::dependency(base_path / "dep_static.txt")},
      base_path / "dep_dynamic.txt");
  EXPECT_TRUE(result_1);
  EXPECT_EQ(read_uint64_t(base_path / "target.txt"), 15);

  // Rebuild with same static dep (cache hit, dynamic dep in .deps is fine)
  const auto result_2 = build.dispatch<std::filesystem::path>(
      handler_sum_with_dynamic, base_path / "target.txt",
      {sourcemeta::one::Build::dependency(base_path / "dep_static.txt")},
      base_path / "dep_dynamic.txt");
  EXPECT_FALSE(result_2);
  EXPECT_EQ(read_uint64_t(base_path / "target.txt"), 15);
}

TEST(Build_e2e, persistence_across_runs) {
  const auto base_path{std::filesystem::path{BINARY_DIRECTORY} /
                       "persistence_across_runs"};
  std::filesystem::remove_all(base_path);
  const auto output_path{base_path / "output"};
  const auto input_path{base_path / "input"};
  std::filesystem::create_directories(output_path);
  std::filesystem::create_directories(input_path);
  write_uint64_t(input_path / "initial.txt", 8);

  // First run: build and write dependencies
  {
    sourcemeta::one::Build build{output_path};
    build.refresh(input_path / "initial.txt");

    const auto result = build.dispatch<std::uint64_t>(
        handler_multiply, output_path / "first.txt",
        {sourcemeta::one::Build::dependency(input_path / "initial.txt")}, 2);
    EXPECT_TRUE(result);
    EXPECT_EQ(read_uint64_t(output_path / "first.txt"), 16);

    build.write_dependencies(
        [](const std::filesystem::path &) { return true; });
  }

  // Second run: new Build instance reads deps.txt, should cache hit
  {
    sourcemeta::one::Build build{output_path};

    const auto result = build.dispatch<std::uint64_t>(
        handler_multiply, output_path / "first.txt",
        {sourcemeta::one::Build::dependency(input_path / "initial.txt")}, 2);
    EXPECT_FALSE(result);
    EXPECT_EQ(read_uint64_t(output_path / "first.txt"), 16);
  }
}

TEST(Build_e2e, persistence_invalidates_on_change) {
  const auto base_path{std::filesystem::path{BINARY_DIRECTORY} /
                       "persistence_invalidates_on_change"};
  std::filesystem::remove_all(base_path);
  const auto output_path{base_path / "output"};
  const auto input_path{base_path / "input"};
  std::filesystem::create_directories(output_path);
  std::filesystem::create_directories(input_path);
  write_uint64_t(input_path / "initial.txt", 8);

  // First run
  {
    sourcemeta::one::Build build{output_path};
    build.refresh(input_path / "initial.txt");

    const auto result = build.dispatch<std::uint64_t>(
        handler_multiply, output_path / "first.txt",
        {sourcemeta::one::Build::dependency(input_path / "initial.txt")}, 2);
    EXPECT_TRUE(result);
    EXPECT_EQ(read_uint64_t(output_path / "first.txt"), 16);

    build.write_dependencies(
        [](const std::filesystem::path &) { return true; });
  }

  // Modify the input between runs and ensure the timestamp is
  // clearly newer, as some filesystems have second-level resolution
  write_uint64_t(input_path / "initial.txt", 100);
  std::filesystem::last_write_time(
      input_path / "initial.txt",
      std::filesystem::file_time_type::clock::now() + std::chrono::seconds(10));

  // Second run: should cache miss because initial.txt changed
  {
    sourcemeta::one::Build build{output_path};

    const auto result = build.dispatch<std::uint64_t>(
        handler_multiply, output_path / "first.txt",
        {sourcemeta::one::Build::dependency(input_path / "initial.txt")}, 2);
    EXPECT_TRUE(result);
    EXPECT_EQ(read_uint64_t(output_path / "first.txt"), 200);
  }
}

TEST(Build_e2e, persistence_filtered_entry_not_restored) {
  const auto base_path{std::filesystem::path{BINARY_DIRECTORY} /
                       "persistence_filtered"};
  std::filesystem::remove_all(base_path);
  const auto output_path{base_path / "output"};
  const auto input_path{base_path / "input"};
  std::filesystem::create_directories(output_path);
  std::filesystem::create_directories(input_path);
  write_uint64_t(input_path / "initial.txt", 8);

  // First run: build two targets, filter one out during write
  {
    sourcemeta::one::Build build{output_path};
    build.refresh(input_path / "initial.txt");

    build.dispatch<std::uint64_t>(
        handler_multiply, output_path / "kept.txt",
        {sourcemeta::one::Build::dependency(input_path / "initial.txt")}, 2);

    build.dispatch<std::uint64_t>(
        handler_multiply, output_path / "dropped.txt",
        {sourcemeta::one::Build::dependency(input_path / "initial.txt")}, 3);

    const auto kept_path{(output_path / "kept.txt").lexically_normal()};
    build.write_dependencies([&kept_path](const std::filesystem::path &path) {
      return path == kept_path;
    });
  }

  // Second run: kept should be cached, dropped should not
  {
    sourcemeta::one::Build build{output_path};

    EXPECT_TRUE(build.has_dependencies(output_path / "kept.txt"));
    EXPECT_FALSE(build.has_dependencies(output_path / "dropped.txt"));
  }
}

TEST(Build_e2e, dynamic_dependency_stale_invalidates) {
  const auto base_path{std::filesystem::path{BINARY_DIRECTORY} /
                       "dynamic_dep_stale_invalidates"};
  std::filesystem::remove_all(base_path);

  sourcemeta::one::Build build{BINARY_DIRECTORY};
  write_uint64_t(base_path / "dep_static.txt", 10);
  build.refresh(base_path / "dep_static.txt");
  write_uint64_t(base_path / "dep_dynamic.txt", 5);
  build.refresh(base_path / "dep_dynamic.txt");

  // Build (cache miss)
  const auto result_1 = build.dispatch<std::filesystem::path>(
      handler_sum_with_dynamic, base_path / "target.txt",
      {sourcemeta::one::Build::dependency(base_path / "dep_static.txt")},
      base_path / "dep_dynamic.txt");
  EXPECT_TRUE(result_1);
  EXPECT_EQ(read_uint64_t(base_path / "target.txt"), 15);

  // Rebuild unchanged (cache hit)
  const auto result_2 = build.dispatch<std::filesystem::path>(
      handler_sum_with_dynamic, base_path / "target.txt",
      {sourcemeta::one::Build::dependency(base_path / "dep_static.txt")},
      base_path / "dep_dynamic.txt");
  EXPECT_FALSE(result_2);
  EXPECT_EQ(read_uint64_t(base_path / "target.txt"), 15);

  // Update the dynamic dep file and rebuild (should be cache miss)
  write_uint64_t(base_path / "dep_dynamic.txt", 100);
  build.refresh(base_path / "dep_dynamic.txt");
  const auto result_3 = build.dispatch<std::filesystem::path>(
      handler_sum_with_dynamic, base_path / "target.txt",
      {sourcemeta::one::Build::dependency(base_path / "dep_static.txt")},
      base_path / "dep_dynamic.txt");
  EXPECT_TRUE(result_3);
  EXPECT_EQ(read_uint64_t(base_path / "target.txt"), 110);
}
