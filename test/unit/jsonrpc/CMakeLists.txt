sourcemeta_googletest(NAMESPACE sourcemeta PROJECT one NAME jsonrpc
  SOURCES jsonrpc_test.cc)

target_link_libraries(sourcemeta_one_jsonrpc_unit
  PRIVATE sourcemeta::one::jsonrpc)
