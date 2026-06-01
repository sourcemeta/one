#ifndef SOURCEMETA_ONE_ACTIONS_DEFAULT_V1_H
#define SOURCEMETA_ONE_ACTIONS_DEFAULT_V1_H

#include <sourcemeta/core/json.h>
#include <sourcemeta/core/jsonrpc.h>
#include <sourcemeta/core/mcp.h>
#include <sourcemeta/core/text.h>
#include <sourcemeta/core/uri.h>
#include <sourcemeta/core/uritemplate.h>

#include <sourcemeta/one/http.h>
#include <sourcemeta/one/router.h>

#include "action_jsonschema_serve_v1.h"

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
      sourcemeta::one::Router &dispatcher)
      : sourcemeta::one::RouterAction{base, router.base_path(),
                                      router.base_url(), dispatcher} {
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
        request.path(), this->server_uri_base_path())};
    if (!stripped.has_value()) {
      sourcemeta::one::json_error(
          request, response, sourcemeta::one::STATUS_NOT_FOUND, "not-found",
          "There is nothing at this URL", this->error_schema_);
      return;
    }

    const auto &path{stripped.value()};

    if (path.empty()) {
      if (request.prefers_html()) {
        const auto root_html{
            this->artifact_resolve_path("", Tree::Explorer, "directory-html")};
        if (root_html.has_value()) {
          this->artifact_serve(root_html.value(), sourcemeta::one::STATUS_OK,
                               false, {}, {}, request, response,
                               this->error_schema_);
          return;
        }
        sourcemeta::one::json_error(
            request, response, sourcemeta::one::STATUS_NOT_FOUND, "not-found",
            "There is nothing at this URL", this->error_schema_);
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

    const auto stripped_json{
        sourcemeta::core::remove_suffix_ignore_case(path, ".json")};
    if (stripped_json.size() != path.size()) {
      ActionJSONSchemaServe_v1::serve(*this, stripped_json, request, response,
                                      this->error_schema_);
      return;
    }

    if (request.method() == "get" || request.method() == "head") {
      if (request.prefers_html()) {
        const auto schema_html{
            this->artifact_resolve_path(path, Tree::Explorer, "schema-html")};
        const auto directory_html{this->artifact_resolve_path(
            path, Tree::Explorer, "directory-html")};
        if (!path.ends_with("/") && schema_html.has_value()) {
          this->artifact_serve(schema_html.value(), sourcemeta::one::STATUS_OK,
                               false, {}, {}, request, response,
                               this->error_schema_);
        } else if (directory_html.has_value()) {
          this->artifact_serve(directory_html.value(),
                               sourcemeta::one::STATUS_OK, false, {}, {},
                               request, response, this->error_schema_);
        } else {
          const auto not_found{
              this->artifact_resolve_path("", Tree::Explorer, "404")};
          if (not_found.has_value()) {
            this->artifact_serve(not_found.value(),
                                 sourcemeta::one::STATUS_NOT_FOUND, false, {},
                                 {}, request, response, this->error_schema_);
          } else {
            sourcemeta::one::json_error(
                request, response, sourcemeta::one::STATUS_NOT_FOUND,
                "not-found", "There is nothing at this URL",
                this->error_schema_);
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

  auto mcp(const sourcemeta::core::MCPProtocolVersion,
           const sourcemeta::core::JSON &id, const sourcemeta::core::JSON &)
      -> sourcemeta::core::JSON override {
    return sourcemeta::core::jsonrpc_make_error_method_not_found(id);
  }

private:
  std::string_view error_schema_;
};

#endif
