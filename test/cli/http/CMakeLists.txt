find_program(NODE_BIN NAMES node REQUIRED)

sourcemeta_executable(NAMESPACE sourcemeta PROJECT one NAME http
  VARIANT wire_echo SOURCES echo.cc)
target_link_libraries(sourcemeta_one_http_wire_echo
  PRIVATE sourcemeta::one::http)

add_test(NAME one.http.wire COMMAND "${NODE_BIN}"
  "${CMAKE_CURRENT_SOURCE_DIR}/wire.test.cjs"
  "$<TARGET_FILE:sourcemeta_one_http_wire_echo>")
set_tests_properties(one.http.wire PROPERTIES TIMEOUT 30)
