sourcemeta_library(NAMESPACE sourcemeta PROJECT one NAME pkg
  SOURCES pkg.cc)

target_link_libraries(sourcemeta_one_pkg PUBLIC sourcemeta::core::build)
target_link_libraries(sourcemeta_one_pkg PRIVATE sourcemeta::core::json)
target_link_libraries(sourcemeta_one_pkg PUBLIC sourcemeta::one::resolver)
target_link_libraries(sourcemeta_one_pkg PUBLIC sourcemeta::one::configuration)
target_link_libraries(sourcemeta_one_pkg PRIVATE sourcemeta::one::shared)

install(FILES npm/npmignore
  DESTINATION "${CMAKE_INSTALL_DATADIR}/sourcemeta/one/pkg/npm"
  COMPONENT sourcemeta_one)

target_sources(sourcemeta_one_pkg PRIVATE
  npm/npmignore)
