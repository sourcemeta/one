sourcemeta_googletest(NAMESPACE sourcemeta PROJECT one NAME search
  SOURCES search_build_test.cc search_query_test.cc search_view_test.cc)

target_link_libraries(sourcemeta_one_search_unit
  PRIVATE sourcemeta::one::search)
target_link_libraries(sourcemeta_one_search_unit
  PRIVATE sourcemeta::one::metapack)

target_compile_definitions(sourcemeta_one_search_unit
  PRIVATE SEARCH_TEST_DIRECTORY="${CMAKE_CURRENT_BINARY_DIR}")
