sourcemeta_executable(NAMESPACE sourcemeta PROJECT one NAME file2metapack
  SOURCES file2metapack.cc)

target_link_libraries(sourcemeta_one_file2metapack
  PRIVATE sourcemeta::one::shared)
target_link_libraries(sourcemeta_one_file2metapack
  PRIVATE sourcemeta::core::options)

# Work around a GCC LTO linker plugin bug
if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
  target_link_options(sourcemeta_one_file2metapack PRIVATE
    "LINKER:--copy-dt-needed-entries")
endif()
