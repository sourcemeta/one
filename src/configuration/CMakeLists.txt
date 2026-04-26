sourcemeta_library(NAMESPACE sourcemeta PROJECT one NAME configuration
  PRIVATE_HEADERS error.h
  SOURCES parse.cc read.cc resolve_schema.cc "${CMAKE_CURRENT_BINARY_DIR}/template.h")

# Compile the configuration schema
set(ONE_SCHEMAS "${PROJECT_SOURCE_DIR}/collections/self/v1/schemas")
add_custom_command(OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/template.h"
  COMMAND "${JSONSCHEMA_BIN}" compile
    --include CONFIGURATION
    "${ONE_SCHEMAS}/configuration/configuration.json"
    > "${CMAKE_CURRENT_BINARY_DIR}/template.h"
  DEPENDS
    "${ONE_SCHEMAS}/configuration/configuration.json"
    "${ONE_SCHEMAS}/configuration/collection.json"
    "${ONE_SCHEMAS}/configuration/contents.json"
    "${ONE_SCHEMAS}/configuration/page.json")

target_include_directories(sourcemeta_one_configuration PRIVATE "${CMAKE_CURRENT_BINARY_DIR}")

target_link_libraries(sourcemeta_one_configuration PUBLIC sourcemeta::core::json)
target_link_libraries(sourcemeta_one_configuration PUBLIC sourcemeta::core::jsonpointer)
target_link_libraries(sourcemeta_one_configuration PUBLIC sourcemeta::blaze::configuration)
target_link_libraries(sourcemeta_one_configuration PUBLIC sourcemeta::core::uri)
target_link_libraries(sourcemeta_one_configuration PUBLIC sourcemeta::blaze::output)
target_link_libraries(sourcemeta_one_configuration PRIVATE sourcemeta::blaze::evaluator)
