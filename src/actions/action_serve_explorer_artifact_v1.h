#ifndef SOURCEMETA_ONE_ACTIONS_SERVE_EXPLORER_ARTIFACT_V1_H
#define SOURCEMETA_ONE_ACTIONS_SERVE_EXPLORER_ARTIFACT_V1_H

#include <sourcemeta/blaze/evaluator.h>
#include <sourcemeta/core/json.h>
#include <sourcemeta/core/jsonrpc.h>
#include <sourcemeta/core/uritemplate.h>

#include <sourcemeta/one/actions.h>
#include <sourcemeta/one/http.h>
#include <sourcemeta/one/mcp.h>
#include <sourcemeta/one/metapack.h>

#include "action_serve_metapack_file_v1.h"

#include <filesystem>  // std::filesystem
#include <span>        // std::span
#include <string>      // std::string
#include <string_view> // std::string_view
#include <utility>     // std::move

class ActionServeExplorerArtifact_v1 : public sourcemeta::one::Action {
public:
  static constexpr std::string_view DESCRIPTION{
      "Read a navigation artifact for browsing schemas"};

  ActionServeExplorerArtifact_v1(
      const std::filesystem::path &base,
      const sourcemeta::core::URITemplateRouterView &router,
      const sourcemeta::core::URITemplateRouter::Identifier identifier)
      : sourcemeta::one::Action{base, router.base_path(), router.base_url()} {
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

  auto mcp(const sourcemeta::core::JSON &envelope)
      -> sourcemeta::core::JSON override {
    const auto *id{sourcemeta::core::jsonrpc_request_id(envelope)};
    const sourcemeta::core::JSON request_id{
        id ? *id : sourcemeta::core::JSON{nullptr}};

    const auto *params{sourcemeta::core::jsonrpc_params(envelope)};
    if (params == nullptr || !params->is_object() ||
        !params->defines("arguments")) {
      return sourcemeta::core::jsonrpc_make_error_invalid_params(request_id);
    }

    const auto &arguments{params->at("arguments")};
    // TODO: Cache the compiled template across invocations
    const auto rpc_schema_template{this->blaze_template(
        this->rpc_schema_, sourcemeta::blaze::Mode::FastValidation)};
    sourcemeta::blaze::Evaluator evaluator;
    if (!evaluator.validate(rpc_schema_template, arguments)) {
      return sourcemeta::core::jsonrpc_make_error_invalid_params(request_id);
    }

    auto absolute_path{this->base() / "explorer"};

    if (this->artifact_ == "list") {
      if (arguments.defines("path")) {
        const auto &path{arguments.at("path").to_string()};
        if (!path.empty()) {
          // Defense in depth: rpc.json's `path` is just `type: string`,
          // so still reject filesystem-traversal attempts before joining.
          const std::filesystem::path relative_path{path};
          if (relative_path.is_absolute()) {
            return sourcemeta::core::jsonrpc_make_error_invalid_params(
                request_id);
          }
          for (const auto &component : relative_path) {
            if (component == ".." || component == ".") {
              return sourcemeta::core::jsonrpc_make_error_invalid_params(
                  request_id);
            }
          }
          absolute_path /= relative_path;
        }
      }
    } else {
      const auto schema_path{
          this->uri_to_relative_path(arguments.at("schema").to_string())};
      if (!schema_path.has_value()) {
        return sourcemeta::one::mcp_make_tool_error(request_id,
                                                    "Schema not found");
      }
      absolute_path /= schema_path.value();
    }

    absolute_path /= "%";
    absolute_path /= std::string{this->artifact_} + ".metapack";

    auto contents{sourcemeta::one::metapack_read_json(absolute_path)};
    if (!contents.has_value()) {
      return sourcemeta::one::mcp_make_tool_error(
          request_id, this->artifact_ == "list" ? "Directory not found"
                                                : "Schema not found");
    }

    return sourcemeta::one::mcp_make_tool_success(request_id,
                                                  std::move(contents).value());
  }

private:
  std::string_view artifact_;
  std::string_view response_schema_;
  std::string_view rpc_schema_;
  std::string_view error_schema_;
};

#endif
