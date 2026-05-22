sourcemeta_googletest(NAMESPACE sourcemeta PROJECT one NAME actions
  SOURCES
    actions_descriptions_test.cc
    actions_schema_directory_test.cc
    actions_uri_to_relative_path_test.cc)

target_link_libraries(sourcemeta_one_actions_unit
  PRIVATE sourcemeta::one::actions)
