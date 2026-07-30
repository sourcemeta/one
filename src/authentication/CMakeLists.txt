if(ONE_ENTERPRISE)
  sourcemeta_library(NAMESPACE sourcemeta PROJECT one NAME authentication
    PRIVATE_HEADERS error.h
    SOURCES
      path.cc
      "${PROJECT_SOURCE_DIR}/enterprise/authentication/authentication.cc"
      "${PROJECT_SOURCE_DIR}/enterprise/authentication/authentication_save.cc"
      "${PROJECT_SOURCE_DIR}/enterprise/authentication/session.cc")
  target_compile_definitions(sourcemeta_one_authentication PUBLIC SOURCEMETA_ONE_ENTERPRISE)
  target_link_libraries(sourcemeta_one_authentication PRIVATE sourcemeta::core::crypto)
  target_link_libraries(sourcemeta_one_authentication PRIVATE sourcemeta::core::http)
  target_link_libraries(sourcemeta_one_authentication PRIVATE sourcemeta::core::oidc)
  # A secret too short to be worth signing with is refused where it is read,
  # and saying so is the only way an operator learns why a login stopped
  target_link_libraries(sourcemeta_one_authentication PRIVATE sourcemeta::one::http)
else()
  sourcemeta_library(NAMESPACE sourcemeta PROJECT one NAME authentication
    PRIVATE_HEADERS error.h
    SOURCES authentication.cc path.cc)
endif()

# Paths are the key the policy trie is built from and looked up by, so
# canonicalising them belongs to this module in both editions
target_link_libraries(sourcemeta_one_authentication PRIVATE sourcemeta::core::text)
target_link_libraries(sourcemeta_one_authentication PRIVATE sourcemeta::core::uri)

target_link_libraries(sourcemeta_one_authentication PUBLIC sourcemeta::core::io)
target_link_libraries(sourcemeta_one_authentication PUBLIC sourcemeta::core::jose)
