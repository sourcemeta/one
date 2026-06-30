#include <sourcemeta/core/http.h>
#include <sourcemeta/one/router.h>

#include <cctype>      // std::tolower
#include <chrono>      // std::chrono::seconds
#include <memory>      // std::make_unique
#include <mutex>       // std::call_once
#include <optional>    // std::optional, std::nullopt
#include <string>      // std::string
#include <string_view> // std::string_view
#include <utility>     // std::pair
#include <vector>      // std::vector

namespace sourcemeta::one {

namespace {

auto parse_max_age(
    const std::vector<std::pair<std::string, std::string>> &headers)
    -> std::optional<std::chrono::seconds> {
  for (const auto &header : headers) {
    if (header.first != "cache-control") {
      continue;
    }

    std::string value{header.second};
    for (auto &character : value) {
      character = static_cast<char>(
          std::tolower(static_cast<unsigned char>(character)));
    }

    bool no_store{false};
    std::optional<std::chrono::seconds> max_age;
    std::size_t start{0};
    while (start <= value.size()) {
      auto end{value.find(',', start)};
      if (end == std::string::npos) {
        end = value.size();
      }

      auto token{std::string_view{value}.substr(start, end - start)};
      while (!token.empty() && token.front() == ' ') {
        token.remove_prefix(1);
      }
      while (!token.empty() && token.back() == ' ') {
        token.remove_suffix(1);
      }

      if (token == "no-store" || token == "no-cache") {
        no_store = true;
      } else if (token.starts_with("max-age=")) {
        token.remove_prefix(std::string_view{"max-age="}.size());
        std::chrono::seconds::rep seconds{0};
        bool digits{!token.empty()};
        for (const char character : token) {
          if (character < '0' || character > '9') {
            digits = false;
            break;
          }

          seconds = (seconds * 10) + (character - '0');
          if (seconds > 100000000) {
            seconds = 100000000;
          }
        }

        if (digits) {
          max_age = std::chrono::seconds{seconds};
        }
      }

      start = end + 1;
    }

    if (no_store) {
      return std::chrono::seconds{0};
    }

    return max_age;
  }

  return std::nullopt;
}

auto default_jwks_fetcher() -> Authentication::JWKSFetcher {
  return [](const std::string_view url)
             -> std::optional<Authentication::JWKSFetchResult> {
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

      return Authentication::JWKSFetchResult{
          .body = response.body, .max_age = parse_max_age(response.headers)};
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
  router.arguments(0, [this](const auto &key, const auto &value) {
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
  std::call_once(slot.flag, [this, &slot, context, identifier] {
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
  // path. A credential-less CORS preflight is never gated
  if (identifier != 0 && request.method() != "options" &&
      !this->authentication_
           .admits(request.path(), credential, instance->server_uri_base_path())
           .allowed) {
    sourcemeta::one::json_error_unauthorized(request, response,
                                             this->default_error_schema_, "*");
    return;
  }

  instance->rest(matches, credential, request, response);
}

} // namespace sourcemeta::one
