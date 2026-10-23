#include <sourcemeta/one/metapack.h>

#include <sourcemeta/core/crypto.h>
#include <sourcemeta/core/gzip.h>
#include <sourcemeta/core/io.h>

#include <bit>         // std::endian
#include <cassert>     // assert
#include <cstring>     // std::memcpy
#include <exception>   // std::exception
#include <optional>    // std::optional, std::nullopt
#include <ostream>     // std::ostream
#include <sstream>     // std::ostringstream
#include <string>      // std::string
#include <string_view> // std::string_view
#include <utility>     // std::move

// Refuse to compile on big-endian hosts so silent magic-mismatch failures
// cannot ship.
static_assert(std::endian::native == std::endian::little,
              "Metapack binary format assumes little-endian host");

namespace sourcemeta::one {

namespace {

// How far a stored payload may be allowed to inflate, so that a small
// artifact cannot ask for an unbounded amount of memory
constexpr std::uint64_t METAPACK_MAX_DECOMPRESSION_RATIO{1024};

// The payload as it reads once the stored compression is undone
auto inflate(const MetapackView &parsed) -> std::optional<std::string> {
  assert(!parsed.payload.empty());
  if (parsed.header->content_bytes / parsed.payload.size() >
      METAPACK_MAX_DECOMPRESSION_RATIO) {
    return std::nullopt;
  }

  try {
    auto decompressed{sourcemeta::core::gunzip(parsed.payload.data(),
                                               parsed.payload.size(),
                                               parsed.header->content_bytes)};
    // The header records the exact uncompressed size, so a payload that
    // inflates to a different size is corrupt or a decompression bomb whose
    // small declared size slipped past the ratio guard
    if (decompressed.size() != parsed.header->content_bytes) {
      return std::nullopt;
    }

    return decompressed;
  } catch (const std::exception &) {
    return std::nullopt;
  }
}

} // namespace

static auto write_binary_header(std::ostream &output,
                                const std::string_view mime,
                                const MetapackEncoding encoding,
                                const std::span<const std::uint8_t> extension,
                                const std::chrono::milliseconds duration,
                                const std::string_view payload,
                                const std::size_t uncompressed_size,
                                const std::size_t compressed_size) -> void {
  MetapackHeader header{};
  header.magic = METAPACK_MAGIC;
  header.format_version = METAPACK_VERSION;
  header.encoding = encoding;
  header.reserved = 0;

  const auto now{std::chrono::system_clock::now()};
  header.last_modified = std::chrono::duration_cast<std::chrono::nanoseconds>(
                             now.time_since_epoch())
                             .count();
  header.content_bytes = uncompressed_size;
  header.compressed_bytes = compressed_size;
  header.duration = duration.count();

  header.checksum = sourcemeta::core::sha256_digest(payload);

  assert(mime.size() <= UINT16_MAX);
  header.mime_length = static_cast<std::uint16_t>(mime.size());

  // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
  output.write(reinterpret_cast<const char *>(&header), sizeof(MetapackHeader));
  output.write(mime.data(), static_cast<std::streamsize>(mime.size()));

  const auto extension_size{static_cast<std::uint32_t>(extension.size())};
  // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
  output.write(reinterpret_cast<const char *>(&extension_size),
               sizeof(extension_size));
  if (!extension.empty()) {
    // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
    output.write(reinterpret_cast<const char *>(extension.data()),
                 static_cast<std::streamsize>(extension.size()));
  }
}

static auto write_metapack(const std::filesystem::path &destination,
                           const std::string_view mime,
                           const MetapackEncoding encoding,
                           const std::span<const std::uint8_t> extension,
                           const std::chrono::milliseconds duration,
                           const std::string_view content) -> void {
  // The codec is gzip today, the field name stays compression-agnostic so a
  // future codec swap is mechanical. An artifact stored as it reads is never
  // compressed here, so it records no compressed size
  const auto compressed{
      encoding == MetapackEncoding::GZIP
          ? sourcemeta::core::gzip(
                // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
                reinterpret_cast<const std::uint8_t *>(content.data()),
                content.size())
          : std::string{}};
  sourcemeta::core::write_file(destination, [&](std::ostream &output) -> void {
    write_binary_header(output, mime, encoding, extension, duration, content,
                        content.size(), compressed.size());

    if (encoding == MetapackEncoding::GZIP) {
      output.write(compressed.data(),
                   static_cast<std::streamsize>(compressed.size()));
    } else {
      output.write(content.data(),
                   static_cast<std::streamsize>(content.size()));
    }
  });
}

auto metapack_write_json(const std::filesystem::path &destination,
                         const sourcemeta::core::JSON &document,
                         const std::string_view mime,
                         const MetapackEncoding encoding,
                         const std::span<const std::uint8_t> extension,
                         const std::chrono::milliseconds duration) -> void {
  std::ostringstream buffer;
  sourcemeta::core::stringify(document, buffer);
  write_metapack(destination, mime, encoding, extension, duration,
                 std::move(buffer).str());
}

auto metapack_write_pretty_json(const std::filesystem::path &destination,
                                const sourcemeta::core::JSON &document,
                                const std::string_view mime,
                                const MetapackEncoding encoding,
                                const std::span<const std::uint8_t> extension,
                                const std::chrono::milliseconds duration)
    -> void {
  std::ostringstream buffer;
  sourcemeta::core::prettify(document, buffer);
  write_metapack(destination, mime, encoding, extension, duration,
                 std::move(buffer).str());
}

auto metapack_write_text(const std::filesystem::path &destination,
                         const std::string_view contents,
                         const std::string_view mime,
                         const MetapackEncoding encoding,
                         const std::span<const std::uint8_t> extension,
                         const std::chrono::milliseconds duration) -> void {
  std::string content{contents};
  content += '\n';
  write_metapack(destination, mime, encoding, extension, duration, content);
}

auto metapack_write_bytes(const std::filesystem::path &destination,
                          const std::span<const std::uint8_t> contents,
                          const std::string_view mime,
                          const MetapackEncoding encoding,
                          const std::span<const std::uint8_t> extension,
                          const std::chrono::milliseconds duration) -> void {
  write_metapack(
      destination, mime, encoding, extension, duration,
      std::string_view{
          // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
          reinterpret_cast<const char *>(contents.data()), contents.size()});
}

auto metapack_write_file(const std::filesystem::path &destination,
                         const std::filesystem::path &source,
                         const std::string_view mime,
                         const MetapackEncoding encoding,
                         const std::span<const std::uint8_t> extension,
                         const std::chrono::milliseconds duration) -> void {
  write_metapack(destination, mime, encoding, extension, duration,
                 sourcemeta::core::read_file_to_string(source));
}

auto metapack_parse(const sourcemeta::core::FileView &view)
    -> std::optional<MetapackView> {
  if (view.size() < sizeof(MetapackHeader) + sizeof(std::uint32_t)) {
    return std::nullopt;
  }

  const auto *header{view.as<MetapackHeader>()};
  if (header->magic != METAPACK_MAGIC ||
      header->format_version != METAPACK_VERSION) {
    return std::nullopt;
  }

  const auto extension_size_offset{sizeof(MetapackHeader) +
                                   header->mime_length};
  if (extension_size_offset + sizeof(std::uint32_t) > view.size()) {
    return std::nullopt;
  }

  // The MIME string decides where this lands, which is rarely an offset a
  // `std::uint32_t` may be read from in place
  std::uint32_t extension_size{0};
  std::memcpy(&extension_size, view.as<std::uint8_t>(extension_size_offset),
              sizeof(extension_size));

  const auto extension_offset{extension_size_offset + sizeof(std::uint32_t)};
  if (extension_size > view.size() - extension_offset) {
    return std::nullopt;
  }

  const auto payload_offset{extension_offset + extension_size};
  const auto *base{view.as<std::uint8_t>(0)};
  return MetapackView{
      .header = header,
      // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
      .mime = std::string_view{reinterpret_cast<const char *>(
                                   base + sizeof(MetapackHeader)),
                               header->mime_length},
      .extension = {base + extension_offset, extension_size},
      .payload = {base + payload_offset, view.size() - payload_offset}};
}

auto metapack_read_json(const MetapackView &parsed)
    -> std::optional<sourcemeta::core::JSON> {
  if (parsed.header == nullptr || parsed.payload.empty()) {
    return std::nullopt;
  }

  // Parsing can run out of memory on a payload whose header overstates how far
  // it inflates, which is a refusal rather than a failure
  try {
    if (parsed.header->encoding == MetapackEncoding::GZIP) {
      const auto decompressed{inflate(parsed)};
      if (!decompressed.has_value()) {
        return std::nullopt;
      }

      return sourcemeta::core::parse_json(decompressed.value());
    }

    if (parsed.header->content_bytes > parsed.payload.size()) {
      return std::nullopt;
    }

    return sourcemeta::core::parse_json(std::string_view{
        // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
        reinterpret_cast<const char *>(parsed.payload.data()),
        parsed.header->content_bytes});
  } catch (const std::exception &) {
    return std::nullopt;
  }
}

auto metapack_read_json(const sourcemeta::core::FileView &view)
    -> std::optional<sourcemeta::core::JSON> {
  const auto parsed{metapack_parse(view)};
  if (!parsed.has_value()) {
    return std::nullopt;
  }

  return metapack_read_json(parsed.value());
}

auto metapack_read_json(const std::filesystem::path &path)
    -> std::optional<sourcemeta::core::JSON> {
  try {
    const sourcemeta::core::FileView view{path};
    return metapack_read_json(view);
  } catch (const sourcemeta::core::FileViewError &) {
    return std::nullopt;
  }
}

auto metapack_checksum_hex(const MetapackHeader &header)
    -> std::array<char, METAPACK_CHECKSUM_HEX_LENGTH> {
  static constexpr std::string_view HEX_CHARS{"0123456789abcdef"};
  std::array<char, METAPACK_CHECKSUM_HEX_LENGTH> result{};
  std::size_t cursor{0};
  for (const auto byte : header.checksum) {
    result[cursor++] = HEX_CHARS[(byte >> 4) & 0x0F];
    result[cursor++] = HEX_CHARS[byte & 0x0F];
  }

  return result;
}

auto metapack_last_modified(const MetapackHeader &header)
    -> std::chrono::system_clock::time_point {
  const auto nanos{std::chrono::nanoseconds{header.last_modified}};
  return std::chrono::system_clock::time_point{
      std::chrono::duration_cast<std::chrono::system_clock::duration>(nanos)};
}

} // namespace sourcemeta::one
