#ifndef SOURCEMETA_ONE_MCP_H
#define SOURCEMETA_ONE_MCP_H

#include <sourcemeta/core/json.h>
#include <sourcemeta/core/jsonrpc.h>

#include <cstddef>     // std::size_t
#include <cstdint>     // std::int64_t
#include <optional>    // std::optional
#include <string_view> // std::string_view

namespace sourcemeta::one {

constexpr std::string_view MCP_PROTOCOL_VERSION{"2025-11-25"};

// Per the streamable-HTTP transport spec, the protocol revision that pre-dates
// the `MCP-Protocol-Version` header is the assumed default when no header is
// sent and must still be honored when sent explicitly.
constexpr std::string_view MCP_LEGACY_PROTOCOL_VERSION{"2025-03-26"};

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

auto mcp_make_text_block(std::string_view text) -> sourcemeta::core::JSON;

auto mcp_make_resource_link(std::string_view uri, std::string_view mime_type,
                            std::string_view name = {},
                            std::string_view description = {})
    -> sourcemeta::core::JSON;

auto mcp_make_tool_success(const sourcemeta::core::JSON &id,
                           sourcemeta::core::JSON result)
    -> sourcemeta::core::JSON;

auto mcp_make_tool_success(const sourcemeta::core::JSON &id,
                           sourcemeta::core::JSON structured,
                           sourcemeta::core::JSON content_blocks)
    -> sourcemeta::core::JSON;

auto mcp_make_tool_error(const sourcemeta::core::JSON &id,
                         std::string_view message) -> sourcemeta::core::JSON;

auto mcp_make_error_resource_not_found(const sourcemeta::core::JSON &id,
                                       std::string_view uri)
    -> sourcemeta::core::JSON;

auto mcp_make_resource(std::string_view uri, std::string_view name,
                       std::string_view mime_type,
                       std::string_view description = {},
                       std::optional<std::size_t> size = std::nullopt)
    -> sourcemeta::core::JSON;

auto mcp_make_resource_text_content(std::string_view uri,
                                    std::string_view mime_type,
                                    std::string_view text)
    -> sourcemeta::core::JSON;

auto mcp_make_resources_read_result(sourcemeta::core::JSON contents)
    -> sourcemeta::core::JSON;

auto mcp_make_resource_template(std::string_view uri_template,
                                std::string_view name,
                                std::string_view description,
                                std::string_view mime_type)
    -> sourcemeta::core::JSON;

auto mcp_make_tool_descriptor(
    std::string_view name, std::string_view description,
    sourcemeta::core::JSON input_schema,
    std::optional<sourcemeta::core::JSON> output_schema = std::nullopt,
    std::optional<sourcemeta::core::JSON> annotations = std::nullopt)
    -> sourcemeta::core::JSON;

auto mcp_make_initialize_result(std::string_view protocol_version,
                                sourcemeta::core::JSON capabilities,
                                sourcemeta::core::JSON server_info,
                                std::string_view instructions = {})
    -> sourcemeta::core::JSON;

auto mcp_tool_call_arguments(const sourcemeta::core::JSON &envelope)
    -> const sourcemeta::core::JSON *;

auto mcp_parse_cursor_as_unsigned_integer(std::string_view cursor)
    -> std::optional<std::size_t>;

constexpr auto mcp_is_request_method(const std::string_view method) noexcept
    -> bool {
  return method == MCP_METHOD_INITIALIZE || method == MCP_METHOD_PING ||
         method == MCP_METHOD_TOOLS_LIST || method == MCP_METHOD_TOOLS_CALL ||
         method == MCP_METHOD_RESOURCES_LIST ||
         method == MCP_METHOD_RESOURCES_READ ||
         method == MCP_METHOD_RESOURCES_TEMPLATES_LIST;
}

constexpr auto
mcp_is_supported_protocol_version(const std::string_view version) noexcept
    -> bool {
  return version == MCP_PROTOCOL_VERSION ||
         version == MCP_LEGACY_PROTOCOL_VERSION;
}

} // namespace sourcemeta::one

#endif
