set(COLLECTION_NAMESPACE "self/v1")

install(DIRECTORY "${CMAKE_CURRENT_LIST_DIR}/schemas"
  DESTINATION "${CMAKE_INSTALL_DATADIR}/sourcemeta/one/collections/${COLLECTION_NAMESPACE}"
  COMPONENT sourcemeta_one)
install(FILES "${CMAKE_CURRENT_LIST_DIR}/one.json" "${CMAKE_CURRENT_LIST_DIR}/jsonschema.json"
  DESTINATION "${CMAKE_INSTALL_DATADIR}/sourcemeta/one/collections/${COLLECTION_NAMESPACE}"
  COMPONENT sourcemeta_one)
