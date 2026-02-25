#include <gtest/gtest.h>

#include <sourcemeta/one/build.h>

#include "build_test_utils.h"

TEST(Build_Adapter_Filesystem, dependencies_path) {
  sourcemeta::one::BuildAdapterFilesystem adapter{BINARY_DIRECTORY};
  const auto result{adapter.dependencies_path("/foo/bar.baz")};
  EXPECT_EQ(result, "/foo/bar.baz.deps");
}

TEST(Build_Adapter_Filesystem, read_dependencies_stub_1) {
  const std::filesystem::path stub{std::filesystem::path{TEST_DIRECTORY} /
                                   "deps_stub_1.json"};
  sourcemeta::one::BuildAdapterFilesystem adapter{BINARY_DIRECTORY};
  const auto dependencies{adapter.read_dependencies(stub)};
  EXPECT_TRUE(dependencies.has_value());
  EXPECT_EQ(dependencies.value().size(), 2);
  auto iterator{dependencies.value().cbegin()};
  EXPECT_EQ(iterator->string(), "/foo/bar");
  std::advance(iterator, 1);
  EXPECT_EQ(iterator->string(), "/test");
}

TEST(Build_Adapter_Filesystem, read_dependencies_not_exists) {
  const std::filesystem::path stub{std::filesystem::path{TEST_DIRECTORY} /
                                   "unknown"};
  sourcemeta::one::BuildAdapterFilesystem adapter{BINARY_DIRECTORY};
  const auto dependencies{adapter.read_dependencies(stub)};
  EXPECT_FALSE(dependencies.has_value());
}

TEST(Build_Adapter_Filesystem, write_dependencies_1) {
  sourcemeta::one::BuildDependencies<
      sourcemeta::one::BuildAdapterFilesystem::node_type>
      dependencies;
  dependencies.emplace_back("/foo/bar");
  dependencies.emplace_back("/baz");
  dependencies.emplace_back("/test");

  const auto node{std::filesystem::path{BINARY_DIRECTORY} /
                  "write_dependencies_1"};

  WRITE_FILE(node, "test");

  sourcemeta::one::BuildAdapterFilesystem adapter{BINARY_DIRECTORY};
  adapter.write_dependencies(node, dependencies);
  const auto back{adapter.read_dependencies(node)};
  EXPECT_TRUE(back.has_value());
  EXPECT_EQ(back.value().size(), 3);

  auto iterator{back.value().cbegin()};
  EXPECT_EQ(iterator->string(), "/foo/bar");
  std::advance(iterator, 1);
  EXPECT_EQ(iterator->string(), "/baz");
  std::advance(iterator, 1);
  EXPECT_EQ(iterator->string(), "/test");
}

TEST(Build_Adapter_Filesystem, mark_stub_1) {
  const std::filesystem::path stub{std::filesystem::path{TEST_DIRECTORY} /
                                   "deps_stub_1.json.deps"};
  sourcemeta::one::BuildAdapterFilesystem adapter{BINARY_DIRECTORY};
  const auto mark{adapter.mark(stub)};
  EXPECT_TRUE(mark.has_value());
}

TEST(Build_Adapter_Filesystem, mark_stub_not_exists) {
  const std::filesystem::path stub{std::filesystem::path{TEST_DIRECTORY} /
                                   "unknown"};
  sourcemeta::one::BuildAdapterFilesystem adapter{BINARY_DIRECTORY};
  const auto mark{adapter.mark(stub)};
  EXPECT_FALSE(mark.has_value());
}

TEST(Build_Adapter_Filesystem, is_newer_than_same_with_refresh) {
  const std::filesystem::path file{std::filesystem::path{BINARY_DIRECTORY} /
                                   "is_newer_than_same"};
  sourcemeta::one::BuildAdapterFilesystem adapter{BINARY_DIRECTORY};
  WRITE_FILE(file, "test");
  adapter.refresh(file);
  const auto mark{adapter.mark(file)};
  EXPECT_TRUE(mark.has_value());
  EXPECT_FALSE(adapter.is_newer_than(mark.value(), mark.value()));
}

TEST(Build_Adapter_Filesystem, is_newer_than_same_without_refresh) {
  const std::filesystem::path file{std::filesystem::path{BINARY_DIRECTORY} /
                                   "is_newer_than_same"};
  WRITE_FILE(file, "test");
  sourcemeta::one::BuildAdapterFilesystem adapter{BINARY_DIRECTORY};
  const auto mark{adapter.mark(file)};
  EXPECT_TRUE(mark.has_value());
  EXPECT_FALSE(adapter.is_newer_than(mark.value(), mark.value()));
}

TEST(Build_Adapter_Filesystem, is_newer_than) {
  const std::filesystem::path file_1{std::filesystem::path{BINARY_DIRECTORY} /
                                     "is_newer_than" / "1.txt"};
  const std::filesystem::path file_2{std::filesystem::path{BINARY_DIRECTORY} /
                                     "is_newer_than" / "2.txt"};

  sourcemeta::one::BuildAdapterFilesystem adapter{BINARY_DIRECTORY};

  WRITE_FILE(file_1, "test_1");
  adapter.refresh(file_1);
  WRITE_FILE(file_2, "test_2");
  adapter.refresh(file_2);

  const auto mark_1{adapter.mark(file_1)};
  const auto mark_2{adapter.mark(file_2)};

  EXPECT_TRUE(mark_1.has_value());
  EXPECT_TRUE(mark_2.has_value());

  EXPECT_FALSE(adapter.is_newer_than(mark_1.value(), mark_2.value()));
  EXPECT_TRUE(adapter.is_newer_than(mark_2.value(), mark_1.value()));
}

TEST(Build_Adapter_Filesystem, is_newer_than_with_update) {
  const std::filesystem::path file_1{std::filesystem::path{BINARY_DIRECTORY} /
                                     "is_newer_than_with_update" / "1.txt"};
  const std::filesystem::path file_2{std::filesystem::path{BINARY_DIRECTORY} /
                                     "is_newer_than_with_update" / "2.txt"};

  sourcemeta::one::BuildAdapterFilesystem adapter{BINARY_DIRECTORY};

  WRITE_FILE(file_1, "test_1");
  adapter.refresh(file_1);
  WRITE_FILE(file_2, "test_2");
  adapter.refresh(file_2);
  WRITE_FILE(file_1, "test_3");
  adapter.refresh(file_1);

  const auto mark_1{adapter.mark(file_1)};
  const auto mark_2{adapter.mark(file_2)};

  EXPECT_TRUE(mark_1.has_value());
  EXPECT_TRUE(mark_2.has_value());

  EXPECT_TRUE(adapter.is_newer_than(mark_1.value(), mark_2.value()));
  EXPECT_FALSE(adapter.is_newer_than(mark_2.value(), mark_1.value()));
}
