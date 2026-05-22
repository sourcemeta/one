#include <sourcemeta/one/mcp.h>

#include <sourcemeta/core/json.h>

#include <gtest/gtest.h>

TEST(MCP, tool_success_with_object_result) {
  const auto identifier{sourcemeta::core::JSON{1}};
  auto result{sourcemeta::core::JSON::make_object()};
  result.assign("foo", sourcemeta::core::JSON{42});
  const auto envelope{sourcemeta::one::mcp_make_tool_success(
      sourcemeta::one::MCPProtocolVersion::V_2025_11_25, identifier,
      std::move(result))};
  const auto expected{sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "id": 1,
    "result": {
      "content": [
        { "type": "text", "text": "{\n  \"foo\": 42\n}" }
      ],
      "structuredContent": { "foo": 42 },
      "isError": false
    }
  })JSON")};
  EXPECT_EQ(envelope, expected);
}

TEST(MCP, tool_success_with_array_result) {
  const auto identifier{sourcemeta::core::JSON{"abc"}};
  const auto envelope{sourcemeta::one::mcp_make_tool_success(
      sourcemeta::one::MCPProtocolVersion::V_2025_11_25, identifier,
      sourcemeta::core::parse_json(R"([ 1, 2, 3 ])"))};
  const auto expected{sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "id": "abc",
    "result": {
      "content": [
        { "type": "text", "text": "[ 1, 2, 3 ]" }
      ],
      "structuredContent": [ 1, 2, 3 ],
      "isError": false
    }
  })JSON")};
  EXPECT_EQ(envelope, expected);
}

TEST(MCP, tool_success_with_null_id) {
  const auto envelope{sourcemeta::one::mcp_make_tool_success(
      sourcemeta::one::MCPProtocolVersion::V_2025_11_25,
      sourcemeta::core::JSON{nullptr}, sourcemeta::core::JSON::make_object())};
  const auto expected{sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "id": null,
    "result": {
      "content": [
        { "type": "text", "text": "{}" }
      ],
      "structuredContent": {},
      "isError": false
    }
  })JSON")};
  EXPECT_EQ(envelope, expected);
}

TEST(MCP, tool_error_with_message) {
  const auto identifier{sourcemeta::core::JSON{7}};
  const auto envelope{
      sourcemeta::one::mcp_make_tool_error(identifier, "Schema not found")};
  const auto expected{sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "id": 7,
    "result": {
      "content": [
        { "type": "text", "text": "Schema not found" }
      ],
      "isError": true
    }
  })JSON")};
  EXPECT_EQ(envelope, expected);
}

TEST(MCP, tool_error_with_string_id) {
  const auto identifier{sourcemeta::core::JSON{"req-1"}};
  const auto envelope{
      sourcemeta::one::mcp_make_tool_error(identifier, "Invalid input")};
  const auto expected{sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "id": "req-1",
    "result": {
      "content": [
        { "type": "text", "text": "Invalid input" }
      ],
      "isError": true
    }
  })JSON")};
  EXPECT_EQ(envelope, expected);
}

TEST(MCP, tool_error_with_null_id) {
  const auto envelope{sourcemeta::one::mcp_make_tool_error(
      sourcemeta::core::JSON{nullptr}, "Boom")};
  const auto expected{sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "id": null,
    "result": {
      "content": [
        { "type": "text", "text": "Boom" }
      ],
      "isError": true
    }
  })JSON")};
  EXPECT_EQ(envelope, expected);
}
