#include <sourcemeta/one/metapack.h>

#include <sourcemeta/core/io.h>
#include <sourcemeta/core/json.h>
#include <sourcemeta/core/test.h>

#include <algorithm>   // std::ranges::equal
#include <array>       // std::array
#include <chrono>      // std::chrono
#include <cstring>     // std::memcpy
#include <filesystem>  // std::filesystem
#include <format>      // std::format
#include <span>        // std::span
#include <string>      // std::string
#include <string_view> // std::string_view
#include <vector>      // std::vector

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
  const auto parsed{sourcemeta::one::metapack_parse(view).value()};
  EXPECT_TRUE(parsed.extension.empty());
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
  const auto parsed{sourcemeta::one::metapack_parse(view).value()};
  EXPECT_EQ(parsed.extension.size(), extension_bytes.size());

  const auto *read_extension{
      sourcemeta::one::metapack_extension_as<TestExtension>(parsed.extension)};
  EXPECT_NE(read_extension, nullptr);
  EXPECT_EQ(read_extension->value, 42);
  EXPECT_EQ(read_extension->name_length, 5);

  // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
  const auto *name_data{reinterpret_cast<const char *>(parsed.extension.data() +
                                                       sizeof(TestExtension))};
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
  const auto parsed{sourcemeta::one::metapack_parse(view).value()};
  EXPECT_EQ(parsed.extension.size(), 2);

  const auto *read_extension{
      sourcemeta::one::metapack_extension_as<TestExtension>(parsed.extension)};
  EXPECT_EQ(read_extension, nullptr);
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

TEST(read_json_nullopt_when_the_view_has_no_header) {
  const std::array<std::uint8_t, 2> bytes{{0x7B, 0x7D}};
  const sourcemeta::one::MetapackView parsed{
      .header = nullptr,
      .mime = {},
      .extension = {},
      .payload = std::span<const std::uint8_t>{bytes}};

  EXPECT_FALSE(sourcemeta::one::metapack_read_json(parsed).has_value());
}

TEST(write_text_appends_a_trailing_newline) {
  const auto path{test_path("text_newline.metapack")};

  sourcemeta::one::metapack_write_text(
      path, "Hello, world!", "text/plain",
      sourcemeta::one::MetapackEncoding::Identity, {},
      std::chrono::milliseconds{0});

  sourcemeta::core::FileView view{path};
  const auto parsed{sourcemeta::one::metapack_parse(view).value()};
  EXPECT_EQ(parsed.mime, "text/plain");
  EXPECT_EQ(parsed.header->encoding,
            sourcemeta::one::MetapackEncoding::Identity);
  EXPECT_TRUE(parsed.extension.empty());
  // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
  const std::string_view contents{
      reinterpret_cast<const char *>(parsed.payload.data()),
      parsed.payload.size()};
  EXPECT_EQ(contents, "Hello, world!\n");
  EXPECT_EQ(parsed.header->content_bytes, contents.size());
}

TEST(write_text_gzip_records_both_sizes) {
  const auto path{test_path("text_gzip.metapack")};
  std::string large_text;
  for (int index = 0; index < 200; index++) {
    large_text += "This is line number " + std::to_string(index) + ". ";
  }

  sourcemeta::one::metapack_write_text(path, large_text, "text/plain",
                                       sourcemeta::one::MetapackEncoding::GZIP,
                                       {}, std::chrono::milliseconds{0});

  sourcemeta::core::FileView view{path};
  const auto parsed{sourcemeta::one::metapack_parse(view).value()};
  EXPECT_EQ(parsed.mime, "text/plain");
  EXPECT_EQ(parsed.header->encoding, sourcemeta::one::MetapackEncoding::GZIP);
  EXPECT_TRUE(parsed.extension.empty());
  EXPECT_EQ(parsed.header->content_bytes, large_text.size() + 1);
  EXPECT_EQ(parsed.header->compressed_bytes, parsed.payload.size());
  EXPECT_LT(parsed.payload.size(), parsed.header->content_bytes);
}

TEST(write_bytes_keeps_the_payload_exact) {
  const auto path{test_path("bytes_identity.metapack")};
  const std::vector<std::uint8_t> contents{0x00, 0x01, 0xFF, 0x0A, 0x00};

  sourcemeta::one::metapack_write_bytes(
      path, std::span<const std::uint8_t>{contents}, "application/octet-stream",
      sourcemeta::one::MetapackEncoding::Identity, {},
      std::chrono::milliseconds{0});

  sourcemeta::core::FileView view{path};
  const auto parsed{sourcemeta::one::metapack_parse(view).value()};
  EXPECT_EQ(parsed.mime, "application/octet-stream");
  EXPECT_EQ(parsed.header->encoding,
            sourcemeta::one::MetapackEncoding::Identity);
  EXPECT_TRUE(parsed.extension.empty());
  EXPECT_EQ(parsed.payload.size(), contents.size());
  EXPECT_EQ(parsed.header->content_bytes, contents.size());
  EXPECT_TRUE(std::ranges::equal(parsed.payload, contents));
}

TEST(write_bytes_gzip_records_both_sizes) {
  const auto path{test_path("bytes_gzip.metapack")};
  std::vector<std::uint8_t> contents;
  for (std::size_t index{0}; index < 4096; index++) {
    contents.push_back(static_cast<std::uint8_t>(index % 7));
  }

  sourcemeta::one::metapack_write_bytes(
      path, std::span<const std::uint8_t>{contents}, "application/octet-stream",
      sourcemeta::one::MetapackEncoding::GZIP, {},
      std::chrono::milliseconds{0});

  sourcemeta::core::FileView view{path};
  const auto parsed{sourcemeta::one::metapack_parse(view).value()};
  EXPECT_EQ(parsed.mime, "application/octet-stream");
  EXPECT_EQ(parsed.header->encoding, sourcemeta::one::MetapackEncoding::GZIP);
  EXPECT_TRUE(parsed.extension.empty());
  EXPECT_EQ(parsed.header->content_bytes, contents.size());
  EXPECT_EQ(parsed.header->compressed_bytes, parsed.payload.size());
  EXPECT_LT(parsed.payload.size(), contents.size());
}

TEST(compressed_bytes_matches_payload_for_gzip_encoding) {
  const auto path{test_path("compressed_bytes_gzip.metapack")};
  auto document{sourcemeta::core::JSON::make_object()};
  document.assign("hello", sourcemeta::core::JSON{"world"});

  sourcemeta::one::metapack_write_json(path, document, "application/json",
                                       sourcemeta::one::MetapackEncoding::GZIP,
                                       {}, std::chrono::milliseconds{0});

  sourcemeta::core::FileView view{path};
  const auto parsed{sourcemeta::one::metapack_parse(view).value()};
  EXPECT_EQ(parsed.header->encoding, sourcemeta::one::MetapackEncoding::GZIP);
  EXPECT_EQ(parsed.header->compressed_bytes, parsed.payload.size());
}

TEST(compressed_bytes_unset_for_identity_encoding) {
  const auto path{test_path("compressed_bytes_identity.metapack")};
  auto document{sourcemeta::core::JSON::make_object()};
  document.assign("hello", sourcemeta::core::JSON{"world"});

  sourcemeta::one::metapack_write_json(
      path, document, "application/json",
      sourcemeta::one::MetapackEncoding::Identity, {},
      std::chrono::milliseconds{0});

  sourcemeta::core::FileView view{path};
  const auto parsed{sourcemeta::one::metapack_parse(view).value()};
  EXPECT_EQ(parsed.header->encoding,
            sourcemeta::one::MetapackEncoding::Identity);
  // Storing an artifact as it reads never compresses it, so there is no
  // compressed size to record
  EXPECT_EQ(parsed.header->compressed_bytes, 0);
  EXPECT_EQ(parsed.header->content_bytes, parsed.payload.size());
}

TEST(checksum_hex_renders_every_header_digest_byte) {
  const auto path{test_path("checksum_hex.metapack")};
  auto document{sourcemeta::core::JSON::make_object()};
  document.assign("hello", sourcemeta::core::JSON{"world"});

  sourcemeta::one::metapack_write_json(
      path, document, "application/json",
      sourcemeta::one::MetapackEncoding::Identity, {},
      std::chrono::milliseconds{0});

  sourcemeta::core::FileView view{path};
  const auto parsed{sourcemeta::one::metapack_parse(view).value()};
  const auto checksum{sourcemeta::one::metapack_checksum_hex(*parsed.header)};

  std::string expected;
  for (const auto byte : parsed.header->checksum) {
    expected += std::format("{:02x}", byte);
  }

  EXPECT_EQ(std::string_view(checksum.data(), checksum.size()), expected);
  EXPECT_EQ(checksum.size(), 64);
}
