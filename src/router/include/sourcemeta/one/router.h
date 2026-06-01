#ifndef SOURCEMETA_ONE_ROUTER_H
#define SOURCEMETA_ONE_ROUTER_H

#include <sourcemeta/blaze/compiler.h>
#include <sourcemeta/blaze/evaluator.h>
#include <sourcemeta/core/json.h>
#include <sourcemeta/core/jsonrpc.h>
#include <sourcemeta/core/mcp.h>
#include <sourcemeta/core/uritemplate.h>

#include <sourcemeta/one/http.h>

#include <cstddef>     // std::size_t
#include <cstdint>     // std::uint8_t
#include <filesystem>  // std::filesystem::path
#include <memory>      // std::unique_ptr, std::make_unique
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
               const std::string_view server_uri)
      : index_directory_{index_directory},
        server_uri_base_path_{server_uri_base_path}, server_uri_{server_uri} {}
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

  enum class Tree : std::uint8_t { Schemas, Explorer };

  [[nodiscard]] auto artifact_resolve_path(std::string_view input, Tree tree,
                                           std::string_view artifact_name) const
      -> std::optional<std::filesystem::path>;

  [[nodiscard]] auto
  artifact_read_json(const std::filesystem::path &absolute_path) const
      -> std::optional<sourcemeta::core::JSON>;

  auto artifact_serve(const std::filesystem::path &absolute_path,
                      const char *code, bool enable_cors, std::string_view mime,
                      std::string_view link, HTTPRequest &request,
                      HTTPResponse &response,
                      std::string_view error_schema) const -> void;

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
  [[nodiscard]] auto blaze_compile(std::string_view schema_uri,
                                   sourcemeta::blaze::Mode mode) const
      -> sourcemeta::blaze::Template;

  // NOLINTNEXTLINE(cppcoreguidelines-avoid-const-or-ref-data-members)
  const std::filesystem::path &index_directory_;
  std::string_view server_uri_base_path_;
  std::string_view server_uri_;
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
             const char *const code, std::string &&identifier,
             std::string &&message) const -> void;

private:
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
};

} // namespace sourcemeta::one

#endif
