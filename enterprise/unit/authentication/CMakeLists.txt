sourcemeta_test(NAMESPACE sourcemeta PROJECT one NAME enterprise_authentication
  SOURCES
    authentication_caller_test.cc
    authentication_compile_test.cc
    authentication_governing_test.cc
    authentication_login_test.cc
    authentication_logout_test.cc
    authentication_path_test.cc
    authentication_permits_apikey_test.cc
    authentication_permits_artifact_test.cc
    authentication_permits_jwt_test.cc
    authentication_permits_path_test.cc
    authentication_permits_session_test.cc
    authentication_reference_permitted_test.cc
    authentication_seal_test.cc
    authentication_sign_in_test.cc
    authentication_views_test.cc
    authentication_visible_test.cc)

target_link_libraries(sourcemeta_one_enterprise_authentication_unit
  PRIVATE sourcemeta::one::authentication)
target_link_libraries(sourcemeta_one_enterprise_authentication_unit
  PRIVATE sourcemeta::core::crypto)
target_link_libraries(sourcemeta_one_enterprise_authentication_unit
  PRIVATE sourcemeta::core::jose)
target_link_libraries(sourcemeta_one_enterprise_authentication_unit
  PRIVATE sourcemeta::core::http)
target_link_libraries(sourcemeta_one_enterprise_authentication_unit
  PRIVATE sourcemeta::core::uri)

target_compile_definitions(sourcemeta_one_enterprise_authentication_unit
  PRIVATE AUTHENTICATION_TEST_DIRECTORY="${CMAKE_CURRENT_BINARY_DIR}")
