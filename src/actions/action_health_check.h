#ifndef SOURCEMETA_ONE_ACTIONS_HEALTH_CHECK_H
#define SOURCEMETA_ONE_ACTIONS_HEALTH_CHECK_H

#include <sourcemeta/one/http.h>

inline auto action_health_check(sourcemeta::one::HTTPRequest &request,
                                sourcemeta::one::HTTPResponse &response)
    -> void {
  if (request.method() != "get" && request.method() != "head") {
    sourcemeta::one::json_error(
        request, response, sourcemeta::one::STATUS_METHOD_NOT_ALLOWED,
        "method-not-allowed", "This HTTP method is invalid for this URL",
        "/self/v1/schemas/api/error");
    return;
  }

  response.write_status(sourcemeta::one::STATUS_OK);
  response.write_header("Access-Control-Allow-Origin", "*");
  sourcemeta::one::send_response(sourcemeta::one::STATUS_OK, request, response);
}

#endif
