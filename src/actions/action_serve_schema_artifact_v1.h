#ifndef SOURCEMETA_ONE_ACTIONS_SERVE_SCHEMA_ARTIFACT_V1_H
#define SOURCEMETA_ONE_ACTIONS_SERVE_SCHEMA_ARTIFACT_V1_H

#include <sourcemeta/core/json.h>
#include <sourcemeta/core/jsonrpc.h>
#include <sourcemeta/core/mcp.h>
#include <sourcemeta/core/uritemplate.h>

#include <sourcemeta/one/http.h>
#include <sourcemeta/one/metapack.h>
#include <sourcemeta/one/router.h>

#include <filesystem>  // std::filesystem
#include <span>        // std::span
#include <string>      // std::string
#include <string_view> // std::string_view
#include <utility>     // std::move

class ActionServeSchemaArtifact_v1 : public sourcemeta::one::RouterAction {
public:
  static constexpr std::string_view DESCRIPTION{
      "Look up a precomputed artifact about a specific schema by its "
      "absolute URI"};
  static constexpr bool READ_ONLY{true};
  static constexpr bool DESTRUCTIVE{false};
  static constexpr bool IDEMPOTENT{true};
  static constexpr bool OPEN_WORLD{false};

  ActionServeSchemaArtifact_v1(
      const std::filesystem::path &base,
      const sourcemeta::core::URITemplateRouterView &router,
      const sourcemeta::core::URITemplateRouter::Identifier identifier,
      sourcemeta::one::Router &dispatcher)
      : sourcemeta::one::RouterAction{base, router.base_path(),
                                      router.base_url(), dispatcher} {
    router.arguments(
        identifier, [this](const auto &key, const auto &value) -> void {
          if (key == "artifact") {
            this->artifact_ = std::get<std::string_view>(value);
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
            std::string_view credential, sourcemeta::one::HTTPRequest &request,
            sourcemeta::one::HTTPResponse &response) -> void override {
    if (request.method() == "options") {
      sourcemeta::one::cors_preflight(request, response, "GET, HEAD, OPTIONS",
                                      "Accept-Encoding, If-None-Match, "
                                      "If-Modified-Since");
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
      sourcemeta::one::json_error(request, response,
                                  sourcemeta::core::HTTP_STATUS_BAD_REQUEST,
                                  "urn:sourcemeta:one:invalid-schema-uri",
                                  "The schema URI must not contain a fragment",
                                  this->error_schema_, "*");
      return;
    }

    const auto resolution{this->artifact_resolve_path(
        {.bearer = credential, .cookies = request.header("cookie")},
        matches.front(), Tree::Schemas, this->artifact_)};
    if (resolution.outcome ==
        sourcemeta::one::ArtifactResolution::Outcome::Denied) {
      sourcemeta::one::json_error_unauthorized(request, response,
                                               this->error_schema_, "*");
      return;
    }
    if (resolution.outcome !=
        sourcemeta::one::ArtifactResolution::Outcome::Found) {
      sourcemeta::one::json_error(
          request, response, sourcemeta::core::HTTP_STATUS_NOT_FOUND,
          "urn:sourcemeta:one:not-found", "There is nothing at this URL",
          this->error_schema_, "*");
      return;
    }
    this->artifact_serve(
        resolution.path.value(), sourcemeta::core::HTTP_STATUS_OK, true, {},
        this->response_schema_, {}, request, response, this->error_schema_,
        this->content_cache_control(resolution.is_public), "Accept-Encoding");
  }

  auto mcp(const sourcemeta::core::MCPProtocolVersion version,
           const sourcemeta::core::JSON &request_id,
           const sourcemeta::core::JSON &arguments, std::string_view credential)
      -> sourcemeta::core::JSON override {
    auto [request_valid, request_output]{
        this->schema_evaluate({.bearer = credential}, this->rpc_request_schema_,
                              arguments, sourcemeta::blaze::Mode::Exhaustive)};
    if (!request_valid) {
      return sourcemeta::core::jsonrpc_make_error(
          &request_id, -32602, "Params fail against the tool request schema",
          std::move(request_output));
    }

    const auto resolution{this->artifact_resolve_path(
        {.bearer = credential}, arguments.at("schema").to_string(),
        Tree::Schemas, this->artifact_)};
    if (resolution.outcome ==
        sourcemeta::one::ArtifactResolution::Outcome::Denied) {
      return sourcemeta::core::mcp_make_tool_error(request_id,
                                                   "Authentication required");
    }
    if (resolution.outcome !=
        sourcemeta::one::ArtifactResolution::Outcome::Found) {
      return sourcemeta::core::mcp_make_tool_error(request_id,
                                                   "Schema not found");
    }
    auto contents{this->artifact_read_json(resolution.path.value())};
    if (!contents.has_value()) {
      return sourcemeta::core::mcp_make_tool_error(request_id,
                                                   "Schema not found");
    }

    return sourcemeta::core::mcp_make_tool_success(version, request_id,
                                                   std::move(contents).value());
  }

protected:
  ActionServeSchemaArtifact_v1(
      const std::filesystem::path &base,
      const sourcemeta::core::URITemplateRouterView &router,
      const sourcemeta::core::URITemplateRouter::Identifier identifier,
      const std::string_view artifact, sourcemeta::one::Router &dispatcher)
      : sourcemeta::one::RouterAction{base, router.base_path(),
                                      router.base_url(), dispatcher} {
    this->artifact_ = artifact;
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
  std::string_view artifact_;
  std::string_view response_schema_;
  std::string_view rpc_request_schema_;
  std::string_view rpc_response_schema_;
  std::string_view error_schema_;
};

#endif
