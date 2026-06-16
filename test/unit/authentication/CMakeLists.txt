sourcemeta_googletest(NAMESPACE sourcemeta PROJECT one NAME authentication
  SOURCES authentication_test.cc)

target_link_libraries(sourcemeta_one_authentication_unit
  PRIVATE sourcemeta::one::authentication)

target_compile_definitions(sourcemeta_one_authentication_unit
  PRIVATE AUTHENTICATION_TEST_DIRECTORY="${CMAKE_CURRENT_BINARY_DIR}")
