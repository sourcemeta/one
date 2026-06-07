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
    // Browser-targeted security headers we apply to every HTML response:
    //
    // - Referrer-Policy (W3C Referrer Policy):
    //   https://www.w3.org/TR/referrer-policy/
    //   Send full URL on same-origin navigation, only the origin on
    //   cross-origin navigation. Schema paths within the browser encode the
    //   user's current view and would otherwise leak via every external link
    //   click.
    //
    // - Content-Security-Policy frame-ancestors (W3C CSP Level 3 §6.4.2):
    //   https://www.w3.org/TR/CSP3/#directive-frame-ancestors
    //   Modern clickjacking control: deny embedding the web UI in any
    //   iframe.
    //
    // - X-Frame-Options (RFC 7034):
    //   https://datatracker.ietf.org/doc/html/rfc7034
    //   Legacy clickjacking control for browsers that predate CSP3
    //   frame-ancestors. Belt-and-suspenders for old client coverage at
    //   near-zero header cost.
    //
    // JSON and static-asset responses pass a default-constructed (all-empty)
    // instance and emit none of these.
    static constexpr sourcemeta::one::RouterAction::BrowserSecurityHeaders
        HTML_BROWSER_SECURITY{
            .referrer_policy = "strict-origin-when-cross-origin",
            .frame_ancestors = "'none'",
            .x_frame_options = "DENY",
        };
    const auto stripped{sourcemeta::core::URI::strip_path_prefix(
        request.path(), this->server_uri_base_path())};
    if (!stripped.has_value()) {
      sourcemeta::one::json_error(
          request, response, sourcemeta::core::HTTP_STATUS_NOT_FOUND,
          "sourcemeta:one/not-found", "There is nothing at this URL",
          this->error_schema_, "*");
      return;
    }

    const auto &path{stripped.value()};

    if (path.empty()) {
      if (request.method() != "get" && request.method() != "head") {
        sourcemeta::one::json_error(
            request, response, sourcemeta::core::HTTP_STATUS_METHOD_NOT_ALLOWED,
            "sourcemeta:one/method-not-allowed",
            "This HTTP method is invalid for this URL", this->error_schema_,
            "*", "GET, HEAD");
        return;
      }
      if (sourcemeta::core::http_match_accept(
              request.header("accept"), {"application/json", "text/html"}) ==
          "text/html") {
        const auto root_html{
            this->artifact_resolve_path("", Tree::Explorer, "directory-html")};
        if (root_html.has_value()) {
          this->artifact_serve(root_html.value(),
                               sourcemeta::core::HTTP_STATUS_OK, false, {}, {},
                               HTML_BROWSER_SECURITY, request, response,
                               this->error_schema_);
          return;
        }
      }
      sourcemeta::one::json_error(
          request, response, sourcemeta::core::HTTP_STATUS_NOT_FOUND,
          "sourcemeta:one/not-found", "There is nothing at this URL",
          this->error_schema_, "*");
      return;
    }

    const auto stripped_json{
        sourcemeta::core::remove_suffix_ignore_case(path, ".json")};
    if (stripped_json.size() != path.size()) {
      ActionJSONSchemaServe_v1::serve(*this, stripped_json, request, response,
                                      this->error_schema_);
      return;
    }

    if (request.method() == "get" || request.method() == "head") {
      if (sourcemeta::core::http_match_accept(
              request.header("accept"), {"application/json", "text/html"}) ==
          "text/html") {
        const auto schema_html{
            this->artifact_resolve_path(path, Tree::Explorer, "schema-html")};
        const auto directory_html{this->artifact_resolve_path(
            path, Tree::Explorer, "directory-html")};
        if (!path.ends_with("/") && schema_html.has_value()) {
          this->artifact_serve(schema_html.value(),
                               sourcemeta::core::HTTP_STATUS_OK, false, {}, {},
                               HTML_BROWSER_SECURITY, request, response,
                               this->error_schema_);
        } else if (directory_html.has_value()) {
          this->artifact_serve(directory_html.value(),
                               sourcemeta::core::HTTP_STATUS_OK, false, {}, {},
                               HTML_BROWSER_SECURITY, request, response,
                               this->error_schema_);
        } else {
          const auto not_found{
              this->artifact_resolve_path("", Tree::Explorer, "404")};
          if (not_found.has_value()) {
            this->artifact_serve(not_found.value(),
                                 sourcemeta::core::HTTP_STATUS_NOT_FOUND, false,
                                 {}, {}, HTML_BROWSER_SECURITY, request,
                                 response, this->error_schema_);
          } else {
            sourcemeta::one::json_error(
                request, response, sourcemeta::core::HTTP_STATUS_NOT_FOUND,
                "sourcemeta:one/not-found", "There is nothing at this URL",
                this->error_schema_, "*");
          }
        }
      } else {
        ActionJSONSchemaServe_v1::serve(*this, path, request, response,
                                        this->error_schema_);
      }
    } else {
      // RFC 9110 §15.5.6: when the path resolves to an existing resource
      // the response must be 405 with Allow listing what is supported.
      // https://datatracker.ietf.org/doc/html/rfc9110#section-15.5.6
      const auto schema_json{
          this->artifact_resolve_path(path, Tree::Schemas, "schema")};
      const auto schema_html{
          this->artifact_resolve_path(path, Tree::Explorer, "schema-html")};
      const auto directory_html{
          this->artifact_resolve_path(path, Tree::Explorer, "directory-html")};
      if (schema_json.has_value() ||
          (!path.ends_with("/") && schema_html.has_value()) ||
          directory_html.has_value()) {
        sourcemeta::one::json_error(
            request, response, sourcemeta::core::HTTP_STATUS_METHOD_NOT_ALLOWED,
            "sourcemeta:one/method-not-allowed",
            "This HTTP method is invalid for this URL", this->error_schema_,
            "*", "GET, HEAD");
      } else {
        sourcemeta::one::json_error(
            request, response, sourcemeta::core::HTTP_STATUS_NOT_FOUND,
            "sourcemeta:one/not-found", "There is nothing at this URL",
            this->error_schema_, "*");
      }
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
