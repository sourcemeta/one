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

  bool bundle{false};
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
    bundle = request.query().has_value() &&
             request.query()->at("bundle").has_value();
  } catch (const std::exception &) {
    return sourcemeta::core::jsonrpc_make_error(
        &id, sourcemeta::core::MCP_CODE_RESOURCE_NOT_FOUND,
        "Resource not found");
  }

  const auto schema_artifact_path{this->artifact_resolve_path(
      uri, Tree::Schemas, bundle ? "bundle" : "schema")};
  if (!schema_artifact_path.has_value()) {
    return sourcemeta::core::jsonrpc_make_error(
        &id, sourcemeta::core::MCP_CODE_RESOURCE_NOT_FOUND,
        "Resource not found");
  }
  const auto schema{this->artifact_read_json(schema_artifact_path.value())};
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
    const sourcemeta::core::JSON &request_json) -> sourcemeta::core::JSON {
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
  auto *instance{this->dispatcher().action(identifier)};
  if (instance == nullptr) [[unlikely]] {
    return sourcemeta::core::jsonrpc_make_error_internal(&id);
  }
  const auto *arguments{
      sourcemeta::core::mcp_tool_call_arguments(request_json)};
  const auto empty_arguments{sourcemeta::core::JSON::make_object()};
  try {
    return instance->mcp(version, id,
                         arguments == nullptr ? empty_arguments : *arguments);
  } catch (const std::exception &error) {
    return sourcemeta::core::mcp_make_tool_error(id, error.what());
  }
}

auto ActionMCP_v1::process_one(
    const sourcemeta::core::MCPProtocolVersion version,
    const sourcemeta::core::JSON &request_json)
    -> std::optional<sourcemeta::core::JSON> {
  if (sourcemeta::core::jsonrpc_is_notification(request_json)) {
    return std::nullopt;
  }
  if (!sourcemeta::core::jsonrpc_is_request(request_json)) {
    return sourcemeta::core::jsonrpc_make_error_invalid_request(
        sourcemeta::core::jsonrpc_request_id(request_json));
  }
  const auto *id{sourcemeta::core::jsonrpc_request_id(request_json)};
  assert(id != nullptr);
  const auto method{sourcemeta::core::jsonrpc_method(request_json)};
  if (!sourcemeta::core::mcp_is_request_method(method)) {
    return sourcemeta::core::jsonrpc_make_error_method_not_found(*id);
  }
  // Schema validation enforces MCP's stricter rules on top of JSON-RPC 2.0.
  // Most notably, MCP requires `id` MUST NOT be null even though JSON-RPC
  // §4 only calls null-id "discouraged" (technically valid). Sourcemeta One
  // follows MCP's tighter rule and rejects null-id requests here.
  // https://www.jsonrpc.org/specification (§4)
  if (!this->schema_evaluate_fast(this->request_schema_, request_json)) {
    return sourcemeta::core::jsonrpc_make_error_invalid_request(id);
  }
  if (method == sourcemeta::core::MCP_METHOD_INITIALIZE) {
    return this->on_initialize(request_json);
  }
  if (method == sourcemeta::core::MCP_METHOD_TOOLS_LIST) {
    return this->on_tools_list(version, request_json);
  }
  if (method == sourcemeta::core::MCP_METHOD_RESOURCES_LIST) {
    return this->on_resources_list(request_json);
  }
  if (method == sourcemeta::core::MCP_METHOD_RESOURCES_READ) {
    return this->on_resources_read(request_json);
  }
  if (method == sourcemeta::core::MCP_METHOD_TOOLS_CALL) {
    return this->on_tools_call(version, request_json);
  }
  if (this->mcp_metadata_.defines(method)) {
    return sourcemeta::core::jsonrpc_make_success(
        *id, this->mcp_metadata_.at(method));
  }
  return sourcemeta::core::jsonrpc_make_success_empty(*id);
}

auto ActionMCP_v1::on_message(
    const sourcemeta::core::MCPProtocolVersion version,
    sourcemeta::one::HTTPRequest &request,
    sourcemeta::one::HTTPResponse &response, std::string &&body) -> void {
  sourcemeta::core::JSON request_json{nullptr};
  try {
    request_json = sourcemeta::core::parse_json(body);
  } catch (const std::exception &) {
    this->write_envelope(request, response, sourcemeta::core::HTTP_STATUS_OK,
                         sourcemeta::core::jsonrpc_make_error_parse(), version);
    return;
  }

  if (sourcemeta::core::jsonrpc_is_batch(request_json)) {
    if (!sourcemeta::core::mcp_supports_jsonrpc_batching(version)) {
      // MCP 2025-06-18 removed JSON-RPC batching. Any array body on a
      // protocol version that doesn't support batching is treated as an
      // unrecognized batch shape: per JSON-RPC 2.0 §6, an unrecognized
      // batch yields a single Invalid Request response object.
      // https://www.jsonrpc.org/specification#batch
      this->write_envelope(
          request, response, sourcemeta::core::HTTP_STATUS_OK,
          sourcemeta::core::jsonrpc_make_error_invalid_request(nullptr),
          version);
      return;
    }
    if (!sourcemeta::core::jsonrpc_is_valid_batch(request_json)) {
      // JSON-RPC 2.0 §6: "If the batch rpc call itself fails to be recognized
      // as a valid JSON or as an Array with at least one value, the response
      // from the Server MUST be a single Response object." Empty array body
      // falls here. https://www.jsonrpc.org/specification#batch
      this->write_envelope(
          request, response, sourcemeta::core::HTTP_STATUS_OK,
          sourcemeta::core::jsonrpc_make_error_invalid_request(nullptr),
          version);
      return;
    }
    auto responses{sourcemeta::core::JSON::make_array()};
    for (const auto &sub : request_json.as_array()) {
      sourcemeta::core::JSON sub_id{nullptr};
      if (const auto *parsed_id{sourcemeta::core::jsonrpc_request_id(sub)};
          parsed_id != nullptr) {
        sub_id = *parsed_id;
      }
      try {
        auto envelope{this->process_one(version, sub)};
        if (envelope.has_value()) {
          responses.push_back(std::move(envelope).value());
        }
      } catch (const std::exception &) {
        // One sub-element throwing cannot poison the rest of the batch
        responses.push_back(
            sourcemeta::core::jsonrpc_make_error_internal(&sub_id));
      }
    }
    if (responses.empty()) {
      // JSON-RPC 2.0 §6: "If there are no Response objects contained within
      // the Response array as it is to be sent to the client, the server
      // MUST NOT return an empty Array and should return nothing at all."
      // A batch of pure notifications falls here.
      // https://www.jsonrpc.org/specification#batch
      response.write_status(sourcemeta::core::HTTP_STATUS_ACCEPTED);
      response.write_header("Access-Control-Allow-Origin",
                            this->allowed_origin_);
      response.write_header("Access-Control-Expose-Headers", "Link, ETag");
      response.write_header(
          "MCP-Protocol-Version",
          sourcemeta::core::mcp_protocol_version_string(version));
      sourcemeta::one::send_response(sourcemeta::core::HTTP_STATUS_ACCEPTED,
                                     request, response);
      return;
    }
    this->write_envelope(request, response, sourcemeta::core::HTTP_STATUS_OK,
                         responses, version);
    return;
  }

  auto envelope{this->process_one(version, request_json)};
  if (envelope.has_value()) {
    this->write_envelope(request, response, sourcemeta::core::HTTP_STATUS_OK,
                         envelope.value(), version);
    return;
  }

  response.write_status(sourcemeta::core::HTTP_STATUS_ACCEPTED);
  response.write_header("Access-Control-Allow-Origin", this->allowed_origin_);
  response.write_header("Access-Control-Expose-Headers", "Link, ETag");
  response.write_header("MCP-Protocol-Version",
                        sourcemeta::core::mcp_protocol_version_string(version));
  sourcemeta::one::send_response(sourcemeta::core::HTTP_STATUS_ACCEPTED,
                                 request, response);
}

} // namespace sourcemeta::one::enterprise
