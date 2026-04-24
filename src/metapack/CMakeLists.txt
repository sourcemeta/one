sourcemeta_library(NAMESPACE sourcemeta PROJECT one NAME metapack
  SOURCES metapack.cc)

target_link_libraries(sourcemeta_one_metapack PUBLIC sourcemeta::core::json)
target_link_libraries(sourcemeta_one_metapack PUBLIC sourcemeta::core::io)
target_link_libraries(sourcemeta_one_metapack PRIVATE sourcemeta::core::crypto)
target_link_libraries(sourcemeta_one_metapack PRIVATE sourcemeta::core::gzip)
