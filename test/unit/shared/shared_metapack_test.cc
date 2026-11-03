#include <sourcemeta/core/test.h>
#include <sourcemeta/one/shared.h>

#include <cstdint> // std::uint8_t
#include <span>    // std::span
#include <vector>  // std::vector

static constexpr auto DIALECT{"https://json-schema.org/draft/2020-12/schema"};

TEST(schema_dialect_round_trip) {
  const auto bytes{sourcemeta::one::make_metapack_schema_extension(DIALECT)};
  EXPECT_EQ(sourcemeta::one::metapack_schema_dialect(
                std::span<const std::uint8_t>{bytes}),
            DIALECT);
}

TEST(schema_dialect_of_an_empty_extension) {
  const std::vector<std::uint8_t> bytes;
  EXPECT_TRUE(sourcemeta::one::metapack_schema_dialect(
                  std::span<const std::uint8_t>{bytes})
                  .empty());
}

TEST(schema_dialect_of_another_kind) {
  auto bytes{sourcemeta::one::make_metapack_schema_extension(DIALECT)};
  bytes.front() = 0xFF;
  EXPECT_TRUE(sourcemeta::one::metapack_schema_dialect(
                  std::span<const std::uint8_t>{bytes})
                  .empty());
}

TEST(schema_dialect_of_a_truncated_extension) {
  auto bytes{sourcemeta::one::make_metapack_schema_extension(DIALECT)};
  bytes.resize(bytes.size() - 1);
  EXPECT_TRUE(sourcemeta::one::metapack_schema_dialect(
                  std::span<const std::uint8_t>{bytes})
                  .empty());
}

TEST(leaf_extension_as_of_another_kind) {
  auto bytes{sourcemeta::one::make_metapack_schema_extension(DIALECT)};
  bytes.front() = 0xFF;
  EXPECT_TRUE(sourcemeta::one::metapack_leaf_extension_as<
                  sourcemeta::one::MetapackSchemaExtension>(
                  std::span<const std::uint8_t>{bytes},
                  sourcemeta::one::LeafKind::Schema) == nullptr);
}

TEST(leaf_extension_as_of_the_expected_kind) {
  const auto bytes{sourcemeta::one::make_metapack_schema_extension(DIALECT)};
  EXPECT_TRUE(sourcemeta::one::metapack_leaf_extension_as<
                  sourcemeta::one::MetapackSchemaExtension>(
                  std::span<const std::uint8_t>{bytes},
                  sourcemeta::one::LeafKind::Schema) != nullptr);
}
