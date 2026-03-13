sourcemeta_library(NAMESPACE sourcemeta PROJECT one NAME build
  PRIVATE_HEADERS state.h
  SOURCES rules.h delta.cc state.cc)
target_link_libraries(sourcemeta_one_build
  PUBLIC sourcemeta::core::json sourcemeta::one::resolver
  PRIVATE sourcemeta::core::io)
