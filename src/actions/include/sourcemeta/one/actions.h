#ifndef SOURCEMETA_ONE_ACTIONS_H
#define SOURCEMETA_ONE_ACTIONS_H

#include <sourcemeta/core/uritemplate.h>

#include <sourcemeta/one/http.h>

#include <cstdint>     // std::uint8_t
#include <filesystem>  // std::filesystem::path
#include <optional>    // std::optional
#include <span>        // std::span
#include <string_view> // std::string_view

namespace sourcemeta::one {

#define SOURCEMETA_ONE_FOR_EACH_ACTION(X)                                      \
  X(DEFAULT, ActionDefault)                                                    \
  X(HEALTH_CHECK, ActionHealthCheck)                                           \
  X(NOT_FOUND, ActionNotFound)                                                 \
  X(SCHEMA_ARTIFACT, ActionServeSchemaArtifact)                                \
  X(EXPLORER_ARTIFACT, ActionServeExplorerArtifact)                            \
  X(JSONSCHEMA_EVALUATE, ActionJSONSchemaEvaluate)                             \
  X(SCHEMA_SEARCH, ActionSchemaSearch)                                         \
  X(SERVE_STATIC, ActionServeStatic)

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

auto actions_initialize(const core::URITemplateRouterView &router) -> void;

auto actions_dispatch(const core::URITemplateRouter::Identifier identifier,
                      const core::URITemplateRouter::Identifier context,
                      const core::URITemplateRouterView &router,
                      const std::filesystem::path &base,
                      const std::span<std::string_view> matches,
                      HTTPRequest &request, HTTPResponse &response) -> void;

} // namespace sourcemeta::one

#endif
