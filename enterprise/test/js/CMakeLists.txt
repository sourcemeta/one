find_program(NODE_BIN NAMES node REQUIRED)

add_test(NAME one.js.editor
  COMMAND "${NODE_BIN}" "${CMAKE_CURRENT_SOURCE_DIR}/editor.test.js")
