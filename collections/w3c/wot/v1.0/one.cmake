set(COLLECTION_NAMESPACE "w3c/wot/v1.0")
set(WOT_PATH "${PROJECT_SOURCE_DIR}/vendor/collections/wot/td/v1/validation")

install(FILES "${WOT_PATH}/td-json-schema-validation.json"
  DESTINATION "${CMAKE_INSTALL_DATADIR}/sourcemeta/one/collections/${COLLECTION_NAMESPACE}/schemas"
  RENAME thing-description.json
  COMPONENT sourcemeta_one)

install(FILES "${CMAKE_CURRENT_LIST_DIR}/one.json"
  DESTINATION "${CMAKE_INSTALL_DATADIR}/sourcemeta/one/collections/${COLLECTION_NAMESPACE}"
  COMPONENT sourcemeta_one)

configure_file("${CMAKE_CURRENT_LIST_DIR}/jsonschema.json"
  "${CMAKE_CURRENT_BINARY_DIR}/${COLLECTION_NAMESPACE}.json" @ONLY)
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/${COLLECTION_NAMESPACE}.json"
  DESTINATION "${CMAKE_INSTALL_DATADIR}/sourcemeta/one/collections/${COLLECTION_NAMESPACE}"
  RENAME jsonschema.json
  COMPONENT sourcemeta_one)
