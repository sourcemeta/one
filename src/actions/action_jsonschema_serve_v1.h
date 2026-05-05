#ifndef SOURCEMETA_ONE_ACTIONS_JSONSCHEMA_SERVE_V1_H
#define SOURCEMETA_ONE_ACTIONS_JSONSCHEMA_SERVE_V1_H

#include <sourcemeta/core/uritemplate.h>

#include <sourcemeta/one/actions.h>
#include <sourcemeta/one/http.h>
#include <sourcemeta/one/shared.h>

#include "action_serve_metapack_file_v1.h"

#include <algorithm>   // std::ranges::transform
#include <cctype>      // std::tolower
#include <filesystem>  // std::filesystem
#include <span>        // std::span
#include <string>      // std::string
#include <string_view> // std::string_view

class ActionJSONSchemaServe_v1 : public sourcemeta::one::Action {
public:
  ActionJSONSchemaServe_v1(
      const std::filesystem::path &base,
      const sourcemeta::core::URITemplateRouterView &router,
      const sourcemeta::core::URITemplateRouter::Identifier identifier)
      : sourcemeta::one::Action{base, router.base_path()} {
    router.arguments(identifier, [this](const auto &key, const auto &value) {
      if (key == "errorSchema") {
        this->error_schema_ = std::get<std::string_view>(value);
      }
    });
  }

  static auto resolve(const std::filesystem::path &base,
                      const std::string_view schema_path, const bool bundle)
      -> std::filesystem::path {
    // Otherwise we may get unexpected results in case-sensitive file-systems
    std::string lowercase_path{schema_path};
    std::ranges::transform(
        lowercase_path, lowercase_path.begin(),
        [](const unsigned char character) { return std::tolower(character); });
    auto absolute_path{base / "schemas" / lowercase_path / "%"};
    if (bundle) {
      absolute_path /= "bundle.metapack";
    } else {
      absolute_path /= "schema.metapack";
    }
    return absolute_path;
  }

  static auto serve(const std::filesystem::path &base,
                    std::string_view schema_path,
                    sourcemeta::one::HTTPRequest &request,
                    sourcemeta::one::HTTPResponse &response,
                    std::string_view error_schema) -> void {
    // Because Visual Studio Code famously does not support `$id` or `id`
    // See
    // https://github.com/microsoft/vscode-json-languageservice/issues/224
    const auto &user_agent{request.header("user-agent")};
    const auto is_vscode{user_agent.starts_with("Visual Studio Code") ||
                         user_agent.starts_with("VSCodium")};
    const auto is_deno{user_agent.starts_with("Deno/")};
    const auto bundle{!request.query("bundle").empty()};
    std::filesystem::path absolute_path;
    if (is_vscode) {
      std::string lowercase_path{schema_path};
      std::ranges::transform(lowercase_path, lowercase_path.begin(),
                             [](const unsigned char character) {
                               return std::tolower(character);
                             });
      absolute_path =
          base / "schemas" / lowercase_path / "%" / "editor.metapack";
    } else {
      absolute_path = resolve(base, schema_path, bundle || is_deno);
    }

    if (request.method() != "get" && request.method() != "head" &&
        !std::filesystem::exists(absolute_path)) {
      sourcemeta::one::json_error(
          request, response, sourcemeta::one::STATUS_NOT_FOUND, "not-found",
          "There is nothing at this URL", error_schema);
      return;
    }

    if (is_deno) {
      // For HTTP imports, as Deno won't like the `application/schema+json` one
      ActionServeMetapackFile_v1::serve(
          absolute_path, sourcemeta::one::STATUS_OK, true, "application/json",
          {}, request, response, error_schema);
    } else {
      ActionServeMetapackFile_v1::serve(absolute_path,
                                        sourcemeta::one::STATUS_OK, true, {},
                                        {}, request, response, error_schema);
    }
  }

  auto run(const std::span<std::string_view> matches,
           sourcemeta::one::HTTPRequest &request,
           sourcemeta::one::HTTPResponse &response) -> void override {
    serve(this->base(), matches.front(), request, response,
          this->error_schema_);
  }

private:
  std::string_view error_schema_;
};

#endif
