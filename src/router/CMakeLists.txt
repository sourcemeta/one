sourcemeta_library(NAMESPACE sourcemeta PROJECT one NAME router
  SOURCES base.cc router.cc)

target_link_libraries(sourcemeta_one_router PUBLIC sourcemeta::one::http)
target_link_libraries(sourcemeta_one_router PUBLIC sourcemeta::core::uritemplate)
target_link_libraries(sourcemeta_one_router PUBLIC sourcemeta::core::json)
target_link_libraries(sourcemeta_one_router PUBLIC sourcemeta::core::jsonrpc)
target_link_libraries(sourcemeta_one_router PUBLIC sourcemeta::one::metapack)
target_link_libraries(sourcemeta_one_router PUBLIC sourcemeta::blaze::evaluator)
target_link_libraries(sourcemeta_one_router PUBLIC sourcemeta::blaze::compiler)
target_link_libraries(sourcemeta_one_router PRIVATE sourcemeta::core::uri)
target_link_libraries(sourcemeta_one_router PRIVATE sourcemeta::one::shared)
