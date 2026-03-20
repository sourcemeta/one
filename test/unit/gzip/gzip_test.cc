#include <gtest/gtest.h>

#include <sourcemeta/one/gzip.h>

#include <cstdint>
#include <cstring>
#include <string>

TEST(GZIP, compress_bytes_1) {
  const std::string value{"Hello World"};
  const auto result{sourcemeta::one::gzip(
      reinterpret_cast<const std::uint8_t *>(value.data()), value.size())};
  EXPECT_GT(result.size(), 0);
}

TEST(GZIP, decompress_bytes_1) {
  const std::string value{"Hello World"};
  const auto compressed{sourcemeta::one::gzip(
      reinterpret_cast<const std::uint8_t *>(value.data()), value.size())};
  const auto decompressed{sourcemeta::one::gunzip(
      reinterpret_cast<const std::uint8_t *>(compressed.data()),
      compressed.size())};
  EXPECT_EQ(decompressed, value);
}

TEST(GZIP, decompress_bytes_with_hint_1) {
  const std::string value{"Hello World"};
  const auto compressed{sourcemeta::one::gzip(
      reinterpret_cast<const std::uint8_t *>(value.data()), value.size())};
  const auto decompressed{sourcemeta::one::gunzip(
      reinterpret_cast<const std::uint8_t *>(compressed.data()),
      compressed.size(), value.size())};
  EXPECT_EQ(decompressed, value);
}

TEST(GZIP, decompress_error_1) {
  const std::string bad_data{"not-zlib-content"};
  EXPECT_THROW(sourcemeta::one::gunzip(
                   reinterpret_cast<const std::uint8_t *>(bad_data.data()),
                   bad_data.size()),
               sourcemeta::one::GZIPError);
}

TEST(GZIP, roundtrip_large_1) {
  std::string value;
  value.resize(100000, 'x');
  const auto compressed{sourcemeta::one::gzip(
      reinterpret_cast<const std::uint8_t *>(value.data()), value.size())};
  EXPECT_LT(compressed.size(), value.size());
  const auto decompressed{sourcemeta::one::gunzip(
      reinterpret_cast<const std::uint8_t *>(compressed.data()),
      compressed.size(), value.size())};
  EXPECT_EQ(decompressed, value);
}
