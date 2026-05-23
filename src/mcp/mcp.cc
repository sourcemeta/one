#include <sourcemeta/one/mcp.h>

#include <sourcemeta/core/json.h>
#include <sourcemeta/core/jsonrpc.h>

#include <cassert>     // assert
#include <cstddef>     // std::size_t
#include <optional>    // std::optional
#include <sstream>     // std::ostringstream
#include <string>      // std::string
#include <string_view> // std::string_view
#include <utility>     // std::move

namespace {

const auto MCP_HASH_ANNOTATIONS{
    sourcemeta::core::JSON::make_object().as_object().hash("annotations")};
const auto MCP_HASH_ARGUMENTS{
    sourcemeta::core::JSON::make_object().as_object().hash("arguments")};
const auto MCP_HASH_CAPABILITIES{
    sourcemeta::core::JSON::make_object().as_object().hash("capabilities")};
const auto MCP_HASH_COMPLETIONS{
    sourcemeta::core::JSON::make_object().as_object().hash("completions")};
const auto MCP_HASH_CONTENT{
    sourcemeta::core::JSON::make_object().as_object().hash("content")};
const auto MCP_HASH_CONTENTS{
    sourcemeta::core::JSON::make_object().as_object().hash("contents")};
const auto MCP_HASH_DESCRIPTION{
    sourcemeta::core::JSON::make_object().as_object().hash("description")};
const auto MCP_HASH_DESTRUCTIVE_HINT{
    sourcemeta::core::JSON::make_object().as_object().hash("destructiveHint")};
const auto MCP_HASH_IDEMPOTENT_HINT{
    sourcemeta::core::JSON::make_object().as_object().hash("idempotentHint")};
const auto MCP_HASH_INPUT_SCHEMA{
    sourcemeta::core::JSON::make_object().as_object().hash("inputSchema")};
const auto MCP_HASH_INSTRUCTIONS{
    sourcemeta::core::JSON::make_object().as_object().hash("instructions")};
const auto MCP_HASH_IS_ERROR{
    sourcemeta::core::JSON::make_object().as_object().hash("isError")};
const auto MCP_HASH_LOGGING{
    sourcemeta::core::JSON::make_object().as_object().hash("logging")};
const auto MCP_HASH_MIME_TYPE{
    sourcemeta::core::JSON::make_object().as_object().hash("mimeType")};
const auto MCP_HASH_NAME{
    sourcemeta::core::JSON::make_object().as_object().hash("name")};
const auto MCP_HASH_OPEN_WORLD_HINT{
    sourcemeta::core::JSON::make_object().as_object().hash("openWorldHint")};
const auto MCP_HASH_OUTPUT_SCHEMA{
    sourcemeta::core::JSON::make_object().as_object().hash("outputSchema")};
const auto MCP_HASH_PROMPTS{
    sourcemeta::core::JSON::make_object().as_object().hash("prompts")};
const auto MCP_HASH_PROTOCOL_VERSION{
    sourcemeta::core::JSON::make_object().as_object().hash("protocolVersion")};
const auto MCP_HASH_READ_ONLY_HINT{
    sourcemeta::core::JSON::make_object().as_object().hash("readOnlyHint")};
const auto MCP_HASH_RESOURCES{
    sourcemeta::core::JSON::make_object().as_object().hash("resources")};
const auto MCP_HASH_SERVER_INFO{
    sourcemeta::core::JSON::make_object().as_object().hash("serverInfo")};
const auto MCP_HASH_SIZE{
    sourcemeta::core::JSON::make_object().as_object().hash("size")};
const auto MCP_HASH_STRUCTURED_CONTENT{
    sourcemeta::core::JSON::make_object().as_object().hash(
        "structuredContent")};
const auto MCP_HASH_TEXT{
    sourcemeta::core::JSON::make_object().as_object().hash("text")};
const auto MCP_HASH_TITLE{
    sourcemeta::core::JSON::make_object().as_object().hash("title")};
const auto MCP_HASH_TOOLS{
    sourcemeta::core::JSON::make_object().as_object().hash("tools")};
const auto MCP_HASH_TYPE{
    sourcemeta::core::JSON::make_object().as_object().hash("type")};
const auto MCP_HASH_URI{
    sourcemeta::core::JSON::make_object().as_object().hash("uri")};
const auto MCP_HASH_URI_TEMPLATE{
    sourcemeta::core::JSON::make_object().as_object().hash("uriTemplate")};
const auto MCP_HASH_VERSION{
    sourcemeta::core::JSON::make_object().as_object().hash("version")};
const auto MCP_HASH_WEBSITE_URL{
    sourcemeta::core::JSON::make_object().as_object().hash("websiteUrl")};

} // namespace

namespace sourcemeta::one {

auto mcp_make_text_block(const std::string_view text)
    -> sourcemeta::core::JSON {
  auto block{sourcemeta::core::JSON::make_object()};
  block.assign_assume_new(std::string{"type"}, sourcemeta::core::JSON{"text"},
                          MCP_HASH_TYPE);
  block.assign_assume_new(std::string{"text"}, sourcemeta::core::JSON{text},
                          MCP_HASH_TEXT);
  return block;
}

auto mcp_make_resource_link(const MCPProtocolVersion version,
                            const std::string_view uri,
                            const std::string_view mime_type,
                            const std::string_view name,
                            const std::string_view description)
    -> sourcemeta::core::JSON {
  if (!mcp_supports_resource_link_content(version)) {
    std::string text;
    if (!name.empty()) {
      text.append(name);
      if (!description.empty()) {
        text.append(" (");
        text.append(description);
        text.append(")");
      }
      text.append(": ");
    } else if (!description.empty()) {
      text.append(description);
      text.append(": ");
    }
    text.append(uri);
    return mcp_make_text_block(text);
  }

  auto block{sourcemeta::core::JSON::make_object()};
  block.assign_assume_new(std::string{"type"},
                          sourcemeta::core::JSON{"resource_link"},
                          MCP_HASH_TYPE);
  block.assign_assume_new(std::string{"uri"}, sourcemeta::core::JSON{uri},
                          MCP_HASH_URI);
  if (!name.empty()) {
    block.assign_assume_new(std::string{"name"}, sourcemeta::core::JSON{name},
                            MCP_HASH_NAME);
  }
  if (!description.empty()) {
    block.assign_assume_new(std::string{"description"},
                            sourcemeta::core::JSON{description},
                            MCP_HASH_DESCRIPTION);
  }
  block.assign_assume_new(std::string{"mimeType"},
                          sourcemeta::core::JSON{mime_type},
                          MCP_HASH_MIME_TYPE);
  return block;
}

auto mcp_make_tool_success(const MCPProtocolVersion version,
                           const sourcemeta::core::JSON &id,
                           sourcemeta::core::JSON result)
    -> sourcemeta::core::JSON {
  std::ostringstream payload;
  sourcemeta::core::prettify(result, payload);

  auto content{sourcemeta::core::JSON::make_array()};
  content.push_back(mcp_make_text_block(payload.str()));

  auto envelope_result{sourcemeta::core::JSON::make_object()};
  envelope_result.assign_assume_new(std::string{"content"}, std::move(content),
                                    MCP_HASH_CONTENT);
  if (mcp_supports_structured_content(version)) {
    envelope_result.assign_assume_new(std::string{"structuredContent"},
                                      std::move(result),
                                      MCP_HASH_STRUCTURED_CONTENT);
  }
  envelope_result.assign_assume_new(
      std::string{"isError"}, sourcemeta::core::JSON{false}, MCP_HASH_IS_ERROR);
  return sourcemeta::core::jsonrpc_make_success(id, std::move(envelope_result));
}

auto mcp_make_tool_success(const MCPProtocolVersion version,
                           const sourcemeta::core::JSON &id,
                           sourcemeta::core::JSON structured,
                           sourcemeta::core::JSON content_blocks)
    -> sourcemeta::core::JSON {
  auto envelope_result{sourcemeta::core::JSON::make_object()};
  envelope_result.assign_assume_new(
      std::string{"content"}, std::move(content_blocks), MCP_HASH_CONTENT);
  if (mcp_supports_structured_content(version)) {
    envelope_result.assign_assume_new(std::string{"structuredContent"},
                                      std::move(structured),
                                      MCP_HASH_STRUCTURED_CONTENT);
  }
  envelope_result.assign_assume_new(
      std::string{"isError"}, sourcemeta::core::JSON{false}, MCP_HASH_IS_ERROR);
  return sourcemeta::core::jsonrpc_make_success(id, std::move(envelope_result));
}

auto mcp_make_tool_error(const sourcemeta::core::JSON &id,
                         const std::string_view message)
    -> sourcemeta::core::JSON {
  auto content{sourcemeta::core::JSON::make_array()};
  content.push_back(mcp_make_text_block(message));

  auto envelope_result{sourcemeta::core::JSON::make_object()};
  envelope_result.assign_assume_new(std::string{"content"}, std::move(content),
                                    MCP_HASH_CONTENT);
  envelope_result.assign_assume_new(
      std::string{"isError"}, sourcemeta::core::JSON{true}, MCP_HASH_IS_ERROR);
  return sourcemeta::core::jsonrpc_make_success(id, std::move(envelope_result));
}

auto mcp_make_error_resource_not_found(const sourcemeta::core::JSON &id,
                                       const std::string_view uri)
    -> sourcemeta::core::JSON {
  return sourcemeta::core::jsonrpc_make_error(&id, MCP_CODE_RESOURCE_NOT_FOUND,
                                              "Resource not found",
                                              sourcemeta::core::JSON{uri});
}

auto mcp_make_resource(const std::string_view uri, const std::string_view name,
                       const std::string_view mime_type,
                       const std::string_view description,
                       const std::optional<std::size_t> size)
    -> sourcemeta::core::JSON {
  auto resource{sourcemeta::core::JSON::make_object()};
  resource.assign_assume_new(std::string{"uri"}, sourcemeta::core::JSON{uri},
                             MCP_HASH_URI);
  resource.assign_assume_new(std::string{"name"}, sourcemeta::core::JSON{name},
                             MCP_HASH_NAME);
  if (!description.empty()) {
    resource.assign_assume_new(std::string{"description"},
                               sourcemeta::core::JSON{description},
                               MCP_HASH_DESCRIPTION);
  }
  resource.assign_assume_new(std::string{"mimeType"},
                             sourcemeta::core::JSON{mime_type},
                             MCP_HASH_MIME_TYPE);
  if (size.has_value()) {
    resource.assign_assume_new(std::string{"size"},
                               sourcemeta::core::JSON{size.value()},
                               MCP_HASH_SIZE);
  }
  return resource;
}

auto mcp_make_resource_template(const std::string_view uri_template,
                                const std::string_view name,
                                const std::string_view description,
                                const std::string_view mime_type)
    -> sourcemeta::core::JSON {
  auto entry{sourcemeta::core::JSON::make_object()};
  entry.assign_assume_new(std::string{"uriTemplate"},
                          sourcemeta::core::JSON{uri_template},
                          MCP_HASH_URI_TEMPLATE);
  entry.assign_assume_new(std::string{"name"}, sourcemeta::core::JSON{name},
                          MCP_HASH_NAME);
  entry.assign_assume_new(std::string{"description"},
                          sourcemeta::core::JSON{description},
                          MCP_HASH_DESCRIPTION);
  entry.assign_assume_new(std::string{"mimeType"},
                          sourcemeta::core::JSON{mime_type},
                          MCP_HASH_MIME_TYPE);
  return entry;
}

auto mcp_make_tool_descriptor(
    const MCPProtocolVersion version, const std::string_view name,
    const std::string_view description, sourcemeta::core::JSON input_schema,
    std::optional<sourcemeta::core::JSON> output_schema,
    const MCPToolAnnotations &annotations) -> sourcemeta::core::JSON {
  assert(!annotations.read_only || !annotations.destructive);
  assert(!annotations.read_only || annotations.idempotent);

  auto entry{sourcemeta::core::JSON::make_object()};
  entry.assign_assume_new(std::string{"name"}, sourcemeta::core::JSON{name},
                          MCP_HASH_NAME);
  entry.assign_assume_new(std::string{"description"},
                          sourcemeta::core::JSON{description},
                          MCP_HASH_DESCRIPTION);
  entry.assign_assume_new(std::string{"inputSchema"}, std::move(input_schema),
                          MCP_HASH_INPUT_SCHEMA);
  if (output_schema.has_value() && mcp_supports_output_schema(version)) {
    entry.assign_assume_new(std::string{"outputSchema"},
                            std::move(output_schema).value(),
                            MCP_HASH_OUTPUT_SCHEMA);
  }

  auto annotations_object{sourcemeta::core::JSON::make_object()};
  if (!annotations.title.empty()) {
    annotations_object.assign_assume_new(
        std::string{"title"}, sourcemeta::core::JSON{annotations.title},
        MCP_HASH_TITLE);
  }
  annotations_object.assign_assume_new(
      std::string{"readOnlyHint"},
      sourcemeta::core::JSON{annotations.read_only}, MCP_HASH_READ_ONLY_HINT);
  annotations_object.assign_assume_new(
      std::string{"destructiveHint"},
      sourcemeta::core::JSON{annotations.destructive},
      MCP_HASH_DESTRUCTIVE_HINT);
  annotations_object.assign_assume_new(
      std::string{"idempotentHint"},
      sourcemeta::core::JSON{annotations.idempotent}, MCP_HASH_IDEMPOTENT_HINT);
  annotations_object.assign_assume_new(
      std::string{"openWorldHint"},
      sourcemeta::core::JSON{annotations.open_world}, MCP_HASH_OPEN_WORLD_HINT);
  entry.assign_assume_new(std::string{"annotations"},
                          std::move(annotations_object), MCP_HASH_ANNOTATIONS);

  return entry;
}

auto mcp_make_initialize_result(const sourcemeta::core::JSON &request,
                                const MCPServerCapabilities &capabilities,
                                const MCPImplementation &server,
                                const std::string_view instructions)
    -> sourcemeta::core::JSON {
  const auto *id{sourcemeta::core::jsonrpc_request_id(request)};
  const auto *params{sourcemeta::core::jsonrpc_params(request)};
  if (id == nullptr || params == nullptr || !params->is_object()) {
    return sourcemeta::core::jsonrpc_make_error_invalid_request(id);
  }

  std::string_view requested_version{};
  if (params->defines("protocolVersion", MCP_HASH_PROTOCOL_VERSION) &&
      params->at("protocolVersion", MCP_HASH_PROTOCOL_VERSION).is_string()) {
    requested_version =
        params->at("protocolVersion", MCP_HASH_PROTOCOL_VERSION).to_string();
  }
  const auto resolved{mcp_resolve_protocol_version(requested_version)};
  const auto version{resolved.value_or(MCPProtocolVersion::V_2025_11_25)};

  auto capabilities_object{sourcemeta::core::JSON::make_object()};
  if (capabilities.prompts) {
    capabilities_object.assign_assume_new(std::string{"prompts"},
                                          sourcemeta::core::JSON::make_object(),
                                          MCP_HASH_PROMPTS);
  }
  if (capabilities.resources) {
    capabilities_object.assign_assume_new(std::string{"resources"},
                                          sourcemeta::core::JSON::make_object(),
                                          MCP_HASH_RESOURCES);
  }
  if (capabilities.tools) {
    capabilities_object.assign_assume_new(std::string{"tools"},
                                          sourcemeta::core::JSON::make_object(),
                                          MCP_HASH_TOOLS);
  }
  if (capabilities.logging) {
    capabilities_object.assign_assume_new(std::string{"logging"},
                                          sourcemeta::core::JSON::make_object(),
                                          MCP_HASH_LOGGING);
  }
  if (capabilities.completions) {
    capabilities_object.assign_assume_new(std::string{"completions"},
                                          sourcemeta::core::JSON::make_object(),
                                          MCP_HASH_COMPLETIONS);
  }

  auto server_info{sourcemeta::core::JSON::make_object()};
  server_info.assign_assume_new(
      std::string{"name"}, sourcemeta::core::JSON{server.name}, MCP_HASH_NAME);
  server_info.assign_assume_new(std::string{"version"},
                                sourcemeta::core::JSON{server.version},
                                MCP_HASH_VERSION);
  if (!server.title.empty() && mcp_supports_implementation_title(version)) {
    server_info.assign_assume_new(std::string{"title"},
                                  sourcemeta::core::JSON{server.title},
                                  MCP_HASH_TITLE);
  }
  if (!server.description.empty() &&
      mcp_supports_implementation_description(version)) {
    server_info.assign_assume_new(std::string{"description"},
                                  sourcemeta::core::JSON{server.description},
                                  MCP_HASH_DESCRIPTION);
  }
  if (!server.website_url.empty() &&
      mcp_supports_implementation_website_url(version)) {
    server_info.assign_assume_new(std::string{"websiteUrl"},
                                  sourcemeta::core::JSON{server.website_url},
                                  MCP_HASH_WEBSITE_URL);
  }

  auto result{sourcemeta::core::JSON::make_object()};
  result.assign_assume_new(
      std::string{"protocolVersion"},
      sourcemeta::core::JSON{mcp_protocol_version_string(version)},
      MCP_HASH_PROTOCOL_VERSION);
  result.assign_assume_new(std::string{"capabilities"},
                           std::move(capabilities_object),
                           MCP_HASH_CAPABILITIES);
  result.assign_assume_new(std::string{"serverInfo"}, std::move(server_info),
                           MCP_HASH_SERVER_INFO);
  if (!instructions.empty()) {
    result.assign_assume_new(std::string{"instructions"},
                             sourcemeta::core::JSON{instructions},
                             MCP_HASH_INSTRUCTIONS);
  }
  return sourcemeta::core::jsonrpc_make_success(*id, std::move(result));
}

auto mcp_make_resource_text_content(const std::string_view uri,
                                    const std::string_view mime_type,
                                    const std::string_view text)
    -> sourcemeta::core::JSON {
  auto entry{sourcemeta::core::JSON::make_object()};
  entry.assign_assume_new(std::string{"uri"}, sourcemeta::core::JSON{uri},
                          MCP_HASH_URI);
  entry.assign_assume_new(std::string{"mimeType"},
                          sourcemeta::core::JSON{mime_type},
                          MCP_HASH_MIME_TYPE);
  entry.assign_assume_new(std::string{"text"}, sourcemeta::core::JSON{text},
                          MCP_HASH_TEXT);
  return entry;
}

auto mcp_make_resources_read_result(sourcemeta::core::JSON contents)
    -> sourcemeta::core::JSON {
  auto result{sourcemeta::core::JSON::make_object()};
  result.assign_assume_new(std::string{"contents"}, std::move(contents),
                           MCP_HASH_CONTENTS);
  return result;
}

auto mcp_tool_call_arguments(const sourcemeta::core::JSON &envelope)
    -> const sourcemeta::core::JSON * {
  const auto *params{sourcemeta::core::jsonrpc_params(envelope)};
  if (params == nullptr || !params->is_object() ||
      !params->defines("arguments", MCP_HASH_ARGUMENTS)) {
    return nullptr;
  }
  return &params->at("arguments", MCP_HASH_ARGUMENTS);
}

} // namespace sourcemeta::one
