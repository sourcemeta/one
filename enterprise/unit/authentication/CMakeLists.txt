sourcemeta_test(NAMESPACE sourcemeta PROJECT one NAME enterprise_authentication
  SOURCES authentication_test.cc session_test.cc)

target_link_libraries(sourcemeta_one_enterprise_authentication_unit
  PRIVATE sourcemeta::one::authentication)
target_link_libraries(sourcemeta_one_enterprise_authentication_unit
  PRIVATE sourcemeta::core::crypto)
target_link_libraries(sourcemeta_one_enterprise_authentication_unit
  PRIVATE sourcemeta::core::jose)

target_compile_definitions(sourcemeta_one_enterprise_authentication_unit
  PRIVATE AUTHENTICATION_TEST_DIRECTORY="${CMAKE_CURRENT_BINARY_DIR}")
