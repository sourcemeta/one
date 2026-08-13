#ifndef SOURCEMETA_ONE_ACTIONS_AUTH_LOGIN_PAGE_V1_H
#define SOURCEMETA_ONE_ACTIONS_AUTH_LOGIN_PAGE_V1_H

#include <sourcemeta/core/http.h>
#include <sourcemeta/core/io.h>
#include <sourcemeta/core/json.h>
#include <sourcemeta/core/jsonrpc.h>
#include <sourcemeta/core/mcp.h>
#include <sourcemeta/core/uritemplate.h>

#include <sourcemeta/one/http.h>
#include <sourcemeta/one/metapack.h>
#include <sourcemeta/one/router.h>

#include <filesystem>  // std::filesystem::path
#include <optional>    // std::optional
#include <span>        // std::span
#include <string_view> // std::string_view
#include <utility>     // std::move

class ActionAuthLoginPage_v1 : public sourcemeta::one::RouterAction {
public:
  static constexpr std::string_view DESCRIPTION{
      "Show the ways of signing in to this instance"};
  static constexpr bool READ_ONLY{true};
  static constexpr bool DESTRUCTIVE{false};
  static constexpr bool IDEMPOTENT{true};
  static constexpr bool OPEN_WORLD{false};

  ActionAuthLoginPage_v1(
      const std::filesystem::path &base,
      const sourcemeta::core::URITemplateRouterView &router,
      const sourcemeta::core::URITemplateRouter::Identifier identifier,
      sourcemeta::one::Router &dispatcher)
      : sourcemeta::one::RouterAction{base, router.base_url(), dispatcher} {
    router.arguments(
        identifier, [this](const auto &key, const auto &value) -> void {
          if (key == "errorSchema") {
            this->error_schema_ = std::get<std::string_view>(value);
          }
        });

    // Signing in is what a caller without a session does, so the page that
    // offers it is the anonymous one for everybody, and it says the same thing
    // for as long as the instance stands
    auto located{this->artifact_resolve_path_unauthenticated(
        sourcemeta::one::VIEW_PUBLIC, "", Tree::Explorer, "login-html")};
    if (!located.has_value()) {
      return;
    }

    const sourcemeta::core::FileView contents{located.value().path()};
    const auto info{sourcemeta::one::metapack_info(contents)};
    // An instance that offers nowhere to sign in writes the page with no body
    // at all, which is a lone newline by the time it is stored
    if (info.has_value() && info->content_bytes > 1) {
      this->page_ = std::move(located);
    }
  }

  [[nodiscard]] auto is_authentication_exempt() const noexcept
      -> bool override {
    return true;
  }

  auto rest(const std::span<std::string_view>, std::string_view,
            std::string_view, sourcemeta::one::HTTPRequest &request,
            sourcemeta::one::HTTPResponse &response) -> void override {
    if (request.method() == "options") {
      response.write_status(sourcemeta::core::HTTP_STATUS_NO_CONTENT);
      response.write_header("Cache-Control", "no-store");
      response.write_header("Allow", "GET, HEAD, OPTIONS");
      sourcemeta::one::send_response(sourcemeta::core::HTTP_STATUS_NO_CONTENT,
                                     request, response);
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

    if (!this->page_.has_value()) {
      sourcemeta::one::json_error(
          request, response, sourcemeta::core::HTTP_STATUS_NOT_FOUND,
          "urn:sourcemeta:one:not-found", "There is nothing at this URL",
          this->error_schema_, "*");
      return;
    }

    // The one page every caller is served, so a shared cache holding it hands
    // the next caller what they would have been given anyway
    this->artifact_serve(
        this->page_.value(), sourcemeta::core::HTTP_STATUS_OK, false, {}, {},
        HTML_BROWSER_SECURITY, request, response, this->error_schema_,
        "public, max-age=0, must-revalidate", "Accept-Encoding");
  }

  auto mcp(const sourcemeta::core::MCPProtocolVersion,
           const sourcemeta::core::JSON &id, const sourcemeta::core::JSON &,
           const sourcemeta::one::Credentials &)
      -> sourcemeta::core::JSON override {
    return sourcemeta::core::jsonrpc_make_error_method_not_found(id);
  }

private:
  std::optional<sourcemeta::one::ResolvedArtifact> page_;
  std::string_view error_schema_;
};

#endif
