#ifndef SOURCEMETA_ONE_METAPACK_H_
#define SOURCEMETA_ONE_METAPACK_H_

#ifndef SOURCEMETA_ONE_METAPACK_EXPORT
#include <sourcemeta/one/metapack_export.h>
#endif

#include <sourcemeta/core/io.h>
#include <sourcemeta/core/json.h>

#include <array>       // std::array
#include <chrono>      // std::chrono
#include <cstddef>     // std::size_t
#include <cstdint>     // std::uint8_t, std::uint16_t, std::uint32_t, etc.
#include <filesystem>  // std::filesystem::path
#include <optional>    // std::optional
#include <span>        // std::span
#include <string_view> // std::string_view

namespace sourcemeta::one {

static constexpr std::uint32_t METAPACK_MAGIC{0x4154454D};
static constexpr std::uint16_t METAPACK_VERSION{3};

enum class MetapackEncoding : std::uint8_t { Identity = 0, GZIP = 1 };

#pragma pack(push, 1)
struct MetapackHeader {
  std::uint32_t magic;
  std::uint16_t format_version;
  MetapackEncoding encoding;
  std::uint8_t reserved;
  std::int64_t last_modified;
  std::uint64_t content_bytes;
  // The size in bytes of the artifact after applying the compression
  // matching the supported wire content-coding, or zero for an artifact
  // stored as it reads, which is never compressed at index time. The codec
  // is gzip today, the field name stays compression-agnostic so a future
  // codec swap is mechanical. For compressed-storage artifacts this equals
  // the payload size on disk.
  std::uint64_t compressed_bytes;
  std::array<std::uint8_t, 32> checksum;
  std::uint16_t mime_length;
};
#pragma pack(pop)

static constexpr std::size_t METAPACK_CHECKSUM_HEX_LENGTH{64};

// Writers

SOURCEMETA_ONE_METAPACK_EXPORT
auto metapack_write_json(const std::filesystem::path &destination,
                         const sourcemeta::core::JSON &document,
                         std::string_view mime, MetapackEncoding encoding,
                         std::span<const std::uint8_t> extension) -> void;

SOURCEMETA_ONE_METAPACK_EXPORT
auto metapack_write_pretty_json(const std::filesystem::path &destination,
                                const sourcemeta::core::JSON &document,
                                std::string_view mime,
                                MetapackEncoding encoding,
                                std::span<const std::uint8_t> extension)
    -> void;

SOURCEMETA_ONE_METAPACK_EXPORT
auto metapack_write_text(const std::filesystem::path &destination,
                         std::string_view contents, std::string_view mime,
                         MetapackEncoding encoding,
                         std::span<const std::uint8_t> extension) -> void;

SOURCEMETA_ONE_METAPACK_EXPORT
auto metapack_write_bytes(const std::filesystem::path &destination,
                          std::span<const std::uint8_t> contents,
                          std::string_view mime, MetapackEncoding encoding,
                          std::span<const std::uint8_t> extension) -> void;

SOURCEMETA_ONE_METAPACK_EXPORT
auto metapack_write_file(const std::filesystem::path &destination,
                         const std::filesystem::path &source,
                         std::string_view mime, MetapackEncoding encoding,
                         std::span<const std::uint8_t> extension) -> void;

// Readers

// Every part of a metapack file, located in a single pass. The views point
// into the memory-mapped file, so they last exactly as long as it stays mapped
struct MetapackView {
  const MetapackHeader *header;
  std::string_view mime;
  std::span<const std::uint8_t> extension;
  std::span<const std::uint8_t> payload;
};

SOURCEMETA_ONE_METAPACK_EXPORT
auto metapack_parse(const sourcemeta::core::FileView &view)
    -> std::optional<MetapackView>;

SOURCEMETA_ONE_METAPACK_EXPORT
auto metapack_read_json(const std::filesystem::path &path)
    -> std::optional<sourcemeta::core::JSON>;

SOURCEMETA_ONE_METAPACK_EXPORT
auto metapack_read_json(const sourcemeta::core::FileView &view)
    -> std::optional<sourcemeta::core::JSON>;

SOURCEMETA_ONE_METAPACK_EXPORT
auto metapack_read_json(const MetapackView &parsed)
    -> std::optional<sourcemeta::core::JSON>;

SOURCEMETA_ONE_METAPACK_EXPORT
auto metapack_checksum_hex(const MetapackHeader &header)
    -> std::array<char, METAPACK_CHECKSUM_HEX_LENGTH>;

SOURCEMETA_ONE_METAPACK_EXPORT
auto metapack_last_modified(const MetapackHeader &header)
    -> std::chrono::system_clock::time_point;

template <typename T>
auto metapack_extension_as(const std::span<const std::uint8_t> extension)
    -> const T * {
  if (extension.size() < sizeof(T)) {
    return nullptr;
  }

  // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
  return reinterpret_cast<const T *>(extension.data());
}

} // namespace sourcemeta::one

#endif
