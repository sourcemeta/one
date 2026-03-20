#include <sourcemeta/one/gzip.h>

extern "C" {
#include <libdeflate.h>
}

namespace sourcemeta::one {

auto gzip(const std::uint8_t *input, const std::size_t size) -> std::string {
  auto *compressor{libdeflate_alloc_compressor(1)};
  if (compressor == nullptr) {
    throw GZIPError{"Could not allocate compressor"};
  }

  const auto max_size{libdeflate_gzip_compress_bound(compressor, size)};
  std::string output;
  output.resize(max_size);

  const auto actual_size{libdeflate_gzip_compress(
      compressor, input, size, output.data(), output.size())};
  libdeflate_free_compressor(compressor);

  if (actual_size == 0) {
    throw GZIPError{"Could not compress input"};
  }

  output.resize(actual_size);
  return output;
}

auto gunzip(const std::uint8_t *input, const std::size_t size,
            const std::size_t output_hint) -> std::string {
  auto *decompressor{libdeflate_alloc_decompressor()};
  if (decompressor == nullptr) {
    throw GZIPError{"Could not allocate decompressor"};
  }

  std::string output;
  auto capacity{output_hint > 0 ? output_hint : size * 4};

  for (;;) {
    output.resize(capacity);
    std::size_t actual_size{0};
    const auto result{libdeflate_gzip_decompress(
        decompressor, input, size, output.data(), output.size(),
        &actual_size)};

    if (result == LIBDEFLATE_SUCCESS) {
      output.resize(actual_size);
      libdeflate_free_decompressor(decompressor);
      return output;
    }

    if (result == LIBDEFLATE_INSUFFICIENT_SPACE) {
      capacity *= 2;
      continue;
    }

    libdeflate_free_decompressor(decompressor);
    throw GZIPError{"Could not decompress input"};
  }
}

} // namespace sourcemeta::one
