#ifndef SOURCEMETA_ONE_ACTIONS_H
#define SOURCEMETA_ONE_ACTIONS_H

#include <sourcemeta/core/uritemplate.h>

#include <sourcemeta/one/http.h>

#include <cstddef>     // std::size_t
#include <cstdint>     // std::uint8_t
#include <filesystem>  // std::filesystem::path
#include <memory>      // std::unique_ptr
#include <mutex>       // std::once_flag
#include <optional>    // std::optional
#include <span>        // std::span
#include <string_view> // std::string_view

namespace sourcemeta::one {

#define SOURCEMETA_ONE_FOR_EACH_ACTION(X)                                      \
  X(DEFAULT_V1, ActionDefault_v1)                                              \
  X(HEALTH_CHECK_V1, ActionHealthCheck_v1)                                     \
  X(NOT_FOUND_V1, ActionNotFound_v1)                                           \
  X(SCHEMA_ARTIFACT_V1, ActionServeSchemaArtifact_v1)                          \
  X(EXPLORER_ARTIFACT_V1, ActionServeExplorerArtifact_v1)                      \
  X(JSONSCHEMA_EVALUATE_V1, ActionJSONSchemaEvaluate_v1)                       \
  X(SCHEMA_SEARCH_V1, ActionSchemaSearch_v1)                                   \
  X(SERVE_STATIC_V1, ActionServeStatic_v1)                                     \
  X(SERVE_STATIC_FILE_V1, ActionServeStaticFile_v1)

#define SOURCEMETA_ONE_DEFINE_ACTION_TYPE(Name, Class) ACTION_TYPE_##Name,

enum : std::uint8_t {
  SOURCEMETA_ONE_FOR_EACH_ACTION(SOURCEMETA_ONE_DEFINE_ACTION_TYPE)
      ACTION_TYPE_COUNT
};

#undef SOURCEMETA_ONE_DEFINE_ACTION_TYPE

class Action {
public:
  Action(const std::filesystem::path &base, const std::string_view base_path)
      : base_{base}, base_path_{base_path} {}
  virtual ~Action() = default;

  // To avoid mistakes
  Action(const Action &) = delete;
  Action(Action &&) = delete;
  auto operator=(const Action &) -> Action & = delete;
  auto operator=(Action &&) -> Action & = delete;

  virtual auto run(const std::span<std::string_view> matches,
                   HTTPRequest &request, HTTPResponse &response) -> void = 0;

  [[nodiscard]] auto base() const noexcept -> const std::filesystem::path & {
    return this->base_;
  }

  [[nodiscard]] auto base_path() const noexcept -> std::string_view {
    return this->base_path_;
  }

  [[nodiscard]] auto schema_directory(const std::string_view uri) const
      -> std::optional<std::filesystem::path>;

private:
  // NOLINTNEXTLINE(cppcoreguidelines-avoid-const-or-ref-data-members)
  const std::filesystem::path &base_;
  std::string_view base_path_;
};

class ActionDispatcher {
public:
  ActionDispatcher(const std::filesystem::path &base,
                   const core::URITemplateRouterView &router);
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

  auto error(const HTTPRequest &request, HTTPResponse &response,
             const char *const code, std::string &&identifier,
             std::string &&message) const -> void;

private:
  struct Slot {
    std::unique_ptr<Action> instance;
    std::once_flag flag;
  };

  // NOLINTNEXTLINE(cppcoreguidelines-avoid-const-or-ref-data-members)
  const std::filesystem::path &base_;
  // NOLINTNEXTLINE(cppcoreguidelines-avoid-const-or-ref-data-members)
  const core::URITemplateRouterView &router_;
  // NOLINTNEXTLINE(modernize-avoid-c-arrays)
  std::unique_ptr<Slot[]> slots_;
  std::size_t slots_size_;
  std::string_view default_error_schema_;
};

} // namespace sourcemeta::one

#endif
