#include <gtest/gtest.h>

#include <sourcemeta/core/json.h>
#include <sourcemeta/core/jsonrpc.h>

#include <sourcemeta/one/actions.h>

namespace {

class TestAction final : public sourcemeta::one::RouterAction {
public:
  TestAction(const std::filesystem::path &base,
             const std::string_view base_path,
             const std::string_view server_uri)
      : sourcemeta::one::RouterAction{base, base_path, server_uri} {}

  auto rest(const std::span<std::string_view>, sourcemeta::one::HTTPRequest &,
            sourcemeta::one::HTTPResponse &) -> void override {}

  auto mcp(const sourcemeta::core::MCPProtocolVersion,
           const sourcemeta::core::JSON &id, const sourcemeta::core::JSON &)
      -> sourcemeta::core::JSON override {
    return sourcemeta::core::jsonrpc_make_error_method_not_found(id);
  }
};

} // namespace

TEST(Actions_uri_to_relative_path, simple) {
  const std::filesystem::path base{"/output"};
  const TestAction action{base, "", "http://localhost:8000"};
  const auto result{
      action.uri_to_relative_path("http://localhost:8000/example/foo#/type")};
  ASSERT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), "example/foo");
}

TEST(Actions_uri_to_relative_path, with_base_path) {
  const std::filesystem::path base{"/output"};
  const TestAction action{base, "/v1/catalog",
                          "http://localhost:8000/v1/catalog"};
  const auto result{action.uri_to_relative_path(
      "http://localhost:8000/v1/catalog/example/foo")};
  ASSERT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), "example/foo");
}

TEST(Actions_uri_to_relative_path, no_path) {
  const std::filesystem::path base{"/output"};
  const TestAction action{base, "", "http://localhost:8000"};
  const auto result{action.uri_to_relative_path("http://localhost:8000")};
  EXPECT_FALSE(result.has_value());
}

TEST(Actions_uri_to_relative_path, normalizes_dot_dot_segments) {
  const std::filesystem::path base{"/output"};
  const TestAction action{base, "", "http://localhost:8000"};
  const auto result{
      action.uri_to_relative_path("http://localhost:8000/foo/../etc/passwd")};
  ASSERT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), "etc/passwd");
}

TEST(Actions_uri_to_relative_path, dot_dot_at_root_does_not_escape) {
  const std::filesystem::path base{"/output"};
  const TestAction action{base, "", "http://localhost:8000"};
  const auto result{
      action.uri_to_relative_path("http://localhost:8000/../../../etc/passwd")};
  ASSERT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), "etc/passwd");
}

TEST(Actions_uri_to_relative_path, rejects_path_outside_base_path) {
  const std::filesystem::path base{"/output"};
  const TestAction action{base, "/v1/catalog",
                          "http://localhost:8000/v1/catalog"};
  const auto result{
      action.uri_to_relative_path("http://localhost:8000/other/foo")};
  EXPECT_FALSE(result.has_value());
}

TEST(Actions_uri_to_relative_path, rejects_external_origin) {
  const std::filesystem::path base{"/output"};
  const TestAction action{base, "", "http://localhost:8000"};
  const auto result{
      action.uri_to_relative_path("http://json-schema.org/draft-07/schema")};
  EXPECT_FALSE(result.has_value());
}

TEST(Actions_uri_to_relative_path, rejects_different_port) {
  const std::filesystem::path base{"/output"};
  const TestAction action{base, "", "http://localhost:8000"};
  const auto result{action.uri_to_relative_path("http://localhost:9000/foo")};
  EXPECT_FALSE(result.has_value());
}

TEST(Actions_uri_to_relative_path, rejects_relative_uri) {
  const std::filesystem::path base{"/output"};
  const TestAction action{base, "", "http://localhost:8000"};
  const auto result{action.uri_to_relative_path("/foo/bar")};
  EXPECT_FALSE(result.has_value());
}

TEST(Actions_uri_to_relative_path, rejects_invalid_uri) {
  const std::filesystem::path base{"/output"};
  const TestAction action{base, "", "http://localhost:8000"};
  const auto result{action.uri_to_relative_path("not a uri at all")};
  EXPECT_FALSE(result.has_value());
}
