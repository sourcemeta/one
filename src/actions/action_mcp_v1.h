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
    router.arguments(identifier, [this](const auto &key, const auto &value) {
      if (key == "responseSchema") {
        this->response_schema_ = std::get<std::string_view>(value);
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
    if (request.method() == "options") {
      response.write_status(sourcemeta::one::STATUS_NO_CONTENT);
      response.write_header("Access-Control-Allow-Origin",
                            this->allowed_origin_);
      response.write_header("Access-Control-Allow-Methods", "POST, OPTIONS");
      response.write_header("Access-Control-Allow-Headers",
                            "Content-Type, MCP-Protocol-Version");
      response.write_header("Access-Control-Max-Age", "3600");
      sourcemeta::one::send_response(sourcemeta::one::STATUS_NO_CONTENT,
                                     request, response);
      return;
    }

    if (request.method() != "post") {
      this->write_envelope(request, response,
                           sourcemeta::one::STATUS_METHOD_NOT_ALLOWED,
                           sourcemeta::core::jsonrpc_make_error(
                               nullptr, -32004, "Method not allowed"));
      return;
    }

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
                           sourcemeta::one::STATUS_BAD_REQUEST,
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
            this->write_envelope(callback_request, callback_response,
                                 sourcemeta::one::STATUS_PAYLOAD_TOO_LARGE,
                                 sourcemeta::core::jsonrpc_make_error(
                                     nullptr, -32005, "Request too large"));
            return;
          }
          sourcemeta::core::JSON request_json{nullptr};
          try {
            request_json = sourcemeta::core::parse_json(body);
          } catch (const std::exception &) {
            this->write_envelope(callback_request, callback_response,
                                 sourcemeta::one::STATUS_BAD_REQUEST,
                                 sourcemeta::core::jsonrpc_make_error_parse());
            return;
          }
          if (sourcemeta::core::jsonrpc_is_batch(request_json)) {
            if (!sourcemeta::core::mcp_supports_jsonrpc_batching(version)) {
              this->write_envelope(
                  callback_request, callback_response,
                  sourcemeta::one::STATUS_OK,
                  sourcemeta::core::jsonrpc_make_error_invalid_request(
                      nullptr));
              return;
            }
            if (!sourcemeta::core::jsonrpc_is_valid_batch(request_json)) {
              // JSON-RPC §6: an empty array body must produce a single Invalid
              // Request envelope, not an empty array response
              this->write_envelope(
                  callback_request, callback_response,
                  sourcemeta::one::STATUS_OK,
                  sourcemeta::core::jsonrpc_make_error_invalid_request(
                      nullptr));
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
              // JSON-RPC §6: a batch containing nothing but notifications must
              // produce no response body
              callback_response.write_status(sourcemeta::one::STATUS_ACCEPTED);
              callback_response.write_header("Access-Control-Allow-Origin",
                                             this->allowed_origin_);
              sourcemeta::one::send_response(sourcemeta::one::STATUS_ACCEPTED,
                                             callback_request,
                                             callback_response);
              return;
            }
            this->write_envelope(callback_request, callback_response,
                                 sourcemeta::one::STATUS_OK, responses);
            return;
          }
          auto envelope{this->process_one(request_json)};
          if (envelope.has_value()) {
            this->write_envelope(callback_request, callback_response,
                                 sourcemeta::one::STATUS_OK, envelope.value());
            return;
          }
          callback_response.write_status(sourcemeta::one::STATUS_ACCEPTED);
          callback_response.write_header("Access-Control-Allow-Origin",
                                         this->allowed_origin_);
          sourcemeta::one::send_response(sourcemeta::one::STATUS_ACCEPTED,
                                         callback_request, callback_response);
        },
        [this](sourcemeta::one::HTTPRequest &callback_request,
               sourcemeta::one::HTTPResponse &callback_response,
               const std::exception_ptr &error) {
          try {
            std::rethrow_exception(error);
          } catch (const std::exception &) {
            this->write_envelope(
                callback_request, callback_response,
                sourcemeta::one::STATUS_INTERNAL_SERVER_ERROR,
                sourcemeta::core::jsonrpc_make_error_internal());
          }
        });
  }

  auto mcp(const sourcemeta::core::MCPProtocolVersion,
           const sourcemeta::core::JSON &id, const sourcemeta::core::JSON &)
      -> sourcemeta::core::JSON override {
    return sourcemeta::core::jsonrpc_make_error_method_not_found(id);
  }

private:
  auto write_envelope(sourcemeta::one::HTTPRequest &request,
                      sourcemeta::one::HTTPResponse &response,
                      const char *status,
                      const sourcemeta::core::JSON &envelope) const -> void {
    response.write_status(status);
    response.write_header("Content-Type", "application/json");
    response.write_header("Access-Control-Allow-Origin", this->allowed_origin_);
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
