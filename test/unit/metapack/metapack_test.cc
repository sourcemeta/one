#include <sourcemeta/one/metapack.h>

#include <sourcemeta/core/io.h>
#include <sourcemeta/core/json.h>
#include <sourcemeta/core/test.h>

#include <chrono>     // std::chrono
#include <cstring>    // std::memcpy
#include <filesystem> // std::filesystem
#include <span>       // std::span
#include <string>     // std::string
#include <vector>     // std::vector

static auto test_path(const std::string &name) -> std::filesystem::path {
  return std::filesystem::path{METAPACK_TEST_DIRECTORY} / name;
}

TEST(write_and_read_json_identity) {
  const auto path{test_path("identity.metapack")};
  auto document{sourcemeta::core::JSON::make_object()};
  document.assign("hello", sourcemeta::core::JSON{"world"});

  sourcemeta::one::metapack_write_json(
      path, document, "application/json",
      sourcemeta::one::MetapackEncoding::Identity, {},
      std::chrono::milliseconds{5});

  const auto result{sourcemeta::one::metapack_read_json(path).value()};
  EXPECT_TRUE(result.is_object());
  EXPECT_EQ(result.at("hello").to_string(), "world");
}

TEST(write_and_read_json_gzip) {
  const auto path{test_path("gzip.metapack")};
  auto document{sourcemeta::core::JSON::make_object()};
  document.assign("foo", sourcemeta::core::JSON{42});

  sourcemeta::one::metapack_write_json(path, document, "application/json",
                                       sourcemeta::one::MetapackEncoding::GZIP,
                                       {}, std::chrono::milliseconds{10});

  const auto result{sourcemeta::one::metapack_read_json(path).value()};
  EXPECT_TRUE(result.is_object());
  EXPECT_TRUE(result.at("foo").is_integer());
  EXPECT_EQ(result.at("foo").to_integer(), 42);
}

TEST(write_and_read_pretty_json) {
  const auto path{test_path("pretty.metapack")};
  auto document{sourcemeta::core::JSON::make_object()};
  document.assign("key", sourcemeta::core::JSON{"value"});

  sourcemeta::one::metapack_write_pretty_json(
      path, document, "application/schema+json",
      sourcemeta::one::MetapackEncoding::GZIP, {},
      std::chrono::milliseconds{3});

  const auto result{sourcemeta::one::metapack_read_json(path).value()};
  EXPECT_EQ(result.at("key").to_string(), "value");
}

TEST(binary_header_magic_and_version) {
  const auto path{test_path("header.metapack")};
  auto document{sourcemeta::core::JSON::make_object()};

  sourcemeta::one::metapack_write_json(
      path, document, "application/json",
      sourcemeta::one::MetapackEncoding::Identity, {},
      std::chrono::milliseconds{0});

  sourcemeta::core::FileView view{path};
  EXPECT_TRUE(view.size() >= sizeof(sourcemeta::one::MetapackHeader));
  const auto *header{view.as<sourcemeta::one::MetapackHeader>()};
  EXPECT_EQ(header->magic, sourcemeta::one::METAPACK_MAGIC);
  EXPECT_EQ(header->format_version, sourcemeta::one::METAPACK_VERSION);
  EXPECT_EQ(header->encoding, sourcemeta::one::MetapackEncoding::Identity);
}

TEST(no_extension) {
  const auto path{test_path("no_ext.metapack")};
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
};
#pragma pack(pop)

TEST(write_and_read_extension) {
  const auto path{test_path("with_ext.metapack")};
  auto document{sourcemeta::core::JSON::make_object()};

  std::vector<std::uint8_t> extension_bytes;
  TestExtension extension_header{};
  extension_header.value = 42;
  const std::string name{"hello"};
  extension_header.name_length = static_cast<std::uint16_t>(name.size());

  // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
  const auto *header_bytes{
      reinterpret_cast<const std::uint8_t *>(&extension_header)};
  extension_bytes.insert(extension_bytes.end(), header_bytes,
                         header_bytes + sizeof(TestExtension));
  extension_bytes.insert(extension_bytes.end(), name.begin(), name.end());

  sourcemeta::one::metapack_write_json(
      path, document, "application/json",
      sourcemeta::one::MetapackEncoding::GZIP,
      std::span<const std::uint8_t>{extension_bytes},
      std::chrono::milliseconds{7});

  sourcemeta::core::FileView view{path};
  EXPECT_EQ(sourcemeta::one::metapack_extension_size(view),
            extension_bytes.size());

  const auto *read_extension{
      sourcemeta::one::metapack_extension<TestExtension>(view)};
  EXPECT_NE(read_extension, nullptr);
  EXPECT_EQ(read_extension->value, 42);
  EXPECT_EQ(read_extension->name_length, 5);

  const auto extension_start{sourcemeta::one::metapack_extension_offset(view)};
  // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
  const auto *name_data{reinterpret_cast<const char *>(
      view.as<std::uint8_t>(extension_start + sizeof(TestExtension)))};
  EXPECT_EQ(std::string_view(name_data, read_extension->name_length), "hello");

  const auto result{sourcemeta::one::metapack_read_json(path).value()};
  EXPECT_TRUE(result.is_object());
}

TEST(extension_nullptr_when_too_small) {
  const auto path{test_path("small_ext.metapack")};
  auto document{sourcemeta::core::JSON::make_object()};

  std::vector<std::uint8_t> small_extension{0x01, 0x02};

  sourcemeta::one::metapack_write_json(
      path, document, "application/json",
      sourcemeta::one::MetapackEncoding::Identity,
      std::span<const std::uint8_t>{small_extension},
      std::chrono::milliseconds{0});

  sourcemeta::core::FileView view{path};
  EXPECT_EQ(sourcemeta::one::metapack_extension_size(view), 2);

  const auto *read_extension{
      sourcemeta::one::metapack_extension<TestExtension>(view)};
  EXPECT_EQ(read_extension, nullptr);
}

TEST(write_and_read_text_identity) {
  const auto path{test_path("text_identity.metapack")};

  sourcemeta::one::metapack_write_text(
      path, "Hello, world!", "text/plain",
      sourcemeta::one::MetapackEncoding::Identity, {},
      std::chrono::milliseconds{1});

  const auto result{sourcemeta::one::metapack_read_text(path)};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), "Hello, world!\n");
}

TEST(write_and_read_text_gzip) {
  const auto path{test_path("text_gzip.metapack")};

  sourcemeta::one::metapack_write_text(path, "Compressed text content",
                                       "text/plain",
                                       sourcemeta::one::MetapackEncoding::GZIP,
                                       {}, std::chrono::milliseconds{2});

  const auto result{sourcemeta::one::metapack_read_text(path)};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), "Compressed text content\n");
}

TEST(write_and_read_text_identity_with_extension) {
  const auto path{test_path("text_identity_ext.metapack")};

  std::vector<std::uint8_t> extension_bytes;
  TestExtension extension_header{};
  extension_header.value = 99;
  extension_header.name_length = 0;

  // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
  const auto *header_bytes{
      reinterpret_cast<const std::uint8_t *>(&extension_header)};
  extension_bytes.insert(extension_bytes.end(), header_bytes,
                         header_bytes + sizeof(TestExtension));

  sourcemeta::one::metapack_write_text(
      path, "Text with extension", "text/plain",
      sourcemeta::one::MetapackEncoding::Identity,
      std::span<const std::uint8_t>{extension_bytes},
      std::chrono::milliseconds{0});

  const auto result{sourcemeta::one::metapack_read_text(path)};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), "Text with extension\n");
}

TEST(write_and_read_text_gzip_with_extension) {
  const auto path{test_path("text_gzip_ext.metapack")};

  std::vector<std::uint8_t> extension_bytes;
  TestExtension extension_header{};
  extension_header.value = 77;
  extension_header.name_length = 0;

  // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
  const auto *header_bytes{
      reinterpret_cast<const std::uint8_t *>(&extension_header)};
  extension_bytes.insert(extension_bytes.end(), header_bytes,
                         header_bytes + sizeof(TestExtension));

  sourcemeta::one::metapack_write_text(
      path, "Compressed text with extension", "text/plain",
      sourcemeta::one::MetapackEncoding::GZIP,
      std::span<const std::uint8_t>{extension_bytes},
      std::chrono::milliseconds{0});

  const auto result{sourcemeta::one::metapack_read_text(path)};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), "Compressed text with extension\n");
}

TEST(write_and_read_json_gzip_large) {
  const auto path{test_path("gzip_large.metapack")};
  auto document{sourcemeta::core::JSON::make_object()};
  for (int index = 0; index < 200; index++) {
    document.assign("key_" + std::to_string(index),
                    sourcemeta::core::JSON{"value_" + std::to_string(index)});
  }

  sourcemeta::one::metapack_write_json(path, document, "application/json",
                                       sourcemeta::one::MetapackEncoding::GZIP,
                                       {}, std::chrono::milliseconds{0});

  const auto result{sourcemeta::one::metapack_read_json(path)};
  EXPECT_TRUE(result.has_value());
  EXPECT_TRUE(result.value().is_object());
  EXPECT_EQ(result.value().at("key_0").to_string(), "value_0");
  EXPECT_EQ(result.value().at("key_199").to_string(), "value_199");
}

TEST(write_and_read_text_gzip_large) {
  const auto path{test_path("text_gzip_large.metapack")};
  std::string large_text;
  for (int index = 0; index < 200; index++) {
    large_text += "This is line number " + std::to_string(index) + ". ";
  }

  sourcemeta::one::metapack_write_text(path, large_text, "text/plain",
                                       sourcemeta::one::MetapackEncoding::GZIP,
                                       {}, std::chrono::milliseconds{0});

  const auto result{sourcemeta::one::metapack_read_text(path)};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), large_text + "\n");
}

TEST(compressed_bytes_matches_payload_for_gzip_encoding) {
  const auto path{test_path("compressed_bytes_gzip.metapack")};
  auto document{sourcemeta::core::JSON::make_object()};
  document.assign("hello", sourcemeta::core::JSON{"world"});

  sourcemeta::one::metapack_write_json(path, document, "application/json",
                                       sourcemeta::one::MetapackEncoding::GZIP,
                                       {}, std::chrono::milliseconds{0});

  sourcemeta::core::FileView view{path};
  const auto payload_offset{
      sourcemeta::one::metapack_payload_offset(view).value()};
  const auto payload_size{view.size() - payload_offset};

  const auto info{sourcemeta::one::metapack_info(view).value()};
  EXPECT_EQ(info.encoding, sourcemeta::one::MetapackEncoding::GZIP);
  EXPECT_EQ(info.compressed_bytes, payload_size);
}

TEST(compressed_bytes_populated_for_identity_encoding) {
  const auto path{test_path("compressed_bytes_identity.metapack")};
  auto document{sourcemeta::core::JSON::make_object()};
  document.assign("hello", sourcemeta::core::JSON{"world"});

  sourcemeta::one::metapack_write_json(
      path, document, "application/json",
      sourcemeta::one::MetapackEncoding::Identity, {},
      std::chrono::milliseconds{0});

  sourcemeta::core::FileView view{path};
  const auto info{sourcemeta::one::metapack_info(view).value()};
  EXPECT_EQ(info.encoding, sourcemeta::one::MetapackEncoding::Identity);
  EXPECT_GT(info.compressed_bytes, 0);
  EXPECT_NE(info.compressed_bytes, info.content_bytes);
}
