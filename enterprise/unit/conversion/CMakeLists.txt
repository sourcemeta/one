sourcemeta_test(NAMESPACE sourcemeta PROJECT one NAME enterprise_conversion
  SOURCES
    conversion_ladder_test.cc)

target_link_libraries(sourcemeta_one_enterprise_conversion_unit
  PRIVATE sourcemeta::one::enterprise_conversion)
