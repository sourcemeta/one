#ifndef SOURCEMETA_ONE_ACTIONS_DEPENDENCY_TREE_V1_H
#define SOURCEMETA_ONE_ACTIONS_DEPENDENCY_TREE_V1_H

#include <sourcemeta/core/io.h>
#include <sourcemeta/core/json.h>
#include <sourcemeta/core/jsonrpc.h>
#include <sourcemeta/core/mcp.h>
#include <sourcemeta/core/metapack.h>
#include <sourcemeta/core/uritemplate.h>

#include <sourcemeta/one/http.h>
#include <sourcemeta/one/router.h>

#include <filesystem>  // std::filesystem
#include <map>         // std::map
#include <set>         // std::set
#include <span>        // std::span
#include <sstream>     // std::ostringstream
#include <string_view> // std::string_view
#include <utility>     // std::move

class ActionDependencyTreeV1 : public sourcemeta::one::RouterAction {
public:
  static constexpr std::string_view TITLE{"Get Dependency Tree"};
  static constexpr std::string_view DESCRIPTION{
      "Look up the dependency graph of a specific schema (incoming or "
      "outgoing)"};
  static constexpr bool READ_ONLY{true};
  static constexpr bool DESTRUCTIVE{false};
  static constexpr bool IDEMPOTENT{true};
  static constexpr bool OPEN_WORLD{false};

  ActionDependencyTreeV1(
      const std::filesystem::path &base,
      const sourcemeta::core::URITemplateRouterView &router,
      const sourcemeta::core::URITemplateRouter::Identifier identifier,
      sourcemeta::one::Router &dispatcher)
      : sourcemeta::one::RouterAction{base, router.base_url(), dispatcher} {
    router.arguments(
        identifier, [this](const auto &key, const auto &value) -> void {
          if (key == "direction") {
            const auto incoming{std::get<std::string_view>(value) == "in"};
            this->metapack_ = incoming ? "dependents" : "dependencies";
            this->tree_ = incoming ? Tree::Explorer : Tree::Schemas;
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
            const sourcemeta::one::Authentication::Caller &caller,
            sourcemeta::one::HTTPRequest &request,
            sourcemeta::one::HTTPResponse &response) -> void override {
    if (request.method() == "options") {
      sourcemeta::one::cors_preflight(request, response, "GET, HEAD, OPTIONS",
                                      "Accept-Encoding, If-None-Match, "
                                      "If-Modified-Since");
      return;
    }

    if (sourcemeta::one::reject_unknown_query(request, response, {},
                                              this->error_schema_, "*")) {
      return;
    }
    if (matches.empty()) {
      sourcemeta::one::json_error(
          request, response,
          sourcemeta::core::HTTP_STATUS_INTERNAL_SERVER_ERROR,
          "urn:sourcemeta:one:missing-schema-match",
          "This action requires a schema path match", this->error_schema_, "*");
      return;
    }

    if (matches.front().find('#') != std::string_view::npos ||
        matches.front().find("%23") != std::string_view::npos) {
      sourcemeta::one::json_error(
          request, response, sourcemeta::core::HTTP_STATUS_BAD_REQUEST,
          "urn:sourcemeta:one:invalid-schema-uri",
          "The URI must not contain a fragment", this->error_schema_, "*");
      return;
    }

    const sourcemeta::one::RequestCookies cookies{request};
    const auto resolution{this->artifact_resolve_path(
        caller, matches.front(), this->tree_, this->metapack_)};
    if (!resolution.path.has_value()) {
      sourcemeta::one::json_error(
          request, response, sourcemeta::core::HTTP_STATUS_NOT_FOUND,
          "urn:sourcemeta:one:not-found", "There is nothing at this URL",
          this->error_schema_, "*");
      return;
    }
    this->artifact_serve(
        resolution.path.value(), sourcemeta::core::HTTP_STATUS_OK, true, {},
        this->response_schema_, {}, request, response, this->error_schema_,
        sourcemeta::one::cache_control_content(resolution.is_public),
        sourcemeta::one::vary_encoding());
  }

  auto mcp(const sourcemeta::core::MCPProtocolVersion version,
           const sourcemeta::core::JSON &request_id,
           const sourcemeta::core::JSON &arguments,
           const sourcemeta::one::Authentication::Caller &caller)
      -> sourcemeta::core::JSON override {
    auto [request_valid, request_output]{
        this->structural_evaluate(this->rpc_request_schema_, arguments,
                                  sourcemeta::blaze::Mode::Exhaustive)};
    if (!request_valid) {
      return sourcemeta::core::jsonrpc_make_error(
          &request_id, -32602, "Params fail against the tool request schema",
          std::move(request_output));
    }

    // A tool call reaches one artifact, so placing the caller here is still
    // once for the request that carried them
    const auto &leaf_uri{arguments.at(this->argument_).to_string()};
    const auto resolution{this->artifact_resolve_path(
        caller, leaf_uri, this->tree_, this->metapack_)};
    if (!resolution.path.has_value()) {
      return sourcemeta::core::mcp_make_tool_error(version, request_id,
                                                   this->missing_);
    }
    auto contents{this->artifact_read_json(resolution.path.value())};
    if (!contents.has_value()) {
      return sourcemeta::core::mcp_make_tool_error(version, request_id,
                                                   this->missing_);
    }

    auto envelope{sourcemeta::core::JSON::make_object()};
    envelope.assign_assume_new("results", std::move(contents.value()));

    // Every edge names what this instance serves at either of its ends, so
    // what a resource is comes off the report rather than being asked again
    std::set<std::string_view> unique_uris;
    std::map<std::string_view, std::string_view> kinds;
    for (const auto &entry : envelope.at("results").as_array()) {
      for (const auto *const field : {"from", "to"}) {
        const auto &end{entry.at(field)};
        const std::string_view uri{end.at("identifier").to_string()};
        if (uri.starts_with(this->server_uri())) {
          unique_uris.emplace(uri);
        }

        kinds.emplace(uri, end.at("type").to_string());
      }
    }

    auto content{sourcemeta::core::JSON::make_array()};
    std::ostringstream payload;
    sourcemeta::core::prettify(envelope, payload);
    content.push_back(sourcemeta::core::mcp_make_text_block(payload.str()));
    for (const auto uri : unique_uris) {
      const auto known{kinds.find(uri)};
      const auto describes_api{known == kinds.cend()
                                   ? this->describes_api(caller, uri)
                                   : known->second == OPENAPI_RESOURCE_KIND};
      content.push_back(sourcemeta::one::mcp_resource_block(
          version, uri, uri.substr(this->server_uri().size()), {},
          describes_api ? OPENAPI_MIME_TYPE : SCHEMA_MIME_TYPE));
    }

    return sourcemeta::core::mcp_make_tool_success(
        version, request_id, std::move(envelope), std::move(content));
  }

protected:
  ActionDependencyTreeV1(
      const std::filesystem::path &base,
      const sourcemeta::core::URITemplateRouterView &router,
      const sourcemeta::core::URITemplateRouter::Identifier identifier,
      const std::string_view metapack, const Tree tree,
      const std::string_view argument, const std::string_view missing,
      sourcemeta::one::Router &dispatcher)
      : sourcemeta::one::RouterAction{base, router.base_url(), dispatcher} {
    this->metapack_ = metapack;
    this->tree_ = tree;
    this->argument_ = argument;
    this->missing_ = missing;
    router.arguments(
        identifier, [this](const auto &key, const auto &value) -> void {
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
  static constexpr std::string_view SCHEMA_MIME_TYPE{"application/schema+json"};
  static constexpr std::string_view OPENAPI_RESOURCE_KIND{"openapi"};
  static constexpr std::string_view OPENAPI_MIME_TYPE{
      "application/openapi+json"};

  std::string_view metapack_;
  // A caller that never names a direction resolves nothing, which the tree it
  // resolves against must not decide
  Tree tree_{Tree::Schemas};
  std::string_view argument_{"uri"};
  std::string_view missing_{"Schema not found"};
  std::string_view response_schema_;
  std::string_view rpc_request_schema_;
  std::string_view rpc_response_schema_;
  std::string_view error_schema_;
};

#endif
