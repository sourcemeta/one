#ifndef SOURCEMETA_ONE_METAPACK_H_
#define SOURCEMETA_ONE_METAPACK_H_

#ifndef SOURCEMETA_ONE_METAPACK_EXPORT
#include <sourcemeta/one/metapack_export.h>
#endif

#include <sourcemeta/core/io.h>
#include <sourcemeta/core/json.h>

#include <array>       // std::array
#include <chrono>      // std::chrono
#include <cstdint>     // std::uint8_t, std::uint16_t, std::uint32_t, etc.
#include <filesystem>  // std::filesystem::path
#include <optional>    // std::optional
#include <span>        // std::span
#include <string_view> // std::string_view

namespace sourcemeta::one {

static constexpr std::uint32_t METAPACK_MAGIC{0x4154454D};
static constexpr std::uint16_t METAPACK_VERSION{1};

enum class MetapackEncoding : std::uint8_t { Identity = 0, GZIP = 1 };

#pragma pack(push, 1)
struct MetapackHeader {
  std::uint32_t magic;
  std::uint16_t format_version;
  MetapackEncoding encoding;
  std::uint8_t reserved;
  std::int64_t last_modified;
  std::uint64_t content_bytes;
  std::int64_t duration;
  std::array<std::uint8_t, 32> checksum;
  std::uint16_t mime_length;
};
#pragma pack(pop)

struct MetapackInfo {
  std::string checksum_hex;
  std::chrono::system_clock::time_point last_modified;
  std::string mime;
  MetapackEncoding encoding;
  std::uint64_t content_bytes;
  std::chrono::milliseconds duration;
};

// Writers

SOURCEMETA_ONE_METAPACK_EXPORT
auto metapack_write_json(const std::filesystem::path &destination,
                         const sourcemeta::core::JSON &document,
                         std::string_view mime, MetapackEncoding encoding,
                         std::span<const std::uint8_t> extension,
                         std::chrono::milliseconds duration) -> void;

SOURCEMETA_ONE_METAPACK_EXPORT
auto metapack_write_pretty_json(const std::filesystem::path &destination,
                                const sourcemeta::core::JSON &document,
                                std::string_view mime,
                                MetapackEncoding encoding,
                                std::span<const std::uint8_t> extension,
                                std::chrono::milliseconds duration) -> void;

SOURCEMETA_ONE_METAPACK_EXPORT
auto metapack_write_text(const std::filesystem::path &destination,
                         std::string_view contents, std::string_view mime,
                         MetapackEncoding encoding,
                         std::span<const std::uint8_t> extension,
                         std::chrono::milliseconds duration) -> void;

SOURCEMETA_ONE_METAPACK_EXPORT
auto metapack_write_file(const std::filesystem::path &destination,
                         const std::filesystem::path &source,
                         std::string_view mime, MetapackEncoding encoding,
                         std::span<const std::uint8_t> extension,
                         std::chrono::milliseconds duration) -> void;

// Readers

SOURCEMETA_ONE_METAPACK_EXPORT
auto metapack_read_json(const std::filesystem::path &path)
    -> std::optional<sourcemeta::core::JSON>;

SOURCEMETA_ONE_METAPACK_EXPORT
auto metapack_read_text(const std::filesystem::path &path)
    -> std::optional<std::string>;

SOURCEMETA_ONE_METAPACK_EXPORT
auto metapack_info(const sourcemeta::core::FileView &view)
    -> std::optional<MetapackInfo>;

SOURCEMETA_ONE_METAPACK_EXPORT
auto metapack_payload_offset(const sourcemeta::core::FileView &view)
    -> std::optional<std::size_t>;

SOURCEMETA_ONE_METAPACK_EXPORT
auto metapack_extension_offset(const sourcemeta::core::FileView &view)
    -> std::size_t;

SOURCEMETA_ONE_METAPACK_EXPORT
auto metapack_extension_size(const sourcemeta::core::FileView &view)
    -> std::uint32_t;

template <typename T>
auto metapack_extension(const sourcemeta::core::FileView &view) -> const T * {
  const auto offset{metapack_extension_offset(view)};
  if (offset == 0) {
    return nullptr;
  }

  const auto size{metapack_extension_size(view)};
  if (size < sizeof(T)) {
    return nullptr;
  }

  return view.as<T>(offset);
}

} // namespace sourcemeta::one

#endif
