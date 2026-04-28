cmake_minimum_required(VERSION 3.16)
project(one VERSION 5.3.1 LANGUAGES C CXX)
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
  set(ONE_EDITION "Enterprise")
else()
  set(ONE_EDITION "Community")
endif()
message(STATUS "Sourcemeta One edition: ${ONE_EDITION}")

find_package(Core REQUIRED)
find_package(Blaze REQUIRED)
find_package(JSONBinPack REQUIRED)
find_program(JSONSCHEMA_BIN NAMES jsonschema
  PATHS "${PROJECT_SOURCE_DIR}/node_modules/.bin"
  NO_DEFAULT_PATH REQUIRED)

include(GNUInstallDirs)

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
file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/VERSION" "${ONE_VERSION}")

if(ONE_INDEX OR ONE_SERVER)
  find_package(uSockets REQUIRED)
  find_package(uWebSockets REQUIRED)
  add_subdirectory(src/shared)
  add_subdirectory(src/metapack)
  add_subdirectory(src/search)
  add_subdirectory(src/http)
  add_subdirectory(src/actions)
endif()

if(ONE_INDEX)
  add_subdirectory(contrib/file2metapack)
  include(commands/file2metapack)
  include(commands/esbuild)
  add_subdirectory(src/build)
  add_subdirectory(src/configuration)
  add_subdirectory(src/resolver)
  add_subdirectory(src/web)
  add_subdirectory(src/index)
  add_subdirectory(src/self)

  if(ONE_ENTERPRISE)
    add_subdirectory(enterprise/index)
  endif()
endif()

if(ONE_SERVER)
  add_subdirectory(src/server)
endif()

sourcemeta_target_clang_format(SOURCES
  src/*.h src/*.cc
  contrib/*.cc
  test/*.h test/*.cc
  enterprise/*.h enterprise/*.cc)
sourcemeta_target_shellcheck(SOURCES
  test/*.sh docker/*.sh enterprise/scripts/*.sh benchmark/*.sh)

if(ONE_TESTS)
  enable_testing()

  add_subdirectory(test/js)

  if(ONE_INDEX OR ONE_SERVER)
    add_subdirectory(test/unit/metapack)
    add_subdirectory(test/unit/search)
  endif()

  if(ONE_INDEX)
    add_subdirectory(test/unit/build)
    add_subdirectory(test/unit/configuration)
    add_subdirectory(test/unit/resolver)
    add_subdirectory(test/unit/actions)
    add_subdirectory(test/unit/self)
  endif()
  add_subdirectory(test/cli)
endif()
