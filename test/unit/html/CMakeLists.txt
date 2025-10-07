sourcemeta_googletest(NAMESPACE sourcemeta PROJECT one NAME html
  SOURCES html_test.cc html_escape_test.cc)

target_link_libraries(sourcemeta_one_html_unit PRIVATE sourcemeta::one::html)
