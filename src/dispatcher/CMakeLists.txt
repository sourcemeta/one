sourcemeta_library(NAMESPACE sourcemeta PROJECT one NAME dispatcher
  SOURCES base.cc dispatch.cc)

target_link_libraries(sourcemeta_one_dispatcher PUBLIC sourcemeta::one::http)
target_link_libraries(sourcemeta_one_dispatcher PUBLIC sourcemeta::core::uritemplate)
target_link_libraries(sourcemeta_one_dispatcher PUBLIC sourcemeta::core::json)
target_link_libraries(sourcemeta_one_dispatcher PUBLIC sourcemeta::core::jsonrpc)
target_link_libraries(sourcemeta_one_dispatcher PUBLIC sourcemeta::one::metapack)
target_link_libraries(sourcemeta_one_dispatcher PUBLIC sourcemeta::blaze::evaluator)
target_link_libraries(sourcemeta_one_dispatcher PUBLIC sourcemeta::blaze::compiler)
target_link_libraries(sourcemeta_one_dispatcher PRIVATE sourcemeta::core::uri)
target_link_libraries(sourcemeta_one_dispatcher PRIVATE sourcemeta::one::shared)
