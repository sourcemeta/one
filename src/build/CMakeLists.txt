sourcemeta_library(NAMESPACE sourcemeta PROJECT one NAME build
  SOURCES build.cc build_state.h)
target_link_libraries(sourcemeta_one_build
  PUBLIC sourcemeta::core::json
  PRIVATE sourcemeta::core::jsonschema sourcemeta::core::io)
