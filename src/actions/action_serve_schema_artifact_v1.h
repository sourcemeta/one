#ifndef SOURCEMETA_ONE_ACTIONS_SERVE_SCHEMA_ARTIFACT_V1_H
#define SOURCEMETA_ONE_ACTIONS_SERVE_SCHEMA_ARTIFACT_V1_H

#include <sourcemeta/blaze/evaluator.h>
#include <sourcemeta/core/json.h>
#include <sourcemeta/core/uritemplate.h>

#include <sourcemeta/one/actions.h>
#include <sourcemeta/one/http.h>
#include <sourcemeta/one/jsonrpc.h>
#include <sourcemeta/one/mcp.h>
#include <sourcemeta/one/metapack.h>

#include "action_serve_metapack_file_v1.h"

#include <filesystem>  // std::filesystem
#include <span>        // std::span
#include <string>      // std::string
#include <string_view> // std::string_view
#include <utility>     // std::move

class ActionServeSchemaArtifact_v1 : public sourcemeta::one::Action {
public:
  static constexpr std::string_view DESCRIPTION{
      "Look up a precomputed artifact about a specific schema by its "
      "absolute URI"};

  ActionServeSchemaArtifact_v1(
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
    if (matches.empty()) {
      sourcemeta::one::json_error(
          request, response, sourcemeta::one::STATUS_INTERNAL_SERVER_ERROR,
          "missing-schema-match", "This action requires a schema path match",
          this->error_schema_);
      return;
    }

    auto absolute_path{this->base() / "schemas" / matches.front() / "%"};
    absolute_path /= std::string{this->artifact_} + ".metapack";
    ActionServeMetapackFile_v1::serve(absolute_path, sourcemeta::one::STATUS_OK,
                                      true, {}, this->response_schema_, request,
                                      response, this->error_schema_);
  }

  auto mcp(const sourcemeta::core::JSON &envelope)
      -> sourcemeta::core::JSON override {
    const auto *id{sourcemeta::one::jsonrpc_request_id(envelope)};
    const sourcemeta::core::JSON request_id{
        id ? *id : sourcemeta::core::JSON{nullptr}};

    const auto *params{sourcemeta::one::jsonrpc_params(envelope)};
    if (params == nullptr || !params->is_object() ||
        !params->defines("arguments")) {
      return sourcemeta::one::jsonrpc_make_error_invalid_params(request_id);
    }

    const auto &arguments{params->at("arguments")};
    // TODO: Cache the compiled template across invocations
    const auto rpc_schema_template{this->blaze_template(
        this->rpc_schema_, sourcemeta::blaze::Mode::FastValidation)};
    sourcemeta::blaze::Evaluator evaluator;
    if (!evaluator.validate(rpc_schema_template, arguments)) {
      return sourcemeta::one::jsonrpc_make_error_invalid_params(request_id);
    }

    const auto directory{
        this->schema_directory(arguments.at("schema").to_string())};
    if (!directory.has_value()) {
      return sourcemeta::one::mcp_make_tool_error(request_id,
                                                  "Schema not found");
    }

    auto contents{sourcemeta::one::metapack_read_json(
        directory.value() / (std::string{this->artifact_} + ".metapack"))};
    if (!contents.has_value()) {
      return sourcemeta::one::mcp_make_tool_error(request_id,
                                                  "Schema not found");
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
