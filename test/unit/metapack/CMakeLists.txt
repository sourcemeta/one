sourcemeta_googletest(NAMESPACE sourcemeta PROJECT one NAME metapack
  SOURCES metapack_test.cc)

target_link_libraries(sourcemeta_one_metapack_unit
  PRIVATE sourcemeta::one::metapack)
