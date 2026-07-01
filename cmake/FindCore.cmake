if(NOT Core_FOUND)
  set(SOURCEMETA_CORE_INSTALL OFF CACHE BOOL "disable installation")

  if(ONE_ENTERPRISE)
    set(SOURCEMETA_CORE_CRYPTO_USE_SYSTEM_OPENSSL ON CACHE BOOL "Enable OpenSSL")
    set(SOURCEMETA_CORE_HTTP_USE_SYSTEM_CURL ON CACHE BOOL "Enable system cURL")
  endif()

  set(SOURCEMETA_CORE_CONTRIB_GOOGLEBENCHMARK OFF CACHE BOOL "GoogleBenchmark")

  add_subdirectory("${PROJECT_SOURCE_DIR}/vendor/core")
  include(Sourcemeta)
  set(Core_FOUND ON)
endif()
