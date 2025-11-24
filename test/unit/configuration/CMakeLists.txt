sourcemeta_googletest(NAMESPACE sourcemeta PROJECT one NAME configuration
  SOURCES configuration_read_test.cc configuration_test.cc)

target_link_libraries(sourcemeta_one_configuration_unit
  PRIVATE sourcemeta::one::configuration)
target_compile_definitions(sourcemeta_one_configuration_unit
  PRIVATE STUB_DIRECTORY="${CMAKE_CURRENT_SOURCE_DIR}/stub")
target_compile_definitions(sourcemeta_one_configuration_unit
  PRIVATE COLLECTIONS_DIRECTORY="${CMAKE_CURRENT_SOURCE_DIR}/stub/collections")
