sourcemeta_library(NAMESPACE sourcemeta PROJECT one NAME authentication
  SOURCES authentication.cc)

target_link_libraries(sourcemeta_one_authentication PRIVATE sourcemeta::core::http)
