#include <sourcemeta/core/http.h>
#include <sourcemeta/core/io.h>
#include <sourcemeta/core/jose.h>
#include <sourcemeta/core/uri.h>
#include <sourcemeta/one/metapack.h>
#include <sourcemeta/one/router.h>

#include <chrono>      // std::chrono::seconds
#include <memory>      // std::make_unique
#include <mutex>       // std::call_once
#include <optional>    // std::optional, std::nullopt
#include <string>      // std::string
#include <string_view> // std::string_view

namespace sourcemeta::one {

namespace {

auto default_jwks_fetcher() -> sourcemeta::core::JWKSProvider::Fetcher {
  return [](const std::string_view url)
             -> std::optional<sourcemeta::core::JWKSProvider::FetchResult> {
    try {
      sourcemeta::core::HTTPSystemRequest request{std::string{url}};
      request.connect_timeout(std::chrono::seconds{2});
      request.timeout(std::chrono::seconds{5});
      request.maximum_response_size(1024UL * 1024UL);
      request.follow_redirects(false);
      const auto response{request.send()};
      if (response.status.code < 200 || response.status.code >= 300) {
        return std::nullopt;
      }

      std::optional<std::chrono::seconds> max_age;
      const auto header{sourcemeta::core::http_header_find(response.headers,
                                                           "cache-control")};
      if (header.has_value()) {
        max_age = sourcemeta::core::http_cache_control_max_age(header.value());
      }

      return sourcemeta::core::JWKSProvider::FetchResult{.body = response.body,
                                                         .max_age = max_age};
    } catch (...) {
      return std::nullopt;
    }
  };
}

} // namespace

Router::Router(const std::filesystem::path &base,
               const sourcemeta::core::URITemplateRouterView &router,
               const std::span<const RouterActionConstructor> constructors)
    : base_{base}, router_{router}, constructors_{constructors},
      // NOLINTNEXTLINE(modernize-avoid-c-arrays)
      slots_{std::make_unique<Slot[]>(router.size() + 1)},
      slots_size_{router.size() + 1},
      authentication_{base / "authentication.bin", default_jwks_fetcher()} {
  router.arguments(0, [this](const auto &key, const auto &value) -> void {
    if (key == "errorSchema") {
      this->default_error_schema_ = std::get<std::string_view>(value);
    }
  });
}

auto Router::error(const sourcemeta::one::HTTPRequest &request,
                   sourcemeta::one::HTTPResponse &response,
                   const sourcemeta::core::HTTPStatus &status,
                   const std::string_view type, const std::string_view detail,
                   const std::string_view origin) const -> void {
  sourcemeta::one::json_error(request, response, status, type, detail,
                              this->default_error_schema_, origin);
}

auto Router::action(
    const sourcemeta::core::URITemplateRouter::Identifier identifier,
    const sourcemeta::core::URITemplateRouter::Identifier context)
    -> RouterAction * {
  if (identifier >= this->slots_size_ || context >= this->constructors_.size())
      [[unlikely]] {
    return nullptr;
  }

  auto &slot{this->slots_[identifier]};
  std::call_once(slot.flag, [this, &slot, context, identifier]() -> void {
    slot.instance = this->constructors_[context](this->base_, this->router_,
                                                 identifier, *this);
  });

  return slot.instance.get();
}

auto Router::action(
    const sourcemeta::core::URITemplateRouter::Identifier identifier)
    -> RouterAction * {
  if (identifier >= this->slots_size_) [[unlikely]] {
    return nullptr;
  }
  return this->action(identifier, this->router_.context(identifier));
}

auto Router::dispatch(
    const sourcemeta::core::URITemplateRouter::Identifier identifier,
    const sourcemeta::core::URITemplateRouter::Identifier context,
    const std::span<std::string_view> matches,
    sourcemeta::one::HTTPRequest &request,
    sourcemeta::one::HTTPResponse &response) -> void {
  auto *instance{this->action(identifier, context)};
  if (instance == nullptr) [[unlikely]] {
    this->error(request, response,
                sourcemeta::core::HTTP_STATUS_NOT_IMPLEMENTED,
                "urn:sourcemeta:one:unknown-action",
                "This version does not implement such action handler for "
                "this URL",
                "*");
    return;
  }

  const auto credential{
      sourcemeta::core::http_parse_bearer(request.header("authorization"))};

  // Identifier zero is the catch-all, whose content the content gate
  // authorises after canonicalising the URL. Explicit routes are reached by
  // exact literal match, so the surface gate authorises them on their literal
  // path. A CORS preflight is never gated, and neither is a route that must
  // stay reachable to establish authentication in the first place, which
  // vouches for itself instead
  if (identifier != 0 && request.method() != "options" &&
      !instance->is_authentication_exempt() &&
      !this->authentication_
           .admits(request.path(), credential, request.header("cookie"),
                   instance->server_uri_base_path())
           .allowed) {
    if (instance->serve_login(request, response)) {
      return;
    }

    sourcemeta::one::json_error_unauthorized(request, response,
                                             this->default_error_schema_, "*");
    return;
  }

  instance->rest(matches, credential, request, response);
}

auto RouterAction::serve_login(sourcemeta::one::HTTPRequest &request,
                               sourcemeta::one::HTTPResponse &response) const
    -> bool {
  if ((request.method() != "get" && request.method() != "head") ||
      !sourcemeta::one::prefers_html(request.header("accept"))) {
    return false;
  }

  static constexpr BrowserSecurityHeaders SECURITY{
      .referrer_policy = "strict-origin-when-cross-origin",
      .frame_ancestors = "'none'",
      .x_frame_options = "DENY",
  };

  // The login page is a per-directory artifact, so a schema or a non-existent
  // path is answered by the nearest directory above it that offers a login.
  // Empty artifacts mark a directory no interactive policy governs, so they are
  // skipped. Because every login page under the same policies is
  // byte-identical, this walk never betrays whether a deeper path exists
  const auto stripped{sourcemeta::core::URI::strip_path_prefix(
      request.path(), this->server_uri_base_path())};
  std::string_view remaining{stripped.has_value()
                                 ? std::string_view{stripped.value()}
                                 : request.path()};
  if (remaining.ends_with("/")) {
    remaining.remove_suffix(1);
  }

  while (true) {
    const auto page{this->artifact_resolve_path_unauthenticated(
        remaining, Tree::Explorer, "login-html")};
    if (page.has_value()) {
      const sourcemeta::core::FileView view{page.value().path()};
      const auto info{sourcemeta::one::metapack_info(view)};
      // A directory no interactive policy governs gets a placeholder page with
      // no content. Text artifacts always carry a trailing newline, so that
      // placeholder is a lone byte, while a real login page is a full document
      if (info.has_value() && info->content_bytes > 1) {
        this->artifact_serve(page.value(),
                             sourcemeta::core::HTTP_STATUS_UNAUTHORIZED, false,
                             {}, {}, SECURITY, request, response, {},
                             "no-store", "Accept, Accept-Encoding");
        return true;
      }
    }

    if (remaining.empty()) {
      return false;
    }

    const auto slash{remaining.find_last_of('/')};
    remaining = slash == std::string_view::npos ? std::string_view{}
                                                : remaining.substr(0, slash);
  }
}

} // namespace sourcemeta::one
