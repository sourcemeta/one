#ifndef SOURCEMETA_ONE_SHARED_METAPACK_H
#define SOURCEMETA_ONE_SHARED_METAPACK_H

#include <sourcemeta/core/metapack.h>

#include <sourcemeta/one/shared_leaf.h>

#include <cassert>     // assert
#include <cstdint>     // std::uint8_t, std::uint16_t
#include <cstring>     // std::memcpy
#include <limits>      // std::numeric_limits
#include <span>        // std::span
#include <string_view> // std::string_view
#include <vector>      // std::vector

namespace sourcemeta::one {

#pragma pack(push, 1)
// The extension area of every leaf-scoped artifact begins with this
struct MetapackLeafExtension {
  LeafKind kind;
};

// The extension area of an artifact holding a schema, which is where the
// dialect lives for a schema whose dialect reserves no keyword to declare one.
// The dialect follows the layout, as many bytes as it declares
struct MetapackSchemaExtension {
  MetapackLeafExtension leaf;
  std::uint16_t dialect_length;
};
#pragma pack(pop)

static_assert(sizeof(MetapackLeafExtension) == 1,
              "The tag is read where it lies, ahead of a layout that is only "
              "safe to read once it has been matched");
static_assert(sizeof(MetapackSchemaExtension) == 3,
              "The layout is part of the format, so moving, resizing or "
              "adding a field means every artifact has to be written again");

// The extension area as the layout that the given kind lays out, or nothing
// when it holds another kind or is too small to hold that layout at all. Every
// read of a leaf-scoped extension goes through here, so that a layout is never
// read off an artifact that holds something else
template <typename T>
[[nodiscard]] auto
metapack_leaf_extension_as(const std::span<const std::uint8_t> extension,
                           const LeafKind kind) -> const T * {
  const auto *leaf{
      sourcemeta::core::metapack_extension_as<MetapackLeafExtension>(
          extension)};
  if (leaf == nullptr || leaf->kind != kind) {
    return nullptr;
  }

  return sourcemeta::core::metapack_extension_as<T>(extension);
}

// The extension area of an artifact holding a schema of the given dialect
[[nodiscard]] inline auto
make_metapack_schema_extension(const std::string_view dialect)
    -> std::vector<std::uint8_t> {
  assert(dialect.size() <= std::numeric_limits<std::uint16_t>::max());
  std::vector<std::uint8_t> result;
  result.resize(sizeof(MetapackSchemaExtension) + dialect.size());
  MetapackSchemaExtension layout{};
  layout.leaf.kind = LeafKind::Schema;
  layout.dialect_length = static_cast<std::uint16_t>(dialect.size());
  std::memcpy(result.data(), &layout, sizeof(layout));
  std::memcpy(result.data() + sizeof(layout), dialect.data(), dialect.size());
  return result;
}

// The dialect an artifact records alongside its contents, or nothing at all
// for an extension area that holds no schema
[[nodiscard]] inline auto
metapack_schema_dialect(const std::span<const std::uint8_t> extension)
    -> std::string_view {
  const auto *layout{metapack_leaf_extension_as<MetapackSchemaExtension>(
      extension, LeafKind::Schema)};
  if (layout == nullptr || layout->dialect_length == 0 ||
      sizeof(MetapackSchemaExtension) + layout->dialect_length >
          extension.size()) {
    return {};
  }

  // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
  return {reinterpret_cast<const char *>(extension.data() +
                                         sizeof(MetapackSchemaExtension)),
          layout->dialect_length};
}

} // namespace sourcemeta::one

#endif
