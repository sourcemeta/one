#ifndef SOURCEMETA_ONE_ACTIONS_DEFAULT_V1_H
#define SOURCEMETA_ONE_ACTIONS_DEFAULT_V1_H

#include <sourcemeta/core/json.h>
#include <sourcemeta/core/jsonrpc.h>
#include <sourcemeta/core/uri.h>
#include <sourcemeta/core/uritemplate.h>

#include <sourcemeta/one/http.h>
#include <sourcemeta/one/router.h>

#include "action_jsonschema_serve_v1.h"
#include "action_serve_metapack_file_v1.h"

#include <filesystem>  // std::filesystem
#include <span>        // std::span
#include <string>      // std::string
#include <string_view> // std::string_view

class ActionDefault_v1 : public sourcemeta::one::RouterAction {
public:
  static constexpr std::string_view DESCRIPTION{
      "Default fallback action for unmatched URIs"};
  static constexpr bool READ_ONLY{true};
  static constexpr bool DESTRUCTIVE{false};
  static constexpr bool IDEMPOTENT{true};
  static constexpr bool OPEN_WORLD{false};

  ActionDefault_v1(
      const std::filesystem::path &base,
      const sourcemeta::core::URITemplateRouterView &router,
      const sourcemeta::core::URITemplateRouter::Identifier identifier,
      sourcemeta::one::Router &)
      : sourcemeta::one::RouterAction{base, router.base_path(),
                                      router.base_url()} {
    router.arguments(identifier, [this](const auto &key, const auto &value) {
      if (key == "errorSchema") {
        this->error_schema_ = std::get<std::string_view>(value);
      }
    });
  }

  auto rest(const std::span<std::string_view>,
            sourcemeta::one::HTTPRequest &request,
            sourcemeta::one::HTTPResponse &response) -> void override {
    const auto stripped{sourcemeta::core::URI::strip_path_prefix(
        request.path(), this->base_path())};
    if (!stripped.has_value()) {
      sourcemeta::one::json_error(
          request, response, sourcemeta::one::STATUS_NOT_FOUND, "not-found",
          "There is nothing at this URL", this->error_schema_);
      return;
    }

    const auto &path{stripped.value()};

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
      ActionJSONSchemaServe_v1::serve(*this, path.substr(0, path.size() - 5),
                                      request, response, this->error_schema_);
      return;
    }

    if (request.method() == "get" || request.method() == "head") {
      if (request.prefers_html()) {
        auto explorer_path{this->base() / "explorer" / path / "%"};
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
        ActionJSONSchemaServe_v1::serve(*this, path, request, response,
                                        this->error_schema_);
      }
    } else {
      sourcemeta::one::json_error(
          request, response, sourcemeta::one::STATUS_NOT_FOUND, "not-found",
          "There is nothing at this URL", this->error_schema_);
    }
  }

  auto mcp(const sourcemeta::one::MCPProtocolVersion,
           const sourcemeta::core::JSON &id, const sourcemeta::core::JSON &,
           const std::string_view) -> sourcemeta::core::JSON override {
    return sourcemeta::core::jsonrpc_make_error_method_not_found(id);
  }

private:
  std::string_view error_schema_;
};

#endif
