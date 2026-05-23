#ifndef SOURCEMETA_ONE_MCP_H
#define SOURCEMETA_ONE_MCP_H

#include <sourcemeta/core/json.h>
#include <sourcemeta/core/jsonrpc.h>

#include <cstddef>     // std::size_t
#include <cstdint>     // std::int64_t, std::uint8_t
#include <optional>    // std::optional
#include <string_view> // std::string_view
#include <utility>     // std::unreachable

namespace sourcemeta::one {

enum class MCPProtocolVersion : std::uint8_t {
  V_2025_03_26,
  V_2025_06_18,
  V_2025_11_25,
};

constexpr auto
mcp_protocol_version_string(const MCPProtocolVersion version) noexcept
    -> std::string_view {
  switch (version) {
    case MCPProtocolVersion::V_2025_03_26:
      return "2025-03-26";
    case MCPProtocolVersion::V_2025_06_18:
      return "2025-06-18";
    case MCPProtocolVersion::V_2025_11_25:
      return "2025-11-25";
  }
  std::unreachable();
}

constexpr std::string_view MCP_METHOD_INITIALIZE{"initialize"};
constexpr std::string_view MCP_METHOD_PING{"ping"};
constexpr std::string_view MCP_METHOD_TOOLS_LIST{"tools/list"};
constexpr std::string_view MCP_METHOD_TOOLS_CALL{"tools/call"};
constexpr std::string_view MCP_METHOD_RESOURCES_LIST{"resources/list"};
constexpr std::string_view MCP_METHOD_RESOURCES_READ{"resources/read"};
constexpr std::string_view MCP_METHOD_RESOURCES_TEMPLATES_LIST{
    "resources/templates/list"};
constexpr std::string_view MCP_METHOD_NOTIFICATIONS_INITIALIZED{
    "notifications/initialized"};

constexpr std::int64_t MCP_CODE_RESOURCE_NOT_FOUND{-32002};
constexpr std::int64_t MCP_CODE_URL_ELICITATION_REQUIRED{-32042};

auto mcp_make_text_block(const std::string_view text) -> sourcemeta::core::JSON;

auto mcp_make_resource_link(const MCPProtocolVersion version,
                            const std::string_view uri,
                            const std::string_view mime_type,
                            const std::string_view name = {},
                            const std::string_view description = {})
    -> sourcemeta::core::JSON;

auto mcp_make_tool_success(const MCPProtocolVersion version,
                           const sourcemeta::core::JSON &id,
                           sourcemeta::core::JSON result)
    -> sourcemeta::core::JSON;

auto mcp_make_tool_success(const MCPProtocolVersion version,
                           const sourcemeta::core::JSON &id,
                           sourcemeta::core::JSON structured,
                           sourcemeta::core::JSON content_blocks)
    -> sourcemeta::core::JSON;

auto mcp_make_tool_error(const sourcemeta::core::JSON &id,
                         const std::string_view message)
    -> sourcemeta::core::JSON;

auto mcp_make_error_resource_not_found(const sourcemeta::core::JSON &id,
                                       const std::string_view uri)
    -> sourcemeta::core::JSON;

auto mcp_make_resource(const std::string_view uri, const std::string_view name,
                       const std::string_view mime_type,
                       const std::string_view description = {},
                       const std::optional<std::size_t> size = std::nullopt)
    -> sourcemeta::core::JSON;

auto mcp_make_resource_text_content(const std::string_view uri,
                                    const std::string_view mime_type,
                                    const std::string_view text)
    -> sourcemeta::core::JSON;

auto mcp_make_resources_read_result(sourcemeta::core::JSON contents)
    -> sourcemeta::core::JSON;

auto mcp_make_resource_template(const std::string_view uri_template,
                                const std::string_view name,
                                const std::string_view description,
                                const std::string_view mime_type)
    -> sourcemeta::core::JSON;

struct MCPToolAnnotations {
  std::string_view title = {};
  bool read_only = false;
  bool destructive = true;
  bool idempotent = false;
  bool open_world = true;
};

auto mcp_make_tool_descriptor(
    const MCPProtocolVersion version, const std::string_view name,
    const std::string_view description, sourcemeta::core::JSON input_schema,
    std::optional<sourcemeta::core::JSON> output_schema = std::nullopt,
    const MCPToolAnnotations &annotations = {}) -> sourcemeta::core::JSON;

struct MCPImplementation {
  std::string_view name;
  std::string_view version;
  std::string_view title = {};
  std::string_view description = {};
  std::string_view website_url = {};
};

struct MCPServerCapabilities {
  bool prompts = false;
  bool resources = false;
  bool tools = false;
  bool logging = false;
  bool completions = false;
};

auto mcp_make_initialize_result(const sourcemeta::core::JSON &request,
                                const MCPServerCapabilities &capabilities,
                                const MCPImplementation &server,
                                const std::string_view instructions = {})
    -> sourcemeta::core::JSON;

auto mcp_tool_call_arguments(const sourcemeta::core::JSON &envelope)
    -> const sourcemeta::core::JSON *;

constexpr auto mcp_is_request_method(const std::string_view method) noexcept
    -> bool {
  return method == MCP_METHOD_INITIALIZE || method == MCP_METHOD_PING ||
         method == MCP_METHOD_TOOLS_LIST || method == MCP_METHOD_TOOLS_CALL ||
         method == MCP_METHOD_RESOURCES_LIST ||
         method == MCP_METHOD_RESOURCES_READ ||
         method == MCP_METHOD_RESOURCES_TEMPLATES_LIST;
}

constexpr auto
mcp_resolve_protocol_version(const std::string_view header) noexcept
    -> std::optional<MCPProtocolVersion> {
  if (header.empty()) {
    return MCPProtocolVersion::V_2025_03_26;
  }
  if (header == "2025-11-25") {
    return MCPProtocolVersion::V_2025_11_25;
  }
  if (header == "2025-06-18") {
    return MCPProtocolVersion::V_2025_06_18;
  }
  if (header == "2025-03-26") {
    return MCPProtocolVersion::V_2025_03_26;
  }
  return std::nullopt;
}

constexpr auto
mcp_supports_output_schema(const MCPProtocolVersion version) noexcept -> bool {
  return version != MCPProtocolVersion::V_2025_03_26;
}

constexpr auto
mcp_supports_structured_content(const MCPProtocolVersion version) noexcept
    -> bool {
  return version != MCPProtocolVersion::V_2025_03_26;
}

constexpr auto
mcp_supports_resource_link_content(const MCPProtocolVersion version) noexcept
    -> bool {
  return version != MCPProtocolVersion::V_2025_03_26;
}

constexpr auto
mcp_supports_implementation_title(const MCPProtocolVersion version) noexcept
    -> bool {
  return version != MCPProtocolVersion::V_2025_03_26;
}

constexpr auto mcp_supports_implementation_description(
    const MCPProtocolVersion version) noexcept -> bool {
  return version == MCPProtocolVersion::V_2025_11_25;
}

constexpr auto mcp_supports_implementation_website_url(
    const MCPProtocolVersion version) noexcept -> bool {
  return version == MCPProtocolVersion::V_2025_11_25;
}

} // namespace sourcemeta::one

#endif
