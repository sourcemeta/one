#include <sourcemeta/one/enterprise_server.h>

#include <sourcemeta/core/json.h>
#include <sourcemeta/core/jsonrpc.h>
#include <sourcemeta/core/uri.h>

#include <sourcemeta/blaze/evaluator.h>
#include <sourcemeta/blaze/output.h>

#include <sourcemeta/one/http.h>
#include <sourcemeta/one/mcp.h>
#include <sourcemeta/one/metapack.h>
#include <sourcemeta/one/router.h>
#include <sourcemeta/one/search.h>
#include <sourcemeta/one/shared_version.h>

#include <algorithm> // std::ranges::transform
#include <cassert>   // assert
#include <cctype>    // std::tolower
#include <charconv>  // std::from_chars
#include <cstddef>   // std::size_t
#include <exception> // std::exception, std::exception_ptr, std::rethrow_exception
#include <filesystem>   // std::filesystem
#include <optional>     // std::optional
#include <sstream>      // std::ostringstream
#include <string>       // std::string
#include <string_view>  // std::string_view
#include <system_error> // std::errc
#include <utility>      // std::move

namespace {

constexpr std::string_view MCP_PROTOCOL_VERSION{"2025-11-25"};
constexpr std::string_view MCP_LEGACY_PROTOCOL_VERSION{"2025-03-26"};
constexpr std::string_view MCP_TEMPLATE_MIME_TYPE{"application/schema+json"};

auto write_envelope(sourcemeta::one::HTTPRequest &request,
                    sourcemeta::one::HTTPResponse &response,
                    const std::string_view allowed_origin,
                    const std::string_view response_schema, const char *status,
                    const sourcemeta::core::JSON &envelope) -> void {
  response.write_status(status);
  response.write_header("Content-Type", "application/json");
  response.write_header("Access-Control-Allow-Origin", allowed_origin);
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
  response.write_header("Access-Control-Allow-Origin", allowed_origin);
  sourcemeta::one::send_response(sourcemeta::one::STATUS_ACCEPTED, request,
                                 response);
}

auto method_not_allowed() -> sourcemeta::core::JSON {
  return sourcemeta::core::jsonrpc_make_error(nullptr, 4, "Method not allowed");
}

auto forbidden_origin() -> sourcemeta::core::JSON {
  return sourcemeta::core::jsonrpc_make_error(nullptr, 2, "Forbidden origin");
}

auto unsupported_protocol_version() -> sourcemeta::core::JSON {
  return sourcemeta::core::jsonrpc_make_error(nullptr, 3,
                                              "Unsupported protocol version");
}

auto request_too_large() -> sourcemeta::core::JSON {
  return sourcemeta::core::jsonrpc_make_error(nullptr, 5, "Request too large");
}

auto resource_not_found(const sourcemeta::core::JSON &id,
                        const std::string_view uri) -> sourcemeta::core::JSON {
  return sourcemeta::core::jsonrpc_make_error(&id, -32002, "Resource not found",
                                              sourcemeta::core::JSON{uri});
}

auto handle_initialize(const sourcemeta::core::JSON &request_json,
                       const sourcemeta::core::JSON &mcp_metadata)
    -> sourcemeta::core::JSON {
  return sourcemeta::core::jsonrpc_make_success(request_json.at("id"),
                                                mcp_metadata.at("initialize"));
}

auto handle_ping(const sourcemeta::core::JSON &request_json)
    -> sourcemeta::core::JSON {
  return sourcemeta::core::jsonrpc_make_success_empty(request_json.at("id"));
}

auto parse_cursor(const std::string_view cursor) -> std::optional<std::size_t> {
  if (cursor.empty()) {
    return std::nullopt;
  }
  if (cursor.size() > 1 && cursor.front() == '0') {
    return std::nullopt;
  }
  std::size_t parsed{0};
  const auto [end, error]{
      std::from_chars(cursor.data(), cursor.data() + cursor.size(), parsed)};
  if (error != std::errc{} || end != cursor.data() + cursor.size()) {
    return std::nullopt;
  }
  return parsed;
}

auto handle_resources_list(const sourcemeta::core::JSON &request_json,
                           const sourcemeta::core::JSON &mcp_metadata)
    -> sourcemeta::core::JSON {
  const auto &id{request_json.at("id")};

  std::string cursor_key{"0"};
  const auto *params{sourcemeta::core::jsonrpc_params(request_json)};
  if (params != nullptr && params->defines("cursor")) {
    const auto &cursor_string{params->at("cursor").to_string()};
    if (!cursor_string.empty()) {
      const auto parsed{parse_cursor(cursor_string)};
      if (!parsed.has_value()) {
        return sourcemeta::core::jsonrpc_make_error_invalid_params(
            id, sourcemeta::core::JSON{cursor_string});
      }
      cursor_key = cursor_string;
    }
  }

  const auto &resources{mcp_metadata.at("resources")};
  if (!resources.defines(cursor_key)) {
    auto result{sourcemeta::core::JSON::make_object()};
    result.assign_assume_new(std::string{"resources"},
                             sourcemeta::core::JSON::make_array());
    return sourcemeta::core::jsonrpc_make_success(id, std::move(result));
  }

  return sourcemeta::core::jsonrpc_make_success(id, resources.at(cursor_key));
}

auto handle_resources_templates_list(const sourcemeta::core::JSON &request_json,
                                     const sourcemeta::core::JSON &mcp_metadata)
    -> sourcemeta::core::JSON {
  auto result{sourcemeta::core::JSON::make_object()};
  result.assign_assume_new(
      std::string{"resourceTemplates"},
      sourcemeta::core::JSON{mcp_metadata.at("resourceTemplates")});
  return sourcemeta::core::jsonrpc_make_success(request_json.at("id"),
                                                std::move(result));
}

auto handle_tools_list(const sourcemeta::core::JSON &request_json,
                       const sourcemeta::core::JSON &mcp_metadata)
    -> sourcemeta::core::JSON {
  auto result{sourcemeta::core::JSON::make_object()};
  result.assign_assume_new(std::string{"tools"},
                           sourcemeta::core::JSON{mcp_metadata.at("tools")});
  return sourcemeta::core::jsonrpc_make_success(request_json.at("id"),
                                                std::move(result));
}

auto handle_tools_call(const sourcemeta::core::JSON &request_json,
                       const sourcemeta::core::JSON &mcp_metadata,
                       sourcemeta::one::Router &dispatcher)
    -> sourcemeta::core::JSON {
  const auto &id{request_json.at("id")};
  const auto &name{request_json.at("params").at("name").to_string()};
  const auto &tool_routes{mcp_metadata.at("toolRoutes")};
  if (!tool_routes.defines(name)) {
    return sourcemeta::core::jsonrpc_make_error_invalid_params(
        id, sourcemeta::core::JSON{name});
  }
  const auto identifier{
      static_cast<sourcemeta::core::URITemplateRouter::Identifier>(
          tool_routes.at(name).to_integer())};
  auto *instance{dispatcher.action(identifier)};
  assert(instance != nullptr);
  try {
    return instance->mcp(request_json);
  } catch (const std::exception &error) {
    return sourcemeta::one::mcp_make_tool_error(id, error.what());
  }
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
  sourcemeta::core::URI request{uri};
  const sourcemeta::core::URI registry{registry_url};
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
  const auto &uri{request_json.at("params").at("uri").to_string()};
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
  entry.assign_assume_new(std::string{"uri"}, sourcemeta::core::JSON{uri});
  entry.assign_assume_new(std::string{"mimeType"},
                          sourcemeta::core::JSON{MCP_TEMPLATE_MIME_TYPE});
  entry.assign_assume_new(std::string{"text"},
                          sourcemeta::core::JSON{payload.str()});

  auto contents{sourcemeta::core::JSON::make_array()};
  contents.push_back(std::move(entry));

  auto result{sourcemeta::core::JSON::make_object()};
  result.assign_assume_new(std::string{"contents"}, std::move(contents));
  return sourcemeta::core::jsonrpc_make_success(id, std::move(result));
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
    const sourcemeta::core::JSON &mcp_metadata,
    sourcemeta::one::Router &dispatcher, std::string &&body) -> void {
  sourcemeta::core::JSON request_json{nullptr};
  try {
    request_json = sourcemeta::core::parse_json(body);
  } catch (const std::exception &) {
    write_json_envelope(request, response, allowed_origin, response_schema,
                        sourcemeta::core::jsonrpc_make_error_parse());
    return;
  }

  if (sourcemeta::core::jsonrpc_is_notification(request_json)) {
    write_accepted(request, response, allowed_origin);
    return;
  }

  if (!sourcemeta::core::jsonrpc_is_request(request_json)) {
    write_json_envelope(
        request, response, allowed_origin, response_schema,
        sourcemeta::core::jsonrpc_make_error_invalid_request(
            sourcemeta::core::jsonrpc_request_id(request_json)));
    return;
  }

  const auto method{sourcemeta::core::jsonrpc_method(request_json)};
  if (method != "initialize" && method != "ping" &&
      method != "resources/list" && method != "resources/templates/list" &&
      method != "resources/read" && method != "tools/list" &&
      method != "tools/call") {
    write_json_envelope(request, response, allowed_origin, response_schema,
                        sourcemeta::core::jsonrpc_make_error_method_not_found(
                            request_json.at("id")));
    return;
  }

  if (!matches_request_schema(request_schema_template, request_json)) {
    write_json_envelope(request, response, allowed_origin, response_schema,
                        sourcemeta::core::jsonrpc_make_error_invalid_request(
                            &request_json.at("id")));
    return;
  }

  if (method == "initialize") {
    write_json_envelope(request, response, allowed_origin, response_schema,
                        handle_initialize(request_json, mcp_metadata));
    return;
  }

  if (method == "resources/list") {
    write_json_envelope(request, response, allowed_origin, response_schema,
                        handle_resources_list(request_json, mcp_metadata));
    return;
  }

  if (method == "resources/templates/list") {
    write_json_envelope(
        request, response, allowed_origin, response_schema,
        handle_resources_templates_list(request_json, mcp_metadata));
    return;
  }

  if (method == "resources/read") {
    write_json_envelope(
        request, response, allowed_origin, response_schema,
        handle_resources_read(request_json, registry_url, base));
    return;
  }

  if (method == "tools/list") {
    write_json_envelope(request, response, allowed_origin, response_schema,
                        handle_tools_list(request_json, mcp_metadata));
    return;
  }

  if (method == "tools/call") {
    write_json_envelope(
        request, response, allowed_origin, response_schema,
        handle_tools_call(request_json, mcp_metadata, dispatcher));
    return;
  }

  write_json_envelope(request, response, allowed_origin, response_schema,
                      handle_ping(request_json));
}

} // namespace

namespace sourcemeta::one::enterprise {

ActionMCP_v1::ActionMCP_v1(
    const std::filesystem::path &base,
    const sourcemeta::core::URITemplateRouterView &router,
    const sourcemeta::core::URITemplateRouter::Identifier identifier,
    sourcemeta::one::Router &dispatcher)
    : sourcemeta::one::RouterAction{base, router.base_path(),
                                    router.base_url()},
      dispatcher_{dispatcher} {
  std::string_view request_schema;
  router.arguments(
      identifier, [this, &request_schema](const auto &key, const auto &value) {
        if (key == "responseSchema") {
          this->response_schema_ = std::get<std::string_view>(value);
        } else if (key == "requestSchema") {
          request_schema = std::get<std::string_view>(value);
        }
      });

  this->request_schema_template_ = this->blaze_template(
      request_schema, sourcemeta::blaze::Mode::FastValidation);

  const auto mcp_metadata_path{this->base() / "explorer" / "%" /
                               "mcp.metapack"};
  auto mcp_metadata_option{
      sourcemeta::one::metapack_read_json(mcp_metadata_path)};
  assert(mcp_metadata_option.has_value());
  this->mcp_metadata_ = std::move(mcp_metadata_option.value());
  this->allowed_origin_ = this->mcp_metadata_.at("origin").to_string();
}

auto ActionMCP_v1::rest(const std::span<std::string_view>,
                        sourcemeta::one::HTTPRequest &request,
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
    response.write_header("Access-Control-Allow-Origin", this->allowed_origin_);
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
       registry_url = this->server_uri(), &base = this->base(),
       &request_schema_template = this->request_schema_template_,
       &mcp_metadata = this->mcp_metadata_, &dispatcher = this->dispatcher_](
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
                               mcp_metadata, dispatcher, std::move(body));
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
                         sourcemeta::core::jsonrpc_make_error_internal());
        }
      });
}

} // namespace sourcemeta::one::enterprise
