#ifndef SOURCEMETA_ONE_ACTIONS_DEFAULT_H
#define SOURCEMETA_ONE_ACTIONS_DEFAULT_H

#include <sourcemeta/core/uritemplate.h>

#include <sourcemeta/one/http.h>

#include "action_jsonschema_serve.h"
#include "action_serve_metapack_file.h"

#include <filesystem>  // std::filesystem
#include <span>        // std::span
#include <string_view> // std::string_view

class ActionDefault {
public:
  ActionDefault(const std::filesystem::path &base,
                const sourcemeta::core::URITemplateRouterView &,
                const sourcemeta::core::URITemplateRouter::Identifier)
      : base_{base} {}

  auto run(const std::span<std::string_view>,
           sourcemeta::one::HTTPRequest &request,
           sourcemeta::one::HTTPResponse &response) -> void {
    if (request.path() == "/") {
      if (request.prefers_html()) {
        ActionServeMetapackFile::serve(
            this->base_ / "explorer" / "%" / "directory-html.metapack",
            sourcemeta::one::STATUS_OK, false, {}, {}, request, response);
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
      ActionJSONSchemaServe::serve(
          this->base_, request.path().substr(1, request.path().size() - 6),
          request, response);
      return;
    }

    const auto url_path{request.path().substr(1)};
    if (request.method() == "get" || request.method() == "head") {
      if (request.prefers_html()) {
        auto explorer_path{this->base_ / "explorer" / url_path / "%"};
        if (std::filesystem::exists(explorer_path / "schema-html.metapack") &&
            !url_path.ends_with("/")) {
          ActionServeMetapackFile::serve(explorer_path / "schema-html.metapack",
                                         sourcemeta::one::STATUS_OK, false, {},
                                         {}, request, response);
        } else {
          explorer_path /= "directory-html.metapack";
          if (std::filesystem::exists(explorer_path)) {
            ActionServeMetapackFile::serve(explorer_path,
                                           sourcemeta::one::STATUS_OK, false,
                                           {}, {}, request, response);
          } else {
            ActionServeMetapackFile::serve(this->base_ / "explorer" / "%" /
                                               "404.metapack",
                                           sourcemeta::one::STATUS_NOT_FOUND,
                                           false, {}, {}, request, response);
          }
        }
      } else {
        ActionJSONSchemaServe::serve(this->base_, url_path, request, response);
      }
    } else {
      sourcemeta::one::json_error(
          request, response, sourcemeta::one::STATUS_NOT_FOUND, "not-found",
          "There is nothing at this URL", "/self/v1/schemas/api/error");
    }
  }

private:
  // NOLINTNEXTLINE(cppcoreguidelines-avoid-const-or-ref-data-members)
  const std::filesystem::path &base_;
};

#endif
