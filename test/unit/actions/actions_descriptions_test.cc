#include <gtest/gtest.h>

#include <sourcemeta/core/uritemplate.h>

#include <sourcemeta/one/actions.h>

#include <limits>      // std::numeric_limits
#include <string_view> // std::string_view

TEST(Actions_description, default_v1) {
  EXPECT_EQ(sourcemeta::one::ActionDispatcher::description(
                sourcemeta::one::ACTION_TYPE_DEFAULT_V1),
            std::string_view{"Default fallback action for unmatched URIs"});
}

TEST(Actions_description, health_check_v1) {
  EXPECT_EQ(sourcemeta::one::ActionDispatcher::description(
                sourcemeta::one::ACTION_TYPE_HEALTH_CHECK_V1),
            std::string_view{"Report the server's health status"});
}

TEST(Actions_description, not_found_v1) {
  EXPECT_EQ(sourcemeta::one::ActionDispatcher::description(
                sourcemeta::one::ACTION_TYPE_NOT_FOUND_V1),
            std::string_view{"Return a 404 Not Found response"});
}

TEST(Actions_description, schema_artifact_v1) {
  EXPECT_EQ(sourcemeta::one::ActionDispatcher::description(
                sourcemeta::one::ACTION_TYPE_SCHEMA_ARTIFACT_V1),
            std::string_view{"Look up a precomputed artifact about a "
                             "specific schema by its absolute URI"});
}

TEST(Actions_description, explorer_artifact_v1) {
  EXPECT_EQ(
      sourcemeta::one::ActionDispatcher::description(
          sourcemeta::one::ACTION_TYPE_EXPLORER_ARTIFACT_V1),
      std::string_view{"Read a navigation artifact for browsing schemas"});
}

TEST(Actions_description, jsonschema_evaluate_v1) {
  EXPECT_EQ(sourcemeta::one::ActionDispatcher::description(
                sourcemeta::one::ACTION_TYPE_JSONSCHEMA_EVALUATE_V1),
            std::string_view{"Validate a JSON instance against a schema and "
                             "return whether it is valid plus any errors"});
}

TEST(Actions_description, jsonschema_trace_v1) {
  EXPECT_EQ(sourcemeta::one::ActionDispatcher::description(
                sourcemeta::one::ACTION_TYPE_JSONSCHEMA_TRACE_V1),
            std::string_view{
                "Validate a JSON instance against a schema and return "
                "detailed information about every step of the evaluation "
                "process"});
}

TEST(Actions_description, schema_search_v1) {
  EXPECT_EQ(sourcemeta::one::ActionDispatcher::description(
                sourcemeta::one::ACTION_TYPE_SCHEMA_SEARCH_V1),
            std::string_view{"Search for schemas by query term"});
}

TEST(Actions_description, serve_static_v1) {
  EXPECT_EQ(sourcemeta::one::ActionDispatcher::description(
                sourcemeta::one::ACTION_TYPE_SERVE_STATIC_V1),
            std::string_view{"Serve a static asset bundled with the server"});
}

TEST(Actions_description, mcp_v1) {
  EXPECT_EQ(
      sourcemeta::one::ActionDispatcher::description(
          sourcemeta::one::ACTION_TYPE_MCP_V1),
      std::string_view{"Handle Model Context Protocol JSON-RPC requests"});
}

TEST(Actions_description, every_action_has_a_non_empty_description) {
  for (sourcemeta::core::URITemplateRouter::Identifier context{0};
       context < sourcemeta::one::ACTION_TYPE_COUNT; ++context) {
    EXPECT_FALSE(
        sourcemeta::one::ActionDispatcher::description(context).empty())
        << "Action context " << context << " has an empty description";
  }
}

TEST(Actions_description, out_of_range_returns_empty) {
  EXPECT_TRUE(sourcemeta::one::ActionDispatcher::description(
                  sourcemeta::one::ACTION_TYPE_COUNT)
                  .empty());
  EXPECT_TRUE(sourcemeta::one::ActionDispatcher::description(
                  std::numeric_limits<
                      sourcemeta::core::URITemplateRouter::Identifier>::max())
                  .empty());
}
