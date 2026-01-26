set(COLLECTION_NAMESPACE "self/v1")

install(DIRECTORY "${CMAKE_CURRENT_LIST_DIR}/schemas"
  DESTINATION "${CMAKE_INSTALL_DATADIR}/sourcemeta/one/collections/${COLLECTION_NAMESPACE}"
  COMPONENT sourcemeta_one)
install(FILES "${CMAKE_CURRENT_LIST_DIR}/one.json"
  DESTINATION "${CMAKE_INSTALL_DATADIR}/sourcemeta/one/collections/${COLLECTION_NAMESPACE}"
  COMPONENT sourcemeta_one)

configure_file("${CMAKE_CURRENT_LIST_DIR}/jsonschema.in.json"
  "${CMAKE_CURRENT_BINARY_DIR}/${COLLECTION_NAMESPACE}.json" @ONLY)
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/${COLLECTION_NAMESPACE}.json"
  DESTINATION "${CMAKE_INSTALL_DATADIR}/sourcemeta/one/collections/${COLLECTION_NAMESPACE}"
  RENAME jsonschema.json
  COMPONENT sourcemeta_one)

# This is to decouple the schemas that describe the instance from the huge
# and ever-growing Sourcemeta standard library. We do it in a simple way:
# copying the ones we need.

macro(__sourcemeta_std_copy_common schema_path)
  set(_std_schema_path "${schema_path}")
  set(_collections "${CMAKE_INSTALL_DATADIR}/sourcemeta/one/collections")
  cmake_path(GET _std_schema_path PARENT_PATH _std_schema_directory)
  install(
    FILES "${PROJECT_SOURCE_DIR}/vendor/collections/sourcemeta/std/v0/schemas/2020-12/${schema_path}.json"
    DESTINATION "${_collections}/${COLLECTION_NAMESPACE}/schemas/common/${_std_schema_directory}"
    COMPONENT sourcemeta_one)
endmacro()

__sourcemeta_std_copy_common(ieee/posix/2017/path)
__sourcemeta_std_copy_common(ieee/posix/2017/path-relative)
__sourcemeta_std_copy_common(ieee/posix/2017/path-absolute)
__sourcemeta_std_copy_common(ietf/uri/uri)
__sourcemeta_std_copy_common(ietf/uri/uri-reference)
__sourcemeta_std_copy_common(ietf/uri/uri-relative)
__sourcemeta_std_copy_common(ietf/uri/url)
__sourcemeta_std_copy_common(ietf/uri/urn)
__sourcemeta_std_copy_common(ietf/email/address)
__sourcemeta_std_copy_common(ietf/jsonpointer/pointer)
__sourcemeta_std_copy_common(ietf/problem-details/problem)
__sourcemeta_std_copy_common(ietf/http/status)
