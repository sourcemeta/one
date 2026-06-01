#ifndef SOURCEMETA_ONE_ACTIONS_DEPENDENCY_TREE_V1_H
#define SOURCEMETA_ONE_ACTIONS_DEPENDENCY_TREE_V1_H

#include <sourcemeta/core/io.h>
#include <sourcemeta/core/json.h>
#include <sourcemeta/core/jsonrpc.h>
#include <sourcemeta/core/mcp.h>
#include <sourcemeta/core/uri.h>
#include <sourcemeta/core/uritemplate.h>

#include <sourcemeta/one/http.h>
#include <sourcemeta/one/metapack.h>
#include <sourcemeta/one/router.h>

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
  static constexpr bool READ_ONLY{true};
  static constexpr bool DESTRUCTIVE{false};
  static constexpr bool IDEMPOTENT{true};
  static constexpr bool OPEN_WORLD{false};

  ActionDependencyTree_v1(
      const std::filesystem::path &base,
      const sourcemeta::core::URITemplateRouterView &router,
      const sourcemeta::core::URITemplateRouter::Identifier identifier,
      sourcemeta::one::Router &dispatcher)
      : sourcemeta::one::RouterAction{base, router.base_path(),
                                      router.base_url(), dispatcher} {
    router.arguments(identifier, [this](const auto &key, const auto &value) {
      if (key == "direction") {
        this->metapack_ = std::get<std::string_view>(value) == "in"
                              ? "dependents"
                              : "dependencies";
      } else if (key == "responseSchema") {
        this->response_schema_ = std::get<std::string_view>(value);
      } else if (key == "mcpRequestSchema") {
        this->rpc_request_schema_ = std::get<std::string_view>(value);
      } else if (key == "mcpResponseSchema") {
        this->rpc_response_schema_ = std::get<std::string_view>(value);
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

    if (matches.front().find('#') != std::string_view::npos ||
        matches.front().find("%23") != std::string_view::npos) {
      sourcemeta::one::json_error(
          request, response, sourcemeta::one::STATUS_BAD_REQUEST,
          "invalid-schema-uri", "The schema URI must not contain a fragment",
          this->error_schema_);
      return;
    }

    const auto path{this->artifact_resolve_path(matches.front(), Tree::Schemas,
                                                this->metapack_)};
    if (!path.has_value()) {
      sourcemeta::one::json_error(
          request, response, sourcemeta::one::STATUS_NOT_FOUND, "not-found",
          "There is nothing at this URL", this->error_schema_);
      return;
    }
    this->artifact_serve(path.value(), sourcemeta::one::STATUS_OK, true, {},
                         this->response_schema_, request, response,
                         this->error_schema_);
  }

  auto mcp(const sourcemeta::core::MCPProtocolVersion version,
           const sourcemeta::core::JSON &request_id,
           const sourcemeta::core::JSON &arguments)
      -> sourcemeta::core::JSON override {
    auto [request_valid, request_output]{
        this->schema_evaluate(this->rpc_request_schema_, arguments,
                              sourcemeta::blaze::Mode::Exhaustive)};
    if (!request_valid) {
      return sourcemeta::core::jsonrpc_make_error(
          &request_id, -32602, "Params fail against the tool request schema",
          std::move(request_output));
    }

    if (!sourcemeta::core::URI::is_uri(arguments.at("schema").to_string()) ||
        arguments.at("schema").to_string().find('#') != std::string::npos ||
        arguments.at("schema").to_string().find('?') != std::string::npos) {
      return sourcemeta::core::jsonrpc_make_error(
          &request_id, -32602, "Invalid tool input schema URI",
          sourcemeta::core::JSON{"The schema must be an absolute URI without "
                                 "a fragment or query parameters"});
    }

    const auto path{this->artifact_resolve_path(
        arguments.at("schema").to_string(), Tree::Schemas, this->metapack_)};
    if (!path.has_value()) {
      return sourcemeta::core::mcp_make_tool_error(request_id,
                                                   "Schema not found");
    }
    auto contents{this->artifact_read_json(path.value())};
    if (!contents.has_value()) {
      return sourcemeta::core::mcp_make_tool_error(request_id,
                                                   "Schema not found");
    }

    auto envelope{sourcemeta::core::JSON::make_object()};
    envelope.assign_assume_new("results", std::move(contents.value()));

    std::set<std::string_view> unique_uris;
    for (const auto &entry : envelope.at("results").as_array()) {
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
    sourcemeta::core::prettify(envelope, payload);
    content.push_back(sourcemeta::core::mcp_make_text_block(payload.str()));
    for (const auto uri : unique_uris) {
      content.push_back(sourcemeta::core::mcp_make_resource_link(
          version, uri, "application/schema+json",
          uri.substr(this->server_uri().size())));
    }

    return sourcemeta::core::mcp_make_tool_success(
        version, request_id, std::move(envelope), std::move(content));
  }

protected:
  ActionDependencyTree_v1(
      const std::filesystem::path &base,
      const sourcemeta::core::URITemplateRouterView &router,
      const sourcemeta::core::URITemplateRouter::Identifier identifier,
      const std::string_view metapack, sourcemeta::one::Router &dispatcher)
      : sourcemeta::one::RouterAction{base, router.base_path(),
                                      router.base_url(), dispatcher} {
    this->metapack_ = metapack;
    router.arguments(identifier, [this](const auto &key, const auto &value) {
      if (key == "responseSchema") {
        this->response_schema_ = std::get<std::string_view>(value);
      } else if (key == "mcpRequestSchema") {
        this->rpc_request_schema_ = std::get<std::string_view>(value);
      } else if (key == "mcpResponseSchema") {
        this->rpc_response_schema_ = std::get<std::string_view>(value);
      } else if (key == "errorSchema") {
        this->error_schema_ = std::get<std::string_view>(value);
      }
    });
  }

private:
  std::string_view metapack_;
  std::string_view response_schema_;
  std::string_view rpc_request_schema_;
  std::string_view rpc_response_schema_;
  std::string_view error_schema_;
};

#endif
