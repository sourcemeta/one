sourcemeta_library(NAMESPACE sourcemeta PROJECT one NAME authentication
  SOURCES authentication.cc authentication_save.cc)

target_link_libraries(sourcemeta_one_authentication PUBLIC sourcemeta::core::io)

if(ONE_ENTERPRISE)
  target_link_libraries(sourcemeta_one_authentication PRIVATE sourcemeta::one::enterprise_authentication)
endif()
