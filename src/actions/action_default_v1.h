#ifndef SOURCEMETA_ONE_ACTIONS_DEFAULT_V1_H
#define SOURCEMETA_ONE_ACTIONS_DEFAULT_V1_H

#include <sourcemeta/core/uritemplate.h>

#include <sourcemeta/one/actions.h>
#include <sourcemeta/one/http.h>

#include "action_jsonschema_serve_v1.h"
#include "action_serve_metapack_file_v1.h"

#include <filesystem>  // std::filesystem
#include <span>        // std::span
#include <string>      // std::string
#include <string_view> // std::string_view

class ActionDefault_v1 : public sourcemeta::one::Action {
public:
  ActionDefault_v1(
      const std::filesystem::path &base,
      const sourcemeta::core::URITemplateRouterView &router,
      const sourcemeta::core::URITemplateRouter::Identifier identifier)
      : sourcemeta::one::Action{base, router.base_path()} {
    // TODO: This implies the API is mounted
    this->error_schema_ =
        std::string{this->base_path()} + "/self/v1/schemas/api/error";

    router.arguments(identifier, [this](const auto &key, const auto &value) {
      if (key == "errorSchema") {
        this->error_schema_ = std::get<std::string_view>(value);
      }
    });
  }

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
            "There is nothing at this URL", this->error_schema_);
        return;
      }

      path = path.substr(this->base_path().size());
    }

    if (path.starts_with('/')) {
      path = path.substr(1);
    }

    if (path.empty()) {
      if (request.prefers_html()) {
        ActionServeMetapackFile_v1::serve(
            this->base() / "explorer" / "%" / "directory-html.metapack",
            sourcemeta::one::STATUS_OK, false, {}, {}, request, response,
            this->error_schema_);
        return;
      } else if (request.method() == "get" || request.method() == "head") {
        sourcemeta::one::json_error(
            request, response, sourcemeta::one::STATUS_NOT_FOUND, "not-found",
            "There is nothing at this URL", this->error_schema_);
        return;
      } else {
        sourcemeta::one::json_error(
            request, response, sourcemeta::one::STATUS_METHOD_NOT_ALLOWED,
            "method-not-allowed", "This HTTP method is invalid for this URL",
            this->error_schema_);
        return;
      }
    }

    if (path.ends_with(".json")) {
      ActionJSONSchemaServe_v1::serve(this->base(),
                                      path.substr(0, path.size() - 5), request,
                                      response, this->error_schema_);
      return;
    }

    if (request.method() == "get" || request.method() == "head") {
      if (request.prefers_html()) {
        auto explorer_path{this->base() / "explorer" / std::string{path} / "%"};
        if (std::filesystem::exists(explorer_path / "schema-html.metapack") &&
            !path.ends_with("/")) {
          ActionServeMetapackFile_v1::serve(
              explorer_path / "schema-html.metapack",
              sourcemeta::one::STATUS_OK, false, {}, {}, request, response,
              this->error_schema_);
        } else {
          explorer_path /= "directory-html.metapack";
          if (std::filesystem::exists(explorer_path)) {
            ActionServeMetapackFile_v1::serve(
                explorer_path, sourcemeta::one::STATUS_OK, false, {}, {},
                request, response, this->error_schema_);
          } else {
            ActionServeMetapackFile_v1::serve(
                this->base() / "explorer" / "%" / "404.metapack",
                sourcemeta::one::STATUS_NOT_FOUND, false, {}, {}, request,
                response, this->error_schema_);
          }
        }
      } else {
        ActionJSONSchemaServe_v1::serve(this->base(), path, request, response,
                                        this->error_schema_);
      }
    } else {
      sourcemeta::one::json_error(
          request, response, sourcemeta::one::STATUS_NOT_FOUND, "not-found",
          "There is nothing at this URL", this->error_schema_);
    }
  }

private:
  // TODO: This should be a string view
  std::string error_schema_;
};

#endif
