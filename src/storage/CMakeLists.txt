sourcemeta_library(NAMESPACE sourcemeta PROJECT one NAME storage
  SOURCES storage.cc)

target_link_libraries(sourcemeta_one_storage PUBLIC sourcemeta::core::json)
target_link_libraries(sourcemeta_one_storage PUBLIC sourcemeta::one::shared)
