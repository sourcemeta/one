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

# Building the Enterprise edition for development, testing, evaluation,
# demonstration, and good-faith security research is permitted by
# LICENSE-COMMERCIAL without a commercial license. Production use is not.
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
find_package(JSONSchema REQUIRED)

include(GNUInstallDirs)

if(ONE_DEBUG_SYMBOLS)
  sourcemeta_debug_symbols_extract(jsonschema_cli
    COMPONENT sourcemeta_jsonschema)
endif()

# The revision this build came from, when anything knows it. An image build
# cannot consult git, as the context leaves it out, so continuous integration
# passes it in instead
set(ONE_REVISION "" CACHE STRING "The revision this build was produced from")
# Whoever hands one over may name it in full or in short, and the same
# revision has to read the same either way or it would look like another build
string(LENGTH "${ONE_REVISION}" ONE_REVISION_LENGTH)
if(ONE_REVISION_LENGTH GREATER 8)
  string(SUBSTRING "${ONE_REVISION}" 0 8 ONE_REVISION)
endif()
if(NOT ONE_REVISION)
  find_program(GIT_BIN NAMES git)
  if(GIT_BIN AND EXISTS "${PROJECT_SOURCE_DIR}/.git")
    execute_process(
      COMMAND "${GIT_BIN}" rev-parse --short=8 HEAD
      WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}"
      OUTPUT_VARIABLE ONE_REVISION
      OUTPUT_STRIP_TRAILING_WHITESPACE
      ERROR_QUIET)
  endif()
endif()

# A release says just its version, so what it is called carries no revision
if(CMAKE_BUILD_TYPE STREQUAL "Release")
  set(ONE_VERSION "${PROJECT_VERSION}")
elseif(ONE_REVISION)
  set(ONE_VERSION "${PROJECT_VERSION}+${ONE_REVISION}")
else()
  set(ONE_VERSION "${PROJECT_VERSION}+unknown")
endif()
message(STATUS "One version: ${ONE_VERSION}")

# What a build is called is a product decision. Whether artifacts an earlier
# build left behind can still be trusted is a different question, so it is
# answered separately and never shown. The version alone cannot tell two
# builds of the same release apart, which is what every image published
# between two releases is, so the dependency pins and the revision answer for
# the code that produced those artifacts
set(ONE_IDENTITY_TEXT "${ONE_PROJECT_VERSION}${ONE_REVISION}")
foreach(dependency_manifest IN ITEMS
    "${PROJECT_SOURCE_DIR}/vendor/core/DEPENDENCIES"
    "${PROJECT_SOURCE_DIR}/vendor/blaze/DEPENDENCIES"
    "${PROJECT_SOURCE_DIR}/vendor/jsonschema/DEPENDENCIES")
  if(EXISTS "${dependency_manifest}")
    # Reading a file does not make this run again when it changes, and an
    # identity left over from the pins of an earlier build vouches for
    # artifacts those produced
    set_property(DIRECTORY APPEND PROPERTY
      CMAKE_CONFIGURE_DEPENDS "${dependency_manifest}")
    file(READ "${dependency_manifest}" dependency_manifest_contents)
    string(APPEND ONE_IDENTITY_TEXT "${dependency_manifest_contents}")
  endif()
endforeach()
string(SHA256 ONE_BUILD_IDENTITY "${ONE_IDENTITY_TEXT}")
message(STATUS "One build identity: ${ONE_BUILD_IDENTITY}")

file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/VERSION" "${ONE_VERSION}")

if(ONE_INDEX OR ONE_SERVER)
  find_package(uSockets REQUIRED)
  find_package(uWebSockets REQUIRED)
  add_subdirectory(src/shared)
  add_subdirectory(src/authentication)
  add_subdirectory(src/search)
  add_subdirectory(src/http)
  add_subdirectory(src/router)

  if(ONE_ENTERPRISE)
    add_subdirectory(enterprise/conversion)
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
  add_subdirectory(ui)
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
  contrib/*.sh
  test/*.sh
  test/e2e/*/*.sh
  docker/*.sh
  enterprise/scripts/*.sh
  enterprise/e2e/*/*.sh
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
