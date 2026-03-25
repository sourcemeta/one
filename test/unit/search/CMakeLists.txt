sourcemeta_googletest(NAMESPACE sourcemeta PROJECT one NAME search
  SOURCES search_test.cc)

target_link_libraries(sourcemeta_one_search_unit
  PRIVATE sourcemeta::one::search)
