#ifndef SOURCEMETA_ONE_ENTERPRISE_SERVER_ACTION_AUTH_CALLBACK_V1_H_
#define SOURCEMETA_ONE_ENTERPRISE_SERVER_ACTION_AUTH_CALLBACK_V1_H_

#include <sourcemeta/core/crypto.h>
#include <sourcemeta/core/http.h>
#include <sourcemeta/core/jose.h>
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

#include <array>       // std::array
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

class ActionAuthCallback_v1 : public sourcemeta::one::RouterAction {
public:
  static constexpr std::string_view DESCRIPTION{
      "Complete an interactive login by exchanging the provider's "
      "authorization code for a session"};
  static constexpr bool READ_ONLY{false};
  static constexpr bool DESTRUCTIVE{false};
  static constexpr bool IDEMPOTENT{false};
  static constexpr bool OPEN_WORLD{false};

  // A session lasts an hour, kept short so that a lost cookie cannot outlive
  // its usefulness, with silent re-authentication as the eventual refresh
  static constexpr std::chrono::seconds SESSION_LIFETIME{3600};

  // How long a browser stays eligible for a silent renewal after signing in.
  // Long enough to outlast a provider session, since the provider is the one
  // that decides whether a renewal succeeds, and losing it early only costs a
  // sign-in page that would otherwise have been skipped
  static constexpr std::chrono::seconds RENEWAL_LIFETIME{43200};

  // RFC 6265 Section 6.1 asks a user agent to support "at least 4096 bytes per
  // cookie (as measured by the sum of the length of the cookie's name, value,
  // and attributes)". That is a floor they should honour rather than a ceiling
  // they must enforce, and what happens above it is left unsaid, so the whole
  // serialised cookie is kept under it with room to spare
  static constexpr std::size_t MAXIMUM_COOKIE_LENGTH{4000};

  ActionAuthCallback_v1(
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
      response.write_header("Cache-Control",
                            sourcemeta::one::cache_control_no_store());
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
    // The redirect URI must be the one the login was started with, since the
    // provider checks it and so does the exchange. It is composed here because
    // which routes this instance serves is not something authentication knows
    constexpr std::string_view CALLBACK_PATH{"/self/v1/auth/callback/"};
    std::string redirect_uri{this->server_uri()};
    redirect_uri += CALLBACK_PATH;
    redirect_uri += matches.front();

    const sourcemeta::one::RequestCookies cookies{request};
    const auto outcome{this->dispatcher().authentication().callback(
        matches.front(), this->server_uri(), redirect_uri,
        {.state = request.query("state"),
         .code = request.query("code"),
         .has_error = request.has_query("error"),
         .error = request.query("error"),
         .has_issuer = request.has_query("iss"),
         .issuer = request.query("iss")},
        {.cookies = cookies})};
    for (const auto &message : outcome.log) {
      sourcemeta::one::HTTP_LOG(message, matches.front());
    }

    using Result = sourcemeta::one::Authentication::Outcome::Result;
    switch (outcome.result) {
      case Result::Redirect:
        break;
      // A failed login leaves its transaction cookie in place, sealed and
      // bound to a state the provider would have to echo, expiring on its own
      // within minutes, so no cookie is cleared here and the error response
      // owns the status line uncontested
      case Result::Declined:
        this->fail(request, response, sourcemeta::core::HTTP_STATUS_FORBIDDEN,
                   "urn:sourcemeta:one:auth-login-declined",
                   "The identity provider declined the login");
        return;
      case Result::NotAdmitted:
        this->fail(request, response, sourcemeta::core::HTTP_STATUS_FORBIDDEN,
                   "urn:sourcemeta:one:auth-not-admitted",
                   "This account is not admitted here");
        return;
      // Every reason a proven callback cannot end in a session answers
      // identically. Anybody may start a login and bring back a code of their
      // own invention, so reaching here proves nothing about who is asking,
      // while telling a secret apart from an unanswering provider apart from a
      // token that would not validate reports how this deployment and its
      // provider are faring. The cause went to the log above
      case Result::Incomplete:
        this->fail(request, response,
                   sourcemeta::core::HTTP_STATUS_INTERNAL_SERVER_ERROR,
                   "urn:sourcemeta:one:auth-incomplete",
                   "The session could not be established");
        return;
      default:
        this->fail(request, response, sourcemeta::core::HTTP_STATUS_BAD_REQUEST,
                   "urn:sourcemeta:one:auth-invalid-callback",
                   "The login could not be completed");
        return;
    }

    response.write_status(sourcemeta::core::HTTP_STATUS_SEE_OTHER);
    for (const auto &cookie : outcome.cookies) {
      response.write_header("Set-Cookie", cookie);
    }

    response.write_header("Location",
                          outcome.location.empty() ? "/" : outcome.location);
    response.write_header("Cache-Control",
                          sourcemeta::one::cache_control_no_store());
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
  auto fail(sourcemeta::one::HTTPRequest &request,
            sourcemeta::one::HTTPResponse &response,
            const sourcemeta::core::HTTPStatus &status,
            const std::string_view type, const std::string_view detail) const
      -> void {
    sourcemeta::one::json_error(request, response, status, type, detail,
                                this->error_schema_, "*");
  }

  std::string_view error_schema_;
};

#endif
