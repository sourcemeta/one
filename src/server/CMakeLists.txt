sourcemeta_executable(NAMESPACE sourcemeta PROJECT one NAME server
  FOLDER "One/Server"
  SOURCES server.cc)

set_target_properties(sourcemeta_one_server PROPERTIES OUTPUT_NAME sourcemeta-one-server)

target_link_libraries(sourcemeta_one_server PRIVATE sourcemeta::one::actions)
target_link_libraries(sourcemeta_one_server PRIVATE sourcemeta::core::uritemplate)

install(TARGETS sourcemeta_one_server
  RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}"
  COMPONENT sourcemeta_one)
