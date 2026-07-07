#ifndef SOURCEMETA_ONE_ACTIONS_MCP_V1_H
#define SOURCEMETA_ONE_ACTIONS_MCP_V1_H

#if defined(SOURCEMETA_ONE_ENTERPRISE)

#include <sourcemeta/one/enterprise_server.h>

using ActionMCP_v1 = sourcemeta::one::enterprise::ActionMCP_v1;

#else

#include <sourcemeta/core/json.h>
#include <sourcemeta/core/jsonrpc.h>
#include <sourcemeta/core/mcp.h>
#include <sourcemeta/core/uri.h>
#include <sourcemeta/core/uritemplate.h>

#include <sourcemeta/one/http.h>
#include <sourcemeta/one/metapack.h>
#include <sourcemeta/one/router.h>
#include <sourcemeta/one/shared.h>

#include "action_jsonschema_serve_v1.h"

#include <cassert>   // assert
#include <exception> // std::exception, std::exception_ptr, std::rethrow_exception
#include <filesystem>  // std::filesystem
#include <optional>    // std::optional
#include <span>        // std::span
#include <sstream>     // std::ostringstream
#include <string>      // std::string
#include <string_view> // std::string_view
#include <utility>     // std::move

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
    router.arguments(
        identifier, [this](const auto &key, const auto &value) -> void {
          if (key == "responseSchema") {
            this->response_schema_ = std::get<std::string_view>(value);
          }
        });

    const auto mcp_metadata_path{
        this->artifact_resolve_path_unauthenticated("", Tree::Explorer, "mcp")};
    assert(mcp_metadata_path.has_value());
    auto mcp_metadata_option{
        this->artifact_read_json(mcp_metadata_path.value())};
    assert(mcp_metadata_option.has_value());
    this->mcp_metadata_ = std::move(mcp_metadata_option.value());
    this->allowed_origin_ = this->mcp_metadata_.at("origin").to_string();
  }

  auto rest(const std::span<std::string_view>, std::string_view,
            sourcemeta::one::HTTPRequest &request,
            sourcemeta::one::HTTPResponse &response) -> void override {
    if (request.method() == "options") {
      response.write_status(sourcemeta::core::HTTP_STATUS_NO_CONTENT);
      response.write_header("Access-Control-Allow-Origin",
                            this->allowed_origin_);
      response.write_header("Access-Control-Expose-Headers", "Link, ETag");
      response.write_header("Access-Control-Allow-Methods", "POST, OPTIONS");
      response.write_header("Access-Control-Allow-Headers",
                            "Content-Type, MCP-Protocol-Version");
      response.write_header("Access-Control-Max-Age", "3600");
      // Browser preflight cache is governed by `Access-Control-Max-Age`;
      // `no-store` keeps shared HTTP caches from storing this response.
      response.write_header("Cache-Control", "no-store");
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

    // MCP Streamable HTTP transport / Sending Messages, item 2: "The client
    // MUST include an Accept header, listing both application/json and
    // text/event-stream as supported content types." The MUST is on the
    // client. Defensively rejecting clients that omit either media type
    // catches integration bugs before SSE lands. Both types must be
    // individually acceptable per RFC 9110 §12.5.1 (q-aware, wildcard-aware).
    // https://modelcontextprotocol.io/specification/2025-06-18/basic/transports#sending-messages-to-the-server
    const auto accept{request.header("accept")};
    if (accept.empty() ||
        !sourcemeta::core::http_accept_includes_all(
            accept, {"application/json", "text/event-stream"})) {
      this->write_envelope(request, response,
                           sourcemeta::core::HTTP_STATUS_NOT_ACCEPTABLE,
                           sourcemeta::core::jsonrpc_make_error(
                               nullptr, -32006,
                               "Accept header must list application/json and "
                               "text/event-stream"));
      return;
    }

    // MCP requests carry a JSON-RPC body. Reject other media types up front
    // rather than letting the parser surface a confusing -32700 (parse
    // error). The MUST is implicit in MCP's wire spec. Parameters do not
    // change the media-type identity, so any params (including
    // `charset=utf-8`) are accepted.
    if (!sourcemeta::core::http_content_type_matches(
            request.header("content-type"), "application/json")) {
      this->write_envelope(
          request, response,
          sourcemeta::core::HTTP_STATUS_UNSUPPORTED_MEDIA_TYPE,
          sourcemeta::core::jsonrpc_make_error(
              nullptr, -32008, "Content-Type must be application/json"));
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

    // RFC 9110 §10.1.1: refuse unrecognised expectations with 417 before
    // touching the body. uWS already auto-acknowledged `100-continue`
    // upstream, so anything left here is a value we cannot honour.
    if (sourcemeta::one::expect_header_unrecognised(request)) {
      this->write_envelope(
          request, response, sourcemeta::core::HTTP_STATUS_EXPECTATION_FAILED,
          sourcemeta::core::jsonrpc_make_error(
              nullptr, -32009,
              "The Expect header carries an unsupported expectation"),
          negotiated_version.value());
      return;
    }

    // RFC 9110 §15.5.14: when the client declares a `Content-Length`
    // beyond the cap, fast-fail with 413 before scheduling the read.
    if (sourcemeta::one::request_body_too_large(request)) {
      this->write_envelope(request, response,
                           sourcemeta::core::HTTP_STATUS_CONTENT_TOO_LARGE,
                           sourcemeta::core::jsonrpc_make_error(
                               nullptr, -32005, "Request body is too large"),
                           negotiated_version.value());
      return;
    }

    request.body(
        [this, version = negotiated_version.value()](
            sourcemeta::one::HTTPRequest &callback_request,
            sourcemeta::one::HTTPResponse &callback_response,
            std::string &&body, bool too_big) -> void {
          if (too_big) {
            this->write_envelope(
                callback_request, callback_response,
                sourcemeta::core::HTTP_STATUS_CONTENT_TOO_LARGE,
                sourcemeta::core::jsonrpc_make_error(
                    nullptr, -32005, "Request body is too large"),
                version);
            return;
          }
          sourcemeta::core::JSON request_json{nullptr};
          try {
            request_json = sourcemeta::core::parse_json(body);
          } catch (const std::exception &) {
            this->write_envelope(callback_request, callback_response,
                                 sourcemeta::core::HTTP_STATUS_BAD_REQUEST,
                                 sourcemeta::core::jsonrpc_make_error_parse(),
                                 version);
            return;
          }
          if (sourcemeta::core::jsonrpc_is_batch(request_json)) {
            if (!sourcemeta::core::mcp_supports_jsonrpc_batching(version)) {
              // MCP 2025-06-18 removed JSON-RPC batching. Any array body on
              // a protocol version that doesn't support batching is treated
              // as an unrecognized batch shape: per JSON-RPC 2.0 §6, a
              // batch that fails to be recognized as a valid Array yields a
              // single Invalid Request response.
              // https://www.jsonrpc.org/specification#batch
              this->write_envelope(
                  callback_request, callback_response,
                  sourcemeta::core::HTTP_STATUS_OK,
                  sourcemeta::core::jsonrpc_make_error_invalid_request(nullptr),
                  version);
              return;
            }
            if (!sourcemeta::core::jsonrpc_is_valid_batch(request_json)) {
              // JSON-RPC 2.0 §6: "If the batch rpc call itself fails to be
              // recognized as a valid JSON or as an Array with at least one
              // value, the response from the Server MUST be a single
              // Response object." Empty array body falls here.
              // https://www.jsonrpc.org/specification#batch
              this->write_envelope(
                  callback_request, callback_response,
                  sourcemeta::core::HTTP_STATUS_OK,
                  sourcemeta::core::jsonrpc_make_error_invalid_request(nullptr),
                  version);
              return;
            }
            auto responses{sourcemeta::core::JSON::make_array()};
            for (const auto &sub : request_json.as_array()) {
              sourcemeta::core::JSON sub_id{nullptr};
              if (const auto *parsed_id{
                      sourcemeta::core::jsonrpc_request_id(sub)};
                  parsed_id != nullptr) {
                sub_id = *parsed_id;
              }
              try {
                auto envelope{this->process_one(sub)};
                if (envelope.has_value()) {
                  responses.push_back(std::move(envelope).value());
                }
              } catch (const std::exception &) {
                responses.push_back(
                    sourcemeta::core::jsonrpc_make_error_internal(&sub_id));
              }
            }
            if (responses.empty()) {
              // JSON-RPC 2.0 §6: "If there are no Response objects contained
              // within the Response array as it is to be sent to the client,
              // the server MUST NOT return an empty Array and should return
              // nothing at all." A batch of pure notifications falls here.
              // https://www.jsonrpc.org/specification#batch
              callback_response.write_status(
                  sourcemeta::core::HTTP_STATUS_ACCEPTED);
              callback_response.write_header("Access-Control-Allow-Origin",
                                             this->allowed_origin_);
              callback_response.write_header("Access-Control-Expose-Headers",
                                             "Link, ETag");
              callback_response.write_header("Cache-Control", "no-store");
              callback_response.write_header(
                  "MCP-Protocol-Version",
                  sourcemeta::core::mcp_protocol_version_string(version));
              sourcemeta::one::send_response(
                  sourcemeta::core::HTTP_STATUS_ACCEPTED, callback_request,
                  callback_response);
              return;
            }
            this->write_envelope(callback_request, callback_response,
                                 sourcemeta::core::HTTP_STATUS_OK, responses,
                                 version);
            return;
          }
          auto envelope{this->process_one(request_json)};
          if (envelope.has_value()) {
            this->write_envelope(callback_request, callback_response,
                                 sourcemeta::core::HTTP_STATUS_OK,
                                 envelope.value(), version);
            return;
          }
          callback_response.write_status(
              sourcemeta::core::HTTP_STATUS_ACCEPTED);
          callback_response.write_header("Access-Control-Allow-Origin",
                                         this->allowed_origin_);
          callback_response.write_header("Access-Control-Expose-Headers",
                                         "Link, ETag");
          callback_response.write_header("Cache-Control", "no-store");
          callback_response.write_header(
              "MCP-Protocol-Version",
              sourcemeta::core::mcp_protocol_version_string(version));
          sourcemeta::one::send_response(sourcemeta::core::HTTP_STATUS_ACCEPTED,
                                         callback_request, callback_response);
        },
        [this, version = negotiated_version.value()](
            sourcemeta::one::HTTPRequest &callback_request,
            sourcemeta::one::HTTPResponse &callback_response,
            const std::exception_ptr &error) -> void {
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
           const sourcemeta::core::JSON &id, const sourcemeta::core::JSON &,
           std::string_view) -> sourcemeta::core::JSON override {
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
    response.write_header("Access-Control-Expose-Headers", "Link, ETag");
    // The envelope embeds the echoed JSON-RPC id from the request, so
    // the response is request-specific and never a sound cache hit
    // for any other request.
    response.write_header("Cache-Control", "no-store");
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

  auto enterprise_required(const sourcemeta::core::JSON *id) const
      -> sourcemeta::core::JSON {
    static constexpr std::string_view MCP_ENTERPRISE_REQUIRED_DATA{
        "MCP support is only available in the Enterprise edition of "
        "Sourcemeta One"};
    return sourcemeta::core::jsonrpc_make_error(
        id, -32000, "Enterprise edition required",
        sourcemeta::core::JSON{MCP_ENTERPRISE_REQUIRED_DATA});
  }

  [[nodiscard]] auto
  process_one(const sourcemeta::core::JSON &request_json) const
      -> std::optional<sourcemeta::core::JSON> {
    if (sourcemeta::core::jsonrpc_is_notification(request_json)) {
      return std::nullopt;
    }

    const auto method{sourcemeta::core::jsonrpc_method(request_json)};
    if (method.empty()) {
      return sourcemeta::core::jsonrpc_make_error_invalid_request(
          sourcemeta::core::jsonrpc_request_id(request_json));
    }

    const auto *id{sourcemeta::core::jsonrpc_request_id(request_json)};
    if (id == nullptr) {
      return this->enterprise_required(nullptr);
    }

    if (method == sourcemeta::core::MCP_METHOD_INITIALIZE) {
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

    if (method == sourcemeta::core::MCP_METHOD_RESOURCES_TEMPLATES_LIST) {
      return sourcemeta::core::jsonrpc_make_success(
          *id, this->mcp_metadata_.at(
                   sourcemeta::core::MCP_METHOD_RESOURCES_TEMPLATES_LIST));
    }

    return this->enterprise_required(id);
  }

  std::string_view response_schema_;
  std::string_view allowed_origin_;
  sourcemeta::core::JSON mcp_metadata_{nullptr};
};

#endif

#endif
