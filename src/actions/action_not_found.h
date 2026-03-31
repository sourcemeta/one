#ifndef SOURCEMETA_ONE_ACTIONS_NOT_FOUND_H
#define SOURCEMETA_ONE_ACTIONS_NOT_FOUND_H

#include <sourcemeta/one/http.h>

inline auto action_not_found(sourcemeta::one::HTTPRequest &request,
                             sourcemeta::one::HTTPResponse &response) -> void {
  sourcemeta::one::json_error(
      request, response, sourcemeta::one::STATUS_NOT_FOUND, "not-found",
      "There is nothing at this URL", "/self/v1/schemas/api/error");
}

#endif
