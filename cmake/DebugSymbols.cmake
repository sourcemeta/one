macro(sourcemeta_debug_symbols_enable)
  message(STATUS "Enabling debug symbols globally")
  # `-g1` emits line-number info only (no variable/type/macro info),
  # which is all we need for crash backtraces (function names plus
  # file:line). Cuts LTO link time and sidecar size significantly
  # compared to default `-g` (== `-g2`)
  add_compile_options(-g1)
  # GCC's `-Wmaybe-uninitialized` produces false positives in
  # template code (`std::tuple`, `std::optional`) when `-g`
  # changes inlining decisions under `-O3 -flto`
  if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    add_compile_options(-Wno-error=maybe-uninitialized)
  endif()
endmacro()

function(sourcemeta_debug_symbols_extract TARGET_NAME)
  cmake_parse_arguments(EXTRACT_DEBUG_SYMBOLS "" "COMPONENT" "" ${ARGN})
  if(NOT EXTRACT_DEBUG_SYMBOLS_COMPONENT)
    message(FATAL_ERROR "The COMPONENT argument is required")
  endif()

  set(BINARY_INPUT "$<TARGET_FILE:${TARGET_NAME}>")
  get_target_property(BINARY_OUTPUT_NAME "${TARGET_NAME}" OUTPUT_NAME)
  if(NOT BINARY_OUTPUT_NAME)
    set(BINARY_OUTPUT_NAME "${TARGET_NAME}")
  endif()
  get_target_property(BINARY_OUTPUT_DIR "${TARGET_NAME}" RUNTIME_OUTPUT_DIRECTORY)
  if(NOT BINARY_OUTPUT_DIR)
    get_target_property(BINARY_OUTPUT_DIR "${TARGET_NAME}" BINARY_DIR)
  endif()
  get_target_property(BINARY_OUTPUT_SUFFIX "${TARGET_NAME}" SUFFIX)
  if(NOT BINARY_OUTPUT_SUFFIX)
    set(BINARY_OUTPUT_SUFFIX "${CMAKE_EXECUTABLE_SUFFIX}")
  endif()
  set(BINARY_PATH "${BINARY_OUTPUT_DIR}/${BINARY_OUTPUT_NAME}${BINARY_OUTPUT_SUFFIX}")

  if(APPLE)
    message(STATUS "Extracting debug symbols (.dSYM) for: ${TARGET_NAME}")
    # With `-flto=full`, the post-LTO object normally lives in
    # `/tmp/lto.o` and is removed immediately after the link, which
    # makes `dsymutil` silently produce a near-empty bundle. Pin
    # the LTO temp under the binary's build dir so it persists long
    # enough for `dsymutil` to consume it
    target_link_options("${TARGET_NAME}" PRIVATE
      "LINKER:-object_path_lto,${BINARY_INPUT}.lto.o")

    find_program(DSYMUTIL_EXECUTABLE dsymutil REQUIRED)
    add_custom_command(OUTPUT "${BINARY_PATH}.dSYM"
      COMMAND "${DSYMUTIL_EXECUTABLE}" "${BINARY_INPUT}"
              -o "${BINARY_PATH}.dSYM"
      DEPENDS "${TARGET_NAME}"
      VERBATIM)
    add_custom_target("${TARGET_NAME}_debug_symbols" ALL
      DEPENDS "${BINARY_PATH}.dSYM")

    install(DIRECTORY "${BINARY_PATH}.dSYM"
      DESTINATION "${CMAKE_INSTALL_BINDIR}"
      COMPONENT "${EXTRACT_DEBUG_SYMBOLS_COMPONENT}")
  elseif(UNIX)
    message(STATUS "Extracting debug symbols (.debug) for: ${TARGET_NAME}")
    # Both commands only read the linked binary. Rewriting it would be an
    # output the build graph does not declare, so nothing can be ordered
    # against it, and a code generation step that runs the binary gets
    # scheduled alongside the rewrite rather than after it. The stripped
    # copy reaches its destination through `sourcemeta_debug_symbols_install`
    add_custom_command(OUTPUT "${BINARY_PATH}.debug"
      COMMAND "${CMAKE_OBJCOPY}" --only-keep-debug
              "${BINARY_INPUT}" "${BINARY_PATH}.debug"
      DEPENDS "${TARGET_NAME}"
      VERBATIM)
    add_custom_command(OUTPUT "${BINARY_PATH}.stripped"
      COMMAND "${CMAKE_OBJCOPY}" --strip-debug
              "--add-gnu-debuglink=${BINARY_PATH}.debug"
              "${BINARY_INPUT}" "${BINARY_PATH}.stripped"
      DEPENDS "${TARGET_NAME}" "${BINARY_PATH}.debug"
      VERBATIM)
    add_custom_target("${TARGET_NAME}_debug_symbols" ALL
      DEPENDS "${BINARY_PATH}.stripped")

    install(FILES "${BINARY_PATH}.debug"
      DESTINATION "${CMAKE_INSTALL_BINDIR}"
      COMPONENT "${EXTRACT_DEBUG_SYMBOLS_COMPONENT}")
    set_property(GLOBAL APPEND PROPERTY
      SOURCEMETA_DEBUG_SYMBOLS_BINARIES "${BINARY_PATH}.stripped")
    set_property(GLOBAL APPEND PROPERTY
      SOURCEMETA_DEBUG_SYMBOLS_NAMES
      "${BINARY_OUTPUT_NAME}${BINARY_OUTPUT_SUFFIX}")
    set_property(GLOBAL APPEND PROPERTY
      SOURCEMETA_DEBUG_SYMBOLS_COMPONENTS
      "${EXTRACT_DEBUG_SYMBOLS_COMPONENT}")
  else()
    message(FATAL_ERROR "Unsupported platform: ${CMAKE_SYSTEM_NAME}")
  endif()
endfunction()

# A stripped binary takes the place of the one `install(TARGETS)` puts in the
# destination, so its rule has to run after that one. CMake emits the install
# rules of a directory before those of its subdirectories, which means this
# cannot live where the binaries are declared, as some of them belong to
# vendored subdirectories. Call it from a directory added last instead
function(sourcemeta_debug_symbols_install)
  get_property(BINARIES GLOBAL PROPERTY SOURCEMETA_DEBUG_SYMBOLS_BINARIES)
  get_property(NAMES GLOBAL PROPERTY SOURCEMETA_DEBUG_SYMBOLS_NAMES)
  get_property(COMPONENTS GLOBAL PROPERTY SOURCEMETA_DEBUG_SYMBOLS_COMPONENTS)
  list(LENGTH BINARIES TOTAL)
  if(TOTAL EQUAL 0)
    return()
  endif()

  math(EXPR LAST_INDEX "${TOTAL} - 1")
  foreach(INDEX RANGE ${LAST_INDEX})
    list(GET BINARIES ${INDEX} BINARY_PATH)
    list(GET NAMES ${INDEX} BINARY_NAME)
    list(GET COMPONENTS ${INDEX} BINARY_COMPONENT)
    message(STATUS "Installing stripped binary for: ${BINARY_NAME}")
    install(PROGRAMS "${BINARY_PATH}"
      DESTINATION "${CMAKE_INSTALL_BINDIR}"
      RENAME "${BINARY_NAME}"
      COMPONENT "${BINARY_COMPONENT}")
  endforeach()
endfunction()
