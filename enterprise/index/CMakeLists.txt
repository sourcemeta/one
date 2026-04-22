sourcemeta_library(NAMESPACE sourcemeta PROJECT one NAME enterprise_index
  SOURCES enterprise_index.cc)

target_compile_definitions(sourcemeta_one_enterprise_index PUBLIC SOURCEMETA_ONE_ENTERPRISE)

target_link_libraries(sourcemeta_one_enterprise_index PRIVATE sourcemeta::one::metapack)
target_link_libraries(sourcemeta_one_enterprise_index PUBLIC sourcemeta::one::resolver)
target_link_libraries(sourcemeta_one_enterprise_index PUBLIC sourcemeta::one::build)

target_link_libraries(sourcemeta_one_enterprise_index PRIVATE sourcemeta::core::error)
target_link_libraries(sourcemeta_one_enterprise_index PRIVATE sourcemeta::core::yaml)
target_link_libraries(sourcemeta_one_enterprise_index PUBLIC sourcemeta::core::html)
target_link_libraries(sourcemeta_one_enterprise_index PRIVATE sourcemeta::core::markdown)
target_link_libraries(sourcemeta_one_enterprise_index PUBLIC sourcemeta::core::jsonschema)

target_link_libraries(sourcemeta_one_enterprise_index PRIVATE sourcemeta::blaze::compiler)
target_link_libraries(sourcemeta_one_enterprise_index PUBLIC sourcemeta::blaze::configuration)
target_link_libraries(sourcemeta_one_enterprise_index PUBLIC sourcemeta::blaze::alterschema)
