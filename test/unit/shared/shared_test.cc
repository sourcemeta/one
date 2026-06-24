#include <sourcemeta/one/shared.h>

#include <gtest/gtest.h>

#include <string> // std::string

namespace {

auto reduce(const std::string &uri, const std::string &server) -> std::string {
  const auto result{sourcemeta::one::request_registry_path(uri, server)};
  if (!result.has_value()) {
    return "<none>";
  }

  return result.value().generic_string();
}

} // namespace

TEST(Shared_request_registry_path, relative_path_under_the_server) {
  EXPECT_EQ(reduce("http://example.com/private/secret", "http://example.com"),
            "private/secret");
}

TEST(Shared_request_registry_path, the_server_root_reduces_to_empty) {
  EXPECT_EQ(reduce("http://example.com/", "http://example.com"), "");
  EXPECT_EQ(reduce("http://example.com", "http://example.com"), "");
}

TEST(Shared_request_registry_path, the_json_extension_is_stripped) {
  EXPECT_EQ(
      reduce("http://example.com/private/secret.json", "http://example.com"),
      "private/secret");
}

TEST(Shared_request_registry_path, the_json_extension_is_stripped_any_case) {
  EXPECT_EQ(
      reduce("http://example.com/private/secret.JSON", "http://example.com"),
      "private/secret");
}

TEST(Shared_request_registry_path, a_non_json_extension_is_preserved) {
  EXPECT_EQ(
      reduce("http://example.com/private/secret.yaml", "http://example.com"),
      "private/secret.yaml");
}

TEST(Shared_request_registry_path, the_path_is_lowercased) {
  EXPECT_EQ(reduce("http://example.com/Private/Secret", "http://example.com"),
            "private/secret");
}

TEST(Shared_request_registry_path, single_dot_segments_are_resolved) {
  EXPECT_EQ(reduce("http://example.com/private/./secret", "http://example.com"),
            "private/secret");
}

TEST(Shared_request_registry_path, parent_dot_segments_are_resolved) {
  EXPECT_EQ(reduce("http://example.com/public/../private/secret",
                   "http://example.com"),
            "private/secret");
}

TEST(Shared_request_registry_path, percent_encoding_is_decoded) {
  EXPECT_EQ(reduce("http://example.com/private/%73ecret", "http://example.com"),
            "private/secret");
}

TEST(Shared_request_registry_path, an_encoded_slash_is_not_a_separator) {
  EXPECT_EQ(reduce("http://example.com/private%2Fsecret", "http://example.com"),
            "private%2fsecret");
}

TEST(Shared_request_registry_path, a_base_path_is_stripped) {
  EXPECT_EQ(reduce("http://example.com/registry/private/secret",
                   "http://example.com/registry"),
            "private/secret");
}

TEST(Shared_request_registry_path, the_base_path_root_reduces_to_empty) {
  EXPECT_EQ(
      reduce("http://example.com/registry", "http://example.com/registry"), "");
}

TEST(Shared_request_registry_path, a_uri_on_another_host_is_rejected) {
  EXPECT_EQ(reduce("http://elsewhere.com/private/secret", "http://example.com"),
            "<none>");
}

TEST(Shared_request_registry_path, a_same_host_path_outside_the_base_resolves) {
  EXPECT_EQ(reduce("http://example.com/elsewhere/private",
                   "http://example.com/registry"),
            "elsewhere/private");
}

TEST(Shared_request_registry_path, every_axis_compounds) {
  EXPECT_EQ(reduce("http://example.com/registry/Public/../Private/Secret.JSON",
                   "http://example.com/registry"),
            "private/secret");
}
