#ifndef SOURCEMETA_ONE_ACTIONS_DEFAULT_H
#define SOURCEMETA_ONE_ACTIONS_DEFAULT_H

#include <sourcemeta/core/uritemplate.h>

#include <sourcemeta/one/actions.h>
#include <sourcemeta/one/http.h>

#include "action_jsonschema_serve.h"
#include "action_serve_metapack_file.h"

#include <filesystem>  // std::filesystem
#include <span>        // std::span
#include <string>      // std::string
#include <string_view> // std::string_view

class ActionDefault : public sourcemeta::one::Action {
public:
  ActionDefault(const std::filesystem::path &base,
                const sourcemeta::core::URITemplateRouterView &router,
                const sourcemeta::core::URITemplateRouter::Identifier)
      : sourcemeta::one::Action{base, router.base_path()} {}

  auto run(const std::span<std::string_view>,
           sourcemeta::one::HTTPRequest &request,
           sourcemeta::one::HTTPResponse &response) -> void override {
    auto path{request.path()};
    if (!this->base_path().empty()) {
      if (!path.starts_with(this->base_path()) ||
          (path.size() > this->base_path().size() &&
           path[this->base_path().size()] != '/')) {
        sourcemeta::one::json_error(
            request, response, sourcemeta::one::STATUS_NOT_FOUND, "not-found",
            "There is nothing at this URL",
            std::string{this->base_path()} + "/self/v1/schemas/api/error");
        return;
      }

      path = path.substr(this->base_path().size());
    }

    if (path.starts_with('/')) {
      path = path.substr(1);
    }

    if (path.empty()) {
      if (request.prefers_html()) {
        ActionServeMetapackFile::serve(
            this->base() / "explorer" / "%" / "directory-html.metapack",
            sourcemeta::one::STATUS_OK, false, {}, {}, request, response,
            this->base_path());
        return;
      } else if (request.method() == "get" || request.method() == "head") {
        sourcemeta::one::json_error(
            request, response, sourcemeta::one::STATUS_NOT_FOUND, "not-found",
            "There is nothing at this URL",
            std::string{this->base_path()} + "/self/v1/schemas/api/error");
        return;
      } else {
        sourcemeta::one::json_error(
            request, response, sourcemeta::one::STATUS_METHOD_NOT_ALLOWED,
            "method-not-allowed", "This HTTP method is invalid for this URL",
            std::string{this->base_path()} + "/self/v1/schemas/api/error");
        return;
      }
    }

    if (path.ends_with(".json")) {
      ActionJSONSchemaServe::serve(this->base(),
                                   path.substr(0, path.size() - 5), request,
                                   response, this->base_path());
      return;
    }

    if (request.method() == "get" || request.method() == "head") {
      if (request.prefers_html()) {
        auto explorer_path{this->base() / "explorer" / std::string{path} / "%"};
        if (std::filesystem::exists(explorer_path / "schema-html.metapack") &&
            !path.ends_with("/")) {
          ActionServeMetapackFile::serve(explorer_path / "schema-html.metapack",
                                         sourcemeta::one::STATUS_OK, false, {},
                                         {}, request, response,
                                         this->base_path());
        } else {
          explorer_path /= "directory-html.metapack";
          if (std::filesystem::exists(explorer_path)) {
            ActionServeMetapackFile::serve(
                explorer_path, sourcemeta::one::STATUS_OK, false, {}, {},
                request, response, this->base_path());
          } else {
            ActionServeMetapackFile::serve(
                this->base() / "explorer" / "%" / "404.metapack",
                sourcemeta::one::STATUS_NOT_FOUND, false, {}, {}, request,
                response, this->base_path());
          }
        }
      } else {
        ActionJSONSchemaServe::serve(this->base(), path, request, response,
                                     this->base_path());
      }
    } else {
      sourcemeta::one::json_error(
          request, response, sourcemeta::one::STATUS_NOT_FOUND, "not-found",
          "There is nothing at this URL",
          std::string{this->base_path()} + "/self/v1/schemas/api/error");
    }
  }
};

#endif
