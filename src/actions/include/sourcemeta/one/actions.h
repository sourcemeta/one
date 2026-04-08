#ifndef SOURCEMETA_ONE_ACTIONS_H
#define SOURCEMETA_ONE_ACTIONS_H

#include <sourcemeta/core/uritemplate.h>

#include <sourcemeta/one/http.h>

#include <filesystem>  // std::filesystem::path
#include <optional>    // std::optional
#include <span>        // std::span
#include <string_view> // std::string_view

namespace sourcemeta::one {

constexpr auto HANDLER_SELF_V1_API_LIST = 1;
constexpr auto HANDLER_SELF_V1_API_LIST_PATH = 2;
constexpr auto HANDLER_SELF_V1_API_SCHEMAS_DEPENDENCIES = 3;
constexpr auto HANDLER_SELF_V1_API_SCHEMAS_DEPENDENTS = 4;
constexpr auto HANDLER_SELF_V1_API_SCHEMAS_HEALTH = 5;
constexpr auto HANDLER_SELF_V1_API_SCHEMAS_LOCATIONS = 6;
constexpr auto HANDLER_SELF_V1_API_SCHEMAS_POSITIONS = 7;
constexpr auto HANDLER_SELF_V1_API_SCHEMAS_STATS = 8;
constexpr auto HANDLER_SELF_V1_API_SCHEMAS_METADATA = 9;
constexpr auto HANDLER_SELF_V1_API_SCHEMAS_EVALUATE = 10;
constexpr auto HANDLER_SELF_V1_API_SCHEMAS_TRACE = 11;
constexpr auto HANDLER_SELF_V1_API_SCHEMAS_SEARCH = 12;
constexpr auto HANDLER_SELF_V1_API_DEFAULT = 13;
constexpr auto HANDLER_SELF_STATIC = 14;
constexpr auto HANDLER_SELF_V1_HEALTH = 15;

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

auto dispatch_action(const core::URITemplateRouter::Identifier identifier,
                     const core::URITemplateRouterView &router,
                     const std::filesystem::path &base,
                     const std::span<std::string_view> matches,
                     HTTPRequest &request, HTTPResponse &response) -> void;

} // namespace sourcemeta::one

#endif
