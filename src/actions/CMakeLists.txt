sourcemeta_library(NAMESPACE sourcemeta PROJECT one NAME actions
  SOURCES dispatch.cc schema_directory.cc
    action_default.h
    action_health_check.h
    action_jsonschema_evaluate.h
    action_jsonschema_serve.h
    action_not_found.h
    action_schema_search.h
    action_serve_explorer_artifact.h
    action_serve_metapack_file.h
    action_serve_schema_artifact.h
    action_serve_static.h)

target_link_libraries(sourcemeta_one_actions PUBLIC sourcemeta::one::http)
target_link_libraries(sourcemeta_one_actions PUBLIC sourcemeta::core::uritemplate)
target_link_libraries(sourcemeta_one_actions PRIVATE sourcemeta::core::json)
target_link_libraries(sourcemeta_one_actions PRIVATE sourcemeta::core::jsonpointer)
target_link_libraries(sourcemeta_one_actions PRIVATE sourcemeta::core::io)
target_link_libraries(sourcemeta_one_actions PRIVATE sourcemeta::core::time)
target_link_libraries(sourcemeta_one_actions PRIVATE sourcemeta::one::shared)
target_link_libraries(sourcemeta_one_actions PRIVATE sourcemeta::one::metapack)
target_link_libraries(sourcemeta_one_actions PRIVATE sourcemeta::one::search)
target_link_libraries(sourcemeta_one_actions PRIVATE sourcemeta::blaze::evaluator)
target_link_libraries(sourcemeta_one_actions PRIVATE sourcemeta::blaze::output)
