if(ONE_ENTERPRISE)
  sourcemeta_library(NAMESPACE sourcemeta PROJECT one NAME authentication
    PRIVATE_HEADERS error.h session.h
    SOURCES
      "${PROJECT_SOURCE_DIR}/enterprise/authentication/authentication.cc"
      "${PROJECT_SOURCE_DIR}/enterprise/authentication/authentication_save.cc"
      "${PROJECT_SOURCE_DIR}/enterprise/authentication/session.cc")
  target_compile_definitions(sourcemeta_one_authentication PUBLIC SOURCEMETA_ONE_ENTERPRISE)
  # The OAuth and OIDC modules are header-only and enterprise-only, kept apart
  # from the shared include tree so they extract cleanly when upstreamed
  target_include_directories(sourcemeta_one_authentication PUBLIC
    "$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/enterprise/authentication>")
  target_link_libraries(sourcemeta_one_authentication PRIVATE sourcemeta::core::crypto)
  target_link_libraries(sourcemeta_one_authentication PRIVATE sourcemeta::core::http)
  target_link_libraries(sourcemeta_one_authentication PRIVATE sourcemeta::core::uri)
else()
  sourcemeta_library(NAMESPACE sourcemeta PROJECT one NAME authentication
    PRIVATE_HEADERS error.h session.h
    SOURCES authentication.cc session.cc)
endif()

target_link_libraries(sourcemeta_one_authentication PUBLIC sourcemeta::core::io)
target_link_libraries(sourcemeta_one_authentication PUBLIC sourcemeta::core::jose)
target_link_libraries(sourcemeta_one_authentication PUBLIC sourcemeta::one::configuration)
target_link_libraries(sourcemeta_one_authentication PUBLIC sourcemeta::core::uritemplate)
