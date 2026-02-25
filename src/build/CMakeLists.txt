sourcemeta_library(NAMESPACE sourcemeta PROJECT one NAME build
  PRIVATE_HEADERS types.h adapter_filesystem.h SOURCES adapter_filesystem.cc)
target_link_libraries(sourcemeta_one_build PRIVATE sourcemeta::core::io)
