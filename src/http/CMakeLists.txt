sourcemeta_library(NAMESPACE sourcemeta PROJECT one NAME http
  PRIVATE_HEADERS uwebsockets.h request.h response.h helpers.h server.h)

target_link_libraries(sourcemeta_one_http INTERFACE sourcemeta::core::json)
target_link_libraries(sourcemeta_one_http INTERFACE sourcemeta::core::time)
target_link_libraries(sourcemeta_one_http INTERFACE sourcemeta::one::gzip)
target_link_libraries(sourcemeta_one_http INTERFACE sourcemeta::one::shared)
target_link_libraries(sourcemeta_one_http INTERFACE uNetworking::uSockets)
target_link_libraries(sourcemeta_one_http INTERFACE uNetworking::uWebSockets)
