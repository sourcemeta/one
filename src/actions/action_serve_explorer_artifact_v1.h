#ifndef SOURCEMETA_ONE_ACTIONS_SERVE_EXPLORER_ARTIFACT_V1_H
#define SOURCEMETA_ONE_ACTIONS_SERVE_EXPLORER_ARTIFACT_V1_H

#include <sourcemeta/core/json.h>
#include <sourcemeta/core/jsonrpc.h>
#include <sourcemeta/core/mcp.h>
#include <sourcemeta/core/uri.h>
#include <sourcemeta/core/uritemplate.h>

#include <sourcemeta/one/http.h>
#include <sourcemeta/one/metapack.h>
#include <sourcemeta/one/router.h>

#include "action_serve_metapack_file_v1.h"

#include <filesystem>  // std::filesystem
#include <span>        // std::span
#include <string>      // std::string
#include <string_view> // std::string_view
#include <utility>     // std::move

class ActionServeExplorerArtifact_v1 : public sourcemeta::one::RouterAction {
public:
  static constexpr std::string_view DESCRIPTION{
      "Read a navigation artifact for browsing schemas"};
  static constexpr bool READ_ONLY{true};
  static constexpr bool DESTRUCTIVE{false};
  static constexpr bool IDEMPOTENT{true};
  static constexpr bool OPEN_WORLD{false};

  ActionServeExplorerArtifact_v1(
      const std::filesystem::path &base,
      const sourcemeta::core::URITemplateRouterView &router,
      const sourcemeta::core::URITemplateRouter::Identifier identifier,
      sourcemeta::one::Router &)
      : sourcemeta::one::RouterAction{base, router.base_path(),
                                      router.base_url()} {
    router.arguments(identifier, [this](const auto &key, const auto &value) {
      if (key == "artifact") {
        this->artifact_ = std::get<std::string_view>(value);
      } else if (key == "responseSchema") {
        this->response_schema_ = std::get<std::string_view>(value);
      } else if (key == "rpcSchema") {
        this->rpc_schema_ = std::get<std::string_view>(value);
      } else if (key == "errorSchema") {
        this->error_schema_ = std::get<std::string_view>(value);
      }
    });
  }

  auto rest(const std::span<std::string_view> matches,
            sourcemeta::one::HTTPRequest &request,
            sourcemeta::one::HTTPResponse &response) -> void override {
    if (!matches.empty() &&
        (matches.front().find('#') != std::string_view::npos ||
         matches.front().find("%23") != std::string_view::npos)) {
      sourcemeta::one::json_error(
          request, response, sourcemeta::one::STATUS_BAD_REQUEST,
          "invalid-schema-uri", "The schema URI must not contain a fragment",
          this->error_schema_);
      return;
    }

    auto absolute_path{this->base() / "explorer"};
    if (!matches.empty() && !matches.front().empty()) {
      absolute_path /= matches.front();
    }
    absolute_path /= "%";
    absolute_path /= std::string{this->artifact_} + ".metapack";
    ActionServeMetapackFile_v1::serve(absolute_path, sourcemeta::one::STATUS_OK,
                                      true, {}, this->response_schema_, request,
                                      response, this->error_schema_);
  }

  auto mcp(const sourcemeta::core::MCPProtocolVersion version,
           const sourcemeta::core::JSON &request_id,
           const sourcemeta::core::JSON &arguments, const std::string_view)
      -> sourcemeta::core::JSON override {
    if (auto output{this->validate_standard(this->rpc_schema_, arguments)};
        output.has_value()) {
      return sourcemeta::core::jsonrpc_make_error_invalid_params(
          request_id, std::move(output));
    }

    if (!sourcemeta::core::URI::is_uri(arguments.at("schema").to_string()) ||
        arguments.at("schema").to_string().find('#') != std::string::npos ||
        arguments.at("schema").to_string().find('?') != std::string::npos) {
      return sourcemeta::core::jsonrpc_make_error_invalid_params(
          request_id,
          sourcemeta::core::JSON{"The schema must be an absolute URI without "
                                 "a fragment or query parameters"});
    }

    const auto schema_path{
        this->uri_to_relative_path(arguments.at("schema").to_string())};
    if (!schema_path.has_value()) {
      return sourcemeta::core::mcp_make_tool_error(request_id,
                                                   "Schema not found");
    }

    auto absolute_path{this->base() / "explorer" / schema_path.value() / "%"};
    absolute_path /= std::string{this->artifact_} + ".metapack";

    auto contents{sourcemeta::one::metapack_read_json(absolute_path)};
    if (!contents.has_value()) {
      return sourcemeta::core::mcp_make_tool_error(request_id,
                                                   "Schema not found");
    }

    return sourcemeta::core::mcp_make_tool_success(version, request_id,
                                                   std::move(contents).value());
  }

protected:
  ActionServeExplorerArtifact_v1(
      const std::filesystem::path &base,
      const sourcemeta::core::URITemplateRouterView &router,
      const sourcemeta::core::URITemplateRouter::Identifier identifier,
      const std::string_view artifact)
      : sourcemeta::one::RouterAction{base, router.base_path(),
                                      router.base_url()} {
    this->artifact_ = artifact;
    router.arguments(identifier, [this](const auto &key, const auto &value) {
      if (key == "responseSchema") {
        this->response_schema_ = std::get<std::string_view>(value);
      } else if (key == "rpcSchema") {
        this->rpc_schema_ = std::get<std::string_view>(value);
      } else if (key == "errorSchema") {
        this->error_schema_ = std::get<std::string_view>(value);
      }
    });
  }

private:
  std::string_view artifact_;
  std::string_view response_schema_;
  std::string_view rpc_schema_;
  std::string_view error_schema_;
};

#endif
