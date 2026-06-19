if(ONE_ENTERPRISE)
  sourcemeta_library(NAMESPACE sourcemeta PROJECT one NAME authentication
    SOURCES
      "${PROJECT_SOURCE_DIR}/enterprise/authentication/authentication.cc"
      "${PROJECT_SOURCE_DIR}/enterprise/authentication/authentication_save.cc")
  target_compile_definitions(sourcemeta_one_authentication PUBLIC SOURCEMETA_ONE_ENTERPRISE)
  target_link_libraries(sourcemeta_one_authentication PRIVATE sourcemeta_one_shared)
else()
  sourcemeta_library(NAMESPACE sourcemeta PROJECT one NAME authentication
    SOURCES authentication.cc)
endif()

target_link_libraries(sourcemeta_one_authentication PUBLIC sourcemeta::core::io)
