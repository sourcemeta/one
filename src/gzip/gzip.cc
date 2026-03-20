#include <sourcemeta/one/gzip.h>

extern "C" {
#include <zlib.h>
}

namespace sourcemeta::one {

auto gzip(const std::uint8_t *input, const std::size_t size) -> std::string {
  z_stream zstream{};
  zstream.next_in = const_cast<Bytef *>(input);
  zstream.avail_in = static_cast<uInt>(size);

  if (deflateInit2(&zstream, Z_BEST_SPEED, Z_DEFLATED, 16 + MAX_WBITS, 9,
                   Z_DEFAULT_STRATEGY) != Z_OK) {
    throw GZIPError{"Could not compress input"};
  }

  std::string output;
  output.resize(deflateBound(&zstream, static_cast<uLong>(size)));
  zstream.next_out = reinterpret_cast<Bytef *>(output.data());
  zstream.avail_out = static_cast<uInt>(output.size());

  const auto code{deflate(&zstream, Z_FINISH)};
  if (code != Z_STREAM_END) {
    deflateEnd(&zstream);
    throw GZIPError{"Could not compress input"};
  }

  output.resize(zstream.total_out);
  deflateEnd(&zstream);
  return output;
}

auto gunzip(const std::uint8_t *input, const std::size_t size,
            const std::size_t output_hint) -> std::string {
  z_stream zstream{};
  zstream.next_in = const_cast<Bytef *>(input);
  zstream.avail_in = static_cast<uInt>(size);

  if (inflateInit2(&zstream, 16 + MAX_WBITS) != Z_OK) {
    throw GZIPError("Could not decompress input");
  }

  std::string output;
  const auto initial_capacity{output_hint > 0 ? output_hint : size * 4};
  output.resize(initial_capacity);
  zstream.next_out = reinterpret_cast<Bytef *>(output.data());
  zstream.avail_out = static_cast<uInt>(output.size());

  for (;;) {
    const auto code{inflate(&zstream, Z_NO_FLUSH)};
    if (code == Z_STREAM_END) {
      break;
    }

    if (code != Z_OK) {
      inflateEnd(&zstream);
      throw GZIPError("Could not decompress input");
    }

    if (zstream.avail_out == 0) {
      const auto current_size{output.size()};
      output.resize(current_size * 2);
      zstream.next_out =
          reinterpret_cast<Bytef *>(output.data() + current_size);
      zstream.avail_out = static_cast<uInt>(current_size);
    }
  }

  output.resize(zstream.total_out);
  inflateEnd(&zstream);
  return output;
}

} // namespace sourcemeta::one
