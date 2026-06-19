set(SELF_SCHEMAS "${PROJECT_SOURCE_DIR}/src/self/v1/schemas")

add_test(NAME one.self.schemas.fmt COMMAND
  "$<TARGET_FILE:jsonschema_cli>" fmt --check "${SELF_SCHEMAS}")
add_test(NAME one.self.schemas.metaschema COMMAND
  "$<TARGET_FILE:jsonschema_cli>" metaschema "${SELF_SCHEMAS}")
add_test(NAME one.self.schemas.lint COMMAND
  "$<TARGET_FILE:jsonschema_cli>" lint "${SELF_SCHEMAS}")
