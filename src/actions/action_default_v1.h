#ifndef SOURCEMETA_ONE_ACTIONS_DEFAULT_V1_H
#define SOURCEMETA_ONE_ACTIONS_DEFAULT_V1_H

#include <sourcemeta/core/http.h>
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
      : sourcemeta::one::RouterAction{base, router.base_url(), dispatcher} {
    router.arguments(
        identifier, [this](const auto &key, const auto &value) -> void {
          if (key == "errorSchema") {
            this->error_schema_ = std::get<std::string_view>(value);
          }
        });
  }

  auto rest(const std::span<std::string_view>,
            const sourcemeta::one::Authentication::Caller &caller,
            sourcemeta::one::HTTPRequest &request,
            sourcemeta::one::HTTPResponse &response) -> void override {
    if (request.method() == "options") {
      sourcemeta::one::cors_preflight(request, response, "GET, HEAD, OPTIONS",
                                      "Accept, Accept-Encoding, If-None-Match, "
                                      "If-Modified-Since");
      return;
    }
    const auto stripped{
        sourcemeta::core::URI::strip_path_prefix(request.path(), "")};
    if (!stripped.has_value()) {
      sourcemeta::one::json_error(
          request, response, sourcemeta::core::HTTP_STATUS_NOT_FOUND,
          "urn:sourcemeta:one:not-found", "There is nothing at this URL",
          this->error_schema_, "*");
      return;
    }

    const auto &path{stripped.value()};
    const sourcemeta::one::RequestCookies cookies{request};

    if (path.empty()) {
      if (request.method() != "get" && request.method() != "head") {
        sourcemeta::one::json_error(
            request, response, sourcemeta::core::HTTP_STATUS_METHOD_NOT_ALLOWED,
            "urn:sourcemeta:one:method-not-allowed",
            "This HTTP method is invalid for this URL", this->error_schema_,
            "*", "GET, HEAD, OPTIONS");
        return;
      }
      const auto serve_html{
          sourcemeta::one::prefers_html(request.header("accept"))};
      const auto root_html{this->artifact_resolve_path(
          caller, "", Tree::Explorer, "directory-html")};
      if (serve_html && root_html.path.has_value()) {
        this->artifact_serve(
            root_html.path.value(), sourcemeta::core::HTTP_STATUS_OK, false, {},
            {}, HTML_BROWSER_SECURITY, request, response, this->error_schema_,
            sourcemeta::one::cache_control_content(root_html.is_public),
            sourcemeta::one::vary_type_and_encoding());
      } else if (serve_html) {
        this->serve_missing_html(caller.view(), request, response);
      } else {
        sourcemeta::one::json_error(
            request, response, sourcemeta::core::HTTP_STATUS_NOT_FOUND,
            "urn:sourcemeta:one:not-found", "There is nothing at this URL",
            this->error_schema_, "*");
      }
      return;
    }

    const auto stripped_json{
        sourcemeta::core::remove_suffix_ignore_case(path, ".json")};
    if (stripped_json.size() != path.size()) {
      ActionJSONSchemaServe_v1::serve(*this, caller, stripped_json, request,
                                      response, this->error_schema_);
      return;
    }

    if (request.method() == "get" || request.method() == "head") {
      if (sourcemeta::one::prefers_html(request.header("accept"))) {
        const auto schema_html{this->artifact_resolve_path(
            caller, path, Tree::Explorer, "schema-html")};
        const auto directory_html{this->artifact_resolve_path(
            caller, path, Tree::Explorer, "directory-html")};
        if (!path.ends_with("/") && schema_html.path.has_value()) {
          this->artifact_serve(
              schema_html.path.value(), sourcemeta::core::HTTP_STATUS_OK, false,
              {}, {}, HTML_BROWSER_SECURITY, request, response,
              this->error_schema_,
              sourcemeta::one::cache_control_content(schema_html.is_public),
              sourcemeta::one::vary_type_and_encoding());
        } else if (directory_html.path.has_value()) {
          this->artifact_serve(
              directory_html.path.value(), sourcemeta::core::HTTP_STATUS_OK,
              false, {}, {}, HTML_BROWSER_SECURITY, request, response,
              this->error_schema_,
              sourcemeta::one::cache_control_content(directory_html.is_public),
              sourcemeta::one::vary_type_and_encoding());
        } else {
          this->serve_missing_html(caller.view(), request, response);
        }
      } else {
        ActionJSONSchemaServe_v1::serve(*this, caller, path, request, response,
                                        this->error_schema_);
      }
    } else {
      // RFC 9110 §15.5.6: when the path resolves to an existing resource
      // the response must be 405 with Allow listing what is supported.
      // https://datatracker.ietf.org/doc/html/rfc9110#section-15.5.6
      const auto schema_json{
          this->artifact_resolve_path(caller, path, Tree::Schemas, "schema")};
      const auto schema_html{this->artifact_resolve_path(
          caller, path, Tree::Explorer, "schema-html")};
      const auto directory_html{this->artifact_resolve_path(
          caller, path, Tree::Explorer, "directory-html")};
      if (schema_json.path.has_value() ||
          (!path.ends_with("/") && schema_html.path.has_value()) ||
          directory_html.path.has_value()) {
        sourcemeta::one::json_error(
            request, response, sourcemeta::core::HTTP_STATUS_METHOD_NOT_ALLOWED,
            "urn:sourcemeta:one:method-not-allowed",
            "This HTTP method is invalid for this URL", this->error_schema_,
            "*", "GET, HEAD, OPTIONS");
      } else {
        sourcemeta::one::json_error(
            request, response, sourcemeta::core::HTTP_STATUS_NOT_FOUND,
            "urn:sourcemeta:one:not-found", "There is nothing at this URL",
            this->error_schema_, "*");
      }
    }
  }

  auto mcp(const sourcemeta::core::MCPProtocolVersion,
           const sourcemeta::core::JSON &id, const sourcemeta::core::JSON &,
           const sourcemeta::one::Authentication::Caller &)
      -> sourcemeta::core::JSON override {
    return sourcemeta::core::jsonrpc_make_error_method_not_found(id);
  }

private:
  // Nothing here for whoever is asking, whether because there is nothing here
  // at all or because their view does not hold it. Neither one is worth
  // telling apart, so both are told the same way
  auto serve_missing_html(const std::string_view view,
                          sourcemeta::one::HTTPRequest &request,
                          sourcemeta::one::HTTPResponse &response) -> void {
    // A browser whose session lapsed is renewed in place first, since what it
    // asks for may well be something the renewed session holds. A caller
    // already placed in a view has a session that stands, so there is nothing
    // to renew and asking again would only send them round in circles
    if (view == sourcemeta::one::VIEW_PUBLIC &&
        this->serve_renewal_page(request, response)) {
      return;
    }

    // The absence is the caller's own, so it is told in the terms of the view
    // they resolve to rather than in the anonymous one's
    const auto not_found{this->artifact_resolve_path_unauthenticated(
        view, "", Tree::Explorer, "404")};
    if (not_found.has_value()) {
      this->artifact_serve(
          not_found.value(), sourcemeta::core::HTTP_STATUS_NOT_FOUND, false, {},
          {}, HTML_BROWSER_SECURITY, request, response, this->error_schema_,
          sourcemeta::one::cache_control_no_store(),
          sourcemeta::one::vary_type_and_encoding());
      return;
    }

    sourcemeta::one::json_error(
        request, response, sourcemeta::core::HTTP_STATUS_NOT_FOUND,
        "urn:sourcemeta:one:not-found", "There is nothing at this URL",
        this->error_schema_, "*");
  }

  std::string_view error_schema_;
};

#endif
