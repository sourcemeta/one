#ifndef SOURCEMETA_ONE_ACTIONS_SERVE_SCHEMA_ARTIFACT_V1_H
#define SOURCEMETA_ONE_ACTIONS_SERVE_SCHEMA_ARTIFACT_V1_H

#include <sourcemeta/blaze/evaluator.h>
#include <sourcemeta/core/json.h>
#include <sourcemeta/core/jsonrpc.h>
#include <sourcemeta/core/uri.h>
#include <sourcemeta/core/uritemplate.h>

#include <sourcemeta/one/http.h>
#include <sourcemeta/one/mcp.h>
#include <sourcemeta/one/metapack.h>
#include <sourcemeta/one/router.h>

#include "action_serve_metapack_file_v1.h"

#include <cstddef>     // std::size_t
#include <filesystem>  // std::filesystem
#include <set>         // std::set
#include <span>        // std::span
#include <sstream>     // std::ostringstream
#include <string>      // std::string
#include <string_view> // std::string_view
#include <utility>     // std::move

class ActionServeSchemaArtifact_v1 : public sourcemeta::one::RouterAction {
public:
  static constexpr std::string_view DESCRIPTION{
      "Look up a precomputed artifact about a specific schema by its "
      "absolute URI"};

  ActionServeSchemaArtifact_v1(
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

    if (!sourcemeta::core::URI::is_uri(arguments.at("schema").to_string())) {
      return sourcemeta::core::jsonrpc_make_error_invalid_params(request_id);
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

    if (this->artifact_ == "dependencies" || this->artifact_ == "dependents") {
      return this->make_relation_envelope(request_id,
                                          std::move(contents).value());
    }

    return sourcemeta::one::mcp_make_tool_success(request_id,
                                                  std::move(contents).value());
  }

private:
  auto make_relation_envelope(const sourcemeta::core::JSON &request_id,
                              sourcemeta::core::JSON &&result)
      -> sourcemeta::core::JSON {
    std::set<std::string> unique_uris;
    for (std::size_t index{0}; index < result.array_size(); ++index) {
      const auto &entry{result.at(index)};
      for (const auto *const field : {"from", "to"}) {
        if (!entry.defines(field)) {
          continue;
        }
        const auto &uri{entry.at(field).to_string()};
        if (uri.starts_with(this->server_uri())) {
          unique_uris.emplace(uri);
        }
      }
    }

    auto content{sourcemeta::core::JSON::make_array()};

    std::ostringstream payload;
    sourcemeta::core::prettify(result, payload);
    auto text_block{sourcemeta::core::JSON::make_object()};
    text_block.assign_assume_new(std::string{"type"},
                                 sourcemeta::core::JSON{"text"});
    text_block.assign_assume_new(std::string{"text"},
                                 sourcemeta::core::JSON{payload.str()});
    content.push_back(std::move(text_block));

    for (const auto &uri : unique_uris) {
      auto resource_link{sourcemeta::core::JSON::make_object()};
      resource_link.assign_assume_new(std::string{"type"},
                                      sourcemeta::core::JSON{"resource_link"});
      resource_link.assign_assume_new(std::string{"uri"},
                                      sourcemeta::core::JSON{uri});
      resource_link.assign_assume_new(
          std::string{"mimeType"},
          sourcemeta::core::JSON{"application/schema+json"});
      content.push_back(std::move(resource_link));
    }

    auto envelope_result{sourcemeta::core::JSON::make_object()};
    envelope_result.assign_assume_new(std::string{"content"},
                                      std::move(content));
    envelope_result.assign_assume_new(std::string{"structuredContent"},
                                      std::move(result));
    envelope_result.assign_assume_new(std::string{"isError"},
                                      sourcemeta::core::JSON{false});
    return sourcemeta::core::jsonrpc_make_success(request_id,
                                                  std::move(envelope_result));
  }

  std::string_view artifact_;
  std::string_view response_schema_;
  std::string_view rpc_schema_;
  std::string_view error_schema_;
};

#endif
