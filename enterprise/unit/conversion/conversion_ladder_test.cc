#include <sourcemeta/core/test.h>
#include <sourcemeta/one/enterprise_conversion.h>

#include <cstddef> // std::size_t
#include <cstdint> // std::uint32_t
#include <vector>  // std::vector

static auto targets(const sourcemeta::one::SchemaDialect dialect)
    -> std::vector<sourcemeta::one::SchemaDialect> {
  const auto result{sourcemeta::one::dialect_conversions(dialect)};
  return std::vector<sourcemeta::one::SchemaDialect>{result.begin(),
                                                     result.end()};
}

TEST(every_target_converts_into_itself_and_everything_newer) {
  EXPECT_EQ(targets(sourcemeta::one::SchemaDialect::Draft202012),
            (std::vector{sourcemeta::one::SchemaDialect::Draft202012,
                         sourcemeta::one::SchemaDialect::OpenAPI31,
                         sourcemeta::one::SchemaDialect::OpenAPI32}));
  EXPECT_EQ(targets(sourcemeta::one::SchemaDialect::OpenAPI31),
            (std::vector{sourcemeta::one::SchemaDialect::OpenAPI31,
                         sourcemeta::one::SchemaDialect::OpenAPI32}));
  EXPECT_EQ(targets(sourcemeta::one::SchemaDialect::OpenAPI32),
            (std::vector{sourcemeta::one::SchemaDialect::OpenAPI32}));
}

TEST(the_newest_dialect_converts_into_nothing_beyond_itself) {
  EXPECT_EQ(targets(sourcemeta::one::SchemaDialect::OpenAPI32).size(), 1);
  EXPECT_TRUE(sourcemeta::one::conversion_applies(
      sourcemeta::one::SchemaDialect::OpenAPI32,
      sourcemeta::one::SchemaDialect::OpenAPI32));
  EXPECT_FALSE(sourcemeta::one::conversion_applies(
      sourcemeta::one::SchemaDialect::OpenAPI32,
      sourcemeta::one::SchemaDialect::Draft202012));
}

// A dialect nothing converts into starts the upgrade order rather than sitting
// on it, so everything the order holds is reachable from it
TEST(a_dialect_nothing_converts_into_reaches_every_target) {
  EXPECT_FALSE(
      sourcemeta::one::conversion_rung(sourcemeta::one::SchemaDialect::Draft3)
          .has_value());
  EXPECT_EQ(targets(sourcemeta::one::SchemaDialect::Draft3).size(),
            sourcemeta::one::SCHEMA_CONVERSION_TARGETS.size());
  EXPECT_EQ(targets(sourcemeta::one::SchemaDialect::Draft3),
            (std::vector<sourcemeta::one::SchemaDialect>{
                sourcemeta::one::SCHEMA_CONVERSION_TARGETS.begin(),
                sourcemeta::one::SCHEMA_CONVERSION_TARGETS.end()}));
}

// Draft 4 is the oldest dialect anything converts into, so it is where every
// starting dialect lands first
TEST(the_oldest_target_is_the_first_rung) {
  EXPECT_EQ(
      sourcemeta::one::conversion_rung(sourcemeta::one::SchemaDialect::Draft4),
      0);
  EXPECT_EQ(sourcemeta::one::conversion_selector(
                sourcemeta::one::SchemaDialect::Draft4),
            0);
  EXPECT_EQ(targets(sourcemeta::one::SchemaDialect::Draft3).front(),
            sourcemeta::one::SchemaDialect::Draft4);
}

// The selection bits index the build rules, so they follow the order of the
// targets rather than the order the dialects happen to be declared in
TEST(selection_bits_follow_the_target_order) {
  for (std::size_t index = 0;
       index < sourcemeta::one::SCHEMA_CONVERSION_TARGETS.size(); index++) {
    EXPECT_EQ(sourcemeta::one::conversion_selector(
                  sourcemeta::one::SCHEMA_CONVERSION_TARGETS.at(index)),
              index);
  }
}

TEST(a_starting_dialect_selects_every_conversion) {
  const auto every{sourcemeta::one::conversion_selection(
      "http://json-schema.org/draft-03/schema#", false)};
  std::uint32_t expected{0};
  for (std::size_t index = 0;
       index < sourcemeta::one::SCHEMA_CONVERSION_TARGETS.size(); index++) {
    expected |= std::uint32_t{1} << index;
  }

  EXPECT_EQ(every, expected);
}

TEST(a_metaschema_selects_no_conversion) {
  EXPECT_EQ(sourcemeta::one::conversion_selection(
                "https://json-schema.org/draft/2020-12/schema", true),
            0);
}

TEST(a_dialect_this_catalog_does_not_know_selects_no_conversion) {
  EXPECT_EQ(sourcemeta::one::conversion_selection("https://example.com/dialect",
                                                  false),
            0);
}
