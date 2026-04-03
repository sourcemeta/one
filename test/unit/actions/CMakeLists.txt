sourcemeta_googletest(NAMESPACE sourcemeta PROJECT one NAME actions
  SOURCES actions_schema_directory_test.cc)

target_link_libraries(sourcemeta_one_actions_unit
  PRIVATE sourcemeta::one::actions)
