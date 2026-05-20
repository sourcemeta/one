#ifndef SOURCEMETA_ONE_DISPATCHER_H
#define SOURCEMETA_ONE_DISPATCHER_H

#include <sourcemeta/blaze/compiler.h>
#include <sourcemeta/blaze/evaluator.h>
#include <sourcemeta/core/json.h>
#include <sourcemeta/core/jsonrpc.h>
#include <sourcemeta/core/uritemplate.h>

#include <sourcemeta/one/http.h>

#include <cstddef>     // std::size_t
#include <filesystem>  // std::filesystem::path
#include <memory>      // std::unique_ptr, std::make_unique
#include <mutex>       // std::once_flag
#include <optional>    // std::optional
#include <span>        // std::span
#include <string>      // std::string
#include <string_view> // std::string_view

namespace sourcemeta::one {

class ActionDispatcher;

class Action {
public:
  Action(const std::filesystem::path &base, const std::string_view base_path,
         const std::string_view server_uri)
      : base_{base}, base_path_{base_path}, server_uri_{server_uri} {}
  virtual ~Action() = default;

  // To avoid mistakes
  Action(const Action &) = delete;
  Action(Action &&) = delete;
  auto operator=(const Action &) -> Action & = delete;
  auto operator=(Action &&) -> Action & = delete;

  virtual auto rest(const std::span<std::string_view> matches,
                    HTTPRequest &request, HTTPResponse &response) -> void = 0;

  virtual auto mcp(const sourcemeta::core::JSON &envelope)
      -> sourcemeta::core::JSON {
    const auto *id{sourcemeta::core::jsonrpc_request_id(envelope)};
    return sourcemeta::core::jsonrpc_make_error_method_not_found(
        id ? *id : sourcemeta::core::JSON{nullptr});
  }

  [[nodiscard]] auto base() const noexcept -> const std::filesystem::path & {
    return this->base_;
  }

  [[nodiscard]] auto base_path() const noexcept -> std::string_view {
    return this->base_path_;
  }

  [[nodiscard]] auto server_uri() const noexcept -> std::string_view {
    return this->server_uri_;
  }

  [[nodiscard]] auto schema_directory(const std::string_view uri) const
      -> std::optional<std::filesystem::path>;

  [[nodiscard]] auto uri_to_relative_path(const std::string_view uri) const
      -> std::optional<std::filesystem::path>;

  // Loads a precompiled Blaze template from disk for the given
  // self-served schema URL (e.g. an `rpcSchema` route argument). The
  // mode picks `blaze-fast.metapack` (FastValidation) or
  // `blaze-exhaustive.metapack` (Exhaustive). Asserts the file exists.
  [[nodiscard]] auto blaze_template(std::string_view schema_uri,
                                    sourcemeta::blaze::Mode mode) const
      -> sourcemeta::blaze::Template;

private:
  // NOLINTNEXTLINE(cppcoreguidelines-avoid-const-or-ref-data-members)
  const std::filesystem::path &base_;
  std::string_view base_path_;
  std::string_view server_uri_;
};

// Concrete actions take this as their fourth constructor argument and decide
// whether to store it. Actions that never reach back into the dispatcher (the
// majority) just ignore it; ones that do (e.g. MCP `tools/call`) keep a member
// reference
using ActionConstructor =
    auto (*)(const std::filesystem::path &,
             const sourcemeta::core::URITemplateRouterView &,
             sourcemeta::core::URITemplateRouter::Identifier,
             ActionDispatcher &) -> std::unique_ptr<Action>;

template <typename T>
auto make_action(
    const std::filesystem::path &base,
    const sourcemeta::core::URITemplateRouterView &router,
    const sourcemeta::core::URITemplateRouter::Identifier identifier,
    ActionDispatcher &dispatcher) -> std::unique_ptr<Action> {
  return std::make_unique<T>(base, router, identifier, dispatcher);
}

class ActionDispatcher {
public:
  ActionDispatcher(const std::filesystem::path &base,
                   const core::URITemplateRouterView &router,
                   std::span<const ActionConstructor> constructors,
                   std::span<const std::string_view> descriptions);
  ~ActionDispatcher() = default;

  // To avoid mistakes
  ActionDispatcher(const ActionDispatcher &) = delete;
  ActionDispatcher(ActionDispatcher &&) = delete;
  auto operator=(const ActionDispatcher &) -> ActionDispatcher & = delete;
  auto operator=(ActionDispatcher &&) -> ActionDispatcher & = delete;

  auto dispatch(const core::URITemplateRouter::Identifier identifier,
                const core::URITemplateRouter::Identifier context,
                const std::span<std::string_view> matches, HTTPRequest &request,
                HTTPResponse &response) -> void;

  [[nodiscard]] auto
  action(const core::URITemplateRouter::Identifier identifier,
         const core::URITemplateRouter::Identifier context) -> Action *;

  [[nodiscard]] auto
  action(const core::URITemplateRouter::Identifier identifier) -> Action *;

  auto error(const HTTPRequest &request, HTTPResponse &response,
             const char *const code, std::string &&identifier,
             std::string &&message) const -> void;

  [[nodiscard]] auto
  description(core::URITemplateRouter::Identifier context) const noexcept
      -> std::string_view;

private:
  struct Slot {
    std::unique_ptr<Action> instance;
    std::once_flag flag;
  };

  // NOLINTNEXTLINE(cppcoreguidelines-avoid-const-or-ref-data-members)
  const std::filesystem::path &base_;
  // NOLINTNEXTLINE(cppcoreguidelines-avoid-const-or-ref-data-members)
  const core::URITemplateRouterView &router_;
  std::span<const ActionConstructor> constructors_;
  std::span<const std::string_view> descriptions_;
  // NOLINTNEXTLINE(modernize-avoid-c-arrays)
  std::unique_ptr<Slot[]> slots_;
  std::size_t slots_size_;
  std::string_view default_error_schema_;
};

} // namespace sourcemeta::one

#endif
