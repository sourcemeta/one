sourcemeta_test(NAMESPACE sourcemeta PROJECT one NAME http
  SOURCES http_helpers_test.cc)

target_link_libraries(sourcemeta_one_http_unit
  PRIVATE sourcemeta::one::http)
