install(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/v1/schemas"
  DESTINATION "${CMAKE_INSTALL_DATADIR}/sourcemeta/one/self/v1"
  COMPONENT sourcemeta_one)
install(FILES "${CMAKE_CURRENT_SOURCE_DIR}/v1/one.json"
  DESTINATION "${CMAKE_INSTALL_DATADIR}/sourcemeta/one/self/v1"
  COMPONENT sourcemeta_one)
