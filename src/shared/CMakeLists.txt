sourcemeta_library(NAMESPACE sourcemeta PROJECT one NAME shared
  PRIVATE_HEADERS encoding.h version.h
  SOURCES version.cc configure.h.in)

if(ONE_ENTERPRISE)
  target_compile_definitions(sourcemeta_one_shared
    PRIVATE SOURCEMETA_ONE_ENTERPRISE)
endif()

configure_file(configure.h.in configure.h @ONLY)
target_include_directories(sourcemeta_one_shared
  PRIVATE "${CMAKE_CURRENT_BINARY_DIR}")
