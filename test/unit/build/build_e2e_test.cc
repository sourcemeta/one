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
                 read_uint64_t(dependencies.front().get()) * value);
}

static auto
handler_sum(const std::filesystem::path &destination,
            const sourcemeta::one::Build::Dependencies &dependencies,
            const sourcemeta::one::Build::DynamicCallback &,
            const std::nullptr_t &) -> void {
  std::uint64_t total{0};
  for (const auto &dependency : dependencies) {
    total += read_uint64_t(dependency.get());
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
    total += read_uint64_t(dependency.get());
  }

  total += read_uint64_t(context);
  callback(context);
  write_uint64_t(destination, total);
}

TEST(Build_e2e, simple_cache_miss_hit) {
  const auto base_path{std::filesystem::path{BINARY_DIRECTORY} /
                       "simple_cache_miss_hit"};
  std::filesystem::remove_all(base_path);
  const auto output_path{base_path / "output"};
  const auto input_path{base_path / "input"};
  std::filesystem::create_directories(input_path);
  write_uint64_t(input_path / "initial.txt", 8);

  const auto initial_txt{input_path / "initial.txt"};
  const auto first_txt{output_path / "first.txt"};
  const auto second_txt{output_path / "second.txt"};

  // First run: build first.txt and second.txt (cache miss)
  {
    sourcemeta::one::Build build{output_path};
    build.refresh(initial_txt);

    const auto result_1 = build.dispatch<std::uint64_t>(
        handler_multiply, first_txt, static_cast<std::uint64_t>(2),
        initial_txt);
    EXPECT_TRUE(result_1);
    EXPECT_EQ(read_uint64_t(first_txt), 16);

    const auto result_2 = build.dispatch<std::uint64_t>(
        handler_multiply, second_txt, static_cast<std::uint64_t>(3), first_txt);
    EXPECT_TRUE(result_2);
    EXPECT_EQ(read_uint64_t(second_txt), 48);

    build.finish();
  }

  // Second run: same inputs, should cache hit
  {
    sourcemeta::one::Build build{output_path};

    const auto result_1 = build.dispatch<std::uint64_t>(
        handler_multiply, first_txt, static_cast<std::uint64_t>(2),
        initial_txt);
    EXPECT_FALSE(result_1);
    EXPECT_EQ(read_uint64_t(first_txt), 16);

    const auto result_2 = build.dispatch<std::uint64_t>(
        handler_multiply, second_txt, static_cast<std::uint64_t>(3), first_txt);
    EXPECT_FALSE(result_2);
    EXPECT_EQ(read_uint64_t(second_txt), 48);

    build.finish();
  }

  // Third run: update initial.txt, should cache miss
  write_uint64_t(initial_txt, 7);
  std::filesystem::last_write_time(
      initial_txt,
      std::filesystem::file_time_type::clock::now() + std::chrono::seconds(10));

  {
    sourcemeta::one::Build build{output_path};

    const auto result_1 = build.dispatch<std::uint64_t>(
        handler_multiply, first_txt, static_cast<std::uint64_t>(2),
        initial_txt);
    EXPECT_TRUE(result_1);
    EXPECT_EQ(read_uint64_t(first_txt), 14);

    const auto result_2 = build.dispatch<std::uint64_t>(
        handler_multiply, second_txt, static_cast<std::uint64_t>(3), first_txt);
    EXPECT_TRUE(result_2);
    EXPECT_EQ(read_uint64_t(second_txt), 42);
  }
}

TEST(Build_e2e, dynamic_dependency) {
  const auto base_path{std::filesystem::path{BINARY_DIRECTORY} /
                       "dynamic_dependency"};
  std::filesystem::remove_all(base_path);
  const auto output_path{base_path / "output"};
  const auto input_path{base_path / "input"};
  std::filesystem::create_directories(input_path);
  write_uint64_t(input_path / "initial.txt", 8);

  const auto initial_txt{input_path / "initial.txt"};
  const auto copy_txt{output_path / "copy.txt"};

  // First run: cache miss, dynamic dependency registered
  {
    sourcemeta::one::Build build{output_path};
    build.refresh(initial_txt);

    const auto result = build.dispatch<std::filesystem::path>(
        handler_mirror_context_node, copy_txt, initial_txt);
    EXPECT_TRUE(result);
    EXPECT_EQ(read_uint64_t(copy_txt), 8);

    build.finish();
  }

  // Second run: cache hit because dynamic dependency was registered
  {
    sourcemeta::one::Build build{output_path};

    const auto result = build.dispatch<std::filesystem::path>(
        handler_mirror_context_node, copy_txt, initial_txt);
    EXPECT_FALSE(result);
    EXPECT_EQ(read_uint64_t(copy_txt), 8);
  }
}

TEST(Build_e2e, missing_dynamic_dependency) {
  const auto base_path{std::filesystem::path{BINARY_DIRECTORY} /
                       "missing_dynamic_dependency"};
  std::filesystem::remove_all(base_path);
  const auto output_path{base_path / "output"};
  const auto input_path{base_path / "input"};
  std::filesystem::create_directories(input_path);
  write_uint64_t(input_path / "initial.txt", 8);

  const auto initial_txt{input_path / "initial.txt"};
  const auto copy_txt{output_path / "copy.txt"};

  // First run: cache miss, dynamic dependency NOT registered
  {
    sourcemeta::one::Build build{output_path};
    build.refresh(initial_txt);

    const auto result = build.dispatch<std::filesystem::path>(
        handler_mirror_context_node_without_callback, copy_txt, initial_txt);
    EXPECT_TRUE(result);
    EXPECT_EQ(read_uint64_t(copy_txt), 8);

    build.finish();
  }

  // Second run: cache miss because dynamic dependency was not registered
  {
    sourcemeta::one::Build build{output_path};

    const auto result = build.dispatch<std::filesystem::path>(
        handler_mirror_context_node_without_callback, copy_txt, initial_txt);
    EXPECT_TRUE(result);
    EXPECT_EQ(read_uint64_t(copy_txt), 8);
  }
}

TEST(Build_e2e, new_dependency_invalidates) {
  const auto base_path{std::filesystem::path{BINARY_DIRECTORY} /
                       "new_dependency_invalidates"};
  std::filesystem::remove_all(base_path);
  const auto output_path{base_path / "output"};
  const auto input_path{base_path / "input"};
  std::filesystem::create_directories(input_path);
  write_uint64_t(input_path / "dep_a.txt", 10);

  const auto dep_a{input_path / "dep_a.txt"};
  const auto dep_b{input_path / "dep_b.txt"};
  const auto target{output_path / "target.txt"};

  // First run: build with one dependency
  {
    sourcemeta::one::Build build{output_path};
    build.refresh(dep_a);

    const auto result =
        build.dispatch<std::nullptr_t>(handler_sum, target, nullptr, dep_a);
    EXPECT_TRUE(result);
    EXPECT_EQ(read_uint64_t(target), 10);

    build.finish();
  }

  // Second run: same dependency, cache hit
  {
    sourcemeta::one::Build build{output_path};

    const auto result =
        build.dispatch<std::nullptr_t>(handler_sum, target, nullptr, dep_a);
    EXPECT_FALSE(result);
    EXPECT_EQ(read_uint64_t(target), 10);

    build.finish();
  }

  // Third run: add a new dependency, cache miss
  write_uint64_t(dep_b, 20);

  {
    sourcemeta::one::Build build{output_path};
    build.refresh(dep_b);

    const auto result = build.dispatch<std::nullptr_t>(handler_sum, target,
                                                       nullptr, dep_a, dep_b);
    EXPECT_TRUE(result);
    EXPECT_EQ(read_uint64_t(target), 30);
  }
}

TEST(Build_e2e, removed_make_dependency_invalidates) {
  const auto base_path{std::filesystem::path{BINARY_DIRECTORY} /
                       "removed_make_dependency_invalidates"};
  std::filesystem::remove_all(base_path);
  const auto output_path{base_path / "output"};
  const auto input_path{base_path / "input"};
  std::filesystem::create_directories(input_path);
  write_uint64_t(input_path / "dep_a.txt", 10);
  write_uint64_t(input_path / "dep_b.txt", 20);

  const auto dep_a{input_path / "dep_a.txt"};
  const auto dep_b{input_path / "dep_b.txt"};
  const auto target{output_path / "target.txt"};

  // First run: build with two dependencies
  {
    sourcemeta::one::Build build{output_path};
    build.refresh(dep_a);
    build.refresh(dep_b);

    const auto result = build.dispatch<std::nullptr_t>(handler_sum, target,
                                                       nullptr, dep_a, dep_b);
    EXPECT_TRUE(result);
    EXPECT_EQ(read_uint64_t(target), 30);

    build.finish();
  }

  // Second run: same two dependencies, cache hit
  {
    sourcemeta::one::Build build{output_path};

    const auto result = build.dispatch<std::nullptr_t>(handler_sum, target,
                                                       nullptr, dep_a, dep_b);
    EXPECT_FALSE(result);
    EXPECT_EQ(read_uint64_t(target), 30);

    build.finish();
  }

  // Third run: remove dep_b, cache miss
  {
    sourcemeta::one::Build build{output_path};

    const auto result =
        build.dispatch<std::nullptr_t>(handler_sum, target, nullptr, dep_a);
    EXPECT_TRUE(result);
    EXPECT_EQ(read_uint64_t(target), 10);
  }
}

TEST(Build_e2e, remove_all_static_dependencies_invalidates) {
  const auto base_path{std::filesystem::path{BINARY_DIRECTORY} /
                       "remove_all_static_deps_invalidates"};
  std::filesystem::remove_all(base_path);
  const auto output_path{base_path / "output"};
  const auto input_path{base_path / "input"};
  std::filesystem::create_directories(input_path);
  write_uint64_t(input_path / "dep_a.txt", 10);

  const auto dep_a{input_path / "dep_a.txt"};
  const auto target{output_path / "target.txt"};

  // First run: build with one dependency
  {
    sourcemeta::one::Build build{output_path};
    build.refresh(dep_a);

    const auto result =
        build.dispatch<std::nullptr_t>(handler_sum, target, nullptr, dep_a);
    EXPECT_TRUE(result);
    EXPECT_EQ(read_uint64_t(target), 10);

    build.finish();
  }

  // Second run: same dependency, cache hit
  {
    sourcemeta::one::Build build{output_path};

    const auto result =
        build.dispatch<std::nullptr_t>(handler_sum, target, nullptr, dep_a);
    EXPECT_FALSE(result);
    EXPECT_EQ(read_uint64_t(target), 10);

    build.finish();
  }

  // Third run: remove all static deps, cache miss
  {
    sourcemeta::one::Build build{output_path};

    const auto result =
        build.dispatch<std::nullptr_t>(handler_sum, target, nullptr);
    EXPECT_TRUE(result);
    EXPECT_EQ(read_uint64_t(target), 0);
  }
}

TEST(Build_e2e, replaced_make_dependency_invalidates) {
  const auto base_path{std::filesystem::path{BINARY_DIRECTORY} /
                       "replaced_make_dependency_invalidates"};
  std::filesystem::remove_all(base_path);
  const auto output_path{base_path / "output"};
  const auto input_path{base_path / "input"};
  std::filesystem::create_directories(input_path);
  write_uint64_t(input_path / "dep_a.txt", 10);
  write_uint64_t(input_path / "dep_b.txt", 20);

  const auto dep_a{input_path / "dep_a.txt"};
  const auto dep_b{input_path / "dep_b.txt"};
  const auto target{output_path / "target.txt"};

  // First run: build with dep_a
  {
    sourcemeta::one::Build build{output_path};
    build.refresh(dep_a);
    build.refresh(dep_b);

    const auto result =
        build.dispatch<std::nullptr_t>(handler_sum, target, nullptr, dep_a);
    EXPECT_TRUE(result);
    EXPECT_EQ(read_uint64_t(target), 10);

    build.finish();
  }

  // Second run: same dep_a, cache hit
  {
    sourcemeta::one::Build build{output_path};

    const auto result =
        build.dispatch<std::nullptr_t>(handler_sum, target, nullptr, dep_a);
    EXPECT_FALSE(result);
    EXPECT_EQ(read_uint64_t(target), 10);

    build.finish();
  }

  // Third run: replace dep_a with dep_b, cache miss
  {
    sourcemeta::one::Build build{output_path};

    const auto result =
        build.dispatch<std::nullptr_t>(handler_sum, target, nullptr, dep_b);
    EXPECT_TRUE(result);
    EXPECT_EQ(read_uint64_t(target), 20);
  }
}

TEST(Build_e2e, reordered_static_dependencies_invalidates) {
  const auto base_path{std::filesystem::path{BINARY_DIRECTORY} /
                       "reordered_static_deps_invalidates"};
  std::filesystem::remove_all(base_path);
  const auto output_path{base_path / "output"};
  const auto input_path{base_path / "input"};
  std::filesystem::create_directories(input_path);
  write_uint64_t(input_path / "dep_a.txt", 10);
  write_uint64_t(input_path / "dep_b.txt", 20);

  const auto dep_a{input_path / "dep_a.txt"};
  const auto dep_b{input_path / "dep_b.txt"};
  const auto target{output_path / "target.txt"};

  // First run: build with {a, b}
  {
    sourcemeta::one::Build build{output_path};
    build.refresh(dep_a);
    build.refresh(dep_b);

    const auto result = build.dispatch<std::nullptr_t>(handler_sum, target,
                                                       nullptr, dep_a, dep_b);
    EXPECT_TRUE(result);
    EXPECT_EQ(read_uint64_t(target), 30);

    build.finish();
  }

  // Second run: same {a, b}, cache hit
  {
    sourcemeta::one::Build build{output_path};

    const auto result = build.dispatch<std::nullptr_t>(handler_sum, target,
                                                       nullptr, dep_a, dep_b);
    EXPECT_FALSE(result);
    EXPECT_EQ(read_uint64_t(target), 30);

    build.finish();
  }

  // Third run: reorder to {b, a}, cache miss
  {
    sourcemeta::one::Build build{output_path};

    const auto result = build.dispatch<std::nullptr_t>(handler_sum, target,
                                                       nullptr, dep_b, dep_a);
    EXPECT_TRUE(result);
    EXPECT_EQ(read_uint64_t(target), 30);
  }
}

TEST(Build_e2e, dynamic_deps_do_not_interfere_with_static_comparison) {
  const auto base_path{std::filesystem::path{BINARY_DIRECTORY} /
                       "dynamic_no_interfere_static"};
  std::filesystem::remove_all(base_path);
  const auto output_path{base_path / "output"};
  const auto input_path{base_path / "input"};
  std::filesystem::create_directories(input_path);
  write_uint64_t(input_path / "dep_static.txt", 10);
  write_uint64_t(input_path / "dep_dynamic.txt", 5);

  const auto dep_static{input_path / "dep_static.txt"};
  const auto dep_dynamic{input_path / "dep_dynamic.txt"};
  const auto target{output_path / "target.txt"};

  // First run: build with one static dep and one dynamic dep
  {
    sourcemeta::one::Build build{output_path};
    build.refresh(dep_static);
    build.refresh(dep_dynamic);

    const auto result = build.dispatch<std::filesystem::path>(
        handler_sum_with_dynamic, target, dep_dynamic, dep_static);
    EXPECT_TRUE(result);
    EXPECT_EQ(read_uint64_t(target), 15);

    build.finish();
  }

  // Second run: same static dep, cache hit
  {
    sourcemeta::one::Build build{output_path};

    const auto result = build.dispatch<std::filesystem::path>(
        handler_sum_with_dynamic, target, dep_dynamic, dep_static);
    EXPECT_FALSE(result);
    EXPECT_EQ(read_uint64_t(target), 15);
  }
}

TEST(Build_e2e, persistence_across_runs) {
  const auto base_path{std::filesystem::path{BINARY_DIRECTORY} /
                       "persistence_across_runs"};
  std::filesystem::remove_all(base_path);
  const auto output_path{base_path / "output"};
  const auto input_path{base_path / "input"};
  std::filesystem::create_directories(input_path);
  write_uint64_t(input_path / "initial.txt", 8);

  const auto initial_txt{input_path / "initial.txt"};
  const auto first_txt{output_path / "first.txt"};

  // First run: build and write dependencies
  {
    sourcemeta::one::Build build{output_path};
    build.refresh(initial_txt);

    const auto result = build.dispatch<std::uint64_t>(
        handler_multiply, first_txt, static_cast<std::uint64_t>(2),
        initial_txt);
    EXPECT_TRUE(result);
    EXPECT_EQ(read_uint64_t(first_txt), 16);

    build.finish();
  }

  // Second run: new Build instance reads deps.bin, should cache hit
  {
    sourcemeta::one::Build build{output_path};

    const auto result = build.dispatch<std::uint64_t>(
        handler_multiply, first_txt, static_cast<std::uint64_t>(2),
        initial_txt);
    EXPECT_FALSE(result);
    EXPECT_EQ(read_uint64_t(first_txt), 16);
  }
}

TEST(Build_e2e, persistence_invalidates_on_change) {
  const auto base_path{std::filesystem::path{BINARY_DIRECTORY} /
                       "persistence_invalidates_on_change"};
  std::filesystem::remove_all(base_path);
  const auto output_path{base_path / "output"};
  const auto input_path{base_path / "input"};
  std::filesystem::create_directories(input_path);
  write_uint64_t(input_path / "initial.txt", 8);

  const auto initial_txt{input_path / "initial.txt"};
  const auto first_txt{output_path / "first.txt"};

  // First run
  {
    sourcemeta::one::Build build{output_path};
    build.refresh(initial_txt);

    const auto result = build.dispatch<std::uint64_t>(
        handler_multiply, first_txt, static_cast<std::uint64_t>(2),
        initial_txt);
    EXPECT_TRUE(result);
    EXPECT_EQ(read_uint64_t(first_txt), 16);

    build.finish();
  }

  // Modify the input between runs and ensure the timestamp is
  // clearly newer, as some filesystems have second-level resolution
  write_uint64_t(initial_txt, 100);
  std::filesystem::last_write_time(
      initial_txt,
      std::filesystem::file_time_type::clock::now() + std::chrono::seconds(10));

  // Second run: should cache miss because initial.txt changed
  {
    sourcemeta::one::Build build{output_path};

    const auto result = build.dispatch<std::uint64_t>(
        handler_multiply, first_txt, static_cast<std::uint64_t>(2),
        initial_txt);
    EXPECT_TRUE(result);
    EXPECT_EQ(read_uint64_t(first_txt), 200);
  }
}

TEST(Build_e2e, dynamic_dependency_stale_invalidates) {
  const auto base_path{std::filesystem::path{BINARY_DIRECTORY} /
                       "dynamic_dep_stale_invalidates"};
  std::filesystem::remove_all(base_path);
  const auto output_path{base_path / "output"};
  const auto input_path{base_path / "input"};
  std::filesystem::create_directories(input_path);
  write_uint64_t(input_path / "dep_static.txt", 10);
  write_uint64_t(input_path / "dep_dynamic.txt", 5);

  const auto dep_static{input_path / "dep_static.txt"};
  const auto dep_dynamic{input_path / "dep_dynamic.txt"};
  const auto target{output_path / "target.txt"};

  // First run: build
  {
    sourcemeta::one::Build build{output_path};
    build.refresh(dep_static);
    build.refresh(dep_dynamic);

    const auto result = build.dispatch<std::filesystem::path>(
        handler_sum_with_dynamic, target, dep_dynamic, dep_static);
    EXPECT_TRUE(result);
    EXPECT_EQ(read_uint64_t(target), 15);

    build.finish();
  }

  // Second run: unchanged, cache hit
  {
    sourcemeta::one::Build build{output_path};

    const auto result = build.dispatch<std::filesystem::path>(
        handler_sum_with_dynamic, target, dep_dynamic, dep_static);
    EXPECT_FALSE(result);
    EXPECT_EQ(read_uint64_t(target), 15);

    build.finish();
  }

  // Update the dynamic dep file between runs
  write_uint64_t(dep_dynamic, 100);
  std::filesystem::last_write_time(
      dep_dynamic,
      std::filesystem::file_time_type::clock::now() + std::chrono::seconds(10));

  // Third run: should cache miss because dynamic dep changed
  {
    sourcemeta::one::Build build{output_path};

    const auto result = build.dispatch<std::filesystem::path>(
        handler_sum_with_dynamic, target, dep_dynamic, dep_static);
    EXPECT_TRUE(result);
    EXPECT_EQ(read_uint64_t(target), 110);
  }
}
