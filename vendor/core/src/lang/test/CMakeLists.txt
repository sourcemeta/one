sourcemeta_library(NAMESPACE sourcemeta PROJECT core NAME test
  SOURCES test.cc)

if(SOURCEMETA_CORE_INSTALL)
  sourcemeta_library_install(NAMESPACE sourcemeta PROJECT core NAME test)
endif()

target_link_libraries(sourcemeta_core_test
  PRIVATE sourcemeta::core::options)
target_link_libraries(sourcemeta_core_test
  PRIVATE sourcemeta::core::stacktrace)
