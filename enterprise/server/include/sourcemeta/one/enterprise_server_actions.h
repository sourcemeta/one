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
                                      router.base_url()},
        dispatcher_{dispatcher} {
    router.arguments(identifier, [this](const auto &key, const auto &value) {
      if (key == "responseSchema") {
        this->response_schema_ = std::get<std::string_view>(value);
      } else if (key == "requestSchema") {
        this->request_schema_ = std::get<std::string_view>(value);
      }
    });

    const auto mcp_metadata_path{this->base() / "explorer" / "%" /
                                 "mcp.metapack"};
    auto mcp_metadata_option{
        sourcemeta::one::metapack_read_json(mcp_metadata_path)};
    assert(mcp_metadata_option.has_value());
    this->mcp_metadata_ = std::move(mcp_metadata_option.value());
    this->allowed_origin_ = this->mcp_metadata_.at("origin").to_string();
  }

  auto rest(const std::span<std::string_view>,
            sourcemeta::one::HTTPRequest &request,
            sourcemeta::one::HTTPResponse &response) -> void override {
    const auto origin_header{request.header("origin")};
    if (!origin_header.empty() && origin_header != this->allowed_origin_) {
      this->write_envelope(
          request, response, sourcemeta::one::STATUS_FORBIDDEN,
          sourcemeta::core::jsonrpc_make_error(
              nullptr, -32001, "Forbidden origin",
              sourcemeta::core::JSON{std::string{origin_header}}));
      return;
    }

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
          this->on_message(version, callback_request, callback_response,
                           std::move(body));
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
                      const char *const status,
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

  // NOLINTNEXTLINE(cppcoreguidelines-avoid-const-or-ref-data-members)
  sourcemeta::one::Router &dispatcher_;
  std::string_view allowed_origin_;
  std::string_view response_schema_;
  std::string_view request_schema_;
  sourcemeta::core::JSON mcp_metadata_{nullptr};
};

} // namespace sourcemeta::one::enterprise

#endif
