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
#include <sourcemeta/one/shared_version.h>

#include "action_jsonschema_serve_v1.h"

#include <cstdint>   // std::int64_t
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
                response_schema, sourcemeta::one::STATUS_OK,
                ActionMCP_v1::enterprise_required(nullptr));
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
                response_schema, sourcemeta::one::STATUS_OK,
                ActionMCP_v1::enterprise_required(nullptr));
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

  static auto envelope_with_id(const sourcemeta::core::JSON &id)
      -> sourcemeta::core::JSON {
    auto envelope{sourcemeta::core::JSON::make_object()};
    envelope.assign("jsonrpc", sourcemeta::core::JSON{"2.0"});
    envelope.assign("id", id);
    return envelope;
  }

  static auto envelope_without_id() -> sourcemeta::core::JSON {
    auto envelope{sourcemeta::core::JSON::make_object()};
    envelope.assign("jsonrpc", sourcemeta::core::JSON{"2.0"});
    return envelope;
  }

  static auto error_object(const std::int64_t code,
                           const std::string_view message)
      -> sourcemeta::core::JSON {
    auto error{sourcemeta::core::JSON::make_object()};
    error.assign("code", sourcemeta::core::JSON{code});
    error.assign("message", sourcemeta::core::JSON{std::string{message}});
    return error;
  }

  static auto enterprise_required(const sourcemeta::core::JSON *id)
      -> sourcemeta::core::JSON {
    auto envelope{id == nullptr ? envelope_without_id()
                                : envelope_with_id(*id)};
    auto error{error_object(1, "Enterprise edition required")};
    error.assign("data", sourcemeta::core::JSON{
                             std::string{MCP_ENTERPRISE_REQUIRED_DATA}});
    envelope.assign("error", std::move(error));
    return envelope;
  }

  static auto resource_not_found(const sourcemeta::core::JSON &id,
                                 const std::string_view uri)
      -> sourcemeta::core::JSON {
    auto envelope{envelope_with_id(id)};
    auto error{error_object(-32002, "Resource not found")};
    error.assign("data", sourcemeta::core::JSON{std::string{uri}});
    envelope.assign("error", std::move(error));
    return envelope;
  }

  static auto method_not_allowed() -> sourcemeta::core::JSON {
    auto envelope{envelope_without_id()};
    envelope.assign("error", error_object(4, "Method not allowed"));
    return envelope;
  }

  static auto handle_initialize(const sourcemeta::core::JSON &id)
      -> sourcemeta::core::JSON {
    auto envelope{envelope_with_id(id)};
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
    envelope.assign("result", std::move(result));
    return envelope;
  }

  static auto handle_resources_list(const sourcemeta::core::JSON &id)
      -> sourcemeta::core::JSON {
    // TODO: support paginated enumeration of catalog schemas
    auto envelope{envelope_with_id(id)};
    auto result{sourcemeta::core::JSON::make_object()};
    result.assign("resources", sourcemeta::core::JSON::make_array());
    envelope.assign("result", std::move(result));
    return envelope;
  }

  static auto
  handle_resources_templates_list(const sourcemeta::core::JSON &id,
                                  const std::string_view registry_url)
      -> sourcemeta::core::JSON {
    // TODO: implement completion/complete to suggest schema paths for the
    // {+path} variable
    auto envelope{envelope_with_id(id)};
    auto result{sourcemeta::core::JSON::make_object()};
    auto templates{sourcemeta::core::JSON::make_array()};
    auto entry{sourcemeta::core::JSON::make_object()};
    std::string template_uri{registry_url};
    while (!template_uri.empty() && template_uri.back() == '/') {
      template_uri.pop_back();
    }
    template_uri.append("/{+path}{?bundle}");
    entry.assign("uriTemplate", sourcemeta::core::JSON{template_uri});
    entry.assign("name", sourcemeta::core::JSON{std::string{"JSON Schema"}});
    entry.assign("description",
                 sourcemeta::core::JSON{std::string{MCP_TEMPLATE_DESCRIPTION}});
    entry.assign("mimeType",
                 sourcemeta::core::JSON{std::string{MCP_TEMPLATE_MIME_TYPE}});
    templates.push_back(std::move(entry));
    result.assign("resourceTemplates", std::move(templates));
    envelope.assign("result", std::move(result));
    return envelope;
  }

  static auto query_has_non_empty_value(const std::string_view query,
                                        const std::string_view key) -> bool {
    std::string prefix{key};
    prefix.push_back('=');
    for (std::size_t pos{0}; pos < query.size();) {
      const auto next{query.find('&', pos)};
      const auto end{next == std::string_view::npos ? query.size() : next};
      const auto segment{query.substr(pos, end - pos)};
      if (segment.starts_with(prefix) && segment.size() > prefix.size()) {
        return true;
      }
      if (next == std::string_view::npos) {
        break;
      }
      pos = next + 1;
    }
    return false;
  }

  static auto resolve_request_uri(const std::string_view uri,
                                  const std::string_view registry_url,
                                  const std::filesystem::path &base)
      -> std::optional<std::filesystem::path> {
    sourcemeta::core::URI request_uri{std::string{uri}};
    sourcemeta::core::URI registry{std::string{registry_url}};
    if (request_uri.scheme() != registry.scheme() ||
        request_uri.host() != registry.host() ||
        request_uri.port() != registry.port()) {
      return std::nullopt;
    }

    const auto registry_path{registry.path().value_or("")};
    const auto request_path{request_uri.path().value_or("")};
    auto trim_trailing_slash = [](std::string_view value) {
      while (!value.empty() && value.back() == '/') {
        value.remove_suffix(1);
      }
      return value;
    };
    const auto registry_prefix{trim_trailing_slash(registry_path)};
    const auto request_trimmed{trim_trailing_slash(request_path)};
    if (!request_trimmed.starts_with(registry_prefix)) {
      return std::nullopt;
    }
    auto remaining{request_trimmed.substr(registry_prefix.size())};
    if (remaining.empty() || remaining.front() != '/') {
      return std::nullopt;
    }
    remaining.remove_prefix(1);
    if (remaining.ends_with(".json")) {
      remaining.remove_suffix(5);
    }
    if (remaining.empty()) {
      return std::nullopt;
    }

    const auto bundle{
        query_has_non_empty_value(request_uri.query().value_or(""), "bundle")};
    auto resolved{ActionJSONSchemaServe_v1::resolve(base, remaining, bundle)};
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
                     sourcemeta::one::STATUS_OK, enterprise_required(nullptr));
      return;
    }

    if (!request_json.is_object() || !request_json.defines("method") ||
        !request_json.at("method").is_string()) {
      write_envelope(request, response, allowed_origin, response_schema,
                     sourcemeta::one::STATUS_OK, enterprise_required(nullptr));
      return;
    }

    const sourcemeta::core::JSON *id_ptr{nullptr};
    if (request_json.defines("id")) {
      const auto &id_json{request_json.at("id")};
      if (id_json.is_string() || id_json.is_integer()) {
        id_ptr = &id_json;
      }
    }

    const auto method{request_json.at("method").to_string()};

    if (id_ptr != nullptr && method == "initialize") {
      write_envelope(request, response, allowed_origin, response_schema,
                     sourcemeta::one::STATUS_OK, handle_initialize(*id_ptr));
      return;
    }

    if (id_ptr != nullptr && method == "resources/list") {
      write_envelope(request, response, allowed_origin, response_schema,
                     sourcemeta::one::STATUS_OK,
                     handle_resources_list(*id_ptr));
      return;
    }

    if (id_ptr != nullptr && method == "resources/templates/list") {
      write_envelope(request, response, allowed_origin, response_schema,
                     sourcemeta::one::STATUS_OK,
                     handle_resources_templates_list(*id_ptr, registry_url));
      return;
    }

    if (id_ptr != nullptr && method == "resources/read") {
      write_envelope(
          request, response, allowed_origin, response_schema,
          sourcemeta::one::STATUS_OK,
          handle_resources_read(*id_ptr, request_json, registry_url, base));
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
