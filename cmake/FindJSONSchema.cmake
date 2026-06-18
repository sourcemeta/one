if(NOT JSONSchema_FOUND)
  if(ONE_ENTERPRISE)
    set(JSONSCHEMA_USE_SYSTEM_CURL ON CACHE BOOL "Enable system cURL")
  endif()

  add_subdirectory("${PROJECT_SOURCE_DIR}/vendor/jsonschema")
  set(JSONSchema_FOUND ON)
endif()
