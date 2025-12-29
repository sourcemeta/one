set(COLLECTION_NAMESPACE "sourcemeta/std")
set(STD_PATH "${PROJECT_SOURCE_DIR}/vendor/collections/${COLLECTION_NAMESPACE}/v0")

install(DIRECTORY "${STD_PATH}/schemas"
  DESTINATION "${CMAKE_INSTALL_DATADIR}/sourcemeta/one/collections/${COLLECTION_NAMESPACE}"
  COMPONENT sourcemeta_one)

configure_file("${CMAKE_CURRENT_LIST_DIR}/one.json"
  "${CMAKE_CURRENT_BINARY_DIR}/${COLLECTION_NAMESPACE}.json" @ONLY)
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/${COLLECTION_NAMESPACE}.json"
  DESTINATION "${CMAKE_INSTALL_DATADIR}/sourcemeta/one/collections/${COLLECTION_NAMESPACE}"
  RENAME one.json
  COMPONENT sourcemeta_one)
