#ifndef SOURCEMETA_ONE_ROUTER_H
#define SOURCEMETA_ONE_ROUTER_H

#include <sourcemeta/blaze/compiler.h>
#include <sourcemeta/blaze/evaluator.h>
#include <sourcemeta/core/json.h>
#include <sourcemeta/core/jsonrpc.h>
#include <sourcemeta/core/mcp.h>
#include <sourcemeta/core/uritemplate.h>

#include <sourcemeta/one/http.h>
#include <sourcemeta/one/router_lru.h>

#include <cstddef>     // std::size_t
#include <cstdint>     // std::uint8_t
#include <filesystem>  // std::filesystem::path
#include <memory>      // std::make_unique, std::shared_ptr, std::unique_ptr
#include <mutex>       // std::once_flag
#include <optional>    // std::optional
#include <span>        // std::span
#include <string>      // std::string
#include <string_view> // std::string_view
#include <utility>     // std::pair

namespace sourcemeta::one {

class Router;

class RouterAction {
public:
  RouterAction(const std::filesystem::path &index_directory,
               const std::string_view server_uri_base_path,
               const std::string_view server_uri, Router &dispatcher)
      : index_directory_{index_directory},
        server_uri_base_path_{server_uri_base_path}, server_uri_{server_uri},
        dispatcher_{dispatcher} {}
  virtual ~RouterAction() = default;

  // To avoid mistakes
  RouterAction(const RouterAction &) = delete;
  RouterAction(RouterAction &&) = delete;
  auto operator=(const RouterAction &) -> RouterAction & = delete;
  auto operator=(RouterAction &&) -> RouterAction & = delete;

  virtual auto rest(const std::span<std::string_view> matches,
                    HTTPRequest &request, HTTPResponse &response) -> void = 0;

  virtual auto mcp(const sourcemeta::core::MCPProtocolVersion version,
                   const sourcemeta::core::JSON &id,
                   const sourcemeta::core::JSON &arguments)
      -> sourcemeta::core::JSON = 0;

  [[nodiscard]] auto server_uri_base_path() const noexcept -> std::string_view {
    return this->server_uri_base_path_;
  }

  [[nodiscard]] auto server_uri() const noexcept -> std::string_view {
    return this->server_uri_;
  }

  [[nodiscard]] auto dispatcher() const noexcept -> Router & {
    return this->dispatcher_;
  }

  enum class Tree : std::uint8_t { Schemas, Explorer };

  struct BrowserSecurityHeaders {
    // W3C Referrer Policy (https://www.w3.org/TR/referrer-policy/)
    std::string_view referrer_policy{};
    // W3C CSP Level 3 §6.4.2
    // (https://www.w3.org/TR/CSP3/#directive-frame-ancestors) Value is the
    // directive's source-list (e.g. `'none'`, `'self'`, an origin allowlist).
    // The full header value is composed as `frame-ancestors <value>`.
    std::string_view frame_ancestors{};
    // RFC 7034 (https://datatracker.ietf.org/doc/html/rfc7034)
    // Legacy clickjacking control for browsers that predate CSP3
    // frame-ancestors. Value is one of "DENY", "SAMEORIGIN", or
    // "ALLOW-FROM <uri>".
    std::string_view x_frame_options{};
  };

  [[nodiscard]] auto artifact_resolve_path(std::string_view input, Tree tree,
                                           std::string_view artifact_name) const
      -> std::optional<std::filesystem::path>;

  [[nodiscard]] auto
  artifact_read_json(const std::filesystem::path &absolute_path) const
      -> std::optional<sourcemeta::core::JSON>;

  auto artifact_serve(const std::filesystem::path &absolute_path,
                      const sourcemeta::core::HTTPStatus &status,
                      bool enable_cors, std::string_view mime,
                      std::string_view link,
                      const BrowserSecurityHeaders &browser_security,
                      HTTPRequest &request, HTTPResponse &response,
                      std::string_view error_schema,
                      std::string_view cache_control,
                      std::string_view vary) const -> void;

  [[nodiscard]] auto
  schema_evaluate_fast(std::string_view schema_uri,
                       const sourcemeta::core::JSON &instance) const -> bool;

  [[nodiscard]] auto schema_evaluate(std::string_view schema_uri,
                                     const sourcemeta::core::JSON &instance,
                                     sourcemeta::blaze::Mode mode) const
      -> std::pair<bool, sourcemeta::core::JSON>;

  [[nodiscard]] auto schema_evaluate_with_tracing(
      std::string_view schema_uri, const sourcemeta::core::JSON &instance,
      const sourcemeta::blaze::Callback &callback) const -> bool;

private:
  [[nodiscard]] auto blaze_template(std::string_view schema_uri,
                                    sourcemeta::blaze::Mode mode) const
      -> std::shared_ptr<const sourcemeta::blaze::Template>;

  // NOLINTNEXTLINE(cppcoreguidelines-avoid-const-or-ref-data-members)
  const std::filesystem::path &index_directory_;
  std::string_view server_uri_base_path_;
  std::string_view server_uri_;
  // NOLINTNEXTLINE(cppcoreguidelines-avoid-const-or-ref-data-members)
  Router &dispatcher_;
};

using RouterActionConstructor =
    auto (*)(const std::filesystem::path &,
             const sourcemeta::core::URITemplateRouterView &,
             sourcemeta::core::URITemplateRouter::Identifier, Router &)
        -> std::unique_ptr<RouterAction>;

template <typename T>
auto make_router_action(
    const std::filesystem::path &base,
    const sourcemeta::core::URITemplateRouterView &router,
    const sourcemeta::core::URITemplateRouter::Identifier identifier,
    Router &dispatcher) -> std::unique_ptr<RouterAction> {
  return std::make_unique<T>(base, router, identifier, dispatcher);
}

class Router {
public:
  Router(const std::filesystem::path &base,
         const core::URITemplateRouterView &router,
         std::span<const RouterActionConstructor> constructors);
  ~Router() = default;

  // To avoid mistakes
  Router(const Router &) = delete;
  Router(Router &&) = delete;
  auto operator=(const Router &) -> Router & = delete;
  auto operator=(Router &&) -> Router & = delete;

  auto dispatch(const core::URITemplateRouter::Identifier identifier,
                const core::URITemplateRouter::Identifier context,
                const std::span<std::string_view> matches, HTTPRequest &request,
                HTTPResponse &response) -> void;

  [[nodiscard]] auto
  action(const core::URITemplateRouter::Identifier identifier,
         const core::URITemplateRouter::Identifier context) -> RouterAction *;

  [[nodiscard]] auto
  action(const core::URITemplateRouter::Identifier identifier)
      -> RouterAction *;

  auto error(const HTTPRequest &request, HTTPResponse &response,
             const sourcemeta::core::HTTPStatus &status, std::string_view type,
             std::string_view detail, std::string_view origin) const -> void;

  [[nodiscard]] auto blaze_template(const std::filesystem::path &absolute_path)
      -> std::shared_ptr<const sourcemeta::blaze::Template>;

private:
  static constexpr std::size_t TEMPLATE_CACHE_CAPACITY{50};

  struct Slot {
    std::unique_ptr<RouterAction> instance;
    std::once_flag flag;
  };

  // NOLINTNEXTLINE(cppcoreguidelines-avoid-const-or-ref-data-members)
  const std::filesystem::path &base_;
  // NOLINTNEXTLINE(cppcoreguidelines-avoid-const-or-ref-data-members)
  const core::URITemplateRouterView &router_;
  std::span<const RouterActionConstructor> constructors_;
  // NOLINTNEXTLINE(modernize-avoid-c-arrays)
  std::unique_ptr<Slot[]> slots_;
  std::size_t slots_size_;
  std::string_view default_error_schema_;
  RouterLRU<std::filesystem::path, sourcemeta::blaze::Template> template_cache_{
      TEMPLATE_CACHE_CAPACITY};
};

} // namespace sourcemeta::one

#endif
