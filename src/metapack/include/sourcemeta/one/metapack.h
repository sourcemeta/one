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
#include <ostream>     // std::ostream
#include <span>        // std::span
#include <string_view> // std::string_view
#include <vector>      // std::vector

namespace sourcemeta::one {

/// The magic number at the start of every metapack file: "META"
static constexpr std::uint32_t METAPACK_MAGIC{0x4154454D};

/// The current binary format version
static constexpr std::uint16_t METAPACK_VERSION{1};

/// Payload encoding
enum class MetapackEncoding : std::uint8_t { Identity = 0, GZIP = 1 };

/// The fixed portion of the binary header. All multi-byte fields are
/// little-endian (native on x86/ARM).
#pragma pack(push, 1)
struct MetapackHeader {
  std::uint32_t magic;
  std::uint16_t format_version;
  MetapackEncoding encoding;
  std::uint8_t reserved;
  std::int64_t last_modified;  // nanoseconds since epoch
  std::uint64_t content_bytes; // uncompressed payload size
  std::int64_t duration;       // build duration in milliseconds
  // SHA-256 checksum (32 raw bytes)
  std::array<std::uint8_t, 32> checksum;
  std::uint16_t mime_length;
  // Followed by:
  //   mime (mime_length bytes, UTF-8)
  //   extension_size (4 bytes, uint32)
  //   extension (extension_size bytes, opaque)
  //   payload data
};
#pragma pack(pop)

/// Write a metapack file with JSON payload (compact serialization)
SOURCEMETA_ONE_METAPACK_EXPORT
auto metapack_write_json(const std::filesystem::path &destination,
                         const sourcemeta::core::JSON &document,
                         const std::string_view mime, MetapackEncoding encoding,
                         std::span<const std::uint8_t> extension,
                         std::chrono::milliseconds duration) -> void;

/// Write a metapack file with JSON payload (pretty serialization)
SOURCEMETA_ONE_METAPACK_EXPORT
auto metapack_write_pretty_json(const std::filesystem::path &destination,
                                const sourcemeta::core::JSON &document,
                                const std::string_view mime,
                                MetapackEncoding encoding,
                                std::span<const std::uint8_t> extension,
                                std::chrono::milliseconds duration) -> void;

/// Write a metapack file with text payload
SOURCEMETA_ONE_METAPACK_EXPORT
auto metapack_write_text(const std::filesystem::path &destination,
                         std::string_view contents, const std::string_view mime,
                         MetapackEncoding encoding,
                         std::span<const std::uint8_t> extension,
                         std::chrono::milliseconds duration) -> void;

/// Write a metapack file with JSONL payload
SOURCEMETA_ONE_METAPACK_EXPORT
auto metapack_write_jsonl(const std::filesystem::path &destination,
                          const std::vector<sourcemeta::core::JSON> &entries,
                          const std::string_view mime,
                          MetapackEncoding encoding,
                          std::span<const std::uint8_t> extension,
                          std::chrono::milliseconds duration) -> void;

/// Write a metapack file with a raw file as payload
SOURCEMETA_ONE_METAPACK_EXPORT
auto metapack_write_file(const std::filesystem::path &destination,
                         const std::filesystem::path &source,
                         const std::string_view mime, MetapackEncoding encoding,
                         std::span<const std::uint8_t> extension,
                         std::chrono::milliseconds duration) -> void;

// ---------------------------------------------------------------------------
// Extension structs for specific file types
// ---------------------------------------------------------------------------

/// Extension for schemas/.../schema.metapack, bundle.metapack, editor.metapack.
/// Contains the dialect URI for the JSON Schema Link header.
#pragma pack(push, 1)
struct MetapackDialectExtension {
  std::uint16_t dialect_length;
  // Followed by dialect_length bytes of UTF-8 dialect URI string
};
#pragma pack(pop)

/// Extension for explorer/.../schema.metapack (SchemaMetadata).
/// Contains all fields needed by search index and directory listing handlers.
#pragma pack(push, 1)
struct MetapackExplorerSchemaExtension {
  std::int64_t health;
  std::int64_t bytes;
  std::int64_t dependencies;
  std::uint16_t path_length;
  std::uint16_t identifier_length;
  std::uint16_t base_dialect_length;
  std::uint16_t dialect_length;
  std::uint16_t title_length;
  std::uint16_t description_length;
  std::uint16_t alert_length;
  std::uint16_t provenance_length;
  // Followed by string data in order:
  //   path, identifier, base_dialect, dialect,
  //   title, description, alert, provenance
};
#pragma pack(pop)

/// Helper to read string fields from a MetapackExplorerSchemaExtension
/// in mmap'd memory. The `base` pointer must point to the start of the
/// extension data in the FileView. Strings are contiguous after the struct.
inline auto metapack_explorer_schema_string(
    const MetapackExplorerSchemaExtension *, const std::uint8_t *base,
    const std::size_t field_offset, const std::size_t field_length)
    -> std::string_view {
  // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
  return {reinterpret_cast<const char *>(
              base + sizeof(MetapackExplorerSchemaExtension) + field_offset),
          field_length};
}

inline auto
metapack_explorer_path(const MetapackExplorerSchemaExtension *extension,
                       const std::uint8_t *base) -> std::string_view {
  return metapack_explorer_schema_string(extension, base, 0,
                                         extension->path_length);
}

inline auto
metapack_explorer_identifier(const MetapackExplorerSchemaExtension *extension,
                             const std::uint8_t *base) -> std::string_view {
  const std::size_t offset{extension->path_length};
  return metapack_explorer_schema_string(extension, base, offset,
                                         extension->identifier_length);
}

inline auto
metapack_explorer_base_dialect(const MetapackExplorerSchemaExtension *extension,
                               const std::uint8_t *base) -> std::string_view {
  const std::size_t offset{static_cast<std::size_t>(extension->path_length) +
                           extension->identifier_length};
  return metapack_explorer_schema_string(extension, base, offset,
                                         extension->base_dialect_length);
}

inline auto
metapack_explorer_dialect(const MetapackExplorerSchemaExtension *extension,
                          const std::uint8_t *base) -> std::string_view {
  const std::size_t offset{static_cast<std::size_t>(extension->path_length) +
                           extension->identifier_length +
                           extension->base_dialect_length};
  return metapack_explorer_schema_string(extension, base, offset,
                                         extension->dialect_length);
}

inline auto
metapack_explorer_title(const MetapackExplorerSchemaExtension *extension,
                        const std::uint8_t *base) -> std::string_view {
  const std::size_t offset{static_cast<std::size_t>(extension->path_length) +
                           extension->identifier_length +
                           extension->base_dialect_length +
                           extension->dialect_length};
  return metapack_explorer_schema_string(extension, base, offset,
                                         extension->title_length);
}

inline auto
metapack_explorer_description(const MetapackExplorerSchemaExtension *extension,
                              const std::uint8_t *base) -> std::string_view {
  const std::size_t offset{static_cast<std::size_t>(extension->path_length) +
                           extension->identifier_length +
                           extension->base_dialect_length +
                           extension->dialect_length + extension->title_length};
  return metapack_explorer_schema_string(extension, base, offset,
                                         extension->description_length);
}

inline auto
metapack_explorer_alert(const MetapackExplorerSchemaExtension *extension,
                        const std::uint8_t *base) -> std::string_view {
  const std::size_t offset{static_cast<std::size_t>(extension->path_length) +
                           extension->identifier_length +
                           extension->base_dialect_length +
                           extension->dialect_length + extension->title_length +
                           extension->description_length};
  return metapack_explorer_schema_string(extension, base, offset,
                                         extension->alert_length);
}

inline auto
metapack_explorer_provenance(const MetapackExplorerSchemaExtension *extension,
                             const std::uint8_t *base) -> std::string_view {
  const std::size_t offset{
      static_cast<std::size_t>(extension->path_length) +
      extension->identifier_length + extension->base_dialect_length +
      extension->dialect_length + extension->title_length +
      extension->description_length + extension->alert_length};
  return metapack_explorer_schema_string(extension, base, offset,
                                         extension->provenance_length);
}

/// Build a MetapackDialectExtension as a byte vector
SOURCEMETA_ONE_METAPACK_EXPORT
auto metapack_make_dialect_extension(std::string_view dialect)
    -> std::vector<std::uint8_t>;

/// Build a MetapackExplorerSchemaExtension as a byte vector
SOURCEMETA_ONE_METAPACK_EXPORT
auto metapack_make_explorer_schema_extension(
    std::int64_t health, std::int64_t bytes, std::int64_t dependencies,
    std::string_view path, std::string_view identifier,
    std::string_view base_dialect, std::string_view dialect,
    std::string_view title, std::string_view description,
    std::string_view alert, std::string_view provenance)
    -> std::vector<std::uint8_t>;

// ---------------------------------------------------------------------------
// Read functions
// ---------------------------------------------------------------------------

/// Read the full JSON payload from a metapack file (decompresses if needed)
SOURCEMETA_ONE_METAPACK_EXPORT
auto metapack_read_json(const std::filesystem::path &path)
    -> sourcemeta::core::JSON;

/// Header metadata extracted from a metapack file
struct MetapackInfo {
  std::string checksum_hex;
  std::chrono::system_clock::time_point last_modified;
  std::string mime;
  MetapackEncoding encoding;
  std::uint64_t content_bytes;
  std::chrono::milliseconds duration;
};

/// Read only the header metadata from a memory-mapped metapack file
SOURCEMETA_ONE_METAPACK_EXPORT
auto metapack_info(const sourcemeta::core::FileView &view) -> MetapackInfo;

/// Compute the byte offset where the payload data begins
SOURCEMETA_ONE_METAPACK_EXPORT
auto metapack_payload_offset(const sourcemeta::core::FileView &view)
    -> std::size_t;

/// Compute the byte offset where the extension data begins within
/// a memory-mapped metapack file. Returns 0 if the extension is empty.
SOURCEMETA_ONE_METAPACK_EXPORT
auto metapack_extension_offset(const sourcemeta::core::FileView &view)
    -> std::size_t;

/// Compute the size of the extension data within a memory-mapped
/// metapack file. Returns 0 if there is no extension.
SOURCEMETA_ONE_METAPACK_EXPORT
auto metapack_extension_size(const sourcemeta::core::FileView &view)
    -> std::uint32_t;

/// Read the typed extension from a memory-mapped metapack file.
/// The returned pointer points directly into the mmap'd memory.
/// The FileView must outlive the returned pointer.
/// Returns nullptr if the file has no extension or extension_size
/// is smaller than sizeof(T).
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

/// Helper: read a string_view from a dialect extension in mmap'd memory.
/// The FileView must outlive the returned string_view.
inline auto metapack_dialect_string(const sourcemeta::core::FileView &view)
    -> std::string_view {
  const auto *extension{metapack_extension<MetapackDialectExtension>(view)};
  if (extension == nullptr || extension->dialect_length == 0) {
    return {};
  }

  const auto string_offset{metapack_extension_offset(view) +
                           sizeof(MetapackDialectExtension)};
  // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
  return {reinterpret_cast<const char *>(view.as<std::uint8_t>(string_offset)),
          extension->dialect_length};
}

} // namespace sourcemeta::one

#endif
