sourcemeta_executable(NAMESPACE sourcemeta PROJECT one NAME server
  FOLDER "One/Server"
  SOURCES
    action_jsonschema_evaluate.h
    action_jsonschema_serve.h
    action_schema_search.h
    action_serve_metapack_file.h
    helpers.h
    request.h
    response.h
    server.cc
    uwebsockets.h)

set_target_properties(sourcemeta_one_server PROPERTIES OUTPUT_NAME sourcemeta-one-server)

target_link_libraries(sourcemeta_one_server PRIVATE sourcemeta::core::json)
target_link_libraries(sourcemeta_one_server PRIVATE sourcemeta::core::jsonpointer)
target_link_libraries(sourcemeta_one_server PRIVATE sourcemeta::core::crypto)
target_link_libraries(sourcemeta_one_server PRIVATE sourcemeta::core::time)
target_link_libraries(sourcemeta_one_server PRIVATE sourcemeta::core::uritemplate)
target_link_libraries(sourcemeta_one_server PRIVATE uNetworking::uSockets)
target_link_libraries(sourcemeta_one_server PRIVATE uNetworking::uWebSockets)
target_link_libraries(sourcemeta_one_server PRIVATE sourcemeta::one::gzip)
target_link_libraries(sourcemeta_one_server PRIVATE sourcemeta::one::shared)
target_link_libraries(sourcemeta_one_server PRIVATE sourcemeta::blaze::evaluator)
target_link_libraries(sourcemeta_one_server PRIVATE sourcemeta::blaze::output)

target_compile_definitions(sourcemeta_one_server
  PRIVATE SOURCEMETA_ONE_STATIC="${ONE_PREFIX}/share/sourcemeta/one/static")

install(TARGETS sourcemeta_one_server
  RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}"
  COMPONENT sourcemeta_one)
