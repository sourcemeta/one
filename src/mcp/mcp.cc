#include <sourcemeta/one/mcp.h>

#include <sourcemeta/core/json.h>
#include <sourcemeta/core/jsonrpc.h>

#include <charconv>     // std::from_chars
#include <cstddef>      // std::size_t
#include <optional>     // std::optional
#include <sstream>      // std::ostringstream
#include <string>       // std::string
#include <string_view>  // std::string_view
#include <system_error> // std::errc
#include <utility>      // std::move

namespace sourcemeta::one {

auto mcp_make_text_block(const std::string_view text)
    -> sourcemeta::core::JSON {
  auto block{sourcemeta::core::JSON::make_object()};
  block.assign_assume_new(std::string{"type"}, sourcemeta::core::JSON{"text"});
  block.assign_assume_new(std::string{"text"}, sourcemeta::core::JSON{text});
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
                          sourcemeta::core::JSON{"resource_link"});
  block.assign_assume_new(std::string{"uri"}, sourcemeta::core::JSON{uri});
  if (!name.empty()) {
    block.assign_assume_new(std::string{"name"}, sourcemeta::core::JSON{name});
  }
  if (!description.empty()) {
    block.assign_assume_new(std::string{"description"},
                            sourcemeta::core::JSON{description});
  }
  block.assign_assume_new(std::string{"mimeType"},
                          sourcemeta::core::JSON{mime_type});
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
  envelope_result.assign_assume_new(std::string{"content"}, std::move(content));
  if (mcp_supports_structured_content(version)) {
    envelope_result.assign_assume_new(std::string{"structuredContent"},
                                      std::move(result));
  }
  envelope_result.assign_assume_new(std::string{"isError"},
                                    sourcemeta::core::JSON{false});
  return sourcemeta::core::jsonrpc_make_success(id, std::move(envelope_result));
}

auto mcp_make_tool_success(const MCPProtocolVersion version,
                           const sourcemeta::core::JSON &id,
                           sourcemeta::core::JSON structured,
                           sourcemeta::core::JSON content_blocks)
    -> sourcemeta::core::JSON {
  auto envelope_result{sourcemeta::core::JSON::make_object()};
  envelope_result.assign_assume_new(std::string{"content"},
                                    std::move(content_blocks));
  if (mcp_supports_structured_content(version)) {
    envelope_result.assign_assume_new(std::string{"structuredContent"},
                                      std::move(structured));
  }
  envelope_result.assign_assume_new(std::string{"isError"},
                                    sourcemeta::core::JSON{false});
  return sourcemeta::core::jsonrpc_make_success(id, std::move(envelope_result));
}

auto mcp_make_tool_error(const sourcemeta::core::JSON &id,
                         const std::string_view message)
    -> sourcemeta::core::JSON {
  auto content{sourcemeta::core::JSON::make_array()};
  content.push_back(mcp_make_text_block(message));

  auto envelope_result{sourcemeta::core::JSON::make_object()};
  envelope_result.assign_assume_new(std::string{"content"}, std::move(content));
  envelope_result.assign_assume_new(std::string{"isError"},
                                    sourcemeta::core::JSON{true});
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
  resource.assign_assume_new(std::string{"uri"}, sourcemeta::core::JSON{uri});
  resource.assign_assume_new(std::string{"name"}, sourcemeta::core::JSON{name});
  if (!description.empty()) {
    resource.assign_assume_new(std::string{"description"},
                               sourcemeta::core::JSON{description});
  }
  resource.assign_assume_new(std::string{"mimeType"},
                             sourcemeta::core::JSON{mime_type});
  if (size.has_value()) {
    resource.assign_assume_new(std::string{"size"},
                               sourcemeta::core::JSON{size.value()});
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
                          sourcemeta::core::JSON{uri_template});
  entry.assign_assume_new(std::string{"name"}, sourcemeta::core::JSON{name});
  entry.assign_assume_new(std::string{"description"},
                          sourcemeta::core::JSON{description});
  entry.assign_assume_new(std::string{"mimeType"},
                          sourcemeta::core::JSON{mime_type});
  return entry;
}

auto mcp_make_tool_descriptor(
    const MCPProtocolVersion version, const std::string_view name,
    const std::string_view description, sourcemeta::core::JSON input_schema,
    std::optional<sourcemeta::core::JSON> output_schema,
    std::optional<sourcemeta::core::JSON> annotations)
    -> sourcemeta::core::JSON {
  auto entry{sourcemeta::core::JSON::make_object()};
  entry.assign_assume_new(std::string{"name"}, sourcemeta::core::JSON{name});
  entry.assign_assume_new(std::string{"description"},
                          sourcemeta::core::JSON{description});
  entry.assign_assume_new(std::string{"inputSchema"}, std::move(input_schema));
  if (output_schema.has_value() && mcp_supports_output_schema(version)) {
    entry.assign_assume_new(std::string{"outputSchema"},
                            std::move(output_schema).value());
  }
  if (annotations.has_value()) {
    auto annotations_value{std::move(annotations).value()};
    if (!mcp_supports_annotations_title(version) &&
        annotations_value.is_object() && annotations_value.defines("title")) {
      annotations_value.erase("title");
    }
    if (!annotations_value.is_object() || !annotations_value.empty()) {
      entry.assign_assume_new(std::string{"annotations"},
                              std::move(annotations_value));
    }
  }
  return entry;
}

auto mcp_make_initialize_result(const sourcemeta::core::JSON &params,
                                sourcemeta::core::JSON capabilities,
                                sourcemeta::core::JSON server_info,
                                const std::string_view instructions)
    -> sourcemeta::core::JSON {
  std::string_view requested_version{};
  if (params.is_object() && params.defines("protocolVersion") &&
      params.at("protocolVersion").is_string()) {
    requested_version = params.at("protocolVersion").to_string();
  }
  const auto resolved{mcp_resolve_protocol_version(requested_version)};
  const auto version{resolved.value_or(MCPProtocolVersion::V_2025_11_25)};

  if (!mcp_supports_implementation_title(version) && server_info.is_object() &&
      server_info.defines("title")) {
    server_info.erase("title");
  }

  auto result{sourcemeta::core::JSON::make_object()};
  result.assign_assume_new(
      std::string{"protocolVersion"},
      sourcemeta::core::JSON{mcp_protocol_version_string(version)});
  result.assign_assume_new(std::string{"capabilities"},
                           std::move(capabilities));
  result.assign_assume_new(std::string{"serverInfo"}, std::move(server_info));
  if (!instructions.empty()) {
    result.assign_assume_new(std::string{"instructions"},
                             sourcemeta::core::JSON{instructions});
  }
  return result;
}

auto mcp_make_resource_text_content(const std::string_view uri,
                                    const std::string_view mime_type,
                                    const std::string_view text)
    -> sourcemeta::core::JSON {
  auto entry{sourcemeta::core::JSON::make_object()};
  entry.assign_assume_new(std::string{"uri"}, sourcemeta::core::JSON{uri});
  entry.assign_assume_new(std::string{"mimeType"},
                          sourcemeta::core::JSON{mime_type});
  entry.assign_assume_new(std::string{"text"}, sourcemeta::core::JSON{text});
  return entry;
}

auto mcp_make_resources_read_result(sourcemeta::core::JSON contents)
    -> sourcemeta::core::JSON {
  auto result{sourcemeta::core::JSON::make_object()};
  result.assign_assume_new(std::string{"contents"}, std::move(contents));
  return result;
}

auto mcp_parse_cursor_as_unsigned_integer(const std::string_view cursor)
    -> std::optional<std::size_t> {
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

auto mcp_tool_call_arguments(const sourcemeta::core::JSON &envelope)
    -> const sourcemeta::core::JSON * {
  const auto *params{sourcemeta::core::jsonrpc_params(envelope)};
  if (params == nullptr || !params->is_object() ||
      !params->defines("arguments")) {
    return nullptr;
  }
  return &params->at("arguments");
}

} // namespace sourcemeta::one
