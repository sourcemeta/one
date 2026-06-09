#include <sourcemeta/one/metapack.h>

#include <sourcemeta/core/crypto.h>
#include <sourcemeta/core/gzip.h>
#include <sourcemeta/core/io.h>

#include <bit>         // std::endian
#include <cassert>     // assert
#include <cstring>     // std::memcpy
#include <optional>    // std::optional, std::nullopt
#include <ostream>     // std::ostream
#include <sstream>     // std::ostringstream
#include <string>      // std::string
#include <string_view> // std::string_view

// Refuse to compile on big-endian hosts so silent magic-mismatch failures
// cannot ship.
static_assert(std::endian::native == std::endian::little,
              "Metapack binary format assumes little-endian host");

namespace sourcemeta::one {

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

  const auto hex_string{sourcemeta::core::sha256(payload)};
  for (std::size_t index{0}; index < 32 && index * 2 + 1 < hex_string.size();
       index++) {
    const auto byte_string{hex_string.substr(index * 2, 2)};
    header.checksum[index] =
        static_cast<std::uint8_t>(std::stoul(byte_string, nullptr, 16));
  }

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
                           const std::string &content) -> void {
  // Always compute the compressed representation so the size lands in
  // the header (currently gzip; the field name stays compression-agnostic
  // so a future codec swap is mechanical). For compressed-storage
  // artifacts the bytes are also what we write to disk; for Identity
  // storage only the size is kept.
  const auto compressed{sourcemeta::core::gzip(
      reinterpret_cast<const std::uint8_t *>(content.data()), content.size())};
  sourcemeta::core::write_file(destination, [&](std::ostream &output) {
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
                 buffer.str());
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
                 buffer.str());
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

auto metapack_write_file(const std::filesystem::path &destination,
                         const std::filesystem::path &source,
                         const std::string_view mime,
                         const MetapackEncoding encoding,
                         const std::span<const std::uint8_t> extension,
                         const std::chrono::milliseconds duration) -> void {
  write_metapack(destination, mime, encoding, extension, duration,
                 sourcemeta::core::read_file_to_string(source));
}

auto metapack_extension_offset(const sourcemeta::core::FileView &view)
    -> std::size_t {
  if (view.size() < sizeof(MetapackHeader) + sizeof(std::uint32_t)) {
    return 0;
  }

  const auto *header{view.as<MetapackHeader>()};
  if (header->magic != METAPACK_MAGIC ||
      header->format_version != METAPACK_VERSION) {
    return 0;
  }

  const auto offset_of_extension_size{sizeof(MetapackHeader) +
                                      header->mime_length};
  if (offset_of_extension_size + sizeof(std::uint32_t) > view.size()) {
    return 0;
  }

  const auto extension_size{*view.as<std::uint32_t>(offset_of_extension_size)};
  if (extension_size == 0) {
    return 0;
  }

  const auto extension_data_offset{offset_of_extension_size +
                                   sizeof(std::uint32_t)};
  if (extension_data_offset + extension_size > view.size()) {
    return 0;
  }

  return extension_data_offset;
}

auto metapack_extension_size(const sourcemeta::core::FileView &view)
    -> std::uint32_t {
  if (view.size() < sizeof(MetapackHeader) + sizeof(std::uint32_t)) {
    return 0;
  }

  const auto *header{view.as<MetapackHeader>()};
  if (header->magic != METAPACK_MAGIC ||
      header->format_version != METAPACK_VERSION) {
    return 0;
  }

  const auto offset_of_extension_size{sizeof(MetapackHeader) +
                                      header->mime_length};
  if (offset_of_extension_size + sizeof(std::uint32_t) > view.size()) {
    return 0;
  }

  return *view.as<std::uint32_t>(offset_of_extension_size);
}

auto metapack_read_json(const std::filesystem::path &path)
    -> std::optional<sourcemeta::core::JSON> {
  if (!std::filesystem::is_regular_file(path)) {
    return std::nullopt;
  }

  sourcemeta::core::FileView view{path};
  if (view.size() < sizeof(MetapackHeader) + sizeof(std::uint32_t)) {
    return std::nullopt;
  }

  const auto *header{view.as<MetapackHeader>()};
  if (header->magic != METAPACK_MAGIC ||
      header->format_version != METAPACK_VERSION) {
    return std::nullopt;
  }

  auto payload_offset{sizeof(MetapackHeader) + header->mime_length};
  if (payload_offset + sizeof(std::uint32_t) > view.size()) {
    return std::nullopt;
  }

  const auto *extension_size{view.as<std::uint32_t>(payload_offset)};
  payload_offset += sizeof(std::uint32_t);
  if (*extension_size > view.size() - payload_offset) {
    return std::nullopt;
  }
  payload_offset += *extension_size;

  const auto payload_data_size{view.size() - payload_offset};
  if (payload_data_size == 0) {
    return std::nullopt;
  }

  if (header->encoding == MetapackEncoding::GZIP) {
    if (header->content_bytes / payload_data_size >
        METAPACK_MAX_DECOMPRESSION_RATIO) {
      return std::nullopt;
    }

    const auto decompressed{
        sourcemeta::core::gunzip(view.as<std::uint8_t>(payload_offset),
                                 payload_data_size, header->content_bytes)};
    return sourcemeta::core::parse_json(decompressed);
  }

  if (header->content_bytes > payload_data_size) {
    return std::nullopt;
  }

  // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
  const auto *payload_data{
      reinterpret_cast<const char *>(view.as<std::uint8_t>(payload_offset))};
  const std::string payload_string{payload_data, header->content_bytes};
  return sourcemeta::core::parse_json(payload_string);
}

auto metapack_read_text(const std::filesystem::path &path)
    -> std::optional<std::string> {
  if (!std::filesystem::is_regular_file(path)) {
    return std::nullopt;
  }
  sourcemeta::core::FileView view{path};
  if (view.size() < sizeof(MetapackHeader) + sizeof(std::uint32_t)) {
    return std::nullopt;
  }

  const auto *header{view.as<MetapackHeader>()};
  if (header->magic != METAPACK_MAGIC ||
      header->format_version != METAPACK_VERSION) {
    return std::nullopt;
  }

  auto payload_offset{sizeof(MetapackHeader) + header->mime_length};
  if (payload_offset + sizeof(std::uint32_t) > view.size()) {
    return std::nullopt;
  }

  const auto *extension_size{view.as<std::uint32_t>(payload_offset)};
  payload_offset += sizeof(std::uint32_t);
  if (*extension_size > view.size() - payload_offset) {
    return std::nullopt;
  }
  payload_offset += *extension_size;

  const auto payload_data_size{view.size() - payload_offset};
  if (payload_data_size == 0) {
    return std::nullopt;
  }

  if (header->encoding == MetapackEncoding::GZIP) {
    if (header->content_bytes / payload_data_size >
        METAPACK_MAX_DECOMPRESSION_RATIO) {
      return std::nullopt;
    }

    return sourcemeta::core::gunzip(view.as<std::uint8_t>(payload_offset),
                                    payload_data_size, header->content_bytes);
  }

  if (header->content_bytes > payload_data_size) {
    return std::nullopt;
  }

  // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
  const auto *payload_data{
      reinterpret_cast<const char *>(view.as<std::uint8_t>(payload_offset))};
  return std::string{payload_data, header->content_bytes};
}

auto metapack_info(const sourcemeta::core::FileView &view)
    -> std::optional<MetapackInfo> {
  if (view.size() < sizeof(MetapackHeader) + sizeof(std::uint32_t)) {
    return std::nullopt;
  }

  const auto *header{view.as<MetapackHeader>()};
  if (header->magic != METAPACK_MAGIC ||
      header->format_version != METAPACK_VERSION) {
    return std::nullopt;
  }

  if (sizeof(MetapackHeader) + header->mime_length > view.size()) {
    return std::nullopt;
  }

  std::string checksum_hex;
  checksum_hex.reserve(64);
  static constexpr const char *hex_chars = "0123456789abcdef";
  for (const auto byte : header->checksum) {
    checksum_hex += hex_chars[(byte >> 4) & 0x0F];
    checksum_hex += hex_chars[byte & 0x0F];
  }

  const auto nanos{std::chrono::nanoseconds{header->last_modified}};
  const auto time_point{std::chrono::system_clock::time_point{
      std::chrono::duration_cast<std::chrono::system_clock::duration>(nanos)}};

  // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
  const auto *mime_data{reinterpret_cast<const char *>(
      view.as<std::uint8_t>(sizeof(MetapackHeader)))};

  return MetapackInfo{.checksum_hex = std::move(checksum_hex),
                      .last_modified = time_point,
                      .mime = std::string{mime_data, header->mime_length},
                      .encoding = header->encoding,
                      .content_bytes = header->content_bytes,
                      .compressed_bytes = header->compressed_bytes,
                      .duration = std::chrono::milliseconds{header->duration}};
}

auto metapack_payload_offset(const sourcemeta::core::FileView &view)
    -> std::optional<std::size_t> {
  if (view.size() < sizeof(MetapackHeader) + sizeof(std::uint32_t)) {
    return std::nullopt;
  }

  const auto *header{view.as<MetapackHeader>()};
  if (header->magic != METAPACK_MAGIC ||
      header->format_version != METAPACK_VERSION) {
    return std::nullopt;
  }

  auto offset{sizeof(MetapackHeader) + header->mime_length};
  if (offset + sizeof(std::uint32_t) > view.size()) {
    return std::nullopt;
  }

  const auto *extension_size{view.as<std::uint32_t>(offset)};
  offset += sizeof(std::uint32_t);
  if (*extension_size > view.size() - offset) {
    return std::nullopt;
  }
  offset += *extension_size;

  return offset;
}

} // namespace sourcemeta::one
