set(SELF_SCHEMAS "${PROJECT_SOURCE_DIR}/src/self/v1/schemas")

add_test(NAME one.self.schemas.fmt COMMAND
  "${JSONSCHEMA_BIN}" fmt --check "${SELF_SCHEMAS}")
add_test(NAME one.self.schemas.metaschema COMMAND
  "${JSONSCHEMA_BIN}" metaschema "${SELF_SCHEMAS}")
add_test(NAME one.self.schemas.lint COMMAND
  "${JSONSCHEMA_BIN}" lint "${SELF_SCHEMAS}")
