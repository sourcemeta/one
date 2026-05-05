#include <sourcemeta/one/enterprise_server.h>

#include <sourcemeta/core/json.h>
#include <sourcemeta/core/uri.h>

#include <sourcemeta/blaze/evaluator.h>
#include <sourcemeta/blaze/output.h>

#include <sourcemeta/one/http.h>
#include <sourcemeta/one/jsonrpc.h>
#include <sourcemeta/one/metapack.h>
#include <sourcemeta/one/shared_version.h>

#include <algorithm> // std::ranges::transform
#include <cassert>   // assert
#include <cctype>    // std::tolower
#include <exception> // std::exception, std::exception_ptr, std::rethrow_exception
#include <filesystem>  // std::filesystem
#include <optional>    // std::optional
#include <sstream>     // std::ostringstream
#include <string>      // std::string
#include <string_view> // std::string_view
#include <utility>     // std::move

namespace {

constexpr std::string_view MCP_PROTOCOL_VERSION{"2025-11-25"};
constexpr std::string_view MCP_LEGACY_PROTOCOL_VERSION{"2025-03-26"};
constexpr std::string_view MCP_SERVER_NAME{"sourcemeta-one-enterprise"};
constexpr std::string_view MCP_SERVER_TITLE{"Sourcemeta One Enterprise"};
constexpr std::string_view MCP_TEMPLATE_DESCRIPTION{
    "A JSON Schema in this catalog (optionally bundled)"};
constexpr std::string_view MCP_TEMPLATE_MIME_TYPE{"application/schema+json"};

auto write_envelope(sourcemeta::one::HTTPRequest &request,
                    sourcemeta::one::HTTPResponse &response,
                    const std::string_view allowed_origin,
                    const std::string_view response_schema, const char *status,
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

auto write_json_envelope(sourcemeta::one::HTTPRequest &request,
                         sourcemeta::one::HTTPResponse &response,
                         const std::string_view allowed_origin,
                         const std::string_view response_schema,
                         const sourcemeta::core::JSON &envelope) -> void {
  write_envelope(request, response, allowed_origin, response_schema,
                 sourcemeta::one::STATUS_OK, envelope);
}

auto write_accepted(sourcemeta::one::HTTPRequest &request,
                    sourcemeta::one::HTTPResponse &response,
                    const std::string_view allowed_origin) -> void {
  response.write_status(sourcemeta::one::STATUS_ACCEPTED);
  response.write_header("Access-Control-Allow-Origin",
                        std::string{allowed_origin});
  sourcemeta::one::send_response(sourcemeta::one::STATUS_ACCEPTED, request,
                                 response);
}

auto method_not_allowed() -> sourcemeta::core::JSON {
  return sourcemeta::one::jsonrpc_make_error(nullptr, 4, "Method not allowed");
}

auto forbidden_origin() -> sourcemeta::core::JSON {
  return sourcemeta::one::jsonrpc_make_error(nullptr, 2, "Forbidden origin");
}

auto unsupported_protocol_version() -> sourcemeta::core::JSON {
  return sourcemeta::one::jsonrpc_make_error(nullptr, 3,
                                             "Unsupported protocol version");
}

auto request_too_large() -> sourcemeta::core::JSON {
  return sourcemeta::one::jsonrpc_make_error(nullptr, 5, "Request too large");
}

auto resource_not_found(const sourcemeta::core::JSON &id,
                        const std::string_view uri) -> sourcemeta::core::JSON {
  return sourcemeta::one::jsonrpc_make_error(
      &id, -32002, "Resource not found",
      sourcemeta::core::JSON{std::string{uri}});
}

auto handle_initialize(const sourcemeta::core::JSON &request_json)
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
  return sourcemeta::one::jsonrpc_make_success(request_json.at("id"),
                                               std::move(result));
}

auto handle_ping(const sourcemeta::core::JSON &request_json)
    -> sourcemeta::core::JSON {
  return sourcemeta::one::jsonrpc_make_success_empty(request_json.at("id"));
}

auto handle_resources_list(const sourcemeta::core::JSON &request_json)
    -> sourcemeta::core::JSON {
  // TODO: support paginated enumeration of catalog schemas
  auto result{sourcemeta::core::JSON::make_object()};
  result.assign("resources", sourcemeta::core::JSON::make_array());
  return sourcemeta::one::jsonrpc_make_success(request_json.at("id"),
                                               std::move(result));
}

auto handle_resources_templates_list(const sourcemeta::core::JSON &request_json,
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
  return sourcemeta::one::jsonrpc_make_success(request_json.at("id"),
                                               std::move(result));
}

auto resolve_metapack_path(const std::filesystem::path &base,
                           const std::string_view schema_path,
                           const bool bundle) -> std::filesystem::path {
  std::string lowercase_path{schema_path};
  std::ranges::transform(
      lowercase_path, lowercase_path.begin(),
      [](const unsigned char character) { return std::tolower(character); });
  auto absolute_path{base / "schemas" / lowercase_path / "%"};
  if (bundle) {
    absolute_path /= "bundle.metapack";
  } else {
    absolute_path /= "schema.metapack";
  }
  return absolute_path;
}

auto resolve_request_uri(const std::string_view uri,
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
  const std::filesystem::path schema_filesystem_path{schema_path};
  if (schema_filesystem_path.is_absolute()) {
    return std::nullopt;
  }
  for (const auto &part : schema_filesystem_path) {
    if (part == ".." || part == ".") {
      return std::nullopt;
    }
  }
  const auto query{request.query()};
  const auto bundle{query.has_value() &&
                    !query->at("bundle").value_or("").empty()};
  auto resolved{resolve_metapack_path(base, schema_path, bundle)};
  if (!std::filesystem::exists(resolved)) {
    return std::nullopt;
  }
  return resolved;
}

auto handle_resources_read(const sourcemeta::core::JSON &request_json,
                           const std::string_view registry_url,
                           const std::filesystem::path &base)
    -> sourcemeta::core::JSON {
  const auto &id{request_json.at("id")};
  const auto uri{request_json.at("params").at("uri").to_string()};
  std::optional<std::filesystem::path> resolved;
  try {
    resolved = resolve_request_uri(uri, registry_url, base);
  } catch (const std::exception &) {
    return resource_not_found(id, uri);
  }
  if (!resolved.has_value()) {
    return resource_not_found(id, uri);
  }
  const auto schema{sourcemeta::one::metapack_read_json(resolved.value())};
  if (!schema.has_value()) {
    return resource_not_found(id, uri);
  }
  std::ostringstream payload;
  sourcemeta::core::prettify(schema.value(), payload);
  auto entry{sourcemeta::core::JSON::make_object()};
  entry.assign("uri", sourcemeta::core::JSON{std::string{uri}});
  entry.assign("mimeType",
               sourcemeta::core::JSON{std::string{MCP_TEMPLATE_MIME_TYPE}});
  entry.assign("text", sourcemeta::core::JSON{payload.str()});
  auto contents{sourcemeta::core::JSON::make_array()};
  contents.push_back(std::move(entry));
  auto result{sourcemeta::core::JSON::make_object()};
  result.assign("contents", std::move(contents));
  return sourcemeta::one::jsonrpc_make_success(id, std::move(result));
}

auto matches_request_schema(const sourcemeta::blaze::Template &schema_template,
                            const sourcemeta::core::JSON &instance) -> bool {
  sourcemeta::blaze::Evaluator evaluator;
  return evaluator.validate(schema_template, instance);
}

auto handle_jsonrpc_message(
    sourcemeta::one::HTTPRequest &request,
    sourcemeta::one::HTTPResponse &response,
    const std::string_view allowed_origin,
    const std::string_view response_schema,
    const sourcemeta::blaze::Template &request_schema_template,
    const std::string_view registry_url, const std::filesystem::path &base,
    std::string &&body) -> void {
  sourcemeta::core::JSON request_json{nullptr};
  try {
    request_json = sourcemeta::core::parse_json(body);
  } catch (const std::exception &) {
    write_json_envelope(request, response, allowed_origin, response_schema,
                        sourcemeta::one::jsonrpc_make_error_parse());
    return;
  }

  if (sourcemeta::one::jsonrpc_is_notification(request_json)) {
    write_accepted(request, response, allowed_origin);
    return;
  }

  if (!sourcemeta::one::jsonrpc_is_request(request_json)) {
    write_json_envelope(request, response, allowed_origin, response_schema,
                        sourcemeta::one::jsonrpc_make_error_invalid_request(
                            sourcemeta::one::jsonrpc_request_id(request_json)));
    return;
  }

  const auto method{sourcemeta::one::jsonrpc_method(request_json)};
  if (method != "initialize" && method != "ping" &&
      method != "resources/list" && method != "resources/templates/list" &&
      method != "resources/read") {
    write_json_envelope(request, response, allowed_origin, response_schema,
                        sourcemeta::one::jsonrpc_make_error_method_not_found(
                            request_json.at("id")));
    return;
  }

  if (!matches_request_schema(request_schema_template, request_json)) {
    write_json_envelope(request, response, allowed_origin, response_schema,
                        sourcemeta::one::jsonrpc_make_error_invalid_request(
                            &request_json.at("id")));
    return;
  }

  if (method == "initialize") {
    write_json_envelope(request, response, allowed_origin, response_schema,
                        handle_initialize(request_json));
    return;
  }

  if (method == "resources/list") {
    write_json_envelope(request, response, allowed_origin, response_schema,
                        handle_resources_list(request_json));
    return;
  }

  if (method == "resources/templates/list") {
    write_json_envelope(
        request, response, allowed_origin, response_schema,
        handle_resources_templates_list(request_json, registry_url));
    return;
  }

  if (method == "resources/read") {
    write_json_envelope(
        request, response, allowed_origin, response_schema,
        handle_resources_read(request_json, registry_url, base));
    return;
  }

  write_json_envelope(request, response, allowed_origin, response_schema,
                      handle_ping(request_json));
}

} // namespace

EnterpriseMCP::EnterpriseMCP(
    const std::filesystem::path &base,
    const sourcemeta::core::URITemplateRouterView &router,
    const sourcemeta::core::URITemplateRouter::Identifier identifier)
    : base_{base} {
  std::string_view request_schema;
  router.arguments(
      identifier, [this, &request_schema](const auto &key, const auto &value) {
        if (key == "responseSchema") {
          this->response_schema_ = std::get<std::string_view>(value);
        } else if (key == "requestSchema") {
          request_schema = std::get<std::string_view>(value);
        }
      });

  const auto metadata{sourcemeta::core::read_json(base / "metadata.json")};
  this->allowed_origin_ = metadata.at("origin").to_string();
  this->registry_url_ = metadata.at("url").to_string();

  const auto base_path{router.base_path()};
  std::string_view request_schema_path{request_schema};
  if (!base_path.empty() && request_schema_path.starts_with(base_path)) {
    request_schema_path.remove_prefix(base_path.size());
  }
  if (request_schema_path.starts_with('/')) {
    request_schema_path.remove_prefix(1);
  }

  const auto template_path{base / "schemas" / request_schema_path / "%" /
                           "blaze-fast.metapack"};
  const auto template_json{sourcemeta::one::metapack_read_json(template_path)};
  assert(template_json.has_value());
  auto compiled{sourcemeta::blaze::from_json(template_json.value())};
  assert(compiled.has_value());
  this->request_schema_template_ = std::move(compiled.value());
}

auto EnterpriseMCP::run(sourcemeta::one::HTTPRequest &request,
                        sourcemeta::one::HTTPResponse &response) -> void {
  const auto origin_header{request.header("origin")};
  if (!origin_header.empty() && origin_header != this->allowed_origin_) {
    write_envelope(request, response, this->allowed_origin_,
                   this->response_schema_, sourcemeta::one::STATUS_FORBIDDEN,
                   forbidden_origin());
    return;
  }

  if (request.method() == "options") {
    response.write_status(sourcemeta::one::STATUS_NO_CONTENT);
    response.write_header("Access-Control-Allow-Origin",
                          std::string{this->allowed_origin_});
    response.write_header("Access-Control-Allow-Methods", "POST, OPTIONS");
    response.write_header("Access-Control-Allow-Headers",
                          "Content-Type, MCP-Protocol-Version");
    response.write_header("Access-Control-Max-Age", "3600");
    sourcemeta::one::send_response(sourcemeta::one::STATUS_NO_CONTENT, request,
                                   response);
    return;
  }

  if (request.method() != "post") {
    write_envelope(
        request, response, this->allowed_origin_, this->response_schema_,
        sourcemeta::one::STATUS_METHOD_NOT_ALLOWED, method_not_allowed());
    return;
  }

  const auto protocol_version_header{request.header("mcp-protocol-version")};
  if (!protocol_version_header.empty() &&
      protocol_version_header != MCP_PROTOCOL_VERSION &&
      protocol_version_header != MCP_LEGACY_PROTOCOL_VERSION) {
    write_envelope(request, response, this->allowed_origin_,
                   this->response_schema_, sourcemeta::one::STATUS_BAD_REQUEST,
                   unsupported_protocol_version());
    return;
  }

  request.body(
      [allowed_origin = std::string_view{this->allowed_origin_},
       response_schema = this->response_schema_,
       registry_url = std::string_view{this->registry_url_},
       &base = this->base_,
       &request_schema_template = this->request_schema_template_](
          sourcemeta::one::HTTPRequest &callback_request,
          sourcemeta::one::HTTPResponse &callback_response, std::string &&body,
          bool too_big) {
        if (too_big) {
          write_envelope(callback_request, callback_response, allowed_origin,
                         response_schema,
                         sourcemeta::one::STATUS_PAYLOAD_TOO_LARGE,
                         request_too_large());
          return;
        }
        handle_jsonrpc_message(callback_request, callback_response,
                               allowed_origin, response_schema,
                               request_schema_template, registry_url, base,
                               std::move(body));
      },
      [allowed_origin = std::string_view{this->allowed_origin_},
       response_schema = this->response_schema_](
          sourcemeta::one::HTTPRequest &callback_request,
          sourcemeta::one::HTTPResponse &callback_response,
          const std::exception_ptr &error) {
        try {
          std::rethrow_exception(error);
        } catch (const std::exception &) {
          write_envelope(callback_request, callback_response, allowed_origin,
                         response_schema,
                         sourcemeta::one::STATUS_INTERNAL_SERVER_ERROR,
                         sourcemeta::one::jsonrpc_make_error_internal());
        }
      });
}
