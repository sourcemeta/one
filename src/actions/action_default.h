#ifndef SOURCEMETA_ONE_ACTIONS_DEFAULT_H
#define SOURCEMETA_ONE_ACTIONS_DEFAULT_H

#include <sourcemeta/one/http.h>

#include "action_jsonschema_serve.h"
#include "action_serve_metapack_file.h"

#include <filesystem> // std::filesystem

inline auto action_default(const std::filesystem::path &base,
                           sourcemeta::one::HTTPRequest &request,
                           sourcemeta::one::HTTPResponse &response) -> void {
  if (request.path() == "/") {
    if (request.prefers_html()) {
      action_serve_metapack_file(request, response,
                                 base / "explorer" / "%" /
                                     "directory-html.metapack",
                                 sourcemeta::one::STATUS_OK);
      return;
    } else if (request.method() == "get" || request.method() == "head") {
      sourcemeta::one::json_error(
          request, response, sourcemeta::one::STATUS_NOT_FOUND, "not-found",
          "There is nothing at this URL", "/self/v1/schemas/api/error");
      return;
    } else {
      sourcemeta::one::json_error(
          request, response, sourcemeta::one::STATUS_METHOD_NOT_ALLOWED,
          "method-not-allowed", "This HTTP method is invalid for this URL",
          "/self/v1/schemas/api/error");
      return;
    }
  }

  if (request.path().ends_with(".json")) {
    action_jsonschema_serve(base,
                            request.path().substr(1, request.path().size() - 6),
                            request, response);
    return;
  }

  const auto path{request.path().substr(1)};
  if (request.method() == "get" || request.method() == "head") {
    if (request.prefers_html()) {
      auto absolute_path{base / "explorer" / path / "%"};
      if (std::filesystem::exists(absolute_path / "schema-html.metapack") &&
          !path.ends_with("/")) {
        action_serve_metapack_file(request, response,
                                   absolute_path / "schema-html.metapack",
                                   sourcemeta::one::STATUS_OK);
      } else {
        absolute_path /= "directory-html.metapack";
        if (std::filesystem::exists(absolute_path)) {
          action_serve_metapack_file(request, response, absolute_path,
                                     sourcemeta::one::STATUS_OK);
        } else {
          action_serve_metapack_file(request, response,
                                     base / "explorer" / "%" / "404.metapack",
                                     sourcemeta::one::STATUS_NOT_FOUND);
        }
      }
    } else {
      action_jsonschema_serve(base, path, request, response);
    }
  } else {
    sourcemeta::one::json_error(
        request, response, sourcemeta::one::STATUS_NOT_FOUND, "not-found",
        "There is nothing at this URL", "/self/v1/schemas/api/error");
  }
}

#endif
