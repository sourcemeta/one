#ifndef SOURCEMETA_ONE_ENTERPRISE_SERVER_ACTION_MCP_PRM_V1_H
#define SOURCEMETA_ONE_ENTERPRISE_SERVER_ACTION_MCP_PRM_V1_H

#include <sourcemeta/core/http.h>
#include <sourcemeta/core/json.h>
#include <sourcemeta/core/jsonrpc.h>
#include <sourcemeta/core/mcp.h>
#include <sourcemeta/core/uritemplate.h>

#include <sourcemeta/one/http.h>
#include <sourcemeta/one/router.h>

#include <cassert>     // assert
#include <filesystem>  // std::filesystem::path
#include <span>        // std::span
#include <sstream>     // std::ostringstream
#include <string>      // std::string
#include <string_view> // std::string_view

class ActionMCPProtectedResourceMetadata_v1
    : public sourcemeta::one::RouterAction {
public:
  static constexpr std::string_view DESCRIPTION{
      "Report where a token for the Model Context Protocol endpoint comes "
      "from"};
  static constexpr bool READ_ONLY{true};
  static constexpr bool DESTRUCTIVE{false};
  static constexpr bool IDEMPOTENT{true};
  static constexpr bool OPEN_WORLD{false};

  ActionMCPProtectedResourceMetadata_v1(
      const std::filesystem::path &base,
      const sourcemeta::core::URITemplateRouterView &router,
      const sourcemeta::core::URITemplateRouter::Identifier identifier,
      sourcemeta::one::Router &dispatcher)
      : sourcemeta::one::RouterAction{base, router.base_url(), dispatcher} {
    router.arguments(
        identifier, [this](const auto &key, const auto &value) -> void {
          if (key == "errorSchema") {
            this->error_schema_ = std::get<std::string_view>(value);
          } else if (key == "responseSchema") {
            this->response_schema_ = std::get<std::string_view>(value);
          }
        });

    const auto metadata_path{
        this->artifact_resolve_path_unauthenticated("", Tree::Explorer, "mcp")};
    assert(metadata_path.has_value());
    const auto metadata{this->artifact_read_json(metadata_path.value())};
    assert(metadata.has_value());
    const auto *document{metadata.value().try_at("protectedResourceMetadata")};
    if (document != nullptr) {
      std::ostringstream payload;
      sourcemeta::core::prettify(*document, payload);
      this->document_ = payload.str();
    }
  }

  // A client arrives here holding nothing, since this document is what tells
  // it where a credential comes from in the first place. Gating it would make
  // the answer reachable only by somebody who no longer needs to ask
  [[nodiscard]] auto is_authentication_exempt() const noexcept
      -> bool override {
    return true;
  }

  auto rest(const std::span<std::string_view>, std::string_view,
            std::string_view, sourcemeta::one::HTTPRequest &request,
            sourcemeta::one::HTTPResponse &response) -> void override {
    if (request.method() == "options") {
      sourcemeta::one::cors_preflight(request, response, "GET, HEAD, OPTIONS",
                                      "Accept, Accept-Encoding");
      return;
    }

    if (request.method() != "get" && request.method() != "head") {
      sourcemeta::one::json_error(
          request, response, sourcemeta::core::HTTP_STATUS_METHOD_NOT_ALLOWED,
          "urn:sourcemeta:one:method-not-allowed",
          "This HTTP method is invalid for this URL", this->error_schema_, "*",
          "GET, HEAD, OPTIONS");
      return;
    }

    // An instance with no policy that can honestly name an authorization
    // server publishes nothing, which is what a client reads as no metadata
    if (this->document_.empty()) {
      sourcemeta::one::json_error(
          request, response, sourcemeta::core::HTTP_STATUS_NOT_FOUND,
          "urn:sourcemeta:one:not-found", "There is nothing at this URL",
          this->error_schema_, "*");
      return;
    }

    response.write_status(sourcemeta::core::HTTP_STATUS_OK);
    response.write_header("Content-Type", "application/json");
    // Public because it is served to anybody, and revalidated because a stale
    // copy would name an authorization server this instance has stopped
    // accepting, sending a client to obtain a token that would be refused
    response.write_header(
        "Cache-Control",
        sourcemeta::one::RouterAction::content_cache_control(true));
    // A browser-based client fetches this from its own origin, so it has to be
    // readable across origins to be worth publishing at all
    response.write_header("Access-Control-Allow-Origin", "*");
    response.write_header("Access-Control-Expose-Headers", "Link, ETag");
    sourcemeta::one::write_link_header(response, this->response_schema_);
    sourcemeta::one::send_response(sourcemeta::core::HTTP_STATUS_OK, request,
                                   response, this->document_,
                                   sourcemeta::one::Encoding::Identity);
  }

  auto mcp(const sourcemeta::core::MCPProtocolVersion,
           const sourcemeta::core::JSON &id, const sourcemeta::core::JSON &,
           const sourcemeta::one::Credentials &)
      -> sourcemeta::core::JSON override {
    return sourcemeta::core::jsonrpc_make_error_method_not_found(id);
  }

private:
  std::string_view error_schema_;
  std::string_view response_schema_;
  std::string document_;
};

#endif
