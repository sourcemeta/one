#ifndef SOURCEMETA_ONE_ENTERPRISE_SERVER_ACTION_AUTH_LOGIN_V1_H_
#define SOURCEMETA_ONE_ENTERPRISE_SERVER_ACTION_AUTH_LOGIN_V1_H_

#include <sourcemeta/core/http.h>
#include <sourcemeta/core/json.h>
#include <sourcemeta/core/jsonrpc.h>
#include <sourcemeta/core/mcp.h>
#include <sourcemeta/core/oauth.h>
#include <sourcemeta/core/oidc.h>
#include <sourcemeta/core/uri.h>
#include <sourcemeta/core/uritemplate.h>

#include <sourcemeta/one/authentication.h>
#include <sourcemeta/one/http.h>
#include <sourcemeta/one/router.h>

#include <algorithm>   // std::ranges::find, std::ranges::sort
#include <chrono>      // std::chrono::seconds, std::chrono::system_clock
#include <filesystem>  // std::filesystem::path
#include <functional>  // std::less
#include <mutex>       // std::mutex, std::scoped_lock
#include <optional>    // std::optional, std::nullopt
#include <set>         // std::set
#include <span>        // std::span
#include <sstream>     // std::ostringstream
#include <string>      // std::string
#include <string_view> // std::string_view
#include <utility>     // std::move
#include <vector>      // std::vector

class ActionAuthLogin_v1 : public sourcemeta::one::RouterAction {
public:
  static constexpr std::string_view DESCRIPTION{
      "Start an interactive login by redirecting the browser to the identity "
      "provider"};
  static constexpr bool READ_ONLY{false};
  static constexpr bool DESTRUCTIVE{false};
  static constexpr bool IDEMPOTENT{true};
  static constexpr bool OPEN_WORLD{false};

  // How long a browser has to complete a login at the provider before the
  // transaction expires
  static constexpr std::chrono::seconds TRANSACTION_LIFETIME{600};

  ActionAuthLogin_v1(
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
  }

  [[nodiscard]] auto is_authentication_exempt() const noexcept
      -> bool override {
    return true;
  }

  auto rest(const std::span<std::string_view> matches,
            const sourcemeta::one::Authentication::Caller &,
            sourcemeta::one::HTTPRequest &request,
            sourcemeta::one::HTTPResponse &response) -> void override {
    if (request.method() == "options") {
      response.write_status(sourcemeta::core::HTTP_STATUS_NO_CONTENT);
      response.write_header("Cache-Control", "no-store");
      // RFC 9110 §9.3.7: OPTIONS responses SHOULD include Allow
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

    if (matches.empty()) {
      sourcemeta::one::json_error(
          request, response,
          sourcemeta::core::HTTP_STATUS_INTERNAL_SERVER_ERROR,
          "urn:sourcemeta:one:auth-missing-policy-match",
          "This action requires a policy name match", this->error_schema_, "*");
      return;
    }

    // Where the browser goes once this completes. An explicit `to` query wins,
    // then the referring page, which for a login reached from a denied page is
    // exactly that page. Only a same-origin local path is ever honoured, so
    // this cannot be turned into an open redirect. What the policy governs
    // stands in where neither says anything, and that is decided by whoever
    // holds the policy rather than here
    std::string return_to;
    const auto destination{request.query("to")};
    if (!destination.empty() && sourcemeta::one::is_local_path(destination)) {
      return_to = destination;
    } else if (const auto referer{request.header("referer")};
               referer.starts_with(this->server_uri())) {
      std::string candidate{referer.substr(this->server_uri().size())};
      if (sourcemeta::one::is_local_path(candidate)) {
        return_to = std::move(candidate);
      }
    }

    // The callback URL is pinned from the configured public URL, never
    // inferred from an incoming request, and it is composed here because which
    // routes this instance serves is not something authentication knows
    constexpr std::string_view CALLBACK_PATH{"/self/v1/auth/callback/"};
    std::string redirect_uri;
    redirect_uri.reserve(this->server_uri().size() + CALLBACK_PATH.size() +
                         matches.front().size());
    redirect_uri += this->server_uri();
    redirect_uri += CALLBACK_PATH;
    redirect_uri += matches.front();

    const auto outcome{this->dispatcher().authentication().login(
        matches.front(), this->server_uri(), redirect_uri,
        !request.query("silent").empty(), return_to)};
    for (const auto &message : outcome.log) {
      sourcemeta::one::HTTP_LOG(message, matches.front());
    }

    if (outcome.result ==
        sourcemeta::one::Authentication::Outcome::Result::Missing) {
      sourcemeta::one::json_error(
          request, response, sourcemeta::core::HTTP_STATUS_NOT_FOUND,
          "urn:sourcemeta:one:not-found", "There is nothing at this URL",
          this->error_schema_, "*");
      return;
    }

    // Every reason a login cannot start answers identically. Which policies
    // exist is published, but whether one is misconfigured and whether its
    // provider is answering are neither, and telling those apart hands a
    // caller who has authenticated to nothing a view of how this deployment is
    // doing. The cause went to the log above, where an operator looks and a
    // caller cannot
    if (outcome.result !=
        sourcemeta::one::Authentication::Outcome::Result::Redirect) {
      sourcemeta::one::json_error(
          request, response,
          sourcemeta::core::HTTP_STATUS_INTERNAL_SERVER_ERROR,
          "urn:sourcemeta:one:auth-unavailable", "This login cannot be started",
          this->error_schema_, "*");
      return;
    }

    response.write_status(sourcemeta::core::HTTP_STATUS_SEE_OTHER);
    for (const auto &cookie : outcome.cookies) {
      response.write_header("Set-Cookie", cookie);
    }

    response.write_header("Location", outcome.location);
    response.write_header("Cache-Control", "no-store");
    sourcemeta::one::send_response(sourcemeta::core::HTTP_STATUS_SEE_OTHER,
                                   request, response);
  }

  auto mcp(const sourcemeta::core::MCPProtocolVersion,
           const sourcemeta::core::JSON &id, const sourcemeta::core::JSON &,
           const sourcemeta::one::Authentication::Caller &)
      -> sourcemeta::core::JSON override {
    return sourcemeta::core::jsonrpc_make_error_method_not_found(id);
  }

private:
  std::string_view error_schema_;
};

#endif
