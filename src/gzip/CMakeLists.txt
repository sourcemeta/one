sourcemeta_library(NAMESPACE sourcemeta PROJECT one NAME gzip
  PRIVATE_HEADERS error.h
  SOURCES gzip.cc)

target_link_libraries(sourcemeta_one_gzip PRIVATE ZLIB::ZLIB)
