sourcemeta_library(NAMESPACE sourcemeta PROJECT one NAME configuration
  PRIVATE_HEADERS error.h
  SOURCES parse.cc read.cc resolve_schema.cc "${CMAKE_CURRENT_BINARY_DIR}/template.h")

set(CONFIGURATION_SCHEMA "${CMAKE_CURRENT_SOURCE_DIR}/schema")

add_custom_command(OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/template.h"
  COMMAND "${JSONSCHEMA_BIN}" compile
    --include CONFIGURATION
    "${CONFIGURATION_SCHEMA}/configuration.json"
    > "${CMAKE_CURRENT_BINARY_DIR}/template.h"
  DEPENDS
    "${CONFIGURATION_SCHEMA}/configuration.json"
    "${CONFIGURATION_SCHEMA}/collection.json"
    "${CONFIGURATION_SCHEMA}/contents.json"
    "${CONFIGURATION_SCHEMA}/page.json")

target_include_directories(sourcemeta_one_configuration PRIVATE "${CMAKE_CURRENT_BINARY_DIR}")

target_link_libraries(sourcemeta_one_configuration PUBLIC sourcemeta::core::json)
target_link_libraries(sourcemeta_one_configuration PUBLIC sourcemeta::core::jsonpointer)
target_link_libraries(sourcemeta_one_configuration PUBLIC sourcemeta::blaze::configuration)
target_link_libraries(sourcemeta_one_configuration PUBLIC sourcemeta::core::uri)
target_link_libraries(sourcemeta_one_configuration PUBLIC sourcemeta::blaze::output)
target_link_libraries(sourcemeta_one_configuration PRIVATE sourcemeta::blaze::evaluator)
