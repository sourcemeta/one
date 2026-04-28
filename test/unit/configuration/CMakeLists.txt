sourcemeta_googletest(NAMESPACE sourcemeta PROJECT one NAME configuration
  SOURCES configuration_read_test.cc configuration_test.cc configuration_resolve_schema_test.cc)

target_link_libraries(sourcemeta_one_configuration_unit
  PRIVATE sourcemeta::one::configuration)
target_compile_definitions(sourcemeta_one_configuration_unit
  PRIVATE STUB_DIRECTORY="${CMAKE_CURRENT_SOURCE_DIR}/stub")
target_compile_definitions(sourcemeta_one_configuration_unit
  PRIVATE SELF_DIRECTORY="${PROJECT_SOURCE_DIR}/src/self")

set(CONFIGURATION_SCHEMA "${PROJECT_SOURCE_DIR}/src/configuration/schema")

add_test(NAME one.configuration.schema COMMAND
  "${JSONSCHEMA_BIN}" test --extension .test.json
  "${CMAKE_CURRENT_SOURCE_DIR}/configuration.test.json")
add_test(NAME one.configuration.schema.fmt COMMAND
  "${JSONSCHEMA_BIN}" fmt --check "${CONFIGURATION_SCHEMA}")
add_test(NAME one.configuration.schema.metaschema COMMAND
  "${JSONSCHEMA_BIN}" metaschema "${CONFIGURATION_SCHEMA}")
add_test(NAME one.configuration.schema.lint COMMAND
  "${JSONSCHEMA_BIN}" lint "${CONFIGURATION_SCHEMA}")
