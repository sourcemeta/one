#include <sourcemeta/one/metapack.h>

#include <sourcemeta/core/io.h>
#include <sourcemeta/core/json.h>

#include <gtest/gtest.h>

#include <chrono>     // std::chrono
#include <cstring>    // std::memcmp
#include <filesystem> // std::filesystem
#include <span>       // std::span
#include <string>     // std::string
#include <vector>     // std::vector

static auto temp_path(const std::string &name) -> std::filesystem::path {
  return std::filesystem::temp_directory_path() / ("metapack_test_" + name);
}

class MetapackTest : public ::testing::Test {
protected:
  void TearDown() override {
    for (const auto &path : cleanup_paths) {
      std::filesystem::remove_all(path);
    }
  }

  auto make_temp(const std::string &name) -> std::filesystem::path {
    auto path{temp_path(name)};
    cleanup_paths.push_back(path);
    return path;
  }

private:
  std::vector<std::filesystem::path> cleanup_paths;
};

TEST_F(MetapackTest, write_and_read_json_identity) {
  const auto path{make_temp("identity.metapack")};
  auto document{sourcemeta::core::JSON::make_object()};
  document.assign("hello", sourcemeta::core::JSON{"world"});

  sourcemeta::one::metapack_write_json(
      path, document, "application/json",
      sourcemeta::one::MetapackEncoding::Identity, {},
      std::chrono::milliseconds{5});

  const auto result{sourcemeta::one::metapack_read_json(path)};
  EXPECT_TRUE(result.is_object());
  EXPECT_EQ(result.at("hello").to_string(), "world");
}

TEST_F(MetapackTest, write_and_read_json_gzip) {
  const auto path{make_temp("gzip.metapack")};
  auto document{sourcemeta::core::JSON::make_object()};
  document.assign("foo", sourcemeta::core::JSON{42});

  sourcemeta::one::metapack_write_json(path, document, "application/json",
                                       sourcemeta::one::MetapackEncoding::GZIP,
                                       {}, std::chrono::milliseconds{10});

  const auto result{sourcemeta::one::metapack_read_json(path)};
  EXPECT_TRUE(result.is_object());
  EXPECT_TRUE(result.at("foo").is_integer());
  EXPECT_EQ(result.at("foo").to_integer(), 42);
}

TEST_F(MetapackTest, write_and_read_pretty_json) {
  const auto path{make_temp("pretty.metapack")};
  auto document{sourcemeta::core::JSON::make_object()};
  document.assign("key", sourcemeta::core::JSON{"value"});

  sourcemeta::one::metapack_write_pretty_json(
      path, document, "application/schema+json",
      sourcemeta::one::MetapackEncoding::GZIP, {},
      std::chrono::milliseconds{3});

  const auto result{sourcemeta::one::metapack_read_json(path)};
  EXPECT_EQ(result.at("key").to_string(), "value");
}

TEST_F(MetapackTest, binary_header_magic_and_version) {
  const auto path{make_temp("header.metapack")};
  auto document{sourcemeta::core::JSON::make_object()};

  sourcemeta::one::metapack_write_json(
      path, document, "application/json",
      sourcemeta::one::MetapackEncoding::Identity, {},
      std::chrono::milliseconds{0});

  sourcemeta::core::FileView view{path};
  EXPECT_GE(view.size(), sizeof(sourcemeta::one::MetapackHeader));
  const auto *header{view.as<sourcemeta::one::MetapackHeader>()};
  EXPECT_EQ(header->magic, sourcemeta::one::METAPACK_MAGIC);
  EXPECT_EQ(header->format_version, sourcemeta::one::METAPACK_VERSION);
  EXPECT_EQ(header->encoding, sourcemeta::one::MetapackEncoding::Identity);
}

TEST_F(MetapackTest, no_extension) {
  const auto path{make_temp("no_ext.metapack")};
  auto document{sourcemeta::core::JSON::make_object()};

  sourcemeta::one::metapack_write_json(
      path, document, "application/json",
      sourcemeta::one::MetapackEncoding::Identity, {},
      std::chrono::milliseconds{0});

  sourcemeta::core::FileView view{path};
  EXPECT_EQ(sourcemeta::one::metapack_extension_size(view), 0);
  EXPECT_EQ(sourcemeta::one::metapack_extension_offset(view), 0);
}

#pragma pack(push, 1)
struct TestExtension {
  std::int32_t value;
  std::uint16_t name_length;
  // followed by name bytes
};
#pragma pack(pop)

TEST_F(MetapackTest, write_and_read_extension) {
  const auto path{make_temp("with_ext.metapack")};
  auto document{sourcemeta::core::JSON::make_object()};

  // Build a test extension: {value=42, name="hello"}
  std::vector<std::uint8_t> extension_bytes;
  TestExtension extension_header{};
  extension_header.value = 42;
  const std::string name{"hello"};
  extension_header.name_length = static_cast<std::uint16_t>(name.size());

  // Serialize the struct
  const auto *header_bytes{
      // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
      reinterpret_cast<const std::uint8_t *>(&extension_header)};
  extension_bytes.insert(extension_bytes.end(), header_bytes,
                         header_bytes + sizeof(TestExtension));
  // Append the name string
  extension_bytes.insert(extension_bytes.end(), name.begin(), name.end());

  sourcemeta::one::metapack_write_json(
      path, document, "application/json",
      sourcemeta::one::MetapackEncoding::GZIP,
      std::span<const std::uint8_t>{extension_bytes},
      std::chrono::milliseconds{7});

  // Read back via mmap
  sourcemeta::core::FileView view{path};
  EXPECT_EQ(sourcemeta::one::metapack_extension_size(view),
            extension_bytes.size());

  const auto *read_extension{
      sourcemeta::one::metapack_extension<TestExtension>(view)};
  ASSERT_NE(read_extension, nullptr);
  EXPECT_EQ(read_extension->value, 42);
  EXPECT_EQ(read_extension->name_length, 5);

  // Read the name string from the extension data
  const auto extension_start{sourcemeta::one::metapack_extension_offset(view)};
  // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
  const auto *name_data{reinterpret_cast<const char *>(
      view.as<std::uint8_t>(extension_start + sizeof(TestExtension)))};
  EXPECT_EQ(std::string_view(name_data, read_extension->name_length), "hello");

  // Also verify the JSON payload is still readable
  const auto result{sourcemeta::one::metapack_read_json(path)};
  EXPECT_TRUE(result.is_object());
}

TEST_F(MetapackTest, extension_nullptr_when_too_small) {
  const auto path{make_temp("small_ext.metapack")};
  auto document{sourcemeta::core::JSON::make_object()};

  // Write a 2-byte extension
  std::vector<std::uint8_t> small_extension{0x01, 0x02};

  sourcemeta::one::metapack_write_json(
      path, document, "application/json",
      sourcemeta::one::MetapackEncoding::Identity,
      std::span<const std::uint8_t>{small_extension},
      std::chrono::milliseconds{0});

  sourcemeta::core::FileView view{path};
  EXPECT_EQ(sourcemeta::one::metapack_extension_size(view), 2);

  // TestExtension is larger than 2 bytes, so this should return nullptr
  const auto *read_extension{
      sourcemeta::one::metapack_extension<TestExtension>(view)};
  EXPECT_EQ(read_extension, nullptr);
}
