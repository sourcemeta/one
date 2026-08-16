if(NOT JSONSchema_FOUND)
  if(ONE_ENTERPRISE)
    set(JSONSCHEMA_USE_SYSTEM_CURL ON CACHE BOOL "Enable system cURL")
  endif()

  # Otherwise the CLI compiles with `-march=native`, which targets whichever
  # CPU happened to build it rather than the ones our images run on. It also
  # defeats `ccache`, as the hash covers the literal flag and not the features
  # it resolves to, so an object built where AVX-512 exists is reused where it
  # does not and the binary dies on an illegal instruction
  set(JSONSCHEMA_PORTABLE ON CACHE BOOL "Build a portable JSON Schema CLI binary")

  add_subdirectory("${PROJECT_SOURCE_DIR}/vendor/jsonschema")
  set(JSONSchema_FOUND ON)
endif()
