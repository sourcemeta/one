sourcemeta_library(NAMESPACE sourcemeta PROJECT one NAME jsonrpc)

target_link_libraries(sourcemeta_one_jsonrpc INTERFACE sourcemeta::core::json)
