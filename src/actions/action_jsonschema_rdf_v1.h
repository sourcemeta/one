#ifndef SOURCEMETA_ONE_ACTIONS_JSONSCHEMA_RDF_V1_H
#define SOURCEMETA_ONE_ACTIONS_JSONSCHEMA_RDF_V1_H

#if defined(SOURCEMETA_ONE_ENTERPRISE)

#include <sourcemeta/one/enterprise_server.h>

using ActionJSONSchemaRDF_v1 =
    sourcemeta::one::enterprise::ActionJSONSchemaRDF_v1;

#else

#include <sourcemeta/core/http.h>
#include <sourcemeta/core/json.h>
#include <sourcemeta/core/jsonrpc.h>
#include <sourcemeta/core/mcp.h>
#include <sourcemeta/core/uritemplate.h>

#include <sourcemeta/one/http.h>
#include <sourcemeta/one/router.h>
#include <sourcemeta/one/shared.h>

#include <filesystem>  // std::filesystem::path
#include <span>        // std::span
#include <string_view> // std::string_view

class ActionJSONSchemaRDF_v1 : public sourcemeta::one::RouterAction {
public:
  static constexpr std::string_view DESCRIPTION{
      "Validate a JSON instance against a schema in the catalog and "
      "promote it to JSON-LD using the x-jsonld-* annotations of the "
      "schema"};
  static constexpr bool READ_ONLY{true};
  static constexpr bool DESTRUCTIVE{false};
  static constexpr bool IDEMPOTENT{true};
  static constexpr bool OPEN_WORLD{false};

  ActionJSONSchemaRDF_v1(
      const std::filesystem::path &base,
      const sourcemeta::core::URITemplateRouterView &router,
      const sourcemeta::core::URITemplateRouter::Identifier identifier,
      sourcemeta::one::Router &dispatcher)
      : sourcemeta::one::RouterAction{base, router.base_path(),
                                      router.base_url(), dispatcher} {
    router.arguments(
        identifier, [this](const auto &key, const auto &value) -> void {
          if (key == "errorSchema") {
            this->error_schema_ = std::get<std::string_view>(value);
          }
        });
  }

  auto rest(const std::span<std::string_view>, std::string_view,
            sourcemeta::one::HTTPRequest &request,
            sourcemeta::one::HTTPResponse &response) -> void override {
    if (request.method() == "options") {
      response.write_status(sourcemeta::core::HTTP_STATUS_NO_CONTENT);
      response.write_header("Access-Control-Allow-Origin", "*");
      response.write_header("Access-Control-Expose-Headers", "Link, ETag");
      response.write_header("Access-Control-Allow-Methods", "POST, OPTIONS");
      response.write_header("Access-Control-Allow-Headers", "Content-Type");
      response.write_header("Access-Control-Max-Age", "3600");
      // Browser preflight cache is governed by `Access-Control-Max-Age`;
      // `no-store` keeps shared HTTP caches from storing this response.
      response.write_header("Cache-Control", "no-store");
      // RFC 9110 §9.3.7: OPTIONS responses SHOULD include Allow. Different
      // audience than Access-Control-Allow-Methods (HTTP vs CORS preflight).
      // https://datatracker.ietf.org/doc/html/rfc9110#section-9.3.7
      response.write_header("Allow", "POST, OPTIONS");
      sourcemeta::one::send_response(sourcemeta::core::HTTP_STATUS_NO_CONTENT,
                                     request, response);
      return;
    }

    if (request.method() != "post") {
      sourcemeta::one::json_error(
          request, response, sourcemeta::core::HTTP_STATUS_METHOD_NOT_ALLOWED,
          "urn:sourcemeta:one:method-not-allowed",
          "This HTTP method is invalid for this URL", this->error_schema_, "*",
          "POST, OPTIONS");
      return;
    }

    sourcemeta::one::json_error(
        request, response, sourcemeta::core::HTTP_STATUS_FORBIDDEN,
        "urn:sourcemeta:one:enterprise-required",
        "This feature is only available in the Enterprise edition",
        this->error_schema_, "*");
  }

  auto mcp(const sourcemeta::core::MCPProtocolVersion,
           const sourcemeta::core::JSON &id, const sourcemeta::core::JSON &,
           std::string_view) -> sourcemeta::core::JSON override {
    return sourcemeta::core::jsonrpc_make_error_method_not_found(id);
  }

private:
  std::string_view error_schema_;
};

#endif

#endif
