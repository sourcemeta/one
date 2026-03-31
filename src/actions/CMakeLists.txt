sourcemeta_library(NAMESPACE sourcemeta PROJECT one NAME actions
  PRIVATE_HEADERS
    ids.h
    helpers.h
    serve_metapack_file.h
    jsonschema_evaluate.h
    jsonschema_serve.h
    schema_search.h
    handlers.h)

target_link_libraries(sourcemeta_one_actions INTERFACE sourcemeta::one::http)
target_link_libraries(sourcemeta_one_actions INTERFACE sourcemeta::core::json)
target_link_libraries(sourcemeta_one_actions INTERFACE sourcemeta::core::jsonpointer)
target_link_libraries(sourcemeta_one_actions INTERFACE sourcemeta::core::io)
target_link_libraries(sourcemeta_one_actions INTERFACE sourcemeta::core::time)
target_link_libraries(sourcemeta_one_actions INTERFACE sourcemeta::one::shared)
target_link_libraries(sourcemeta_one_actions INTERFACE sourcemeta::one::metapack)
target_link_libraries(sourcemeta_one_actions INTERFACE sourcemeta::one::search)
target_link_libraries(sourcemeta_one_actions INTERFACE sourcemeta::blaze::evaluator)
target_link_libraries(sourcemeta_one_actions INTERFACE sourcemeta::blaze::output)
