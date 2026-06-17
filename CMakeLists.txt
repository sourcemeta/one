cmake_minimum_required(VERSION 3.16)
file(READ "${CMAKE_CURRENT_LIST_DIR}/VERSION" ONE_PROJECT_VERSION)
set_property(DIRECTORY APPEND PROPERTY
  CMAKE_CONFIGURE_DEPENDS "${CMAKE_CURRENT_LIST_DIR}/VERSION")
string(STRIP "${ONE_PROJECT_VERSION}" ONE_PROJECT_VERSION)
project(one VERSION "${ONE_PROJECT_VERSION}" LANGUAGES C CXX)
list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")

include(DebugSymbols)
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
  set(ONE_DEBUG_SYMBOLS_DEFAULT OFF)
else()
  set(ONE_DEBUG_SYMBOLS_DEFAULT ON)
endif()
option(ONE_DEBUG_SYMBOLS "Compile and extract debug symbols" ${ONE_DEBUG_SYMBOLS_DEFAULT})
if(ONE_DEBUG_SYMBOLS)
  sourcemeta_debug_symbols_enable()
endif()

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
  add_subdirectory(src/authentication)
  add_subdirectory(src/search)
  add_subdirectory(src/http)
  add_subdirectory(src/router)

  if(ONE_ENTERPRISE)
    add_subdirectory(enterprise/server)
  endif()

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

  if(ONE_DEBUG_SYMBOLS)
    sourcemeta_debug_symbols_extract(sourcemeta_one_index
      COMPONENT sourcemeta_one)
  endif()
endif()

if(ONE_SERVER)
  add_subdirectory(src/server)
  if(ONE_DEBUG_SYMBOLS)
    sourcemeta_debug_symbols_extract(sourcemeta_one_server
      COMPONENT sourcemeta_one)
  endif()
endif()

sourcemeta_target_clang_format(SOURCES
  src/*.h src/*.cc
  contrib/*.cc
  test/*.h test/*.cc
  enterprise/*.h enterprise/*.cc)
sourcemeta_target_shellcheck(SOURCES
  release.sh
  test/*.sh
  docker/*.sh
  enterprise/scripts/*.sh
  benchmark/*.sh)

if(ONE_TESTS)
  enable_testing()

  add_subdirectory(test/js)

  if(ONE_INDEX OR ONE_SERVER)
    if(ONE_ENTERPRISE)
      add_subdirectory(enterprise/unit/authentication)
    else()
      add_subdirectory(test/unit/authentication)
    endif()
    add_subdirectory(test/unit/metapack)
    add_subdirectory(test/unit/router)
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
