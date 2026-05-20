sourcemeta_googletest(NAMESPACE sourcemeta PROJECT one NAME mcp
  SOURCES mcp_test.cc)

target_link_libraries(sourcemeta_one_mcp_unit
  PRIVATE sourcemeta::one::mcp)
target_link_libraries(sourcemeta_one_mcp_unit
  PRIVATE sourcemeta::core::jsonrpc)
target_link_libraries(sourcemeta_one_mcp_unit
  PRIVATE sourcemeta::core::json)
