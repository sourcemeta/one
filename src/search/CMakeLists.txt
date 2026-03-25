sourcemeta_library(NAMESPACE sourcemeta PROJECT one NAME search
  SOURCES search.cc)

target_link_libraries(sourcemeta_one_search PUBLIC sourcemeta::core::json)
target_link_libraries(sourcemeta_one_search PUBLIC sourcemeta::core::io)
target_link_libraries(sourcemeta_one_search PRIVATE sourcemeta::one::metapack)
