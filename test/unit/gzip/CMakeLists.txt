sourcemeta_googletest(NAMESPACE sourcemeta PROJECT one NAME gzip
  SOURCES gzip_test.cc)

target_link_libraries(sourcemeta_one_gzip_unit
  PRIVATE sourcemeta::one::gzip)
