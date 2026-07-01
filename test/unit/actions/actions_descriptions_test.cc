#include <sourcemeta/core/test.h>
#include <sourcemeta/core/uritemplate.h>

#include <sourcemeta/one/actions.h>

#include <limits>      // std::numeric_limits
#include <string_view> // std::string_view

TEST(default_v1) {
  EXPECT_EQ(sourcemeta::one::action_description(
                sourcemeta::one::ACTION_TYPE_DEFAULT_V1),
            std::string_view{"Default fallback action for unmatched URIs"});
}

TEST(health_check_v1) {
  EXPECT_EQ(sourcemeta::one::action_description(
                sourcemeta::one::ACTION_TYPE_HEALTH_CHECK_V1),
            std::string_view{"Report the server's health status"});
}

TEST(not_found_v1) {
  EXPECT_EQ(sourcemeta::one::action_description(
                sourcemeta::one::ACTION_TYPE_NOT_FOUND_V1),
            std::string_view{"Return a 404 Not Found response"});
}

TEST(schema_artifact_v1) {
  EXPECT_EQ(sourcemeta::one::action_description(
                sourcemeta::one::ACTION_TYPE_SCHEMA_ARTIFACT_V1),
            std::string_view{"Look up a precomputed artifact about a "
                             "specific schema by its absolute URI"});
}

TEST(explorer_artifact_v1) {
  EXPECT_EQ(
      sourcemeta::one::action_description(
          sourcemeta::one::ACTION_TYPE_EXPLORER_ARTIFACT_V1),
      std::string_view{"Read a navigation artifact for browsing schemas"});
}

TEST(list_directory_v1) {
  EXPECT_EQ(
      sourcemeta::one::action_description(
          sourcemeta::one::ACTION_TYPE_LIST_DIRECTORY_V1),
      std::string_view{"List the contents of a directory in the catalog"});
}

TEST(dependency_tree_v1) {
  EXPECT_EQ(sourcemeta::one::action_description(
                sourcemeta::one::ACTION_TYPE_DEPENDENCY_TREE_V1),
            std::string_view{"Look up the dependency graph of a specific "
                             "schema (incoming or outgoing)"});
}

TEST(jsonschema_evaluate_v1) {
  EXPECT_EQ(sourcemeta::one::action_description(
                sourcemeta::one::ACTION_TYPE_JSONSCHEMA_EVALUATE_V1),
            std::string_view{"Validate a JSON instance against a schema and "
                             "return whether it is valid plus any errors"});
}

TEST(jsonschema_trace_v1) {
  EXPECT_EQ(sourcemeta::one::action_description(
                sourcemeta::one::ACTION_TYPE_JSONSCHEMA_TRACE_V1),
            std::string_view{
                "Validate a JSON instance against a schema and return a "
                "step-by-step trace of the evaluation. The trace can be "
                "substantial, so prefer non-tracing evaluation when only a "
                "valid or invalid verdict is needed and use this when "
                "per-step detail is required"});
}

TEST(get_schema_health_v1) {
  EXPECT_EQ(sourcemeta::one::action_description(
                sourcemeta::one::ACTION_TYPE_GET_SCHEMA_HEALTH_V1),
            std::string_view{"Return the lint health score (0 to 100) and "
                             "any lint findings for a schema in the catalog"});
}

TEST(get_schema_locations_v1) {
  EXPECT_EQ(sourcemeta::one::action_description(
                sourcemeta::one::ACTION_TYPE_GET_SCHEMA_LOCATIONS_V1),
            std::string_view{
                "Return every URI exposed by a schema, including subschemas "
                "and anchors, with their parent and base URI relationships. "
                "The response carries per-pointer metadata and grows with "
                "the schema's pointer count, so substantial payloads are "
                "expected for large schemas"});
}

TEST(get_schema_positions_v1) {
  EXPECT_EQ(sourcemeta::one::action_description(
                sourcemeta::one::ACTION_TYPE_GET_SCHEMA_POSITIONS_V1),
            std::string_view{"Return the source line and column position of "
                             "every JSON Pointer inside a schema"});
}

TEST(get_schema_stats_v1) {
  EXPECT_EQ(sourcemeta::one::action_description(
                sourcemeta::one::ACTION_TYPE_GET_SCHEMA_STATS_V1),
            std::string_view{
                "Return per-vocabulary keyword usage statistics for a "
                "schema in the catalog, identifying which JSON Schema "
                "keywords the schema relies on and how often each appears"});
}

TEST(get_schema_metadata_v1) {
  EXPECT_EQ(sourcemeta::one::action_description(
                sourcemeta::one::ACTION_TYPE_GET_SCHEMA_METADATA_V1),
            std::string_view{
                "Return navigation metadata for a schema in the catalog, "
                "including title, description, dialect, byte size, "
                "dependency count, and breadcrumb path"});
}

TEST(get_schema_dependencies_v1) {
  EXPECT_EQ(sourcemeta::one::action_description(
                sourcemeta::one::ACTION_TYPE_GET_SCHEMA_DEPENDENCIES_V1),
            std::string_view{
                "List the schemas this schema directly or transitively "
                "references, the outgoing edges of the dependency graph"});
}

TEST(get_schema_dependents_v1) {
  EXPECT_EQ(sourcemeta::one::action_description(
                sourcemeta::one::ACTION_TYPE_GET_SCHEMA_DEPENDENTS_V1),
            std::string_view{
                "List the schemas that directly or transitively reference "
                "this schema, the incoming edges of the dependency graph"});
}

TEST(schema_search_v1) {
  EXPECT_EQ(sourcemeta::one::action_description(
                sourcemeta::one::ACTION_TYPE_SCHEMA_SEARCH_V1),
            std::string_view{"Search for schemas by query term"});
}

TEST(serve_static_v1) {
  EXPECT_EQ(sourcemeta::one::action_description(
                sourcemeta::one::ACTION_TYPE_SERVE_STATIC_V1),
            std::string_view{"Serve a static asset bundled with the server"});
}

TEST(mcp_v1) {
  EXPECT_EQ(
      sourcemeta::one::action_description(sourcemeta::one::ACTION_TYPE_MCP_V1),
      std::string_view{"Handle Model Context Protocol JSON-RPC requests"});
}

TEST(every_action_has_a_non_empty_description) {
  for (sourcemeta::core::URITemplateRouter::Identifier context{0};
       context < sourcemeta::one::ACTION_TYPE_COUNT; ++context) {
    EXPECT_FALSE(sourcemeta::one::action_description(context).empty());
  }
}

TEST(out_of_range_returns_empty) {
  EXPECT_TRUE(
      sourcemeta::one::action_description(sourcemeta::one::ACTION_TYPE_COUNT)
          .empty());
  EXPECT_TRUE(sourcemeta::one::action_description(
                  std::numeric_limits<
                      sourcemeta::core::URITemplateRouter::Identifier>::max())
                  .empty());
}
