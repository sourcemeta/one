#ifndef SOURCEMETA_ONE_ENTERPRISE_SERVER_ACTIONS_H_
#define SOURCEMETA_ONE_ENTERPRISE_SERVER_ACTIONS_H_

#include <sourcemeta/core/json.h>
#include <sourcemeta/core/jsonrpc.h>
#include <sourcemeta/core/mcp.h>
#include <sourcemeta/core/uritemplate.h>

#include <sourcemeta/one/http.h>
#include <sourcemeta/one/metapack.h>
#include <sourcemeta/one/router.h>

#include <cassert>   // assert
#include <exception> // std::exception, std::exception_ptr, std::rethrow_exception
#include <filesystem>  // std::filesystem::path
#include <optional>    // std::optional
#include <span>        // std::span
#include <sstream>     // std::ostringstream
#include <string>      // std::string
#include <string_view> // std::string_view
#include <utility>     // std::move

namespace sourcemeta::one::enterprise {

class ActionMCP_v1 : public sourcemeta::one::RouterAction {
public:
  static constexpr std::string_view DESCRIPTION{
      "Handle Model Context Protocol JSON-RPC requests"};
  static constexpr bool READ_ONLY{true};
  static constexpr bool DESTRUCTIVE{false};
  static constexpr bool IDEMPOTENT{true};
  static constexpr bool OPEN_WORLD{false};

  ActionMCP_v1(const std::filesystem::path &base,
               const sourcemeta::core::URITemplateRouterView &router,
               const sourcemeta::core::URITemplateRouter::Identifier identifier,
               sourcemeta::one::Router &dispatcher)
      : sourcemeta::one::RouterAction{base, router.base_path(),
                                      router.base_url(), dispatcher} {
    router.arguments(identifier, [this](const auto &key, const auto &value) {
      if (key == "responseSchema") {
        this->response_schema_ = std::get<std::string_view>(value);
      } else if (key == "requestSchema") {
        this->request_schema_ = std::get<std::string_view>(value);
      }
    });

    const auto mcp_metadata_path{
        this->artifact_resolve_path("", Tree::Explorer, "mcp")};
    assert(mcp_metadata_path.has_value());
    auto mcp_metadata_option{
        this->artifact_read_json(mcp_metadata_path.value())};
    assert(mcp_metadata_option.has_value());
    this->mcp_metadata_ = std::move(mcp_metadata_option.value());
    this->allowed_origin_ = this->mcp_metadata_.at("origin").to_string();
  }

  auto rest(const std::span<std::string_view>,
            sourcemeta::one::HTTPRequest &request,
            sourcemeta::one::HTTPResponse &response) -> void override {
    // MCP Streamable HTTP transport / Security Warning:
    // "Servers MUST validate the Origin header on all incoming connections
    // to prevent DNS rebinding attacks." Mandatory per the MCP spec, not
    // defensive-only. Do not remove this check.
    // https://modelcontextprotocol.io/specification/2025-06-18/basic/transports#security-warning
    //
    // RFC 6454 §7.3: user agents send `Origin: null` from privacy-sensitive
    // contexts (sandboxed iframes, file://, data:). The literal "null" is
    // never a real allowlisted origin, so the byte-compare below correctly
    // rejects it. https://datatracker.ietf.org/doc/html/rfc6454#section-7.3
    const auto origin_header{request.header("origin")};
    if (!origin_header.empty() && origin_header != this->allowed_origin_) {
      this->write_envelope(
          request, response, sourcemeta::core::HTTP_STATUS_FORBIDDEN,
          sourcemeta::core::jsonrpc_make_error(
              nullptr, -32001, "Forbidden origin",
              sourcemeta::core::JSON{std::string{origin_header}}));
      return;
    }

    if (request.method() == "options") {
      response.write_status(sourcemeta::core::HTTP_STATUS_NO_CONTENT);
      response.write_header("Access-Control-Allow-Origin",
                            this->allowed_origin_);
      response.write_header("Access-Control-Allow-Methods", "POST, OPTIONS");
      response.write_header("Access-Control-Allow-Headers",
                            "Content-Type, MCP-Protocol-Version");
      response.write_header("Access-Control-Max-Age", "3600");
      // RFC 9110 §9.3.7: OPTIONS responses SHOULD include Allow. Different
      // audience than Access-Control-Allow-Methods (HTTP vs CORS preflight).
      // https://datatracker.ietf.org/doc/html/rfc9110#section-9.3.7
      response.write_header("Allow", "POST, OPTIONS");
      // Debuggability echo (spec default, no negotiation has happened).
      response.write_header(
          "MCP-Protocol-Version",
          sourcemeta::core::mcp_protocol_version_string(
              sourcemeta::core::MCPProtocolVersion::V_2025_03_26));
      sourcemeta::one::send_response(sourcemeta::core::HTTP_STATUS_NO_CONTENT,
                                     request, response);
      return;
    }

    if (request.method() != "post") {
      this->write_envelope(request, response,
                           sourcemeta::core::HTTP_STATUS_METHOD_NOT_ALLOWED,
                           sourcemeta::core::jsonrpc_make_error(
                               nullptr, -32004, "Method not allowed"));
      return;
    }

    // MCP Streamable HTTP transport / Protocol Version Header:
    // - Client MUST send `MCP-Protocol-Version` on every request after init
    // - If header is absent, server SHOULD assume 2025-03-26 (handled inside
    //   `mcp_resolve_protocol_version`)
    // - If header is invalid/unsupported, server MUST respond with 400
    // https://modelcontextprotocol.io/specification/2025-06-18/basic/transports#protocol-version-header
    const auto requested_version{request.header("mcp-protocol-version")};
    const auto negotiated_version{
        sourcemeta::core::mcp_resolve_protocol_version(requested_version)};
    if (!negotiated_version.has_value()) {
      auto data{sourcemeta::core::JSON::make_object()};
      data.assign("current",
                  sourcemeta::core::JSON{std::string{requested_version}});
      auto supported{sourcemeta::core::JSON::make_array()};
      supported.push_back(sourcemeta::core::JSON{"2025-03-26"});
      supported.push_back(sourcemeta::core::JSON{"2025-06-18"});
      supported.push_back(sourcemeta::core::JSON{"2025-11-25"});
      data.assign("supported", std::move(supported));
      this->write_envelope(request, response,
                           sourcemeta::core::HTTP_STATUS_BAD_REQUEST,
                           sourcemeta::core::jsonrpc_make_error(
                               nullptr, -32003, "Unsupported protocol version",
                               std::move(data)));
      return;
    }

    request.body(
        [this, version = negotiated_version.value()](
            sourcemeta::one::HTTPRequest &callback_request,
            sourcemeta::one::HTTPResponse &callback_response,
            std::string &&body, bool too_big) {
          if (too_big) {
            this->write_envelope(
                callback_request, callback_response,
                sourcemeta::core::HTTP_STATUS_CONTENT_TOO_LARGE,
                sourcemeta::core::jsonrpc_make_error(nullptr, -32005,
                                                     "Request too large"),
                version);
            return;
          }
          this->on_message(version, callback_request, callback_response,
                           std::move(body));
        },
        [this, version = negotiated_version.value()](
            sourcemeta::one::HTTPRequest &callback_request,
            sourcemeta::one::HTTPResponse &callback_response,
            const std::exception_ptr &error) {
          try {
            std::rethrow_exception(error);
          } catch (const std::exception &) {
            this->write_envelope(
                callback_request, callback_response,
                sourcemeta::core::HTTP_STATUS_INTERNAL_SERVER_ERROR,
                sourcemeta::core::jsonrpc_make_error_internal(), version);
          }
        });
  }

  auto mcp(const sourcemeta::core::MCPProtocolVersion,
           const sourcemeta::core::JSON &id, const sourcemeta::core::JSON &)
      -> sourcemeta::core::JSON override {
    return sourcemeta::core::jsonrpc_make_error_method_not_found(id);
  }

private:
  auto
  write_envelope(sourcemeta::one::HTTPRequest &request,
                 sourcemeta::one::HTTPResponse &response,
                 const sourcemeta::core::HTTPStatus &status,
                 const sourcemeta::core::JSON &envelope,
                 const sourcemeta::core::MCPProtocolVersion version =
                     sourcemeta::core::MCPProtocolVersion::V_2025_03_26) const
      -> void {
    response.write_status(status);
    response.write_header("Content-Type", "application/json");
    response.write_header("Access-Control-Allow-Origin", this->allowed_origin_);
    // RFC 9110 §15.5.6: 405 responses MUST carry Allow listing supported
    // methods. The MCP endpoint accepts POST and OPTIONS only.
    // https://datatracker.ietf.org/doc/html/rfc9110#section-15.5.6
    if (status == sourcemeta::core::HTTP_STATUS_METHOD_NOT_ALLOWED) {
      response.write_header("Allow", "POST, OPTIONS");
    }
    // Debuggability echo: not mandated by MCP, but lets clients confirm
    // which protocol revision the server interpreted. For error paths
    // where no negotiation succeeded, we echo the spec-default `2025-03-26`.
    // https://modelcontextprotocol.io/specification/2025-06-18/basic/transports#protocol-version-header
    response.write_header(
        "MCP-Protocol-Version",
        sourcemeta::core::mcp_protocol_version_string(version));
    if (!this->response_schema_.empty()) {
      sourcemeta::one::write_link_header(response, this->response_schema_);
    }
    std::ostringstream payload;
    sourcemeta::core::prettify(envelope, payload);
    sourcemeta::one::send_response(status, request, response, payload.str(),
                                   sourcemeta::one::Encoding::Identity);
  }

  auto on_resources_list(const sourcemeta::core::JSON &request_json) const
      -> sourcemeta::core::JSON;

  auto on_initialize(const sourcemeta::core::JSON &request_json) const
      -> sourcemeta::core::JSON;

  auto on_tools_list(sourcemeta::core::MCPProtocolVersion version,
                     const sourcemeta::core::JSON &request_json) const
      -> sourcemeta::core::JSON;

  auto on_resources_read(const sourcemeta::core::JSON &request_json) const
      -> sourcemeta::core::JSON;

  auto on_tools_call(sourcemeta::core::MCPProtocolVersion version,
                     const sourcemeta::core::JSON &request_json)
      -> sourcemeta::core::JSON;

  auto on_message(sourcemeta::core::MCPProtocolVersion version,
                  sourcemeta::one::HTTPRequest &request,
                  sourcemeta::one::HTTPResponse &response, std::string &&body)
      -> void;

  auto process_one(sourcemeta::core::MCPProtocolVersion version,
                   const sourcemeta::core::JSON &request_json)
      -> std::optional<sourcemeta::core::JSON>;

  std::string_view allowed_origin_;
  std::string_view response_schema_;
  std::string_view request_schema_;
  sourcemeta::core::JSON mcp_metadata_{nullptr};
};

} // namespace sourcemeta::one::enterprise

#endif
