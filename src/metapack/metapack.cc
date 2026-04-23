#include <sourcemeta/one/metapack.h>

#include <sourcemeta/core/crypto.h>
#include <sourcemeta/core/io.h>

#include <sourcemeta/one/gzip.h>

#include <cassert>     // assert
#include <cstring>     // std::memcpy
#include <fstream>     // std::ofstream
#include <optional>    // std::optional, std::nullopt
#include <sstream>     // std::ostringstream, std::stringstream
#include <string>      // std::string
#include <string_view> // std::string_view

namespace sourcemeta::one {

static auto write_binary_header(std::ostream &output,
                                const std::string_view mime,
                                const MetapackEncoding encoding,
                                const std::span<const std::uint8_t> extension,
                                const std::chrono::milliseconds duration,
                                const std::string_view payload,
                                const std::size_t uncompressed_size) -> void {
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
  header.duration = duration.count();

  std::ostringstream checksum_hex;
  sourcemeta::core::sha256(payload, checksum_hex);
  const auto hex_string{checksum_hex.str()};
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
  std::ofstream output{destination, std::ios::binary};
  assert(!output.fail());

  write_binary_header(output, mime, encoding, extension, duration, content,
                      content.size());

  if (encoding == MetapackEncoding::GZIP) {
    const auto compressed{sourcemeta::one::gzip(
        reinterpret_cast<const std::uint8_t *>(content.data()),
        content.size())};
    output.write(compressed.data(),
                 static_cast<std::streamsize>(compressed.size()));
  } else {
    output.write(content.data(), static_cast<std::streamsize>(content.size()));
  }

  output.flush();
}

auto metapack_write_json(const std::filesystem::path &destination,
                         const sourcemeta::core::JSON &document,
                         const std::string_view mime,
                         const MetapackEncoding encoding,
                         const std::span<const std::uint8_t> extension,
                         const std::chrono::milliseconds duration) -> void {
  std::ostringstream buffer;
  sourcemeta::core::stringify(document, buffer);
  std::filesystem::create_directories(destination.parent_path());
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
  std::filesystem::create_directories(destination.parent_path());
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
  std::filesystem::create_directories(destination.parent_path());
  write_metapack(destination, mime, encoding, extension, duration, content);
}

auto metapack_write_file(const std::filesystem::path &destination,
                         const std::filesystem::path &source,
                         const std::string_view mime,
                         const MetapackEncoding encoding,
                         const std::span<const std::uint8_t> extension,
                         const std::chrono::milliseconds duration) -> void {
  auto stream{sourcemeta::core::read_file(source)};
  std::ostringstream buffer;
  buffer << stream.rdbuf();
  std::filesystem::create_directories(destination.parent_path());
  write_metapack(destination, mime, encoding, extension, duration,
                 buffer.str());
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
    if (header->content_bytes / payload_data_size > METAPACK_MAX_DECOMPRESSION_RATIO) {
      return std::nullopt;
    }

    const auto decompressed{
        sourcemeta::one::gunzip(view.as<std::uint8_t>(payload_offset),
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
    if (header->content_bytes / payload_data_size > METAPACK_MAX_DECOMPRESSION_RATIO) {
      return std::nullopt;
    }

    return sourcemeta::one::gunzip(view.as<std::uint8_t>(payload_offset),
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
