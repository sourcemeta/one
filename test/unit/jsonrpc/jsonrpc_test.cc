#include <sourcemeta/one/jsonrpc.h>

#include <sourcemeta/core/json.h>

#include <gtest/gtest.h>

#include <cstdint> // std::int64_t

TEST(JSONRPC, code_constants) {
  EXPECT_EQ(sourcemeta::one::JSONRPC_CODE_PARSE,
            static_cast<std::int64_t>(-32700));
  EXPECT_EQ(sourcemeta::one::JSONRPC_CODE_INVALID_REQUEST,
            static_cast<std::int64_t>(-32600));
  EXPECT_EQ(sourcemeta::one::JSONRPC_CODE_METHOD_NOT_FOUND,
            static_cast<std::int64_t>(-32601));
  EXPECT_EQ(sourcemeta::one::JSONRPC_CODE_INVALID_PARAMS,
            static_cast<std::int64_t>(-32602));
  EXPECT_EQ(sourcemeta::one::JSONRPC_CODE_INTERNAL,
            static_cast<std::int64_t>(-32603));
  EXPECT_EQ(sourcemeta::one::JSONRPC_CODE_SERVER_ERROR_MIN,
            static_cast<std::int64_t>(-32099));
  EXPECT_EQ(sourcemeta::one::JSONRPC_CODE_SERVER_ERROR_MAX,
            static_cast<std::int64_t>(-32000));
}

TEST(JSONRPC, is_server_error_lower_bound) {
  EXPECT_TRUE(sourcemeta::one::jsonrpc_is_server_error(-32099));
}

TEST(JSONRPC, is_server_error_upper_bound) {
  EXPECT_TRUE(sourcemeta::one::jsonrpc_is_server_error(-32000));
}

TEST(JSONRPC, is_server_error_inside_range) {
  EXPECT_TRUE(sourcemeta::one::jsonrpc_is_server_error(-32050));
}

TEST(JSONRPC, is_server_error_below_range) {
  EXPECT_FALSE(sourcemeta::one::jsonrpc_is_server_error(-32100));
}

TEST(JSONRPC, is_server_error_above_range) {
  EXPECT_FALSE(sourcemeta::one::jsonrpc_is_server_error(-31999));
}

TEST(JSONRPC, is_server_error_predefined_internal) {
  EXPECT_FALSE(sourcemeta::one::jsonrpc_is_server_error(-32603));
}

TEST(JSONRPC, is_server_error_positive_application_code) {
  EXPECT_FALSE(sourcemeta::one::jsonrpc_is_server_error(1));
}

TEST(JSONRPC, request_id_integer) {
  const auto request{sourcemeta::core::parse_json(
      R"({ "jsonrpc": "2.0", "id": 7, "method": "ping" })")};
  const auto *identifier{sourcemeta::one::jsonrpc_request_id(request)};
  ASSERT_NE(identifier, nullptr);
  EXPECT_TRUE(identifier->is_integer());
  EXPECT_EQ(identifier->to_integer(), 7);
}

TEST(JSONRPC, request_id_string) {
  const auto request{sourcemeta::core::parse_json(
      R"({ "jsonrpc": "2.0", "id": "abc", "method": "ping" })")};
  const auto *identifier{sourcemeta::one::jsonrpc_request_id(request)};
  ASSERT_NE(identifier, nullptr);
  EXPECT_TRUE(identifier->is_string());
  EXPECT_EQ(identifier->to_string(), "abc");
}

TEST(JSONRPC, request_id_missing) {
  const auto request{sourcemeta::core::parse_json(
      R"({ "jsonrpc": "2.0", "method": "ping" })")};
  EXPECT_EQ(sourcemeta::one::jsonrpc_request_id(request), nullptr);
}

TEST(JSONRPC, request_id_null) {
  const auto request{sourcemeta::core::parse_json(
      R"({ "jsonrpc": "2.0", "id": null, "method": "ping" })")};
  EXPECT_EQ(sourcemeta::one::jsonrpc_request_id(request), nullptr);
}

TEST(JSONRPC, request_id_floating_point) {
  const auto request{sourcemeta::core::parse_json(
      R"({ "jsonrpc": "2.0", "id": 1.5, "method": "ping" })")};
  EXPECT_EQ(sourcemeta::one::jsonrpc_request_id(request), nullptr);
}

TEST(JSONRPC, request_id_array) {
  const auto request{sourcemeta::core::parse_json(R"([ 1, 2, 3 ])")};
  EXPECT_EQ(sourcemeta::one::jsonrpc_request_id(request), nullptr);
}

TEST(JSONRPC, request_id_primitive) {
  const auto request{sourcemeta::core::parse_json(R"(42)")};
  EXPECT_EQ(sourcemeta::one::jsonrpc_request_id(request), nullptr);
}

TEST(JSONRPC, is_request_valid_integer_id) {
  const auto request{sourcemeta::core::parse_json(
      R"({ "jsonrpc": "2.0", "id": 1, "method": "ping" })")};
  EXPECT_TRUE(sourcemeta::one::jsonrpc_is_request(request));
}

TEST(JSONRPC, is_request_valid_string_id) {
  const auto request{sourcemeta::core::parse_json(
      R"({ "jsonrpc": "2.0", "id": "x", "method": "ping" })")};
  EXPECT_TRUE(sourcemeta::one::jsonrpc_is_request(request));
}

TEST(JSONRPC, is_request_missing_id) {
  const auto request{sourcemeta::core::parse_json(
      R"({ "jsonrpc": "2.0", "method": "ping" })")};
  EXPECT_FALSE(sourcemeta::one::jsonrpc_is_request(request));
}

TEST(JSONRPC, is_request_null_id) {
  const auto request{sourcemeta::core::parse_json(
      R"({ "jsonrpc": "2.0", "id": null, "method": "ping" })")};
  EXPECT_FALSE(sourcemeta::one::jsonrpc_is_request(request));
}

TEST(JSONRPC, is_request_missing_method) {
  const auto request{
      sourcemeta::core::parse_json(R"({ "jsonrpc": "2.0", "id": 1 })")};
  EXPECT_FALSE(sourcemeta::one::jsonrpc_is_request(request));
}

TEST(JSONRPC, is_request_non_string_method) {
  const auto request{sourcemeta::core::parse_json(
      R"({ "jsonrpc": "2.0", "id": 1, "method": 5 })")};
  EXPECT_FALSE(sourcemeta::one::jsonrpc_is_request(request));
}

TEST(JSONRPC, is_request_missing_jsonrpc) {
  const auto request{
      sourcemeta::core::parse_json(R"({ "id": 1, "method": "ping" })")};
  EXPECT_FALSE(sourcemeta::one::jsonrpc_is_request(request));
}

TEST(JSONRPC, is_request_wrong_jsonrpc_version) {
  const auto request{sourcemeta::core::parse_json(
      R"({ "jsonrpc": "1.0", "id": 1, "method": "ping" })")};
  EXPECT_FALSE(sourcemeta::one::jsonrpc_is_request(request));
}

TEST(JSONRPC, is_request_jsonrpc_not_string) {
  const auto request{sourcemeta::core::parse_json(
      R"({ "jsonrpc": 2, "id": 1, "method": "ping" })")};
  EXPECT_FALSE(sourcemeta::one::jsonrpc_is_request(request));
}

TEST(JSONRPC, is_request_array) {
  const auto request{sourcemeta::core::parse_json(R"([ 1, 2 ])")};
  EXPECT_FALSE(sourcemeta::one::jsonrpc_is_request(request));
}

TEST(JSONRPC, is_request_primitive) {
  const auto request{sourcemeta::core::parse_json(R"("hello")")};
  EXPECT_FALSE(sourcemeta::one::jsonrpc_is_request(request));
}

TEST(JSONRPC, is_notification_valid) {
  const auto request{sourcemeta::core::parse_json(
      R"({ "jsonrpc": "2.0", "method": "notifications/initialized" })")};
  EXPECT_TRUE(sourcemeta::one::jsonrpc_is_notification(request));
}

TEST(JSONRPC, is_notification_with_id) {
  const auto request{sourcemeta::core::parse_json(
      R"({ "jsonrpc": "2.0", "id": 1, "method": "ping" })")};
  EXPECT_FALSE(sourcemeta::one::jsonrpc_is_notification(request));
}

TEST(JSONRPC, is_notification_with_null_id) {
  const auto request{sourcemeta::core::parse_json(
      R"({ "jsonrpc": "2.0", "id": null, "method": "ping" })")};
  EXPECT_FALSE(sourcemeta::one::jsonrpc_is_notification(request));
}

TEST(JSONRPC, is_notification_missing_method) {
  const auto request{sourcemeta::core::parse_json(R"({ "jsonrpc": "2.0" })")};
  EXPECT_FALSE(sourcemeta::one::jsonrpc_is_notification(request));
}

TEST(JSONRPC, is_notification_non_string_method) {
  const auto request{
      sourcemeta::core::parse_json(R"({ "jsonrpc": "2.0", "method": 5 })")};
  EXPECT_FALSE(sourcemeta::one::jsonrpc_is_notification(request));
}

TEST(JSONRPC, is_notification_missing_jsonrpc) {
  const auto request{sourcemeta::core::parse_json(R"({ "method": "ping" })")};
  EXPECT_FALSE(sourcemeta::one::jsonrpc_is_notification(request));
}

TEST(JSONRPC, is_notification_wrong_jsonrpc_version) {
  const auto request{sourcemeta::core::parse_json(
      R"({ "jsonrpc": "1.0", "method": "ping" })")};
  EXPECT_FALSE(sourcemeta::one::jsonrpc_is_notification(request));
}

TEST(JSONRPC, is_notification_array) {
  const auto request{sourcemeta::core::parse_json(R"([ 1, 2 ])")};
  EXPECT_FALSE(sourcemeta::one::jsonrpc_is_notification(request));
}

TEST(JSONRPC, method_present) {
  const auto request{sourcemeta::core::parse_json(
      R"({ "jsonrpc": "2.0", "id": 1, "method": "ping" })")};
  EXPECT_EQ(sourcemeta::one::jsonrpc_method(request), "ping");
}

TEST(JSONRPC, method_missing) {
  const auto request{
      sourcemeta::core::parse_json(R"({ "jsonrpc": "2.0", "id": 1 })")};
  EXPECT_EQ(sourcemeta::one::jsonrpc_method(request), "");
}

TEST(JSONRPC, method_non_string) {
  const auto request{sourcemeta::core::parse_json(
      R"({ "jsonrpc": "2.0", "id": 1, "method": 5 })")};
  EXPECT_EQ(sourcemeta::one::jsonrpc_method(request), "");
}

TEST(JSONRPC, method_on_non_object) {
  const auto request{sourcemeta::core::parse_json(R"([ 1, 2 ])")};
  EXPECT_EQ(sourcemeta::one::jsonrpc_method(request), "");
}

TEST(JSONRPC, params_object) {
  const auto request{sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "id": 1,
    "method": "ping",
    "params": { "uri": "http://example.com" }
  })JSON")};
  const auto *params{sourcemeta::one::jsonrpc_params(request)};
  ASSERT_NE(params, nullptr);
  EXPECT_TRUE(params->is_object());
  EXPECT_EQ(params->at("uri").to_string(), "http://example.com");
}

TEST(JSONRPC, params_array) {
  const auto request{sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "id": 1,
    "method": "subtract",
    "params": [ 42, 23 ]
  })JSON")};
  const auto *params{sourcemeta::one::jsonrpc_params(request)};
  ASSERT_NE(params, nullptr);
  EXPECT_TRUE(params->is_array());
  EXPECT_EQ(params->size(), 2);
}

TEST(JSONRPC, params_missing) {
  const auto request{sourcemeta::core::parse_json(
      R"({ "jsonrpc": "2.0", "id": 1, "method": "ping" })")};
  EXPECT_EQ(sourcemeta::one::jsonrpc_params(request), nullptr);
}

TEST(JSONRPC, params_primitive) {
  const auto request{sourcemeta::core::parse_json(
      R"({ "jsonrpc": "2.0", "id": 1, "method": "ping", "params": "x" })")};
  EXPECT_EQ(sourcemeta::one::jsonrpc_params(request), nullptr);
}

TEST(JSONRPC, params_null) {
  const auto request{sourcemeta::core::parse_json(
      R"({ "jsonrpc": "2.0", "id": 1, "method": "ping", "params": null })")};
  EXPECT_EQ(sourcemeta::one::jsonrpc_params(request), nullptr);
}

TEST(JSONRPC, params_on_non_object) {
  const auto request{sourcemeta::core::parse_json(R"([ 1, 2 ])")};
  EXPECT_EQ(sourcemeta::one::jsonrpc_params(request), nullptr);
}

TEST(JSONRPC, make_success_empty) {
  const auto identifier{sourcemeta::core::JSON{1}};
  const auto envelope{sourcemeta::one::jsonrpc_make_success_empty(identifier)};
  const auto expected{sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "id": 1,
    "result": {}
  })JSON")};
  EXPECT_EQ(envelope, expected);
}

TEST(JSONRPC, make_success_empty_object_result) {
  const auto identifier{sourcemeta::core::JSON{1}};
  const auto envelope{sourcemeta::one::jsonrpc_make_success(
      identifier, sourcemeta::core::JSON::make_object())};
  const auto expected{sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "id": 1,
    "result": {}
  })JSON")};
  EXPECT_EQ(envelope, expected);
}

TEST(JSONRPC, make_success_populated_object_result) {
  const auto identifier{sourcemeta::core::JSON{"abc"}};
  auto result{sourcemeta::core::JSON::make_object()};
  result.assign("foo", sourcemeta::core::JSON{42});
  const auto envelope{
      sourcemeta::one::jsonrpc_make_success(identifier, std::move(result))};
  const auto expected{sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "id": "abc",
    "result": { "foo": 42 }
  })JSON")};
  EXPECT_EQ(envelope, expected);
}

TEST(JSONRPC, make_success_array_result) {
  const auto identifier{sourcemeta::core::JSON{2}};
  const auto envelope{sourcemeta::one::jsonrpc_make_success(
      identifier, sourcemeta::core::parse_json(R"([ 1, 2, 3 ])"))};
  const auto expected{sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "id": 2,
    "result": [ 1, 2, 3 ]
  })JSON")};
  EXPECT_EQ(envelope, expected);
}

TEST(JSONRPC, make_error_with_id_no_data) {
  const auto identifier{sourcemeta::core::JSON{1}};
  const auto envelope{
      sourcemeta::one::jsonrpc_make_error(&identifier, -32000, "Server error")};
  const auto expected{sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "id": 1,
    "error": {
      "code": -32000,
      "message": "Server error"
    }
  })JSON")};
  EXPECT_EQ(envelope, expected);
}

TEST(JSONRPC, make_error_with_id_and_data) {
  const auto identifier{sourcemeta::core::JSON{"req-1"}};
  const auto envelope{sourcemeta::one::jsonrpc_make_error(
      &identifier, -32000, "Server error",
      sourcemeta::core::JSON{"more details"})};
  const auto expected{sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "id": "req-1",
    "error": {
      "code": -32000,
      "message": "Server error",
      "data": "more details"
    }
  })JSON")};
  EXPECT_EQ(envelope, expected);
}

TEST(JSONRPC, make_error_without_id) {
  const auto envelope{
      sourcemeta::one::jsonrpc_make_error(nullptr, -32000, "Server error")};
  const auto expected{sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "error": {
      "code": -32000,
      "message": "Server error"
    }
  })JSON")};
  EXPECT_EQ(envelope, expected);
}

TEST(JSONRPC, make_error_without_id_with_object_data) {
  auto data{sourcemeta::core::JSON::make_object()};
  data.assign("hint", sourcemeta::core::JSON{"check input"});
  const auto envelope{sourcemeta::one::jsonrpc_make_error(
      nullptr, -32000, "Server error", std::move(data))};
  const auto expected{sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "error": {
      "code": -32000,
      "message": "Server error",
      "data": { "hint": "check input" }
    }
  })JSON")};
  EXPECT_EQ(envelope, expected);
}

TEST(JSONRPC, make_error_parse) {
  const auto &envelope{sourcemeta::one::jsonrpc_make_error_parse()};
  const auto expected{sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "error": {
      "code": -32700,
      "message": "Parse error"
    }
  })JSON")};
  EXPECT_EQ(envelope, expected);
}

TEST(JSONRPC, make_error_parse_returns_same_reference) {
  const auto *first{&sourcemeta::one::jsonrpc_make_error_parse()};
  const auto *second{&sourcemeta::one::jsonrpc_make_error_parse()};
  EXPECT_EQ(first, second);
}

TEST(JSONRPC, make_error_invalid_request_with_id) {
  const auto identifier{sourcemeta::core::JSON{1}};
  const auto envelope{
      sourcemeta::one::jsonrpc_make_error_invalid_request(&identifier)};
  const auto expected{sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "id": 1,
    "error": {
      "code": -32600,
      "message": "Invalid Request"
    }
  })JSON")};
  EXPECT_EQ(envelope, expected);
}

TEST(JSONRPC, make_error_invalid_request_without_id) {
  const auto envelope{sourcemeta::one::jsonrpc_make_error_invalid_request()};
  const auto expected{sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "error": {
      "code": -32600,
      "message": "Invalid Request"
    }
  })JSON")};
  EXPECT_EQ(envelope, expected);
}

TEST(JSONRPC, make_error_method_not_found) {
  const auto identifier{sourcemeta::core::JSON{2}};
  const auto envelope{
      sourcemeta::one::jsonrpc_make_error_method_not_found(identifier)};
  const auto expected{sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "id": 2,
    "error": {
      "code": -32601,
      "message": "Method not found"
    }
  })JSON")};
  EXPECT_EQ(envelope, expected);
}

TEST(JSONRPC, make_error_invalid_params) {
  const auto identifier{sourcemeta::core::JSON{"req-7"}};
  const auto envelope{
      sourcemeta::one::jsonrpc_make_error_invalid_params(identifier)};
  const auto expected{sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "id": "req-7",
    "error": {
      "code": -32602,
      "message": "Invalid params"
    }
  })JSON")};
  EXPECT_EQ(envelope, expected);
}

TEST(JSONRPC, make_error_invalid_params_with_data) {
  const auto identifier{sourcemeta::core::JSON{"req-8"}};
  const auto envelope{sourcemeta::one::jsonrpc_make_error_invalid_params(
      identifier, sourcemeta::core::JSON{"abc"})};
  const auto expected{sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "id": "req-8",
    "error": {
      "code": -32602,
      "message": "Invalid params",
      "data": "abc"
    }
  })JSON")};
  EXPECT_EQ(envelope, expected);
}

TEST(JSONRPC, make_error_internal_with_id) {
  const auto identifier{sourcemeta::core::JSON{3}};
  const auto envelope{
      sourcemeta::one::jsonrpc_make_error_internal(&identifier)};
  const auto expected{sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "id": 3,
    "error": {
      "code": -32603,
      "message": "Internal error"
    }
  })JSON")};
  EXPECT_EQ(envelope, expected);
}

TEST(JSONRPC, make_error_internal_without_id) {
  const auto envelope{sourcemeta::one::jsonrpc_make_error_internal()};
  const auto expected{sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "error": {
      "code": -32603,
      "message": "Internal error"
    }
  })JSON")};
  EXPECT_EQ(envelope, expected);
}
