cmake_minimum_required(VERSION 3.16)
project(one VERSION 4.1.0 LANGUAGES C CXX)
list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")

# Options
option(ONE_TESTS "Build the One unit tests" ON)
option(ONE_SERVER "Build the One server" ON)
option(ONE_INDEX "Build the One index tool" ON)
set(ONE_PREFIX "/usr" CACHE STRING "Expected installation prefix")

cmake_path(IS_ABSOLUTE ONE_PREFIX ONE_PREFIX_IS_ABSOLUTE)
if(NOT ONE_PREFIX_IS_ABSOLUTE)
  message(FATAL_ERROR "ONE_PREFIX must be an absolute path but it was: ${ONE_PREFIX}")
endif()

# Commercial editions require a paid license
# See https://one.sourcemeta.com/commercial/
option(ONE_ENTERPRISE "Build the One Enterprise edition (commercial)" OFF)
if(ONE_ENTERPRISE)
  message(STATUS "Sourcemeta One edition: Enterprise")
else()
  message(STATUS "Sourcemeta One edition: Community")
endif()

find_package(Core REQUIRED)
find_package(Blaze REQUIRED)
find_package(Codegen REQUIRED)
find_package(Hydra REQUIRED)
find_package(JSONBinPack REQUIRED)
find_package(JSONSchema REQUIRED)

include(GNUInstallDirs)

# Always optimize the current architecture
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -march=native")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -mtune=native")

if(CMAKE_BUILD_TYPE STREQUAL "Release")
  set(ONE_VERSION "${PROJECT_VERSION}")
else()
  find_program(GIT_BIN NAMES git)
  if(GIT_BIN AND JSONSCHEMA_CONTINUOUS)
    execute_process(COMMAND "${GIT_BIN}" rev-parse --git-dir
      ERROR_QUIET
      OUTPUT_STRIP_TRAILING_WHITESPACE
      OUTPUT_VARIABLE GIT_DIR)
  endif()
  if(GIT_BIN AND EXISTS "${GIT_DIR}")
    execute_process(
      COMMAND "${GIT_BIN}" rev-parse --short=8 HEAD
      WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}"
      OUTPUT_VARIABLE PROJECT_GIT_SHA
      OUTPUT_STRIP_TRAILING_WHITESPACE
      COMMAND_ERROR_IS_FATAL ANY)
  else()
    set(PROJECT_GIT_SHA "unknown")
  endif()
  set(ONE_VERSION "${PROJECT_VERSION}+${PROJECT_GIT_SHA}")
endif()
message(STATUS "One version: ${ONE_VERSION}")

if(ONE_INDEX OR ONE_SERVER)
  add_subdirectory(src/shared)
  add_subdirectory(src/gzip)
endif()

if(ONE_INDEX)
  add_subdirectory(contrib/file2metapack)
  include(commands/file2metapack)
  include(commands/esbuild)
  add_subdirectory(src/configuration)
  add_subdirectory(src/resolver)
  add_subdirectory(src/web)
  add_subdirectory(src/index)
  add_subdirectory(collections)
  add_dependencies(sourcemeta_one_index
    sourcemeta_one_collections)
endif()

if(ONE_SERVER)
  find_package(uSockets REQUIRED)
  find_package(uWebSockets REQUIRED)
  add_subdirectory(src/server)
endif()

sourcemeta_target_clang_format(SOURCES src/*.h src/*.cc test/*.cc contrib/*.cc)
sourcemeta_target_shellcheck(SOURCES test/*.sh docker/*.sh)

set(SOURCEMETA_SCHEMAS "${PROJECT_SOURCE_DIR}/collections/self/v1/schemas")
add_custom_target(jsonschema_fmt_test
  COMMAND "$<TARGET_FILE:jsonschema_cli>" fmt --check ${SOURCEMETA_SCHEMAS})
add_custom_target(jsonschema_fmt
  COMMAND "$<TARGET_FILE:jsonschema_cli>" fmt ${SOURCEMETA_SCHEMAS})
add_custom_target(jsonschema_metaschema
  COMMAND "$<TARGET_FILE:jsonschema_cli>" metaschema ${SOURCEMETA_SCHEMAS})
add_custom_target(jsonschema_lint
  COMMAND "$<TARGET_FILE:jsonschema_cli>" lint
  ${SOURCEMETA_SCHEMAS})

if(ONE_TESTS)
  enable_testing()

  set(SCHEMA_TEST_DIRECTORIES "${PROJECT_SOURCE_DIR}/collections")

  if(ONE_ENTERPRISE)
    list(APPEND SCHEMA_TEST_DIRECTORIES "${PROJECT_SOURCE_DIR}/enterprise/collections")
  endif()

  add_test(NAME one.schemas COMMAND
    "$<TARGET_FILE:jsonschema_cli>" test --extension .test.json
    ${SCHEMA_TEST_DIRECTORIES})

  add_subdirectory(test/js)

  if(ONE_INDEX OR ONE_SERVER)
    add_subdirectory(test/unit/gzip)
  endif()

  if(ONE_INDEX)
    add_subdirectory(test/unit/configuration)
    add_subdirectory(test/unit/resolver)
  endif()
  add_subdirectory(test/cli)
endif()
