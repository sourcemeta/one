set(ONE_COLLECTION_SOURCES)

include("./self/v1/one.cmake")

add_custom_target(sourcemeta_one_collections DEPENDS ${ONE_COLLECTION_SOURCES})
