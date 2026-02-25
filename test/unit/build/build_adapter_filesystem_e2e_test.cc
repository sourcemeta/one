#include <gtest/gtest.h>

#include <sourcemeta/one/build.h>

#include <cassert>
#include <string>

#include "build_test_utils.h"

auto read_uint64_t(const std::filesystem::path &path) -> std::uint64_t {
  return std::stoull(READ_FILE(path));
}

auto write_uint64_t(const std::filesystem::path &path,
                    const std::uint64_t input) -> void {
  WRITE_FILE(path, std::to_string(input));
}

template <typename Context, typename Adapter>
auto HANDLER_MULTIPLY(
    const typename Adapter::node_type &destination,
    const sourcemeta::one::BuildDependencies<typename Adapter::node_type>
        &dependencies,
    const sourcemeta::one::BuildDynamicCallback<typename Adapter::node_type> &,
    const Context &value) -> void {
  assert(dependencies.size() == 1);
  write_uint64_t(destination, read_uint64_t(dependencies.front()) * value);
}

template <typename Context, typename Adapter>
auto HANDLER_MIRROR_CONTEXT_NODE(
    const typename Adapter::node_type &destination,
    const sourcemeta::one::BuildDependencies<typename Adapter::node_type> &,
    const sourcemeta::one::BuildDynamicCallback<typename Adapter::node_type>
        &callback,
    const Context &context) -> void {
  const auto input{read_uint64_t(context)};
  callback(context);
  write_uint64_t(destination, input);
}

template <typename Context, typename Adapter>
auto HANDLER_MIRROR_CONTEXT_NODE_WITHOUT_CALLBACK(
    const typename Adapter::node_type &destination,
    const sourcemeta::one::BuildDependencies<typename Adapter::node_type> &,
    const sourcemeta::one::BuildDynamicCallback<typename Adapter::node_type> &,
    const Context &context) -> void {
  const auto input{read_uint64_t(context)};
  write_uint64_t(destination, input);
}

TEST(Build_Adapter_Filesystem_e2e, simple_cache_miss_hit) {
  using Adapter = sourcemeta::one::BuildAdapterFilesystem;
  using Context = std::uint64_t;
  Adapter adapter{BINARY_DIRECTORY};

  const auto base_path{std::filesystem::path{BINARY_DIRECTORY} /
                       "simple_cache_miss_hit"};
  write_uint64_t(base_path / "initial.txt", 8);
  EXPECT_EQ(read_uint64_t(base_path / "initial.txt"), 8);
  adapter.refresh(base_path / "initial.txt");

  // Create first.txt from initial.txt (cache miss)
  const auto result_1 = sourcemeta::one::build<Context>(
      adapter, HANDLER_MULTIPLY<Context, Adapter>, base_path / "first.txt",
      {base_path / "initial.txt"}, 2);
  EXPECT_TRUE(result_1);
  EXPECT_EQ(read_uint64_t(base_path / "first.txt"), 16);

  // Create second.txt from first.txt (cache miss)
  const auto result_2 = sourcemeta::one::build<Context>(
      adapter, HANDLER_MULTIPLY<Context, Adapter>, base_path / "second.txt",
      {base_path / "first.txt"}, 3);
  EXPECT_TRUE(result_2);
  EXPECT_EQ(read_uint64_t(base_path / "second.txt"), 48);

  // Create first.txt from initial.txt (cache hit)
  const auto result_3 = sourcemeta::one::build<Context>(
      adapter, HANDLER_MULTIPLY<Context, Adapter>, base_path / "first.txt",
      {base_path / "initial.txt"}, 2);
  EXPECT_FALSE(result_3);
  EXPECT_EQ(read_uint64_t(base_path / "first.txt"), 16);

  // Create second.txt from first.txt (cache hit)
  const auto result_4 = sourcemeta::one::build<Context>(
      adapter, HANDLER_MULTIPLY<Context, Adapter>, base_path / "second.txt",
      {base_path / "first.txt"}, 3);
  EXPECT_FALSE(result_4);
  EXPECT_EQ(read_uint64_t(base_path / "second.txt"), 48);

  // Update initial.txt
  write_uint64_t(base_path / "initial.txt", 7);
  EXPECT_EQ(read_uint64_t(base_path / "initial.txt"), 7);
  adapter.refresh(base_path / "initial.txt");

  // Create first.txt from initial.txt (cache miss)
  const auto result_5 = sourcemeta::one::build<Context>(
      adapter, HANDLER_MULTIPLY<Context, Adapter>, base_path / "first.txt",
      {base_path / "initial.txt"}, 2);
  EXPECT_TRUE(result_5);
  EXPECT_EQ(read_uint64_t(base_path / "first.txt"), 14);

  // Create second.txt from first.txt (cache miss)
  const auto result_6 = sourcemeta::one::build<Context>(
      adapter, HANDLER_MULTIPLY<Context, Adapter>, base_path / "second.txt",
      {base_path / "first.txt"}, 3);
  EXPECT_TRUE(result_6);
  EXPECT_EQ(read_uint64_t(base_path / "second.txt"), 42);
}

TEST(Build_Adapter_Filesystem_e2e, dynamic_dependency) {
  using Adapter = sourcemeta::one::BuildAdapterFilesystem;
  using Context = Adapter::node_type;
  Adapter adapter{BINARY_DIRECTORY};

  const auto base_path{std::filesystem::path{BINARY_DIRECTORY} /
                       "dynamic_dependency"};
  write_uint64_t(base_path / "initial.txt", 8);
  EXPECT_EQ(read_uint64_t(base_path / "initial.txt"), 8);
  adapter.refresh(base_path / "initial.txt");

  // Create copy.txt from initial.txt (cache miss)
  const auto result_1 = sourcemeta::one::build<Context>(
      adapter, HANDLER_MIRROR_CONTEXT_NODE<Context, Adapter>,
      base_path / "copy.txt", {}, base_path / "initial.txt");
  EXPECT_TRUE(result_1);
  EXPECT_EQ(read_uint64_t(base_path / "copy.txt"), 8);

  // Create copy.txt from initial.txt (cache hit)
  const auto result_2 = sourcemeta::one::build<Context>(
      adapter, HANDLER_MIRROR_CONTEXT_NODE<Context, Adapter>,
      base_path / "copy.txt", {}, base_path / "initial.txt");
  // As the context was dynamically registered
  EXPECT_FALSE(result_2);
  EXPECT_EQ(read_uint64_t(base_path / "copy.txt"), 8);
}

TEST(Build_Adapter_Filesystem_e2e, missing_dynamic_dependency) {
  using Adapter = sourcemeta::one::BuildAdapterFilesystem;
  using Context = Adapter::node_type;
  Adapter adapter{BINARY_DIRECTORY};

  const auto base_path{std::filesystem::path{BINARY_DIRECTORY} /
                       "missing_dynamic_dependency"};
  write_uint64_t(base_path / "initial.txt", 8);
  EXPECT_EQ(read_uint64_t(base_path / "initial.txt"), 8);
  adapter.refresh(base_path / "initial.txt");

  // Create copy.txt from initial.txt (cache miss)
  const auto result_1 = sourcemeta::one::build<Context>(
      adapter, HANDLER_MIRROR_CONTEXT_NODE_WITHOUT_CALLBACK<Context, Adapter>,
      base_path / "copy.txt", {}, base_path / "initial.txt");
  EXPECT_TRUE(result_1);
  EXPECT_EQ(read_uint64_t(base_path / "copy.txt"), 8);

  // Create copy.txt from initial.txt (cache miss)
  const auto result_2 = sourcemeta::one::build<Context>(
      adapter, HANDLER_MIRROR_CONTEXT_NODE_WITHOUT_CALLBACK<Context, Adapter>,
      base_path / "copy.txt", {}, base_path / "initial.txt");
  // As the context was NOT dynamically registered
  EXPECT_TRUE(result_2);
  EXPECT_EQ(read_uint64_t(base_path / "copy.txt"), 8);
}
