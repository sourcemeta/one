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

// The OpenAPI 3.0 Schema Object is the second dialect nothing converts into,
// which makes it a sibling of Draft 3 rather than a rung between others
TEST(openapi_3_0_starts_the_order_beside_draft3) {
  EXPECT_FALSE(sourcemeta::one::conversion_rung(
                   sourcemeta::one::SchemaDialect::OpenAPI30)
                   .has_value());
  EXPECT_EQ(targets(sourcemeta::one::SchemaDialect::OpenAPI30),
            targets(sourcemeta::one::SchemaDialect::Draft3));
  EXPECT_EQ(targets(sourcemeta::one::SchemaDialect::OpenAPI30).size(),
            sourcemeta::one::SCHEMA_CONVERSION_TARGETS.size());
}

TEST(openapi_3_0_converts_into_every_target_including_the_openapi_ones) {
  for (const auto target : sourcemeta::one::SCHEMA_CONVERSION_TARGETS) {
    EXPECT_TRUE(sourcemeta::one::conversion_applies(
        sourcemeta::one::SchemaDialect::OpenAPI30, target));
  }

  EXPECT_TRUE(sourcemeta::one::conversion_applies(
      sourcemeta::one::SchemaDialect::OpenAPI30,
      sourcemeta::one::SchemaDialect::Draft4));
  EXPECT_TRUE(sourcemeta::one::conversion_applies(
      sourcemeta::one::SchemaDialect::OpenAPI30,
      sourcemeta::one::SchemaDialect::Draft202012));
  EXPECT_TRUE(sourcemeta::one::conversion_applies(
      sourcemeta::one::SchemaDialect::OpenAPI30,
      sourcemeta::one::SchemaDialect::OpenAPI31));
  EXPECT_TRUE(sourcemeta::one::conversion_applies(
      sourcemeta::one::SchemaDialect::OpenAPI30,
      sourcemeta::one::SchemaDialect::OpenAPI32));
}

// Nothing converts into either of the two dialects that start the order, so
// neither is reachable from the other
TEST(the_two_starting_dialects_do_not_convert_into_each_other) {
  EXPECT_FALSE(sourcemeta::one::conversion_applies(
      sourcemeta::one::SchemaDialect::Draft3,
      sourcemeta::one::SchemaDialect::OpenAPI30));
  EXPECT_FALSE(sourcemeta::one::conversion_applies(
      sourcemeta::one::SchemaDialect::OpenAPI30,
      sourcemeta::one::SchemaDialect::Draft3));
  EXPECT_FALSE(sourcemeta::one::conversion_applies(
      sourcemeta::one::SchemaDialect::Draft202012,
      sourcemeta::one::SchemaDialect::OpenAPI30));
}

TEST(both_openapi_3_0_dialect_spellings_name_the_same_dialect) {
  EXPECT_EQ(sourcemeta::one::schema_dialect(
                "tag:spec.openapis.org,2024-10-18:oas/3.0/dialect"),
            sourcemeta::one::SchemaDialect::OpenAPI30);
  EXPECT_EQ(sourcemeta::one::schema_dialect(
                "tag:spec.openapis.org,2021-09-28:oas/3.0/dialect"),
            sourcemeta::one::SchemaDialect::OpenAPI30);
  EXPECT_EQ(sourcemeta::one::conversion_selection(
                "tag:spec.openapis.org,2024-10-18:oas/3.0/dialect", false),
            sourcemeta::one::conversion_selection(
                "tag:spec.openapis.org,2021-09-28:oas/3.0/dialect", false));
}

TEST(an_openapi_3_0_schema_selects_every_conversion) {
  EXPECT_EQ(sourcemeta::one::conversion_selection(
                "tag:spec.openapis.org,2024-10-18:oas/3.0/dialect", false),
            sourcemeta::one::conversion_selection(
                "http://json-schema.org/draft-03/schema#", false));
}

// Nothing converts into it, so asking for it by name is asking for nothing
TEST(openapi_3_0_is_not_a_conversion_target) {
  EXPECT_FALSE(sourcemeta::one::conversion_target("openapi3.0").has_value());
  EXPECT_TRUE(sourcemeta::one::conversion_target("openapi3.1").has_value());
  EXPECT_FALSE(sourcemeta::one::conversion_target("draft3").has_value());
}

TEST(openapi_3_0_rests_on_draft4_and_names_its_newest_dialect_uri) {
  EXPECT_EQ(sourcemeta::one::conversion_name(
                sourcemeta::one::SchemaDialect::OpenAPI30),
            "openapi3.0");
  EXPECT_EQ(sourcemeta::one::conversion_uri(
                sourcemeta::one::SchemaDialect::OpenAPI30),
            "tag:spec.openapis.org,2024-10-18:oas/3.0/dialect");
  EXPECT_EQ(sourcemeta::one::conversion_base_dialect(
                sourcemeta::one::SchemaDialect::OpenAPI30),
            sourcemeta::core::SchemaBaseDialect::JSON_SCHEMA_DRAFT_4);
}

// A Schema Object cannot declare vocabularies, so nothing about it can make a
// schema read as a meta-schema other than another schema naming it as one
TEST(an_openapi_3_0_schema_is_a_metaschema_only_by_being_named_one) {
  EXPECT_FALSE(sourcemeta::one::is_metaschema(
      "tag:spec.openapis.org,2024-10-18:oas/3.0/dialect", false, false));
  EXPECT_FALSE(sourcemeta::one::is_metaschema(
      "tag:spec.openapis.org,2024-10-18:oas/3.0/dialect", true, false));
  EXPECT_TRUE(sourcemeta::one::is_metaschema(
      "tag:spec.openapis.org,2024-10-18:oas/3.0/dialect", false, true));
}
