#include <gtest/gtest.h>

#include <sourcemeta/one/actions.h>

TEST(Actions_schema_directory, no_base_path_simple) {
  const auto result{sourcemeta::one::schema_directory(
      "/output", "", "http://localhost:8000/test/schemas/string#/type")};
  ASSERT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), "/output/schemas/test/schemas/string/%");
}

TEST(Actions_schema_directory, no_base_path_nested) {
  const auto result{sourcemeta::one::schema_directory(
      "/output", "", "http://localhost:8000/test/v2.0/schema#/type")};
  ASSERT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), "/output/schemas/test/v2.0/schema/%");
}

TEST(Actions_schema_directory, with_base_path) {
  const auto result{sourcemeta::one::schema_directory(
      "/output", "/v1/catalog",
      "http://localhost:8000/v1/catalog/example/string#/type")};
  ASSERT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), "/output/schemas/example/string/%");
}

TEST(Actions_schema_directory, with_deep_base_path) {
  const auto result{sourcemeta::one::schema_directory(
      "/output", "/api/v2/schemas",
      "http://localhost:8000/api/v2/schemas/example/foo#/properties/name")};
  ASSERT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), "/output/schemas/example/foo/%");
}

TEST(Actions_schema_directory, no_fragment) {
  const auto result{sourcemeta::one::schema_directory(
      "/output", "", "http://localhost:8000/test/schemas/string")};
  ASSERT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), "/output/schemas/test/schemas/string/%");
}

TEST(Actions_schema_directory, no_fragment_with_base_path) {
  const auto result{sourcemeta::one::schema_directory(
      "/output", "/v1/catalog",
      "http://localhost:8000/v1/catalog/example/string")};
  ASSERT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), "/output/schemas/example/string/%");
}

TEST(Actions_schema_directory, no_fragment_nested) {
  const auto result{sourcemeta::one::schema_directory(
      "/output", "", "http://localhost:8000/test/v2.0/schema")};
  ASSERT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), "/output/schemas/test/v2.0/schema/%");
}

TEST(Actions_schema_directory, no_fragment_external) {
  const auto result{sourcemeta::one::schema_directory(
      "/output", "", "http://json-schema.org/draft-07/schema")};
  ASSERT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), "/output/schemas/draft-07/schema/%");
}

TEST(Actions_schema_directory, no_fragment_deep_base_path) {
  const auto result{sourcemeta::one::schema_directory(
      "/output", "/api/v2/schemas",
      "http://localhost:8000/api/v2/schemas/example/foo")};
  ASSERT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), "/output/schemas/example/foo/%");
}

TEST(Actions_schema_directory, external_metaschema) {
  const auto result{sourcemeta::one::schema_directory(
      "/output", "", "http://json-schema.org/draft-07/schema#/type")};
  ASSERT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), "/output/schemas/draft-07/schema/%");
}

TEST(Actions_schema_directory, external_metaschema_with_base_path) {
  const auto result{sourcemeta::one::schema_directory(
      "/output", "/v1/catalog",
      "http://json-schema.org/draft-07/schema#/type")};
  ASSERT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), "/output/schemas/draft-07/schema/%");
}

TEST(Actions_schema_directory, empty_uri) {
  const auto result{sourcemeta::one::schema_directory("/output", "", "")};
  EXPECT_FALSE(result.has_value());
}

TEST(Actions_schema_directory, fragment_only) {
  const auto result{sourcemeta::one::schema_directory("/output", "", "#/type")};
  EXPECT_FALSE(result.has_value());
}

TEST(Actions_schema_directory, uri_without_path) {
  const auto result{sourcemeta::one::schema_directory("/output", "",
                                                      "http://localhost:8000")};
  EXPECT_FALSE(result.has_value());
}

TEST(Actions_schema_directory, uri_without_path_with_base_path) {
  const auto result{sourcemeta::one::schema_directory("/output", "/v1/catalog",
                                                      "http://localhost:8000")};
  EXPECT_FALSE(result.has_value());
}

TEST(Actions_schema_directory, uri_with_only_slash) {
  const auto result{sourcemeta::one::schema_directory(
      "/output", "", "http://localhost:8000/")};
  EXPECT_FALSE(result.has_value());
}

TEST(Actions_schema_directory, uri_equals_base_path) {
  const auto result{sourcemeta::one::schema_directory(
      "/output", "/v1/catalog", "http://localhost:8000/v1/catalog")};
  EXPECT_FALSE(result.has_value());
}

TEST(Actions_schema_directory, uri_with_port_no_path) {
  const auto result{sourcemeta::one::schema_directory(
      "/output", "", "https://example.com:443")};
  EXPECT_FALSE(result.has_value());
}

TEST(Actions_schema_directory, base_path_with_trailing_slash) {
  const auto result{sourcemeta::one::schema_directory(
      "/output", "/v1/catalog/",
      "http://localhost:8000/v1/catalog/example/foo#/type")};
  ASSERT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), "/output/schemas/example/foo/%");
}

TEST(Actions_schema_directory, nested_schema_with_base_path) {
  const auto result{sourcemeta::one::schema_directory(
      "/output", "/v1/catalog",
      "http://localhost:8000/v1/catalog/rebased/nested/deep#/minimum")};
  ASSERT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), "/output/schemas/rebased/nested/deep/%");
}

TEST(Actions_schema_directory, different_host_same_path_prefix) {
  const auto result{sourcemeta::one::schema_directory(
      "/output", "/v1/catalog",
      "https://other.example.com/v1/catalog/foo#/type")};
  ASSERT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), "/output/schemas/foo/%");
}
