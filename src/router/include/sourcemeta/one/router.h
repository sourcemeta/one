#ifndef SOURCEMETA_ONE_ROUTER_H
#define SOURCEMETA_ONE_ROUTER_H

#include <sourcemeta/blaze/compiler.h>
#include <sourcemeta/blaze/evaluator.h>
#include <sourcemeta/core/json.h>
#include <sourcemeta/core/jsonrpc.h>
#include <sourcemeta/core/mcp.h>
#include <sourcemeta/core/uritemplate.h>

#include <sourcemeta/one/authentication.h>
#include <sourcemeta/one/http.h>
#include <sourcemeta/one/router_lru.h>

#include <cstddef>     // std::size_t
#include <cstdint>     // std::uint8_t
#include <filesystem>  // std::filesystem::path
#include <memory>      // std::make_unique, std::shared_ptr, std::unique_ptr
#include <mutex>       // std::once_flag
#include <optional>    // std::optional
#include <span>        // std::span
#include <string_view> // std::string_view
#include <utility>     // std::move, std::pair

namespace sourcemeta::one {

class Router;
class RouterAction;

// Proof that a path was produced by the artifact resolver. Only the
// action base class can mint one, so every artifact read must have
// passed through resolution by construction
class ResolvedArtifact {
public:
  [[nodiscard]] auto path() const noexcept -> const std::filesystem::path & {
    return this->path_;
  }

private:
  friend class RouterAction;
  explicit ResolvedArtifact(std::filesystem::path path)
      : path_{std::move(path)} {}
  std::filesystem::path path_;
};

// The three-outcome result of artifact resolution. Denial is distinct
// from absence so surfaces can answer 401 versus 404. Nothing produces
// Denied until authorisation lands in the resolver
struct ArtifactResolution {
  enum class Outcome : std::uint8_t { Found, NotFound, Denied };
  Outcome outcome{Outcome::NotFound};
  std::optional<ResolvedArtifact> path;
  // Whether the path is served to anonymous callers, as opposed to admitted
  // only because the caller presented a matching credential
  bool is_public{true};
};

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
                    std::string_view credential, HTTPRequest &request,
                    HTTPResponse &response) -> void = 0;

  virtual auto mcp(const sourcemeta::core::MCPProtocolVersion version,
                   const sourcemeta::core::JSON &id,
                   const sourcemeta::core::JSON &arguments,
                   std::string_view credential) -> sourcemeta::core::JSON = 0;

  // Whether this route stays reachable no matter which policies cover its path.
  // A route that a caller must reach in order to establish authentication
  // cannot itself sit behind that authentication, so it opts out of the gate
  // and guards itself instead. The default denies the exemption
  [[nodiscard]] virtual auto is_authentication_exempt() const noexcept -> bool {
    return false;
  }

  // Send an unauthenticated browser navigating to a path that exactly one
  // interactive policy governs to begin a login, returning true when it wrote
  // the redirect so the caller stops. Machines, non-navigations, and paths
  // with no or several interactive policies fall through to the plain denial
  [[nodiscard]] auto redirect_to_login(HTTPRequest &request,
                                       HTTPResponse &response) const -> bool;

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

  // Browser-targeted security headers we apply to every HTML response:
  //
  // - Referrer-Policy (W3C Referrer Policy):
  //   https://www.w3.org/TR/referrer-policy/
  //   Send full URL on same-origin navigation, only the origin on
  //   cross-origin navigation. Schema paths within the browser encode the
  //   user's current view and would otherwise leak via every external link
  //   click.
  //
  // - Content-Security-Policy frame-ancestors (W3C CSP Level 3 §6.4.2):
  //   https://www.w3.org/TR/CSP3/#directive-frame-ancestors
  //   Modern clickjacking control: deny embedding the web UI in any
  //   iframe.
  //
  // - X-Frame-Options (RFC 7034):
  //   https://datatracker.ietf.org/doc/html/rfc7034
  //   Legacy clickjacking control for browsers that predate CSP3
  //   frame-ancestors. Belt-and-suspenders for old client coverage at
  //   near-zero header cost.
  //
  // JSON and static-asset responses pass a default-constructed (all-empty)
  // instance and emit none of these.
  static constexpr BrowserSecurityHeaders HTML_BROWSER_SECURITY{
      .referrer_policy = "strict-origin-when-cross-origin",
      .frame_ancestors = "'none'",
      .x_frame_options = "DENY",
  };

  [[nodiscard]] auto artifact_resolve_path(Credentials credentials,
                                           std::string_view input, Tree tree,
                                           std::string_view artifact_name) const
      -> ArtifactResolution;

  // The caching directive for served registry content. A response admitted
  // only via a credential must not be stored by shared caches, so it is
  // marked private rather than public
  [[nodiscard]] static auto content_cache_control(const bool is_public) noexcept
      -> std::string_view {
    return is_public ? "public, max-age=0, must-revalidate"
                     : "private, max-age=0, must-revalidate";
  }

  [[nodiscard]] auto artifact_read_json(const ResolvedArtifact &artifact) const
      -> std::optional<sourcemeta::core::JSON>;

  auto artifact_serve(const ResolvedArtifact &artifact,
                      const sourcemeta::core::HTTPStatus &status,
                      bool enable_cors, std::string_view mime,
                      std::string_view link,
                      const BrowserSecurityHeaders &browser_security,
                      HTTPRequest &request, HTTPResponse &response,
                      std::string_view error_schema,
                      std::string_view cache_control,
                      std::string_view vary) const -> void;

  [[nodiscard]] auto
  schema_evaluate_fast(Credentials credentials, std::string_view schema_uri,
                       const sourcemeta::core::JSON &instance) const -> bool;

  [[nodiscard]] auto schema_evaluate(Credentials credentials,
                                     std::string_view schema_uri,
                                     const sourcemeta::core::JSON &instance,
                                     sourcemeta::blaze::Mode mode) const
      -> std::pair<bool, sourcemeta::core::JSON>;

  [[nodiscard]] auto schema_evaluate_with_tracing(
      Credentials credentials, std::string_view schema_uri,
      const sourcemeta::core::JSON &instance,
      const sourcemeta::blaze::Callback &callback) const -> bool;

protected:
  // Resolution for trees that are not registry content, such as the
  // compile-time static asset bundle. Same containment discipline as
  // registry resolution, against a caller-declared root, but access to
  // these trees is governed at the route level, as their identity is a
  // URL rather than a registry path. Existence is left to the serving
  // layer, which orders its method check before the existence check
  [[nodiscard]] auto artifact_resolve_static(const std::filesystem::path &root,
                                             std::string_view relative) const
      -> ArtifactResolution;

  // Escape hatch for indexer-generated infrastructure metadata read at
  // action construction, where no request and therefore no context
  // exists. Never use this on a request-driven path
  [[nodiscard]] auto
  artifact_resolve_path_unauthenticated(std::string_view input, Tree tree,
                                        std::string_view artifact_name) const
      -> std::optional<ResolvedArtifact>;

private:
  [[nodiscard]] auto artifact_locate(std::string_view input, Tree tree,
                                     std::string_view artifact_name) const
      -> std::optional<std::filesystem::path>;

  [[nodiscard]] auto blaze_template(Credentials credentials,
                                    std::string_view schema_uri,
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

  [[nodiscard]] auto blaze_template(const ResolvedArtifact &artifact)
      -> std::shared_ptr<const sourcemeta::blaze::Template>;

  [[nodiscard]] auto authentication() const noexcept -> const Authentication & {
    return this->authentication_;
  }

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
  Authentication authentication_;
};

} // namespace sourcemeta::one

#endif
