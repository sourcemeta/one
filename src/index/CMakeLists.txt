sourcemeta_executable(NAMESPACE sourcemeta PROJECT one NAME index
  FOLDER "One/Index"
  SOURCES index.cc output.h generators.h explorer.h)

set_target_properties(sourcemeta_one_index PROPERTIES OUTPUT_NAME sourcemeta-one-index)

target_compile_definitions(sourcemeta_one_index
  PRIVATE SOURCEMETA_ONE_COLLECTIONS="${ONE_PREFIX}/share/sourcemeta/one/collections")

target_link_libraries(sourcemeta_one_index PRIVATE sourcemeta::one::resolver)
target_link_libraries(sourcemeta_one_index PRIVATE sourcemeta::one::shared)
target_link_libraries(sourcemeta_one_index PRIVATE sourcemeta::one::configuration)

if(ONE_ENTERPRISE)
  target_link_libraries(sourcemeta_one_index PRIVATE sourcemeta::one::web)
  target_compile_definitions(sourcemeta_one_index PRIVATE SOURCEMETA_ONE_ENTERPRISE)
endif()

target_link_libraries(sourcemeta_one_index PRIVATE sourcemeta::core::build)
target_link_libraries(sourcemeta_one_index PRIVATE sourcemeta::core::uri)
target_link_libraries(sourcemeta_one_index PRIVATE sourcemeta::core::json)
target_link_libraries(sourcemeta_one_index PRIVATE sourcemeta::core::jsonschema)
target_link_libraries(sourcemeta_one_index PRIVATE sourcemeta::core::alterschema)
target_link_libraries(sourcemeta_one_index PRIVATE sourcemeta::core::editorschema)
target_link_libraries(sourcemeta_one_index PRIVATE sourcemeta::core::parallel)
target_link_libraries(sourcemeta_one_index PRIVATE sourcemeta::core::options)
target_link_libraries(sourcemeta_one_index PRIVATE sourcemeta::core::uritemplate)
target_link_libraries(sourcemeta_one_index PRIVATE sourcemeta::blaze::compiler)
target_link_libraries(sourcemeta_one_index PRIVATE sourcemeta::blaze::evaluator)
target_link_libraries(sourcemeta_one_index PRIVATE sourcemeta::blaze::output)
target_link_libraries(sourcemeta_one_index PRIVATE sourcemeta::blaze::linter)

install(TARGETS sourcemeta_one_index
  RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}"
  COMPONENT sourcemeta_one)
