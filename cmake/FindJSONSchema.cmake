if(NOT JSONSchema_FOUND)
  if(ONE_ENTERPRISE)
    set(JSONSCHEMA_USE_SYSTEM_CURL ON CACHE BOOL "Enable system cURL")
  endif()

  # For maximum processor compatibility
  set(JSONSCHEMA_PORTABLE ON CACHE BOOL "Build a portable JSON Schema CLI binary")

  add_subdirectory("${PROJECT_SOURCE_DIR}/vendor/jsonschema")
  set(JSONSchema_FOUND ON)
endif()
