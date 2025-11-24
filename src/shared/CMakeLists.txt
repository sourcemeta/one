sourcemeta_library(NAMESPACE sourcemeta PROJECT one NAME shared
  PRIVATE_HEADERS metapack.h encoding.h version.h
  SOURCES metapack.cc version.cc configure.h.in)

target_link_libraries(sourcemeta_one_shared PUBLIC sourcemeta::core::json)
target_link_libraries(sourcemeta_one_shared PRIVATE sourcemeta::core::io)
target_link_libraries(sourcemeta_one_shared PRIVATE sourcemeta::core::uuid)
target_link_libraries(sourcemeta_one_shared PRIVATE sourcemeta::core::md5)
target_link_libraries(sourcemeta_one_shared PRIVATE sourcemeta::core::time)
target_link_libraries(sourcemeta_one_shared PRIVATE sourcemeta::one::gzip)

configure_file(configure.h.in configure.h @ONLY)
target_include_directories(sourcemeta_one_shared
  PRIVATE "${CMAKE_CURRENT_BINARY_DIR}")
