sourcemeta_googletest(NAMESPACE sourcemeta PROJECT one NAME shared
  SOURCES shared_test.cc)

target_link_libraries(sourcemeta_one_shared_unit
  PRIVATE sourcemeta::one::shared)
