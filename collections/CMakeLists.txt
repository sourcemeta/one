set(ONE_COLLECTION_SOURCES)

if(ONE_ENTERPRISE)
  include("../enterprise/collections/sourcemeta/std/v0/one.cmake")
endif()

include("./self/v1/one.cmake")

add_custom_target(sourcemeta_one_collections DEPENDS ${ONE_COLLECTION_SOURCES})
