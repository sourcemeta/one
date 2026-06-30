# Generate a translation unit that defines the test runner entry point and return
# its path. The entry point must be compiled into each test executable rather than
# provided by the (potentially shared) library, as an executable entry point
# cannot be resolved from a shared library. It is generated once per directory, as
# a directory may declare more than one test executable
function(sourcemeta_test_main OUTPUT_VARIABLE)
  get_property(GENERATED_MAIN DIRECTORY PROPERTY SOURCEMETA_CORE_TEST_MAIN)
  if(NOT GENERATED_MAIN)
    set(GENERATED_MAIN "${CMAKE_CURRENT_BINARY_DIR}/sourcemeta_core_test_main.cc")
    file(GENERATE OUTPUT "${GENERATED_MAIN}" CONTENT
"#include <sourcemeta/core/test.h>

auto main(int argc, char **argv) -> int {
  return sourcemeta::core::test_run(argc, argv);
}
")
    set_property(DIRECTORY PROPERTY SOURCEMETA_CORE_TEST_MAIN "${GENERATED_MAIN}")
  endif()

  set("${OUTPUT_VARIABLE}" "${GENERATED_MAIN}" PARENT_SCOPE)
endfunction()

function(sourcemeta_test)
  cmake_parse_arguments(SOURCEMETA_TEST ""
    "NAMESPACE;PROJECT;NAME;VARIANT" "SOURCES" ${ARGN})

  if(SOURCEMETA_TEST_VARIANT)
    set(TARGET_VARIANT "${SOURCEMETA_TEST_VARIANT}_unit")
  else()
    set(TARGET_VARIANT "unit")
  endif()

  sourcemeta_test_main(GENERATED_MAIN)

  sourcemeta_executable(
    NAMESPACE "${SOURCEMETA_TEST_NAMESPACE}"
    PROJECT "${SOURCEMETA_TEST_PROJECT}"
    NAME "${SOURCEMETA_TEST_NAME}"
    VARIANT "${TARGET_VARIANT}"
    SOURCES "${SOURCEMETA_TEST_SOURCES}" "${GENERATED_MAIN}"
    OUTPUT TARGET_NAME)

  target_link_libraries("${TARGET_NAME}"
    PRIVATE sourcemeta::core::test)

  # Test executables are not shipped, so LTO buys nothing and significantly
  # slows the link step (GCC's LTRANS phase serializes per executable)
  if(SOURCEMETA_COMPILER_LLVM OR SOURCEMETA_COMPILER_GCC)
    target_compile_options("${TARGET_NAME}" PRIVATE -fno-lto)
    target_link_options("${TARGET_NAME}" PRIVATE -fno-lto)
  endif()

  add_test(NAME "${SOURCEMETA_TEST_PROJECT}.${SOURCEMETA_TEST_NAME}"
    COMMAND "${TARGET_NAME}")
endfunction()
