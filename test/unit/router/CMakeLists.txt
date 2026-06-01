sourcemeta_googletest(NAMESPACE sourcemeta PROJECT one NAME router
  SOURCES router_lru_test.cc)

target_link_libraries(sourcemeta_one_router_unit
  PRIVATE sourcemeta::one::router)
