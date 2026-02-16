if(NOT Hydra_FOUND)
  set(SOURCEMETA_HYDRA_INSTALL OFF CACHE BOOL "disable installation")

  if(ONE_ENTERPRISE)
    set(SOURCEMETA_HYDRA_USE_SYSTEM_OPENSSL ON CACHE BOOL "Enable OpenSSL")
  endif()

  add_subdirectory("${PROJECT_SOURCE_DIR}/vendor/hydra")
  set(Hydra_FOUND ON)
endif()
