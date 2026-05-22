sourcemeta_library(NAMESPACE sourcemeta PROJECT one NAME mcp
  SOURCES mcp.cc)

target_link_libraries(sourcemeta_one_mcp PUBLIC sourcemeta::core::json)
target_link_libraries(sourcemeta_one_mcp PUBLIC sourcemeta::core::jsonrpc)
