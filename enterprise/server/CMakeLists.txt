sourcemeta_library(NAMESPACE sourcemeta PROJECT one NAME enterprise_server
  PRIVATE_HEADERS action_mcp_v1.h action_jsonschema_rdf_v1.h)

target_compile_definitions(sourcemeta_one_enterprise_server INTERFACE SOURCEMETA_ONE_ENTERPRISE)

# The auth endpoints compiled through this target consume the header-only OIDC
# module that lives beside the authentication library, so this target carries
# both its include directory and the libraries its inline bodies call into
target_include_directories(sourcemeta_one_enterprise_server INTERFACE
  "$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/enterprise/authentication>")
target_link_libraries(sourcemeta_one_enterprise_server INTERFACE sourcemeta::core::crypto)
target_link_libraries(sourcemeta_one_enterprise_server INTERFACE sourcemeta::core::jose)
target_link_libraries(sourcemeta_one_enterprise_server INTERFACE sourcemeta::core::oauth)

target_link_libraries(sourcemeta_one_enterprise_server INTERFACE sourcemeta::one::router)
target_link_libraries(sourcemeta_one_enterprise_server INTERFACE sourcemeta::one::http)
target_link_libraries(sourcemeta_one_enterprise_server INTERFACE sourcemeta::one::search)
target_link_libraries(sourcemeta_one_enterprise_server INTERFACE sourcemeta::core::uritemplate)
target_link_libraries(sourcemeta_one_enterprise_server INTERFACE sourcemeta::blaze::evaluator)
target_link_libraries(sourcemeta_one_enterprise_server INTERFACE sourcemeta::core::json)
target_link_libraries(sourcemeta_one_enterprise_server INTERFACE sourcemeta::core::jsonpointer)
target_link_libraries(sourcemeta_one_enterprise_server INTERFACE sourcemeta::core::jsonrpc)
target_link_libraries(sourcemeta_one_enterprise_server INTERFACE sourcemeta::core::uri)
target_link_libraries(sourcemeta_one_enterprise_server INTERFACE sourcemeta::one::shared)
target_link_libraries(sourcemeta_one_enterprise_server INTERFACE sourcemeta::core::mcp)
target_link_libraries(sourcemeta_one_enterprise_server INTERFACE sourcemeta::core::text)
target_link_libraries(sourcemeta_one_enterprise_server INTERFACE sourcemeta::one::metapack)
target_link_libraries(sourcemeta_one_enterprise_server INTERFACE sourcemeta::blaze::output)
target_link_libraries(sourcemeta_one_enterprise_server INTERFACE sourcemeta::core::jsonld)
target_link_libraries(sourcemeta_one_enterprise_server INTERFACE sourcemeta::core::numeric)
target_link_libraries(sourcemeta_one_enterprise_server INTERFACE sourcemeta::core::http)
