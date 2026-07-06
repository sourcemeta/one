#include <sourcemeta/core/json.h>
#include <sourcemeta/core/test.h>
#include <sourcemeta/one/metapack.h>

#include <chrono>     // std::chrono
#include <cstddef>    // offsetof
#include <cstdint>    // std::uint64_t
#include <filesystem> // std::filesystem
#include <fstream>    // std::fstream
#include <ios>        // std::ios, std::streamoff
#include <string>     // std::string

namespace {

class MetapackCorruption {
protected:
  static auto test_path(const std::string &name) -> std::filesystem::path {
    return std::filesystem::path{METAPACK_TEST_DIRECTORY} / name;
  }

  // Overwrite the trailing bytes so that a GZIP payload fails to inflate
  static auto corrupt_last_bytes(const std::filesystem::path &path,
                                 const std::size_t count) -> void {
    const auto size{std::filesystem::file_size(path)};
    std::fstream file{path, std::ios::binary | std::ios::in | std::ios::out};
    EXPECT_TRUE(file.good());
    file.seekp(static_cast<std::streamoff>(size - count), std::ios::beg);
    for (std::size_t index{0}; index < count; ++index) {
      const char byte{static_cast<char>(0xFF)};
      file.write(&byte, 1);
    }
  }

  // Rewrite the header's advertised uncompressed size
  static auto set_content_bytes(const std::filesystem::path &path,
                                const std::uint64_t value) -> void {
    std::fstream file{path, std::ios::binary | std::ios::in | std::ios::out};
    EXPECT_TRUE(file.good());
    file.seekp(offsetof(sourcemeta::one::MetapackHeader, content_bytes),
               std::ios::beg);
    // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
    file.write(reinterpret_cast<const char *>(&value), sizeof(value));
  }
};

} // namespace

TEST_F(MetapackCorruption,
       read_text_nullopt_when_content_bytes_exceeds_payload) {
  const auto path{test_path("bad_content_bytes.metapack")};

  sourcemeta::one::metapack_write_text(
      path, "short", "text/plain", sourcemeta::one::MetapackEncoding::Identity,
      {}, std::chrono::milliseconds{0});

  // Advertise more payload than actually exists
  set_content_bytes(path, 999999);

  const auto result{sourcemeta::one::metapack_read_text(path)};
  EXPECT_FALSE(result.has_value());
}

TEST_F(MetapackCorruption, read_json_nullopt_on_corrupt_gzip) {
  const auto path{test_path("corrupt_gzip_json.metapack")};
  auto document{sourcemeta::core::JSON::make_object()};
  document.assign("hello", sourcemeta::core::JSON{"world"});

  sourcemeta::one::metapack_write_json(path, document, "application/json",
                                       sourcemeta::one::MetapackEncoding::GZIP,
                                       {}, std::chrono::milliseconds{0});

  // Damaging the trailer makes the payload fail to inflate, which must be
  // reported as a missing value rather than propagate an exception
  corrupt_last_bytes(path, 4);

  const auto result{sourcemeta::one::metapack_read_json(path)};
  EXPECT_FALSE(result.has_value());
}

TEST_F(MetapackCorruption, read_text_nullopt_on_corrupt_gzip) {
  const auto path{test_path("corrupt_gzip_text.metapack")};

  sourcemeta::one::metapack_write_text(path, "Compressed text content",
                                       "text/plain",
                                       sourcemeta::one::MetapackEncoding::GZIP,
                                       {}, std::chrono::milliseconds{0});

  corrupt_last_bytes(path, 4);

  const auto result{sourcemeta::one::metapack_read_text(path)};
  EXPECT_FALSE(result.has_value());
}

TEST_F(MetapackCorruption, read_json_nullopt_on_invalid_json) {
  const auto path{test_path("invalid_json.metapack")};

  sourcemeta::one::metapack_write_text(
      path, "this is not json {{{", "text/plain",
      sourcemeta::one::MetapackEncoding::Identity, {},
      std::chrono::milliseconds{0});

  const auto result{sourcemeta::one::metapack_read_json(path)};
  EXPECT_FALSE(result.has_value());
}

TEST_F(MetapackCorruption, read_json_nullopt_on_content_bytes_size_mismatch) {
  const auto path{test_path("gzip_size_mismatch.metapack")};
  auto document{sourcemeta::core::JSON::make_object()};
  document.assign("hello", sourcemeta::core::JSON{"world"});

  sourcemeta::one::metapack_write_json(path, document, "application/json",
                                       sourcemeta::one::MetapackEncoding::GZIP,
                                       {}, std::chrono::milliseconds{0});

  // A small `content_bytes` survives the ratio guard but no longer matches the
  // real decompressed size, so the reader must reject the payload
  set_content_bytes(path, 2);

  const auto result{sourcemeta::one::metapack_read_json(path)};
  EXPECT_FALSE(result.has_value());
}
