#ifndef SOURCEMETA_ONE_ACTIONS_DEPENDENCY_TREE_V1_H
#define SOURCEMETA_ONE_ACTIONS_DEPENDENCY_TREE_V1_H

#include <sourcemeta/core/io.h>
#include <sourcemeta/core/json.h>
#include <sourcemeta/core/jsonrpc.h>
#include <sourcemeta/core/uri.h>
#include <sourcemeta/core/uritemplate.h>

#include <sourcemeta/one/http.h>
#include <sourcemeta/one/mcp.h>
#include <sourcemeta/one/metapack.h>
#include <sourcemeta/one/router.h>

#include "action_serve_metapack_file_v1.h"

#include <filesystem>  // std::filesystem
#include <set>         // std::set
#include <span>        // std::span
#include <sstream>     // std::ostringstream
#include <string_view> // std::string_view
#include <utility>     // std::move

class ActionDependencyTree_v1 : public sourcemeta::one::RouterAction {
public:
  static constexpr std::string_view DESCRIPTION{
      "Look up the dependency graph of a specific schema (incoming or "
      "outgoing)"};

  ActionDependencyTree_v1(
      const std::filesystem::path &base,
      const sourcemeta::core::URITemplateRouterView &router,
      const sourcemeta::core::URITemplateRouter::Identifier identifier,
      sourcemeta::one::Router &)
      : sourcemeta::one::RouterAction{base, router.base_path(),
                                      router.base_url()} {
    router.arguments(identifier, [this](const auto &key, const auto &value) {
      if (key == "direction") {
        this->metapack_ = std::get<std::string_view>(value) == "in"
                              ? "dependents.metapack"
                              : "dependencies.metapack";
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

    const auto schemas_root{this->base() / "schemas"};
    auto absolute_path{schemas_root / matches.front() / "%"};
    absolute_path /= this->metapack_;

    const auto safe_path{sourcemeta::core::weakly_canonical(absolute_path)};
    if (!sourcemeta::core::is_under_path(safe_path, schemas_root)) {
      sourcemeta::one::json_error(
          request, response, sourcemeta::one::STATUS_NOT_FOUND, "not-found",
          "There is nothing at this URL", this->error_schema_);
      return;
    }

    ActionServeMetapackFile_v1::serve(safe_path, sourcemeta::one::STATUS_OK,
                                      true, {}, this->response_schema_, request,
                                      response, this->error_schema_);
  }

  auto mcp(const sourcemeta::core::JSON &request_id,
           const sourcemeta::core::JSON &arguments, const std::string_view)
      -> sourcemeta::core::JSON override {
    if (!this->validate(this->rpc_schema_, arguments)) {
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

    auto contents{sourcemeta::one::metapack_read_json(directory.value() /
                                                      this->metapack_)};
    if (!contents.has_value()) {
      return sourcemeta::one::mcp_make_tool_error(request_id,
                                                  "Schema not found");
    }

    auto &result{contents.value()};
    std::set<std::string_view> unique_uris;
    for (const auto &entry : result.as_array()) {
      for (const auto *const field : {"from", "to"}) {
        if (!entry.defines(field)) {
          continue;
        }
        const std::string_view uri{entry.at(field).to_string()};
        if (uri.starts_with(this->server_uri())) {
          unique_uris.emplace(uri);
        }
      }
    }

    auto content{sourcemeta::core::JSON::make_array()};
    std::ostringstream payload;
    sourcemeta::core::prettify(result, payload);
    content.push_back(sourcemeta::one::mcp_make_text_block(payload.str()));
    for (const auto uri : unique_uris) {
      content.push_back(sourcemeta::one::mcp_make_resource_link(
          uri, "application/schema+json"));
    }

    return sourcemeta::one::mcp_make_tool_success(request_id, std::move(result),
                                                  std::move(content));
  }

private:
  std::string_view metapack_;
  std::string_view response_schema_;
  std::string_view rpc_schema_;
  std::string_view error_schema_;
};

#endif
