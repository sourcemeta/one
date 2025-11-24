sourcemeta_executable(NAMESPACE sourcemeta PROJECT one NAME file2metapack
  SOURCES file2metapack.cc)

target_link_libraries(sourcemeta_one_file2metapack
  PRIVATE sourcemeta::one::shared)
target_link_libraries(sourcemeta_one_file2metapack
  PRIVATE sourcemeta::core::options)
