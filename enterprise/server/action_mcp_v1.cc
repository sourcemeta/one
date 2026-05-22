#include <sourcemeta/one/enterprise_server.h>

#include <sourcemeta/core/io.h>
#include <sourcemeta/core/json.h>
#include <sourcemeta/core/jsonrpc.h>
#include <sourcemeta/core/uri.h>

#include <sourcemeta/one/http.h>
#include <sourcemeta/one/mcp.h>
#include <sourcemeta/one/metapack.h>

#include <exception>   // std::exception
#include <filesystem>  // std::filesystem
#include <sstream>     // std::ostringstream
#include <string>      // std::string
#include <string_view> // std::string_view
#include <utility>     // std::move

namespace {

constexpr std::string_view MCP_TEMPLATE_MIME_TYPE{"application/schema+json"};

} // namespace

namespace sourcemeta::one::enterprise {

auto ActionMCP_v1::on_resources_list(const sourcemeta::core::JSON &request_json)
    const -> sourcemeta::core::JSON {
  const auto &id{request_json.at("id")};

  std::string cursor_key{"0"};
  const auto *params{sourcemeta::core::jsonrpc_params(request_json)};
  if (params != nullptr && params->defines("cursor")) {
    const auto &cursor_string{params->at("cursor").to_string()};
    if (!cursor_string.empty()) {
      const auto parsed{
          sourcemeta::one::mcp_parse_cursor_as_unsigned_integer(cursor_string)};
      if (!parsed.has_value()) {
        return sourcemeta::core::jsonrpc_make_error_invalid_params(
            id, sourcemeta::core::JSON{cursor_string});
      }
      cursor_key = cursor_string;
    }
  }

  const auto &resources{this->mcp_metadata_.at("resources")};
  if (!resources.defines(cursor_key)) {
    auto result{sourcemeta::core::JSON::make_object()};
    result.assign_assume_new(std::string{"resources"},
                             sourcemeta::core::JSON::make_array());
    return sourcemeta::core::jsonrpc_make_success(id, std::move(result));
  }

  return sourcemeta::core::jsonrpc_make_success(id, resources.at(cursor_key));
}

auto ActionMCP_v1::on_initialize(const sourcemeta::core::JSON &request_json)
    const -> sourcemeta::core::JSON {
  const auto &parts{
      this->mcp_metadata_.at(sourcemeta::one::MCP_METHOD_INITIALIZE)};
  auto result{sourcemeta::one::mcp_make_initialize_result(
      request_json.at("params"), parts.at("capabilities"),
      parts.at("serverInfo"), parts.at("instructions").to_string())};
  return sourcemeta::core::jsonrpc_make_success(request_json.at("id"),
                                                std::move(result));
}

auto ActionMCP_v1::on_tools_list(
    const sourcemeta::one::MCPProtocolVersion version,
    const sourcemeta::core::JSON &request_json) const
    -> sourcemeta::core::JSON {
  const auto &precomputed{
      this->mcp_metadata_.at(sourcemeta::one::MCP_METHOD_TOOLS_LIST)};
  if (sourcemeta::one::mcp_supports_output_schema(version)) {
    return sourcemeta::core::jsonrpc_make_success(request_json.at("id"),
                                                  precomputed);
  }

  auto tools{sourcemeta::core::JSON::make_array()};
  for (const auto &tool : precomputed.at("tools").as_array()) {
    std::optional<sourcemeta::core::JSON> output_schema;
    if (tool.defines("outputSchema")) {
      output_schema = tool.at("outputSchema");
    }
    std::optional<sourcemeta::core::JSON> annotations;
    if (tool.defines("annotations")) {
      annotations = tool.at("annotations");
    }
    tools.push_back(sourcemeta::one::mcp_make_tool_descriptor(
        version, tool.at("name").to_string(),
        tool.at("description").to_string(), tool.at("inputSchema"),
        std::move(output_schema), std::move(annotations)));
  }
  auto result{sourcemeta::core::JSON::make_object()};
  result.assign_assume_new(std::string{"tools"}, std::move(tools));
  return sourcemeta::core::jsonrpc_make_success(request_json.at("id"),
                                                std::move(result));
}

auto ActionMCP_v1::on_resources_read(const sourcemeta::core::JSON &request_json)
    const -> sourcemeta::core::JSON {
  const auto &id{request_json.at("id")};
  const auto &uri{request_json.at("params").at("uri").to_string()};

  std::filesystem::path resolved;
  try {
    sourcemeta::core::URI request{uri};
    request.relative_to(sourcemeta::core::URI{this->server_uri()});
    if (request.is_absolute()) {
      return sourcemeta::one::mcp_make_error_resource_not_found(id, uri);
    }
    const auto path{request.path().value_or("")};
    std::string_view schema_path{path};
    if (schema_path.ends_with(".json")) {
      schema_path.remove_suffix(5);
    }
    if (schema_path.empty()) {
      return sourcemeta::one::mcp_make_error_resource_not_found(id, uri);
    }
    const auto query{request.query()};
    const auto bundle{query.has_value() &&
                      !query->at("bundle").value_or("").empty()};
    resolved = sourcemeta::core::weakly_canonical(
        this->resolve_schema_path(schema_path, bundle));
  } catch (const std::exception &) {
    return sourcemeta::one::mcp_make_error_resource_not_found(id, uri);
  }

  if (!sourcemeta::core::is_under_path(resolved, this->base() / "schemas")) {
    return sourcemeta::one::mcp_make_error_resource_not_found(id, uri);
  }

  const auto schema{sourcemeta::one::metapack_read_json(resolved)};
  if (!schema.has_value()) {
    return sourcemeta::one::mcp_make_error_resource_not_found(id, uri);
  }

  std::ostringstream payload;
  sourcemeta::core::prettify(schema.value(), payload);

  auto contents{sourcemeta::core::JSON::make_array()};
  contents.push_back(sourcemeta::one::mcp_make_resource_text_content(
      uri, MCP_TEMPLATE_MIME_TYPE, payload.str()));
  return sourcemeta::core::jsonrpc_make_success(
      id, sourcemeta::one::mcp_make_resources_read_result(std::move(contents)));
}

auto ActionMCP_v1::on_tools_call(
    const sourcemeta::one::MCPProtocolVersion version,
    const sourcemeta::core::JSON &request_json, const std::string_view envelope)
    -> sourcemeta::core::JSON {
  const auto &id{request_json.at("id")};
  const auto &name{request_json.at("params").at("name").to_string()};
  const auto &tool_routes{this->mcp_metadata_.at("toolRoutes")};
  if (!tool_routes.defines(name)) {
    return sourcemeta::core::jsonrpc_make_error_invalid_params(
        id, sourcemeta::core::JSON{name});
  }
  const auto identifier{
      static_cast<sourcemeta::core::URITemplateRouter::Identifier>(
          tool_routes.at(name).to_integer())};
  auto *instance{this->dispatcher_.action(identifier)};
  if (instance == nullptr) [[unlikely]] {
    return sourcemeta::core::jsonrpc_make_error_internal(&id);
  }
  const auto *arguments{sourcemeta::one::mcp_tool_call_arguments(request_json)};
  if (arguments == nullptr) {
    return sourcemeta::core::jsonrpc_make_error_invalid_params(id);
  }
  try {
    return instance->mcp(version, id, *arguments, envelope);
  } catch (const std::exception &error) {
    return sourcemeta::one::mcp_make_tool_error(id, error.what());
  }
}

auto ActionMCP_v1::on_message(const sourcemeta::one::MCPProtocolVersion version,
                              sourcemeta::one::HTTPRequest &request,
                              sourcemeta::one::HTTPResponse &response,
                              std::string &&body) -> void {
  sourcemeta::core::JSON request_json{nullptr};
  try {
    request_json = sourcemeta::core::parse_json(body);
  } catch (const std::exception &) {
    this->write_envelope(request, response, sourcemeta::one::STATUS_OK,
                         sourcemeta::core::jsonrpc_make_error_parse());
    return;
  }

  if (sourcemeta::core::jsonrpc_is_notification(request_json)) {
    response.write_status(sourcemeta::one::STATUS_ACCEPTED);
    response.write_header("Access-Control-Allow-Origin", this->allowed_origin_);
    sourcemeta::one::send_response(sourcemeta::one::STATUS_ACCEPTED, request,
                                   response);
    return;
  }

  if (!sourcemeta::core::jsonrpc_is_request(request_json)) {
    this->write_envelope(
        request, response, sourcemeta::one::STATUS_OK,
        sourcemeta::core::jsonrpc_make_error_invalid_request(
            sourcemeta::core::jsonrpc_request_id(request_json)));
    return;
  }

  const auto &id{request_json.at("id")};
  const auto method{sourcemeta::core::jsonrpc_method(request_json)};
  sourcemeta::core::JSON envelope{nullptr};
  if (!sourcemeta::one::mcp_is_request_method(method)) {
    envelope = sourcemeta::core::jsonrpc_make_error_method_not_found(id);
  } else if (!this->validate(this->request_schema_, request_json)) {
    envelope = sourcemeta::core::jsonrpc_make_error_invalid_request(&id);
  } else if (method == sourcemeta::one::MCP_METHOD_INITIALIZE) {
    envelope = this->on_initialize(request_json);
  } else if (method == sourcemeta::one::MCP_METHOD_TOOLS_LIST) {
    envelope = this->on_tools_list(version, request_json);
  } else if (method == sourcemeta::one::MCP_METHOD_RESOURCES_LIST) {
    envelope = this->on_resources_list(request_json);
  } else if (method == sourcemeta::one::MCP_METHOD_RESOURCES_READ) {
    envelope = this->on_resources_read(request_json);
  } else if (method == sourcemeta::one::MCP_METHOD_TOOLS_CALL) {
    envelope = this->on_tools_call(version, request_json, body);
  } else if (this->mcp_metadata_.defines(method)) {
    envelope = sourcemeta::core::jsonrpc_make_success(
        id, this->mcp_metadata_.at(method));
  } else {
    envelope = sourcemeta::core::jsonrpc_make_success_empty(id);
  }

  this->write_envelope(request, response, sourcemeta::one::STATUS_OK, envelope);
}

} // namespace sourcemeta::one::enterprise
