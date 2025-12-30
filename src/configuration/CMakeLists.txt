sourcemeta_library(NAMESPACE sourcemeta PROJECT one NAME configuration
  PRIVATE_HEADERS error.h
  SOURCES parse.cc read.cc "${CMAKE_CURRENT_BINARY_DIR}/template.h")

# Compile the configuration schema
set(ONE_SCHEMAS "${PROJECT_SOURCE_DIR}/collections/self/v1/schemas")
set(STD_SCHEMAS "${PROJECT_SOURCE_DIR}/vendor/collections/sourcemeta/std/v0/schemas/2020-12")
add_custom_command(OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/template.h"
  COMMAND "$<TARGET_FILE:jsonschema_cli>" compile
    --include CONFIGURATION
    "${ONE_SCHEMAS}/configuration/configuration.json"
    > "${CMAKE_CURRENT_BINARY_DIR}/template.h"
  DEPENDS
    "$<TARGET_FILE:jsonschema_cli>"
    "${ONE_SCHEMAS}/configuration/configuration.json"
    "${ONE_SCHEMAS}/configuration/collection.json"
    "${ONE_SCHEMAS}/configuration/contents.json"
    "${ONE_SCHEMAS}/configuration/extends.json"
    "${ONE_SCHEMAS}/configuration/page.json"
    "${ONE_SCHEMAS}/configuration/rpath.json"
    "${STD_SCHEMAS}/ietf/email/address.json"
    "${STD_SCHEMAS}/ieee/posix/2017/path.json"
    "${STD_SCHEMAS}/ieee/posix/2017/path-absolute.json"
    "${STD_SCHEMAS}/ieee/posix/2017/path-relative.json"
    "${STD_SCHEMAS}/ietf/uri/uri-reference.json"
    "${STD_SCHEMAS}/ietf/uri/uri-relative.json"
    "${STD_SCHEMAS}/ietf/uri/uri.json"
    "${STD_SCHEMAS}/ietf/uri/url.json"
    "${STD_SCHEMAS}/ietf/uri/urn.json")

target_include_directories(sourcemeta_one_configuration PRIVATE "${CMAKE_CURRENT_BINARY_DIR}")

target_link_libraries(sourcemeta_one_configuration PUBLIC sourcemeta::core::json)
target_link_libraries(sourcemeta_one_configuration PUBLIC sourcemeta::core::jsonpointer)
target_link_libraries(sourcemeta_one_configuration PUBLIC sourcemeta::core::schemaconfig)
target_link_libraries(sourcemeta_one_configuration PRIVATE sourcemeta::core::uri)
target_link_libraries(sourcemeta_one_configuration PUBLIC sourcemeta::blaze::output)
target_link_libraries(sourcemeta_one_configuration PRIVATE sourcemeta::blaze::evaluator)
