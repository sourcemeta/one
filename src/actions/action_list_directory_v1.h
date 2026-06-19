#ifndef SOURCEMETA_ONE_ACTIONS_LIST_DIRECTORY_V1_H
#define SOURCEMETA_ONE_ACTIONS_LIST_DIRECTORY_V1_H

#include <sourcemeta/core/io.h>
#include <sourcemeta/core/json.h>
#include <sourcemeta/core/jsonrpc.h>
#include <sourcemeta/core/mcp.h>
#include <sourcemeta/core/text.h>
#include <sourcemeta/core/uritemplate.h>

#include <sourcemeta/one/http.h>
#include <sourcemeta/one/metapack.h>
#include <sourcemeta/one/router.h>

#include <filesystem>  // std::filesystem
#include <span>        // std::span
#include <sstream>     // std::ostringstream
#include <string>      // std::string
#include <string_view> // std::string_view
#include <utility>     // std::move

class ActionListDirectory_v1 : public sourcemeta::one::RouterAction {
public:
  static constexpr std::string_view DESCRIPTION{
      "List the contents of a directory in the catalog"};
  static constexpr bool READ_ONLY{true};
  static constexpr bool DESTRUCTIVE{false};
  static constexpr bool IDEMPOTENT{true};
  static constexpr bool OPEN_WORLD{false};

  ActionListDirectory_v1(
      const std::filesystem::path &base,
      const sourcemeta::core::URITemplateRouterView &router,
      const sourcemeta::core::URITemplateRouter::Identifier identifier,
      sourcemeta::one::Router &dispatcher)
      : sourcemeta::one::RouterAction{base, router.base_path(),
                                      router.base_url(), dispatcher} {
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

  auto rest(const std::span<std::string_view> matches,
            std::string_view credential, sourcemeta::one::HTTPRequest &request,
            sourcemeta::one::HTTPResponse &response) -> void override {
    if (request.method() == "options") {
      sourcemeta::one::cors_preflight(request, response, "GET, HEAD, OPTIONS",
                                      "Accept-Encoding, If-None-Match, "
                                      "If-Modified-Since");
      return;
    }
    const std::string_view path_match{matches.empty() ? std::string_view{}
                                                      : matches.front()};
    const auto resolution{this->artifact_resolve_path(
        credential, path_match, Tree::Explorer, "directory")};
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
        this->schema_evaluate(credential, this->rpc_request_schema_, arguments,
                              sourcemeta::blaze::Mode::Exhaustive)};
    if (!request_valid) {
      return sourcemeta::core::jsonrpc_make_error(
          &request_id, -32602, "Params fail against the tool request schema",
          std::move(request_output));
    }

    static const sourcemeta::core::JSON EMPTY_STRING{""};
    const auto &path_arg{arguments.at_or("path", EMPTY_STRING).to_string()};
    const auto resolution{this->artifact_resolve_path(
        credential, path_arg, Tree::Explorer, "directory")};
    if (resolution.outcome ==
        sourcemeta::one::ArtifactResolution::Outcome::Denied) {
      return sourcemeta::core::mcp_make_tool_error(request_id,
                                                   "Authentication required");
    }
    if (resolution.outcome !=
        sourcemeta::one::ArtifactResolution::Outcome::Found) {
      return sourcemeta::core::mcp_make_tool_error(request_id,
                                                   "Directory not found");
    }
    auto contents{this->artifact_read_json(resolution.path.value())};
    if (!contents.has_value()) {
      return sourcemeta::core::mcp_make_tool_error(request_id,
                                                   "Directory not found");
    }

    auto &result{contents.value()};
    auto content{sourcemeta::core::JSON::make_array()};

    std::ostringstream payload;
    sourcemeta::core::prettify(result, payload);
    content.push_back(sourcemeta::core::mcp_make_text_block(payload.str()));

    if (const auto *entries{result.try_at("entries")};
        entries != nullptr && entries->is_array()) {
      for (const auto &entry : entries->as_array()) {
        if (entry.at_or("type", EMPTY_STRING).to_string() != "schema") {
          continue;
        }
        if (!entry.defines("identifier")) {
          continue;
        }
        const auto &title{entry.at_or("title", EMPTY_STRING).to_string()};
        content.push_back(sourcemeta::core::mcp_make_resource_link(
            version, entry.at("identifier").to_string(),
            "application/schema+json",
            title.empty() ? entry.at("path").to_string() : title,
            entry.at_or("description", EMPTY_STRING).to_string()));
      }
    }

    return sourcemeta::core::mcp_make_tool_success(
        version, request_id, std::move(result), std::move(content));
  }

private:
  std::string_view response_schema_;
  std::string_view rpc_request_schema_;
  std::string_view rpc_response_schema_;
  std::string_view error_schema_;
};

#endif
