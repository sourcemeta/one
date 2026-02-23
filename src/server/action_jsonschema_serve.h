#ifndef SOURCEMETA_ONE_SERVER_ACTION_JSONSCHEMA_SERVE_H
#define SOURCEMETA_ONE_SERVER_ACTION_JSONSCHEMA_SERVE_H

#include <sourcemeta/one/shared.h>
#include <sourcemeta/one/storage.h>

#include "action_serve_metapack_file.h"
#include "helpers.h"
#include "request.h"
#include "response.h"

#include <algorithm>   // std::transform
#include <cctype>      // std::tolower
#include <string>      // std::string
#include <string_view> // std::string_view

static auto action_jsonschema_serve(const sourcemeta::one::Storage &storage,
                                    const std::string_view &path,
                                    sourcemeta::one::HTTPRequest &request,
                                    sourcemeta::one::HTTPResponse &response)
    -> void {
  // Otherwise we may get unexpected results in case-sensitive file-systems
  std::string lowercase_path{path};
  std::transform(
      lowercase_path.begin(), lowercase_path.end(), lowercase_path.begin(),
      [](const unsigned char character) { return std::tolower(character); });

  // Because Visual Studio Code famously does not support `$id` or `id`
  // See
  // https://github.com/microsoft/vscode-json-languageservice/issues/224
  const auto &user_agent{request.header("user-agent")};
  const auto is_vscode{user_agent.starts_with("Visual Studio Code") ||
                       user_agent.starts_with("VSCodium")};
  const auto is_deno{user_agent.starts_with("Deno/")};
  const auto bundle{!request.query("bundle").empty()};
  std::string filename;
  if (is_vscode) {
    filename = "editor.metapack";
  } else if (bundle || is_deno) {
    filename = "bundle.metapack";
  } else {
    filename = "schema.metapack";
  }

  const auto key{sourcemeta::one::Storage::key("schemas", lowercase_path,
                                               SENTINEL, filename)};

  if (request.method() != "get" && request.method() != "head" &&
      !storage.exists(key)) {
    json_error(request, response, sourcemeta::one::STATUS_NOT_FOUND,
               "not-found", "There is nothing at this URL");
    return;
  }

  if (is_deno) {
    action_serve_metapack_file(storage, request, response, key,
                               sourcemeta::one::STATUS_OK, true,
                               // For HTTP imports, as Deno won't like the
                               // `application/schema+json` one
                               "application/json");
  } else {
    action_serve_metapack_file(storage, request, response, key,
                               sourcemeta::one::STATUS_OK, true);
  }
}

#endif
