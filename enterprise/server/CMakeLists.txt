sourcemeta_library(NAMESPACE sourcemeta PROJECT one NAME enterprise_server
  PRIVATE_HEADERS actions.h
  SOURCES action_mcp_v1.cc)

target_compile_definitions(sourcemeta_one_enterprise_server PUBLIC SOURCEMETA_ONE_ENTERPRISE)

# TODO: This introduces a cycle. How can we avoid it?
target_link_libraries(sourcemeta_one_enterprise_server PUBLIC sourcemeta::one::actions)

target_link_libraries(sourcemeta_one_enterprise_server PUBLIC sourcemeta::one::http)
target_link_libraries(sourcemeta_one_enterprise_server PUBLIC sourcemeta::core::uritemplate)
target_link_libraries(sourcemeta_one_enterprise_server PUBLIC sourcemeta::blaze::evaluator)
target_link_libraries(sourcemeta_one_enterprise_server PRIVATE sourcemeta::core::json)
target_link_libraries(sourcemeta_one_enterprise_server PRIVATE sourcemeta::core::jsonpointer)
target_link_libraries(sourcemeta_one_enterprise_server PRIVATE sourcemeta::one::shared)
target_link_libraries(sourcemeta_one_enterprise_server PRIVATE sourcemeta::one::metapack)
target_link_libraries(sourcemeta_one_enterprise_server PRIVATE sourcemeta::blaze::output)
