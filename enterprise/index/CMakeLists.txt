sourcemeta_library(NAMESPACE sourcemeta PROJECT one NAME enterprise_index
  SOURCES enterprise_index.cc)

target_compile_definitions(sourcemeta_one_enterprise_index PUBLIC SOURCEMETA_ONE_ENTERPRISE)

target_link_libraries(sourcemeta_one_enterprise_index PUBLIC sourcemeta::one::resolver)
target_link_libraries(sourcemeta_one_enterprise_index PUBLIC sourcemeta::blaze::configuration)
target_link_libraries(sourcemeta_one_enterprise_index PRIVATE sourcemeta::blaze::compiler)
target_link_libraries(sourcemeta_one_enterprise_index PRIVATE sourcemeta::blaze::linter)
target_link_libraries(sourcemeta_one_enterprise_index PRIVATE sourcemeta::core::yaml)
target_link_libraries(sourcemeta_one_enterprise_index PUBLIC sourcemeta::core::build)
target_link_libraries(sourcemeta_one_enterprise_index PUBLIC sourcemeta::core::jsonschema)
