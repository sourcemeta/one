#include <sourcemeta/one/enterprise_server.h>

#include <sourcemeta/core/io.h>
#include <sourcemeta/core/json.h>
#include <sourcemeta/core/jsonrpc.h>
#include <sourcemeta/core/mcp.h>
#include <sourcemeta/core/numeric.h>
#include <sourcemeta/core/text.h>
#include <sourcemeta/core/uri.h>

#include <sourcemeta/one/http.h>
#include <sourcemeta/one/metapack.h>

#include <cassert>     // assert
#include <cstddef>     // std::size_t, std::ptrdiff_t
#include <exception>   // std::exception
#include <filesystem>  // std::filesystem
#include <iterator>    // std::ranges::distance
#include <optional>    // std::optional, std::nullopt
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
    const auto cursor_input{params->at("cursor").to_string()};
    if (!cursor_input.empty()) {
      const auto parsed{sourcemeta::core::to_uint64_t(cursor_input)};
      if (!parsed.has_value()) {
        return sourcemeta::core::jsonrpc_make_error(
            &id, -32602, "Invalid resource list cursor",
            sourcemeta::core::JSON{
                "Use the `nextCursor` returned by a prior resources/list "
                "response, or omit it to start from the beginning"});
      }
      cursor_key = std::to_string(parsed.value());
    }
  }

  const auto &resources{this->mcp_metadata_.at("resources")};
  if (!resources.defines(cursor_key)) {
    return sourcemeta::core::jsonrpc_make_error(
        &id, -32602, "Invalid resource list cursor",
        sourcemeta::core::JSON{
            "Use the `nextCursor` returned by a prior resources/list "
            "response, or omit it to start from the beginning"});
  }

  return sourcemeta::core::jsonrpc_make_success(id, resources.at(cursor_key));
}

auto ActionMCP_v1::on_initialize(const sourcemeta::core::JSON &request_json)
    const -> sourcemeta::core::JSON {
  const auto &parts{
      this->mcp_metadata_.at(sourcemeta::core::MCP_METHOD_INITIALIZE)};
  return sourcemeta::core::mcp_make_initialize_result(
      request_json,
      sourcemeta::core::MCPServerCapabilities{
          .prompts = parts.at(0).to_boolean(),
          .resources = parts.at(1).to_boolean(),
          .tools = parts.at(2).to_boolean(),
          .logging = parts.at(3).to_boolean(),
          .completions = parts.at(4).to_boolean(),
      },
      sourcemeta::core::MCPImplementation{
          .name = parts.at(5).to_string(),
          .version = parts.at(6).to_string(),
          .title = parts.at(7).to_string(),
          .description = parts.at(8).to_string(),
          .website_url = parts.at(9).to_string(),
      },
      parts.at(10).to_string());
}

auto ActionMCP_v1::on_tools_list(
    const sourcemeta::core::MCPProtocolVersion version,
    const sourcemeta::core::JSON &request_json) const
    -> sourcemeta::core::JSON {
  const auto &precomputed{
      this->mcp_metadata_.at(sourcemeta::core::MCP_METHOD_TOOLS_LIST)};

  auto tools{sourcemeta::core::JSON::make_array()};
  for (const auto &tool : precomputed.as_array()) {
    std::optional<sourcemeta::core::JSON> output_schema;
    if (!tool.at(3).is_null()) {
      output_schema = tool.at(3);
    }
    tools.push_back(sourcemeta::core::mcp_make_tool_descriptor(
        version, tool.at(0).to_string(), tool.at(1).to_string(), tool.at(2),
        std::move(output_schema),
        sourcemeta::core::MCPToolAnnotations{
            .title = tool.at(4).to_string(),
            .read_only = tool.at(5).to_boolean(),
            .destructive = tool.at(6).to_boolean(),
            .idempotent = tool.at(7).to_boolean(),
            .open_world = tool.at(8).to_boolean(),
        }));
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
    if (request.fragment().has_value()) {
      return sourcemeta::core::jsonrpc_make_error(
          &id, -32602, "Invalid resource schema URI",
          sourcemeta::core::JSON{
              "URIs accepted by resources/read must not contain a fragment "
              "and may only carry an optional `bundle` query parameter"});
    }
    request.canonicalize();
    // The MCP `resources/read` URI must match the `{+path}{?bundle}` resource
    // template exactly. Any query parameter other than `bundle` is outside the
    // template and must be rejected to keep the input shape predictable for
    // clients
    if (const auto query_view{request.query()}; query_view.has_value()) {
      const auto expected_size{
          static_cast<std::ptrdiff_t>(query_view->at("bundle").has_value())};
      if (std::ranges::distance(*query_view) != expected_size) {
        return sourcemeta::core::jsonrpc_make_error(
            &id, -32602, "Invalid resource schema URI",
            sourcemeta::core::JSON{
                "URIs accepted by resources/read must not contain a fragment "
                "and may only carry an optional `bundle` query parameter"});
      }
    }
    request.relative_to(sourcemeta::core::URI{this->server_uri()});
    if (request.is_absolute()) {
      return sourcemeta::core::jsonrpc_make_error(
          &id, -32007, "Foreign URI",
          sourcemeta::core::JSON{"This URI lies outside this catalog's "
                                 "namespace. Query the appropriate registry "
                                 "instead"});
    }
    const auto path{request.path().value_or("")};
    const auto schema_path{
        sourcemeta::core::remove_suffix_ignore_case(path, ".json")};
    if (schema_path.empty()) {
      return sourcemeta::core::jsonrpc_make_error(
          &id, sourcemeta::core::MCP_CODE_RESOURCE_NOT_FOUND,
          "Resource not found");
    }
    const auto query{request.query()};
    const auto bundle{query.has_value() && query->at("bundle").has_value()};
    resolved = sourcemeta::core::weakly_canonical(
        this->resolve_schema_path(schema_path, bundle));
  } catch (const std::exception &) {
    return sourcemeta::core::jsonrpc_make_error(
        &id, sourcemeta::core::MCP_CODE_RESOURCE_NOT_FOUND,
        "Resource not found");
  }

  if (!sourcemeta::core::is_under_path(resolved, this->base() / "schemas")) {
    return sourcemeta::core::jsonrpc_make_error(
        &id, sourcemeta::core::MCP_CODE_RESOURCE_NOT_FOUND,
        "Resource not found");
  }

  const auto schema{sourcemeta::one::metapack_read_json(resolved)};
  if (!schema.has_value()) {
    return sourcemeta::core::jsonrpc_make_error(
        &id, sourcemeta::core::MCP_CODE_RESOURCE_NOT_FOUND,
        "Resource not found");
  }

  std::ostringstream payload;
  sourcemeta::core::prettify(schema.value(), payload);

  auto contents{sourcemeta::core::JSON::make_array()};
  contents.push_back(sourcemeta::core::mcp_make_resource_text_content(
      uri, MCP_TEMPLATE_MIME_TYPE, payload.str()));
  return sourcemeta::core::jsonrpc_make_success(
      id,
      sourcemeta::core::mcp_make_resources_read_result(std::move(contents)));
}

auto ActionMCP_v1::on_tools_call(
    const sourcemeta::core::MCPProtocolVersion version,
    const sourcemeta::core::JSON &request_json, const std::string_view envelope)
    -> sourcemeta::core::JSON {
  const auto &id{request_json.at("id")};
  const auto &name{request_json.at("params").at("name").to_string()};
  const auto &tool_routes{this->mcp_metadata_.at("toolRoutes")};
  if (!tool_routes.defines(name)) {
    return sourcemeta::core::jsonrpc_make_error(
        &id, -32602, "Invalid tool name",
        sourcemeta::core::JSON{
            "Use `tools/list` to discover the available tools"});
  }
  const auto identifier{
      static_cast<sourcemeta::core::URITemplateRouter::Identifier>(
          tool_routes.at(name).to_integer())};
  auto *instance{this->dispatcher_.action(identifier)};
  if (instance == nullptr) [[unlikely]] {
    return sourcemeta::core::jsonrpc_make_error_internal(&id);
  }
  const auto *arguments{
      sourcemeta::core::mcp_tool_call_arguments(request_json)};
  const auto empty_arguments{sourcemeta::core::JSON::make_object()};
  try {
    return instance->mcp(version, id,
                         arguments == nullptr ? empty_arguments : *arguments,
                         envelope);
  } catch (const std::exception &error) {
    return sourcemeta::core::mcp_make_tool_error(id, error.what());
  }
}

auto ActionMCP_v1::on_message(
    const sourcemeta::core::MCPProtocolVersion version,
    sourcemeta::one::HTTPRequest &request,
    sourcemeta::one::HTTPResponse &response, std::string &&body) -> void {
  sourcemeta::core::JSON request_json{nullptr};
  try {
    request_json = sourcemeta::core::parse_json(body);
  } catch (const std::exception &) {
    this->write_envelope(request, response, sourcemeta::one::STATUS_OK,
                         sourcemeta::core::jsonrpc_make_error_parse());
    return;
  }

  if (request_json.is_array()) {
    if (version == sourcemeta::core::MCPProtocolVersion::V_2025_03_26) {
      // TODO: Support batches for strict compliance to MCP 2025-03-26
      this->write_envelope(
          request, response, sourcemeta::one::STATUS_OK,
          sourcemeta::core::jsonrpc_make_error(
              nullptr, -32006, "Unsupported operation",
              sourcemeta::core::JSON{
                  "Batch operations are not supported in this protocol "
                  "version yet"}));
    } else {
      this->write_envelope(
          request, response, sourcemeta::one::STATUS_OK,
          sourcemeta::core::jsonrpc_make_error_invalid_request(nullptr));
    }
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

  const auto *id{sourcemeta::core::jsonrpc_request_id(request_json)};
  assert(id != nullptr);
  const auto method{sourcemeta::core::jsonrpc_method(request_json)};
  sourcemeta::core::JSON envelope{nullptr};
  if (!sourcemeta::core::mcp_is_request_method(method)) {
    envelope = sourcemeta::core::jsonrpc_make_error_method_not_found(*id);
  } else if (!this->validate(this->request_schema_, request_json)) {
    envelope = sourcemeta::core::jsonrpc_make_error_invalid_request(id);
  } else if (method == sourcemeta::core::MCP_METHOD_INITIALIZE) {
    envelope = this->on_initialize(request_json);
  } else if (method == sourcemeta::core::MCP_METHOD_TOOLS_LIST) {
    envelope = this->on_tools_list(version, request_json);
  } else if (method == sourcemeta::core::MCP_METHOD_RESOURCES_LIST) {
    envelope = this->on_resources_list(request_json);
  } else if (method == sourcemeta::core::MCP_METHOD_RESOURCES_READ) {
    envelope = this->on_resources_read(request_json);
  } else if (method == sourcemeta::core::MCP_METHOD_TOOLS_CALL) {
    envelope = this->on_tools_call(version, request_json, body);
  } else if (this->mcp_metadata_.defines(method)) {
    envelope = sourcemeta::core::jsonrpc_make_success(
        *id, this->mcp_metadata_.at(method));
  } else {
    envelope = sourcemeta::core::jsonrpc_make_success_empty(*id);
  }

  this->write_envelope(request, response, sourcemeta::one::STATUS_OK, envelope);
}

} // namespace sourcemeta::one::enterprise
