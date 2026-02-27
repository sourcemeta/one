#include <gtest/gtest.h>

#include <sourcemeta/core/uri.h>
#include <sourcemeta/one/configuration.h>

static auto make_collection(const std::filesystem::path &absolute_path,
                            const std::string &base)
    -> sourcemeta::one::Configuration::Collection {
  sourcemeta::one::Configuration::Collection collection;
  collection.absolute_path = absolute_path;
  collection.base = base;
  return collection;
}

TEST(Configuration_resolve_path, absolute_uri_matching_collection) {
  sourcemeta::one::Configuration configuration;
  configuration.url = "https://sourcemeta.com";
  configuration.entries.emplace(
      "schemas", make_collection("/tmp/schemas", "https://example.com"));

  const sourcemeta::core::URI input{
      "https://sourcemeta.com/schemas/foo/bar.json"};
  const auto result{configuration.resolve_path(input)};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), "/tmp/schemas/foo/bar.json");
}

TEST(Configuration_resolve_path,
     absolute_uri_matching_collection_trailing_slash) {
  sourcemeta::one::Configuration configuration;
  configuration.url = "https://sourcemeta.com/";
  configuration.entries.emplace(
      "schemas", make_collection("/tmp/schemas", "https://example.com"));

  const sourcemeta::core::URI input{
      "https://sourcemeta.com/schemas/foo/bar.json"};
  const auto result{configuration.resolve_path(input)};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), "/tmp/schemas/foo/bar.json");
}

TEST(Configuration_resolve_path, path_with_leading_slash) {
  sourcemeta::one::Configuration configuration;
  configuration.url = "https://sourcemeta.com";
  configuration.entries.emplace(
      "schemas", make_collection("/tmp/schemas", "https://example.com"));

  const sourcemeta::core::URI input{"/schemas/foo/bar.json"};
  const auto result{configuration.resolve_path(input)};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), "/tmp/schemas/foo/bar.json");
}

TEST(Configuration_resolve_path, path_without_leading_slash) {
  sourcemeta::one::Configuration configuration;
  configuration.url = "https://sourcemeta.com";
  configuration.entries.emplace(
      "schemas", make_collection("/tmp/schemas", "https://example.com"));

  const sourcemeta::core::URI input{"schemas/foo/bar.json"};
  const auto result{configuration.resolve_path(input)};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), "/tmp/schemas/foo/bar.json");
}

TEST(Configuration_resolve_path, no_matching_collection) {
  sourcemeta::one::Configuration configuration;
  configuration.url = "https://sourcemeta.com";
  configuration.entries.emplace(
      "schemas", make_collection("/tmp/schemas", "https://example.com"));

  const sourcemeta::core::URI input{"/unknown/foo"};
  const auto result{configuration.resolve_path(input)};
  EXPECT_FALSE(result.has_value());
}

TEST(Configuration_resolve_path, wrong_origin) {
  sourcemeta::one::Configuration configuration;
  configuration.url = "https://sourcemeta.com";
  configuration.entries.emplace(
      "schemas", make_collection("/tmp/schemas", "https://example.com"));

  const sourcemeta::core::URI input{"https://other.com/schemas/foo"};
  const auto result{configuration.resolve_path(input)};
  EXPECT_FALSE(result.has_value());
}

TEST(Configuration_resolve_path, nested_collection_key) {
  sourcemeta::one::Configuration configuration;
  configuration.url = "https://sourcemeta.com";
  configuration.entries.emplace(
      "foo/bar", make_collection("/tmp/nested", "https://example.com"));

  const sourcemeta::core::URI input{"/foo/bar/baz.json"};
  const auto result{configuration.resolve_path(input)};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), "/tmp/nested/baz.json");
}

TEST(Configuration_resolve_path, key_is_string_prefix_not_path_prefix) {
  sourcemeta::one::Configuration configuration;
  configuration.url = "https://sourcemeta.com";
  configuration.entries.emplace(
      "schemas", make_collection("/tmp/schemas", "https://example.com"));

  const sourcemeta::core::URI input{"/schemas-extra/foo"};
  const auto result{configuration.resolve_path(input)};
  EXPECT_FALSE(result.has_value());
}

TEST(Configuration_resolve_path, collection_root_exact) {
  sourcemeta::one::Configuration configuration;
  configuration.url = "https://sourcemeta.com";
  configuration.entries.emplace(
      "schemas", make_collection("/tmp/schemas", "https://example.com"));

  const sourcemeta::core::URI input{"/schemas"};
  const auto result{configuration.resolve_path(input)};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), "/tmp/schemas/");
}

TEST(Configuration_resolve_path, collection_root_trailing_slash) {
  sourcemeta::one::Configuration configuration;
  configuration.url = "https://sourcemeta.com";
  configuration.entries.emplace(
      "schemas", make_collection("/tmp/schemas", "https://example.com"));

  const sourcemeta::core::URI input{"/schemas/"};
  const auto result{configuration.resolve_path(input)};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), "/tmp/schemas/");
}

TEST(Configuration_resolve_path, multiple_collections_matches_correct_one) {
  sourcemeta::one::Configuration configuration;
  configuration.url = "https://sourcemeta.com";
  configuration.entries.emplace(
      "alpha", make_collection("/tmp/alpha", "https://alpha.com"));
  configuration.entries.emplace(
      "beta", make_collection("/tmp/beta", "https://beta.com"));

  const sourcemeta::core::URI input{"/beta/test.json"};
  const auto result{configuration.resolve_path(input)};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), "/tmp/beta/test.json");
}

TEST(Configuration_resolve_path, page_entry_is_not_matched) {
  sourcemeta::one::Configuration configuration;
  configuration.url = "https://sourcemeta.com";
  sourcemeta::one::Configuration::Page page;
  page.title = "My Page";
  configuration.entries.emplace("docs", std::move(page));

  const sourcemeta::core::URI input{"/docs/foo.json"};
  const auto result{configuration.resolve_path(input)};
  EXPECT_FALSE(result.has_value());
}

TEST(Configuration_resolve_path, empty_path_from_url) {
  sourcemeta::one::Configuration configuration;
  configuration.url = "https://sourcemeta.com";
  configuration.entries.emplace(
      "schemas", make_collection("/tmp/schemas", "https://example.com"));

  const sourcemeta::core::URI input{"https://sourcemeta.com"};
  const auto result{configuration.resolve_path(input)};
  EXPECT_FALSE(result.has_value());
}

TEST(Configuration_resolve_path, empty_path_from_url_trailing_slash) {
  sourcemeta::one::Configuration configuration;
  configuration.url = "https://sourcemeta.com/";
  configuration.entries.emplace(
      "schemas", make_collection("/tmp/schemas", "https://example.com"));

  const sourcemeta::core::URI input{"https://sourcemeta.com/"};
  const auto result{configuration.resolve_path(input)};
  EXPECT_FALSE(result.has_value());
}

TEST(Configuration_resolve_path, deeply_nested_file) {
  sourcemeta::one::Configuration configuration;
  configuration.url = "https://sourcemeta.com";
  configuration.entries.emplace(
      "schemas", make_collection("/tmp/schemas", "https://example.com"));

  const sourcemeta::core::URI input{"/schemas/a/b/c/d.json"};
  const auto result{configuration.resolve_path(input)};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), "/tmp/schemas/a/b/c/d.json");
}

TEST(Configuration_resolve_path, no_entries) {
  sourcemeta::one::Configuration configuration;
  configuration.url = "https://sourcemeta.com";

  const sourcemeta::core::URI input{"/schemas/foo.json"};
  const auto result{configuration.resolve_path(input)};
  EXPECT_FALSE(result.has_value());
}

TEST(Configuration_resolve_path, page_and_collection_only_matches_collection) {
  sourcemeta::one::Configuration configuration;
  configuration.url = "https://sourcemeta.com";
  sourcemeta::one::Configuration::Page page;
  page.title = "My Page";
  configuration.entries.emplace("docs", std::move(page));
  configuration.entries.emplace(
      "schemas", make_collection("/tmp/schemas", "https://example.com"));

  const sourcemeta::core::URI input{"/schemas/test.json"};
  const auto result{configuration.resolve_path(input)};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), "/tmp/schemas/test.json");
}

TEST(Configuration_resolve_path, absolute_uri_collection_root) {
  sourcemeta::one::Configuration configuration;
  configuration.url = "https://sourcemeta.com";
  configuration.entries.emplace(
      "schemas", make_collection("/tmp/schemas", "https://example.com"));

  const sourcemeta::core::URI input{"https://sourcemeta.com/schemas"};
  const auto result{configuration.resolve_path(input)};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), "/tmp/schemas/");
}

TEST(Configuration_resolve_path, different_scheme) {
  sourcemeta::one::Configuration configuration;
  configuration.url = "https://sourcemeta.com";
  configuration.entries.emplace(
      "schemas", make_collection("/tmp/schemas", "https://example.com"));

  const sourcemeta::core::URI input{"http://sourcemeta.com/schemas/foo"};
  const auto result{configuration.resolve_path(input)};
  EXPECT_FALSE(result.has_value());
}

TEST(Configuration_resolve_path, url_with_port) {
  sourcemeta::one::Configuration configuration;
  configuration.url = "http://localhost:8000";
  configuration.entries.emplace(
      "schemas", make_collection("/tmp/schemas", "https://example.com"));

  const sourcemeta::core::URI input{"http://localhost:8000/schemas/test.json"};
  const auto result{configuration.resolve_path(input)};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), "/tmp/schemas/test.json");
}

TEST(Configuration_resolve_path, url_with_port_wrong_port) {
  sourcemeta::one::Configuration configuration;
  configuration.url = "http://localhost:8000";
  configuration.entries.emplace(
      "schemas", make_collection("/tmp/schemas", "https://example.com"));

  const sourcemeta::core::URI input{"http://localhost:9000/schemas/test.json"};
  const auto result{configuration.resolve_path(input)};
  EXPECT_FALSE(result.has_value());
}

TEST(Configuration_resolve_path, multiple_collections_first_match) {
  sourcemeta::one::Configuration configuration;
  configuration.url = "https://sourcemeta.com";
  configuration.entries.emplace(
      "alpha", make_collection("/tmp/alpha", "https://alpha.com"));
  configuration.entries.emplace(
      "beta", make_collection("/tmp/beta", "https://beta.com"));
  configuration.entries.emplace(
      "gamma", make_collection("/tmp/gamma", "https://gamma.com"));

  const sourcemeta::core::URI input{"/alpha/test.json"};
  const auto result{configuration.resolve_path(input)};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), "/tmp/alpha/test.json");
}

TEST(Configuration_resolve_path, multiple_collections_no_match) {
  sourcemeta::one::Configuration configuration;
  configuration.url = "https://sourcemeta.com";
  configuration.entries.emplace(
      "alpha", make_collection("/tmp/alpha", "https://alpha.com"));
  configuration.entries.emplace(
      "beta", make_collection("/tmp/beta", "https://beta.com"));

  const sourcemeta::core::URI input{"/delta/test.json"};
  const auto result{configuration.resolve_path(input)};
  EXPECT_FALSE(result.has_value());
}

TEST(Configuration_resolve_path, multiple_collections_similar_names) {
  sourcemeta::one::Configuration configuration;
  configuration.url = "https://sourcemeta.com";
  configuration.entries.emplace(
      "schemas", make_collection("/tmp/schemas", "https://example.com"));
  configuration.entries.emplace(
      "schemas-v2",
      make_collection("/tmp/schemas-v2", "https://v2.example.com"));

  const sourcemeta::core::URI input{"/schemas-v2/test.json"};
  const auto result{configuration.resolve_path(input)};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), "/tmp/schemas-v2/test.json");
}

TEST(Configuration_resolve_path, multiple_collections_similar_names_shorter) {
  sourcemeta::one::Configuration configuration;
  configuration.url = "https://sourcemeta.com";
  configuration.entries.emplace(
      "schemas", make_collection("/tmp/schemas", "https://example.com"));
  configuration.entries.emplace(
      "schemas-v2",
      make_collection("/tmp/schemas-v2", "https://v2.example.com"));

  const sourcemeta::core::URI input{"/schemas/test.json"};
  const auto result{configuration.resolve_path(input)};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), "/tmp/schemas/test.json");
}

TEST(Configuration_resolve_path, nested_collection_deeper_path) {
  sourcemeta::one::Configuration configuration;
  configuration.url = "https://sourcemeta.com";
  configuration.entries.emplace(
      "a/b/c", make_collection("/tmp/deep", "https://example.com"));

  const sourcemeta::core::URI input{"/a/b/c/x/y.json"};
  const auto result{configuration.resolve_path(input)};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), "/tmp/deep/x/y.json");
}

TEST(Configuration_resolve_path, nested_collection_exact_match) {
  sourcemeta::one::Configuration configuration;
  configuration.url = "https://sourcemeta.com";
  configuration.entries.emplace(
      "a/b/c", make_collection("/tmp/deep", "https://example.com"));

  const sourcemeta::core::URI input{"/a/b/c"};
  const auto result{configuration.resolve_path(input)};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), "/tmp/deep/");
}

TEST(Configuration_resolve_path, nested_collection_partial_key_no_match) {
  sourcemeta::one::Configuration configuration;
  configuration.url = "https://sourcemeta.com";
  configuration.entries.emplace(
      "a/b/c", make_collection("/tmp/deep", "https://example.com"));

  const sourcemeta::core::URI input{"/a/b/other.json"};
  const auto result{configuration.resolve_path(input)};
  EXPECT_FALSE(result.has_value());
}

TEST(Configuration_resolve_path, nested_and_flat_collections_nested_wins) {
  sourcemeta::one::Configuration configuration;
  configuration.url = "https://sourcemeta.com";
  configuration.entries.emplace(
      "docs/api", make_collection("/tmp/api", "https://api.example.com"));
  sourcemeta::one::Configuration::Page page;
  page.title = "Docs";
  configuration.entries.emplace("docs", std::move(page));

  const sourcemeta::core::URI input{"/docs/api/endpoint.json"};
  const auto result{configuration.resolve_path(input)};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), "/tmp/api/endpoint.json");
}

TEST(Configuration_resolve_path, nested_page_does_not_shadow_collection) {
  sourcemeta::one::Configuration configuration;
  configuration.url = "https://sourcemeta.com";
  sourcemeta::one::Configuration::Page page;
  page.title = "Docs";
  configuration.entries.emplace("docs", std::move(page));
  configuration.entries.emplace(
      "schemas", make_collection("/tmp/schemas", "https://example.com"));

  const sourcemeta::core::URI input{"/docs/something"};
  const auto result{configuration.resolve_path(input)};
  EXPECT_FALSE(result.has_value());
}

TEST(Configuration_resolve_path,
     multiple_collections_absolute_uri_matches_correct) {
  sourcemeta::one::Configuration configuration;
  configuration.url = "https://sourcemeta.com";
  configuration.entries.emplace(
      "alpha", make_collection("/tmp/alpha", "https://alpha.com"));
  configuration.entries.emplace(
      "beta", make_collection("/tmp/beta", "https://beta.com"));
  configuration.entries.emplace(
      "gamma", make_collection("/tmp/gamma", "https://gamma.com"));

  const sourcemeta::core::URI input{
      "https://sourcemeta.com/gamma/deep/file.json"};
  const auto result{configuration.resolve_path(input)};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), "/tmp/gamma/deep/file.json");
}

TEST(Configuration_resolve_path, longest_prefix_wins_over_shorter) {
  sourcemeta::one::Configuration configuration;
  configuration.url = "https://sourcemeta.com";
  configuration.entries.emplace(
      "schemas", make_collection("/tmp/schemas", "https://example.com"));
  configuration.entries.emplace(
      "schemas/v2", make_collection("/tmp/schemas-v2", "https://example.com"));

  const sourcemeta::core::URI input{"/schemas/v2/test.json"};
  const auto result{configuration.resolve_path(input)};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), "/tmp/schemas-v2/test.json");
}

TEST(Configuration_resolve_path, path_traversal_dot_dot_rejected) {
  sourcemeta::one::Configuration configuration;
  configuration.url = "https://sourcemeta.com";
  configuration.entries.emplace(
      "schemas", make_collection("/tmp/schemas", "https://example.com"));

  const sourcemeta::core::URI input{"/schemas/../other.json"};
  const auto result{configuration.resolve_path(input)};
  EXPECT_FALSE(result.has_value());
}
