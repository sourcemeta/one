#ifndef SOURCEMETA_ONE_ACTIONS_MCP_V1_H
#define SOURCEMETA_ONE_ACTIONS_MCP_V1_H

#if defined(SOURCEMETA_ONE_ENTERPRISE)
#include <sourcemeta/core/uritemplate.h>

#include <sourcemeta/one/actions.h>
#include <sourcemeta/one/enterprise_server.h>
#include <sourcemeta/one/http.h>

#include <filesystem>  // std::filesystem
#include <span>        // std::span
#include <string_view> // std::string_view

class ActionMCP_v1 : public sourcemeta::one::Action, public EnterpriseMCP {
public:
  ActionMCP_v1(const std::filesystem::path &base,
               const sourcemeta::core::URITemplateRouterView &router,
               const sourcemeta::core::URITemplateRouter::Identifier identifier)
      : sourcemeta::one::Action{base, router.base_path()},
        EnterpriseMCP{base, router, identifier} {}

  auto run(const std::span<std::string_view>,
           sourcemeta::one::HTTPRequest &request,
           sourcemeta::one::HTTPResponse &response) -> void override {
    EnterpriseMCP::run(request, response);
  }
};

#else

#include <sourcemeta/core/json.h>
#include <sourcemeta/core/uri.h>
#include <sourcemeta/core/uritemplate.h>

#include <sourcemeta/one/actions.h>
#include <sourcemeta/one/http.h>
#include <sourcemeta/one/jsonrpc.h>
#include <sourcemeta/one/shared.h>

#include "action_jsonschema_serve_v1.h"

#include <exception> // std::exception, std::exception_ptr, std::rethrow_exception
#include <filesystem>  // std::filesystem
#include <optional>    // std::optional
#include <span>        // std::span
#include <sstream>     // std::ostringstream
#include <string>      // std::string
#include <string_view> // std::string_view
#include <utility>     // std::move

class ActionMCP_v1 : public sourcemeta::one::Action {
public:
  ActionMCP_v1(const std::filesystem::path &base,
               const sourcemeta::core::URITemplateRouterView &router,
               const sourcemeta::core::URITemplateRouter::Identifier identifier)
      : sourcemeta::one::Action{base, router.base_path()} {
    router.arguments(identifier, [this](const auto &key, const auto &value) {
      if (key == "responseSchema") {
        this->response_schema_ = std::get<std::string_view>(value);
      }
    });

    const auto metadata{
        sourcemeta::core::read_json(this->base() / "metadata.json")};
    this->allowed_origin_ = metadata.at("origin").to_string();
    this->registry_url_ = metadata.at("url").to_string();
  }

  auto run(const std::span<std::string_view>,
           sourcemeta::one::HTTPRequest &request,
           sourcemeta::one::HTTPResponse &response) -> void override {
    if (request.method() == "options") {
      response.write_status(sourcemeta::one::STATUS_NO_CONTENT);
      response.write_header("Access-Control-Allow-Origin",
                            this->allowed_origin_);
      response.write_header("Access-Control-Allow-Methods", "POST, OPTIONS");
      response.write_header("Access-Control-Allow-Headers",
                            "Content-Type, MCP-Protocol-Version");
      response.write_header("Access-Control-Max-Age", "3600");
      sourcemeta::one::send_response(sourcemeta::one::STATUS_NO_CONTENT,
                                     request, response);
      return;
    }

    if (request.method() != "post") {
      ActionMCP_v1::write_envelope(request, response, this->allowed_origin_,
                                   this->response_schema_,
                                   sourcemeta::one::STATUS_METHOD_NOT_ALLOWED,
                                   ActionMCP_v1::method_not_allowed());
      return;
    }

    request.body(
        [allowed_origin = std::string_view{this->allowed_origin_},
         response_schema = this->response_schema_,
         registry_url = std::string_view{this->registry_url_},
         &base = this->base()](sourcemeta::one::HTTPRequest &callback_request,
                               sourcemeta::one::HTTPResponse &callback_response,
                               std::string &&body, bool too_big) {
          if (too_big) {
            ActionMCP_v1::write_envelope(
                callback_request, callback_response, allowed_origin,
                response_schema, sourcemeta::one::STATUS_PAYLOAD_TOO_LARGE,
                ActionMCP_v1::request_too_large());
            return;
          }
          ActionMCP_v1::handle_message(callback_request, callback_response,
                                       allowed_origin, response_schema,
                                       registry_url, base, std::move(body));
        },
        [allowed_origin = std::string_view{this->allowed_origin_},
         response_schema = this->response_schema_](
            sourcemeta::one::HTTPRequest &callback_request,
            sourcemeta::one::HTTPResponse &callback_response,
            const std::exception_ptr &error) {
          try {
            std::rethrow_exception(error);
          } catch (const std::exception &) {
            ActionMCP_v1::write_envelope(
                callback_request, callback_response, allowed_origin,
                response_schema, sourcemeta::one::STATUS_INTERNAL_SERVER_ERROR,
                sourcemeta::one::jsonrpc_make_error_internal());
          }
        });
  }

private:
  static constexpr std::string_view MCP_PROTOCOL_VERSION{"2025-11-25"};
  static constexpr std::string_view MCP_SERVER_NAME{"sourcemeta-one"};
  static constexpr std::string_view MCP_SERVER_TITLE{"Sourcemeta One"};
  static constexpr std::string_view MCP_TEMPLATE_DESCRIPTION{
      "A JSON Schema in this catalog (optionally bundled)"};
  static constexpr std::string_view MCP_TEMPLATE_MIME_TYPE{
      "application/schema+json"};
  static constexpr std::string_view MCP_ENTERPRISE_REQUIRED_DATA{
      "MCP support is only available in the Enterprise edition of "
      "Sourcemeta One"};

  static auto enterprise_required(const sourcemeta::core::JSON *id)
      -> sourcemeta::core::JSON {
    return sourcemeta::one::jsonrpc_make_error(
        id, 1, "Enterprise edition required",
        sourcemeta::core::JSON{std::string{MCP_ENTERPRISE_REQUIRED_DATA}});
  }

  static auto resource_not_found(const sourcemeta::core::JSON &id,
                                 const std::string_view uri)
      -> sourcemeta::core::JSON {
    return sourcemeta::one::jsonrpc_make_error(
        &id, -32002, "Resource not found",
        sourcemeta::core::JSON{std::string{uri}});
  }

  static auto method_not_allowed() -> sourcemeta::core::JSON {
    return sourcemeta::one::jsonrpc_make_error(nullptr, 4,
                                               "Method not allowed");
  }

  static auto request_too_large() -> sourcemeta::core::JSON {
    return sourcemeta::one::jsonrpc_make_error(nullptr, 5, "Request too large");
  }

  static auto handle_initialize(const sourcemeta::core::JSON &id)
      -> sourcemeta::core::JSON {
    auto result{sourcemeta::core::JSON::make_object()};
    result.assign("protocolVersion",
                  sourcemeta::core::JSON{std::string{MCP_PROTOCOL_VERSION}});
    auto capabilities{sourcemeta::core::JSON::make_object()};
    capabilities.assign("resources", sourcemeta::core::JSON::make_object());
    result.assign("capabilities", std::move(capabilities));
    auto server_info{sourcemeta::core::JSON::make_object()};
    server_info.assign("name",
                       sourcemeta::core::JSON{std::string{MCP_SERVER_NAME}});
    server_info.assign("title",
                       sourcemeta::core::JSON{std::string{MCP_SERVER_TITLE}});
    server_info.assign("version", sourcemeta::core::JSON{
                                      std::string{sourcemeta::one::version()}});
    result.assign("serverInfo", std::move(server_info));
    return sourcemeta::one::jsonrpc_make_success(id, std::move(result));
  }

  static auto handle_resources_list(const sourcemeta::core::JSON &id)
      -> sourcemeta::core::JSON {
    // TODO: support paginated enumeration of catalog schemas
    auto result{sourcemeta::core::JSON::make_object()};
    result.assign("resources", sourcemeta::core::JSON::make_array());
    return sourcemeta::one::jsonrpc_make_success(id, std::move(result));
  }

  static auto
  handle_resources_templates_list(const sourcemeta::core::JSON &id,
                                  const std::string_view registry_url)
      -> sourcemeta::core::JSON {
    // TODO: implement completion/complete to suggest schema paths for the
    // {+path} variable
    std::string template_uri{registry_url};
    if (!template_uri.empty() && template_uri.back() == '/') {
      template_uri.pop_back();
    }
    template_uri.append("/{+path}{?bundle}");
    auto entry{sourcemeta::core::JSON::make_object()};
    entry.assign("uriTemplate", sourcemeta::core::JSON{template_uri});
    entry.assign("name", sourcemeta::core::JSON{std::string{"JSON Schema"}});
    entry.assign("description",
                 sourcemeta::core::JSON{std::string{MCP_TEMPLATE_DESCRIPTION}});
    entry.assign("mimeType",
                 sourcemeta::core::JSON{std::string{MCP_TEMPLATE_MIME_TYPE}});
    auto templates{sourcemeta::core::JSON::make_array()};
    templates.push_back(std::move(entry));
    auto result{sourcemeta::core::JSON::make_object()};
    result.assign("resourceTemplates", std::move(templates));
    return sourcemeta::one::jsonrpc_make_success(id, std::move(result));
  }

  static auto resolve_request_uri(const std::string_view uri,
                                  const std::string_view registry_url,
                                  const std::filesystem::path &base)
      -> std::optional<std::filesystem::path> {
    sourcemeta::core::URI request{std::string{uri}};
    const sourcemeta::core::URI registry{std::string{registry_url}};
    request.relative_to(registry);
    if (request.is_absolute()) {
      return std::nullopt;
    }
    const auto path{request.path().value_or("")};
    std::string_view schema_path{path};
    if (schema_path.ends_with(".json")) {
      schema_path.remove_suffix(5);
    }
    if (schema_path.empty()) {
      return std::nullopt;
    }
    const auto query{request.query()};
    const auto bundle{query.has_value() &&
                      !query->at("bundle").value_or("").empty()};
    auto resolved{ActionJSONSchemaServe_v1::resolve(base, schema_path, bundle)};
    if (!std::filesystem::exists(resolved)) {
      return std::nullopt;
    }
    return resolved;
  }

  static auto handle_resources_read(const sourcemeta::core::JSON &id,
                                    const sourcemeta::core::JSON &request_json,
                                    const std::string_view registry_url,
                                    const std::filesystem::path &base)
      -> sourcemeta::core::JSON {
    if (!request_json.defines("params") ||
        !request_json.at("params").is_object() ||
        !request_json.at("params").defines("uri") ||
        !request_json.at("params").at("uri").is_string()) {
      return enterprise_required(&id);
    }
    const auto uri{request_json.at("params").at("uri").to_string()};
    try {
      const auto resolved{resolve_request_uri(uri, registry_url, base)};
      if (!resolved.has_value()) {
        return resource_not_found(id, uri);
      }
    } catch (const std::exception &) {
      return resource_not_found(id, uri);
    }
    return enterprise_required(&id);
  }

  static auto handle_message(sourcemeta::one::HTTPRequest &request,
                             sourcemeta::one::HTTPResponse &response,
                             const std::string_view allowed_origin,
                             const std::string_view response_schema,
                             const std::string_view registry_url,
                             const std::filesystem::path &base,
                             std::string &&body) -> void {
    sourcemeta::core::JSON request_json{nullptr};
    try {
      request_json = sourcemeta::core::parse_json(body);
    } catch (const std::exception &) {
      write_envelope(request, response, allowed_origin, response_schema,
                     sourcemeta::one::STATUS_BAD_REQUEST,
                     sourcemeta::one::jsonrpc_make_error_parse());
      return;
    }

    if (!request_json.is_object() || !request_json.defines("method") ||
        !request_json.at("method").is_string()) {
      write_envelope(request, response, allowed_origin, response_schema,
                     sourcemeta::one::STATUS_OK,
                     sourcemeta::one::jsonrpc_make_error_invalid_request(
                         sourcemeta::one::jsonrpc_request_id(request_json)));
      return;
    }

    const auto *id{sourcemeta::one::jsonrpc_request_id(request_json)};
    const auto method{request_json.at("method").to_string()};

    if (id != nullptr && method == "initialize") {
      write_envelope(request, response, allowed_origin, response_schema,
                     sourcemeta::one::STATUS_OK, handle_initialize(*id));
      return;
    }

    if (id != nullptr && method == "resources/list") {
      write_envelope(request, response, allowed_origin, response_schema,
                     sourcemeta::one::STATUS_OK, handle_resources_list(*id));
      return;
    }

    if (id != nullptr && method == "resources/templates/list") {
      write_envelope(request, response, allowed_origin, response_schema,
                     sourcemeta::one::STATUS_OK,
                     handle_resources_templates_list(*id, registry_url));
      return;
    }

    if (id != nullptr && method == "resources/read") {
      write_envelope(
          request, response, allowed_origin, response_schema,
          sourcemeta::one::STATUS_OK,
          handle_resources_read(*id, request_json, registry_url, base));
      return;
    }

    write_envelope(request, response, allowed_origin, response_schema,
                   sourcemeta::one::STATUS_OK, enterprise_required(nullptr));
  }

  static auto write_envelope(sourcemeta::one::HTTPRequest &request,
                             sourcemeta::one::HTTPResponse &response,
                             const std::string_view allowed_origin,
                             const std::string_view response_schema,
                             const char *status,
                             const sourcemeta::core::JSON &envelope) -> void {
    response.write_status(status);
    response.write_header("Content-Type", "application/json");
    response.write_header("Access-Control-Allow-Origin",
                          std::string{allowed_origin});
    if (!response_schema.empty()) {
      sourcemeta::one::write_link_header(response, response_schema);
    }
    std::ostringstream payload;
    sourcemeta::core::prettify(envelope, payload);
    sourcemeta::one::send_response(status, request, response, payload.str(),
                                   sourcemeta::one::Encoding::Identity);
  }

  std::string allowed_origin_;
  std::string registry_url_;
  std::string_view response_schema_;
};

#endif

#endif
