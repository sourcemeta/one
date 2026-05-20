sourcemeta_library(NAMESPACE sourcemeta PROJECT one NAME mcp)

target_link_libraries(sourcemeta_one_mcp INTERFACE sourcemeta::core::json)
target_link_libraries(sourcemeta_one_mcp INTERFACE sourcemeta::core::jsonrpc)
