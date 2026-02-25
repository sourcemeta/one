sourcemeta_googletest(NAMESPACE sourcemeta PROJECT one NAME build
  SOURCES
    build_test_utils.h
    build_adapter_filesystem_test.cc
    build_adapter_filesystem_e2e_test.cc)
target_link_libraries(sourcemeta_one_build_unit PRIVATE sourcemeta::one::build)
target_compile_definitions(sourcemeta_one_build_unit
  PRIVATE TEST_DIRECTORY="${CMAKE_CURRENT_SOURCE_DIR}")
target_compile_definitions(sourcemeta_one_build_unit
  PRIVATE BINARY_DIRECTORY="${CMAKE_CURRENT_BINARY_DIR}")
