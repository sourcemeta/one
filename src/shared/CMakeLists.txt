sourcemeta_library(NAMESPACE sourcemeta PROJECT one NAME shared
  PRIVATE_HEADERS encoding.h uri.h version.h
  SOURCES version.cc uri.cc configure.h.in)

if(ONE_ENTERPRISE)
  target_compile_definitions(sourcemeta_one_shared
    PRIVATE SOURCEMETA_ONE_ENTERPRISE)
endif()

target_link_libraries(sourcemeta_one_shared PRIVATE sourcemeta::core::uri)
target_link_libraries(sourcemeta_one_shared PRIVATE sourcemeta::core::text)

configure_file(configure.h.in configure.h @ONLY)
target_include_directories(sourcemeta_one_shared
  PRIVATE "${CMAKE_CURRENT_BINARY_DIR}")
