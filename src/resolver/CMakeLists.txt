sourcemeta_library(NAMESPACE sourcemeta PROJECT one NAME resolver
  PRIVATE_HEADERS error.h SOURCES resolver.cc)

target_link_libraries(sourcemeta_one_resolver PUBLIC
  sourcemeta::core::json)
target_link_libraries(sourcemeta_one_resolver PUBLIC
  sourcemeta::one::configuration)

target_link_libraries(sourcemeta_one_resolver PRIVATE
  sourcemeta::core::uri)
target_link_libraries(sourcemeta_one_resolver PRIVATE
  sourcemeta::core::jsonschema)
target_link_libraries(sourcemeta_one_resolver PRIVATE
  sourcemeta::core::yaml)
target_link_libraries(sourcemeta_one_resolver PRIVATE
  sourcemeta::one::shared)
