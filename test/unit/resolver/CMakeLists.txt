sourcemeta_googletest(NAMESPACE sourcemeta PROJECT one NAME resolver
  SOURCES resolver_test.cc)

target_link_libraries(sourcemeta_one_resolver_unit
  PRIVATE sourcemeta::one::configuration)
target_link_libraries(sourcemeta_one_resolver_unit
  PRIVATE sourcemeta::one::resolver)
target_compile_definitions(sourcemeta_one_resolver_unit
  PRIVATE SCHEMAS_PATH="${CMAKE_CURRENT_SOURCE_DIR}/schemas")
target_compile_definitions(sourcemeta_one_resolver_unit
  PRIVATE CONFIGURATION_PATH="${CMAKE_CURRENT_SOURCE_DIR}/one.json")
