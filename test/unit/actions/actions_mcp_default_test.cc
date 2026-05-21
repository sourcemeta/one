#include <gtest/gtest.h>

#include <sourcemeta/core/json.h>

#include <sourcemeta/one/actions.h>

namespace {

class TestAction final : public sourcemeta::one::RouterAction {
public:
  TestAction(const std::filesystem::path &base,
             const std::string_view base_path)
      : sourcemeta::one::RouterAction{base, base_path,
                                      "http://localhost:8000"} {}

  auto rest(const std::span<std::string_view>, sourcemeta::one::HTTPRequest &,
            sourcemeta::one::HTTPResponse &) -> void override {}
};

} // namespace

TEST(Actions_mcp_default, returns_method_not_found_with_integer_id) {
  const std::filesystem::path base{"/output"};
  TestAction action{base, ""};
  const auto envelope{sourcemeta::core::parse_json(
      R"({ "jsonrpc": "2.0", "id": 1, "method": "tools/call" })")};
  const auto response{action.mcp(envelope)};
  const auto expected{sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "id": 1,
    "error": {
      "code": -32601,
      "message": "Method not found"
    }
  })JSON")};
  EXPECT_EQ(response, expected);
}

TEST(Actions_mcp_default, returns_method_not_found_with_string_id) {
  const std::filesystem::path base{"/output"};
  TestAction action{base, ""};
  const auto envelope{sourcemeta::core::parse_json(
      R"({ "jsonrpc": "2.0", "id": "abc", "method": "tools/call" })")};
  const auto response{action.mcp(envelope)};
  const auto expected{sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "id": "abc",
    "error": {
      "code": -32601,
      "message": "Method not found"
    }
  })JSON")};
  EXPECT_EQ(response, expected);
}

TEST(Actions_mcp_default, missing_id_uses_null) {
  const std::filesystem::path base{"/output"};
  TestAction action{base, ""};
  const auto envelope{sourcemeta::core::parse_json(
      R"({ "jsonrpc": "2.0", "method": "tools/call" })")};
  const auto response{action.mcp(envelope)};
  const auto expected{sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "id": null,
    "error": {
      "code": -32601,
      "message": "Method not found"
    }
  })JSON")};
  EXPECT_EQ(response, expected);
}
