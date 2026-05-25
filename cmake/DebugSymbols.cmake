macro(sourcemeta_debug_symbols_enable)
  add_compile_options(-g)
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

  get_target_property(BINARY_OUTPUT_NAME "${TARGET_NAME}" OUTPUT_NAME)
  if(NOT BINARY_OUTPUT_NAME)
    set(BINARY_OUTPUT_NAME "${TARGET_NAME}")
  endif()
  get_target_property(BINARY_OUTPUT_DIR "${TARGET_NAME}" BINARY_DIR)
  set(BINARY_PATH "${BINARY_OUTPUT_DIR}/${BINARY_OUTPUT_NAME}")

  if(APPLE)
    # With `-flto=full`, the post-LTO object normally lives in
    # `/tmp/lto.o` and is removed immediately after the link, which
    # makes `dsymutil` silently produce a near-empty bundle. Pin
    # the LTO temp under the binary's build dir so it persists long
    # enough for `dsymutil` to consume it
    target_link_options("${TARGET_NAME}" PRIVATE
      "LINKER:-object_path_lto,${BINARY_PATH}.lto.o")

    find_program(DSYMUTIL_EXECUTABLE dsymutil REQUIRED)
    add_custom_command(OUTPUT "${BINARY_PATH}.dSYM"
      COMMAND "${DSYMUTIL_EXECUTABLE}" "${BINARY_PATH}"
              -o "${BINARY_PATH}.dSYM"
      DEPENDS "${TARGET_NAME}"
      VERBATIM)
    add_custom_target("${TARGET_NAME}_debug_symbols" ALL
      DEPENDS "${BINARY_PATH}.dSYM")

    install(DIRECTORY "${BINARY_PATH}.dSYM"
      DESTINATION "${CMAKE_INSTALL_BINDIR}"
      COMPONENT "${EXTRACT_DEBUG_SYMBOLS_COMPONENT}")
  elseif(UNIX)
    add_custom_command(OUTPUT "${BINARY_PATH}.debug"
      COMMAND "${CMAKE_OBJCOPY}" --only-keep-debug
              "${BINARY_PATH}" "${BINARY_PATH}.debug"
      COMMAND "${CMAKE_OBJCOPY}" --strip-debug "${BINARY_PATH}"
      COMMAND "${CMAKE_OBJCOPY}"
              "--add-gnu-debuglink=${BINARY_PATH}.debug" "${BINARY_PATH}"
      DEPENDS "${TARGET_NAME}"
      VERBATIM)
    add_custom_target("${TARGET_NAME}_debug_symbols" ALL
      DEPENDS "${BINARY_PATH}.debug")

    install(FILES "${BINARY_PATH}.debug"
      DESTINATION "${CMAKE_INSTALL_BINDIR}"
      COMPONENT "${EXTRACT_DEBUG_SYMBOLS_COMPONENT}")
  endif()
endfunction()
