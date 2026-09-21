find_program(NPM_BIN NAMES npm REQUIRED)

# The dependencies are installed by whoever drives the build, the same way the
# ones at the top of the repository are, so that configuring stays free of the
# network. Saying so here turns a missing tree into one clear line rather than
# whatever the bundler would have said about it
if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/node_modules")
  message(FATAL_ERROR
    "The web UI dependencies are not installed. Run `npm ci` in ${CMAKE_CURRENT_SOURCE_DIR}")
endif()

# A bundler decides what to emit from everything it reads, so what it reads has
# to be named for the build system rather than worked out from what came out.
# The glob is re-evaluated on every build, which is what makes adding or
# deleting a component rebuild the UI instead of quietly keeping the old output
file(GLOB_RECURSE ONE_UI_SOURCES CONFIGURE_DEPENDS
  "${CMAKE_CURRENT_SOURCE_DIR}/src/*"
  "${CMAKE_CURRENT_SOURCE_DIR}/public/*")

# What configures the bundler itself. A change to any of these moves the output
# as much as a change to a component does
list(APPEND ONE_UI_SOURCES
  "${CMAKE_CURRENT_SOURCE_DIR}/index.html"
  "${CMAKE_CURRENT_SOURCE_DIR}/package.json"
  "${CMAKE_CURRENT_SOURCE_DIR}/package-lock.json"
  "${CMAKE_CURRENT_SOURCE_DIR}/tsconfig.json"
  "${CMAKE_CURRENT_SOURCE_DIR}/tsconfig.app.json"
  "${CMAKE_CURRENT_SOURCE_DIR}/tsconfig.node.json"
  "${CMAKE_CURRENT_SOURCE_DIR}/vite.config.ts")

set(ONE_UI_DIST "${CMAKE_CURRENT_BINARY_DIR}/dist")
set(ONE_UI_METAPACK "${CMAKE_CURRENT_BINARY_DIR}/metapack")
set(ONE_UI_STAMP "${CMAKE_CURRENT_BINARY_DIR}/ui.stamp")
set(ONE_UI_CONVERT "${CMAKE_CURRENT_BINARY_DIR}/metapack.cmake")

# Unlike the static assets next door, none of these names can be spelled out
# when the build is configured, because a bundler names what it emits after the
# contents it produced. So the walk is written out here and run at build time,
# once the names exist. Everything it varies on reaches it as a definition,
# which is why nothing below is expanded while it is being written
file(GENERATE OUTPUT "${ONE_UI_CONVERT}" CONTENT [=[
# A name carries a content hash, so a file whose contents changed leaves the
# name it had before behind. Clearing the tree is what stops an asset nothing
# points at any more from being converted for good
file(REMOVE_RECURSE "${OUTPUT}")
file(MAKE_DIRECTORY "${OUTPUT}")

file(GLOB_RECURSE ONE_UI_ENTRIES RELATIVE "${INPUT}" "${INPUT}/*")

foreach(entry IN LISTS ONE_UI_ENTRIES)
  # The shortest extension, as a bundler puts its hash between the stem and the
  # extension, which makes the longest extension of `json.worker-A1B2C3.js`
  # everything past the first dot
  get_filename_component(file_extension "${entry}" LAST_EXT)
  string(TOLOWER "${file_extension}" file_extension)

  if(file_extension STREQUAL ".html")
    # The pages the index writes name the encoding, and a browser left to
    # guess at it is a browser that can guess wrong
    set(MIME "text/html; charset=utf-8")
  elseif(file_extension STREQUAL ".js")
    set(MIME "text/javascript")
  elseif(file_extension STREQUAL ".css")
    set(MIME "text/css")
  elseif(file_extension STREQUAL ".json")
    set(MIME "application/json")
  elseif(file_extension STREQUAL ".webmanifest")
    set(MIME "application/manifest+json")
  elseif(file_extension STREQUAL ".svg")
    set(MIME "image/svg+xml")
  elseif(file_extension STREQUAL ".png")
    set(MIME "image/png")
  elseif(file_extension STREQUAL ".ico")
    set(MIME "image/vnd.microsoft.icon")
  elseif(file_extension STREQUAL ".woff2")
    set(MIME "font/woff2")
  elseif(file_extension STREQUAL ".woff")
    set(MIME "font/woff")
  elseif(file_extension STREQUAL ".ttf")
    set(MIME "font/ttf")
  else()
    # Refusing what this does not recognise is what makes a bundler that starts
    # emitting a new kind of asset a build failure rather than a file the
    # server later serves as something it is not
    message(FATAL_ERROR "Cannot determine MIME type for ${entry}")
  endif()

  execute_process(
    COMMAND "${FILE2METAPACK}" --gzip
      "${INPUT}/${entry}" "${MIME}" "${OUTPUT}/${entry}"
    RESULT_VARIABLE result)
  if(NOT result EQUAL 0)
    message(FATAL_ERROR "Could not convert ${entry} into a metapack file")
  endif()
endforeach()
]=])

# One stamp stands for the whole build, since a bundler decides on its own how
# to divide what it reads into what it writes. There is no finer unit to depend
# on than the tree it was given
add_custom_command(OUTPUT "${ONE_UI_STAMP}"
  # Emitting into the build directory leaves the checkout clean and lets two
  # build directories of it hold different outputs at once
  COMMAND "${NPM_BIN}" run build --
    --outDir "${ONE_UI_DIST}" --emptyOutDir
  COMMAND "${CMAKE_COMMAND}"
    -DINPUT=${ONE_UI_DIST}
    -DOUTPUT=${ONE_UI_METAPACK}
    -DFILE2METAPACK=$<TARGET_FILE:sourcemeta_one_file2metapack>
    -P "${ONE_UI_CONVERT}"
  COMMAND "${CMAKE_COMMAND}" -E touch "${ONE_UI_STAMP}"
  WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
  DEPENDS ${ONE_UI_SOURCES} "${ONE_UI_CONVERT}"
    "$<TARGET_FILE:sourcemeta_one_file2metapack>"
  COMMENT "Building the experimental web UI"
  VERBATIM)

add_custom_target(sourcemeta_one_ui ALL DEPENDS "${ONE_UI_STAMP}")

set(ONE_UI_INSTALL_DIR "${CMAKE_INSTALL_DATADIR}/sourcemeta/one/ui")

# Installing a tree only ever adds to what is already there, and an asset that
# changed arrives under a new name rather than over the old one. Emptying the
# directory first is what keeps an install from carrying every asset the build
# has ever produced. It names the UI directory alone, so nothing else under the
# prefix is within its reach
install(CODE
  "file(REMOVE_RECURSE \"\$ENV{DESTDIR}\${CMAKE_INSTALL_PREFIX}/${ONE_UI_INSTALL_DIR}\")"
  COMPONENT sourcemeta_one)

# The names the bundler emits carry a content hash, so which files to install
# cannot be written down here. Installing the directory leaves that to install
# time, when what the build produced is what is on disk
install(DIRECTORY "${ONE_UI_METAPACK}/"
  DESTINATION "${ONE_UI_INSTALL_DIR}"
  COMPONENT sourcemeta_one)
