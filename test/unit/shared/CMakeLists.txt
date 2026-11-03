sourcemeta_test(NAMESPACE sourcemeta PROJECT one NAME shared
  SOURCES shared_metapack_test.cc)

target_link_libraries(sourcemeta_one_shared_unit
  PRIVATE sourcemeta::one::shared)
target_link_libraries(sourcemeta_one_shared_unit
  PRIVATE sourcemeta::core::metapack)
