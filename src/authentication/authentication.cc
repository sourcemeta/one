#include <sourcemeta/one/authentication.h>

#include <sourcemeta/core/io.h>

#include <cstddef>     // std::byte, std::size_t
#include <filesystem>  // std::filesystem::path
#include <optional>    // std::optional, std::nullopt
#include <span>        // std::span
#include <string>      // std::string
#include <string_view> // std::string_view
#include <utility>     // std::move
#include <vector>      // std::vector

namespace sourcemeta::one {

// The community edition serves every path publicly. Restricting access by
// policy is an enterprise feature, rejected at index time, so this edition
// never reads the artifact and admits every caller. The artifact is still
// emitted, empty, to keep the build output identical in shape across editions
struct Authentication::Table::Impl {};

// This edition serves every path publicly, so the registry looks the same to
// everybody and there is exactly one way to see it. The view is still named
// rather than implied, which keeps the output one shape across editions
auto Authentication::Table::enumerate(
    const std::span<const Authentication::Policy>)
    -> std::vector<Authentication::Table::View> {
  std::vector<Authentication::Table::View> result;
  result.push_back({.name = std::string{VIEW_PUBLIC}, .policies = {}});
  return result;
}

auto Authentication::Table::compile(
    const std::span<const Authentication::Policy> policies,
    const std::filesystem::path &configuration,
    const Authentication::PathGuard &) -> std::vector<std::byte> {
  if (!policies.empty()) {
    throw EnterpriseOnlyFeatureError(
        configuration,
        "Authentication is only available on the enterprise edition");
  }

  return {};
}

auto Authentication::Table::write(const std::span<const std::byte> bytes,
                                  const std::filesystem::path &destination)
    -> void {
  sourcemeta::core::write_file(destination, bytes);
}

Authentication::Table::Table(const std::filesystem::path &) {}

Authentication::Table::Table(const std::span<const std::byte>) {}

Authentication::Table::~Table() = default;

Authentication::Table::Table(Authentication::Table &&) noexcept = default;

auto Authentication::Table::operator=(Authentication::Table &&) noexcept
    -> Authentication::Table & = default;

// NOLINTNEXTLINE(performance-unnecessary-value-param)
Authentication::Authentication(Authentication::Table &&table, Fetcher)
    : table_{std::move(table)} {}

auto Authentication::table() const noexcept -> const Authentication::Table & {
  return this->table_;
}

Authentication::~Authentication() = default;

// This edition signs nobody in, so there is no login to start and no session
// to end, and both answer as the endpoints that reach them already do
auto Authentication::login(const std::string_view, const std::string_view,
                           const std::string_view, const bool,
                           const std::string_view) const
    -> Authentication::Outcome {
  return {.result = Authentication::Outcome::Result::Missing};
}

// This edition signs nobody in, so no marker it could carry names a policy
auto Authentication::renewal(const Authentication::Path &,
                             const Credentials &) const
    -> std::optional<std::string_view> {
  return std::nullopt;
}

auto Authentication::callback(const std::string_view, const std::string_view,
                              const std::string_view,
                              const Authentication::CallbackRequest &,
                              const Credentials &) const
    -> Authentication::Outcome {
  return {.result = Authentication::Outcome::Result::Missing};
}

auto Authentication::logout(const Credentials &, const std::string_view,
                            const std::string_view) const
    -> Authentication::Outcome {
  return {.result = Authentication::Outcome::Result::Missing};
}

// This edition declares no policies, so there is nobody to be other than
// anonymous and every caller is placed the same way
auto Authentication::caller(const Credentials &) const
    -> Authentication::Caller {
  Authentication::Caller result;
  result.view_ = VIEW_PUBLIC;
  return result;
}

// Nothing is governed here, so every location is part of what the one view
// shows
auto Authentication::permits(const Authentication::Path &,
                             const Authentication::Caller &) const -> bool {
  return true;
}

auto Authentication::permits(const RouteTarget &,
                             const Authentication::Caller &,
                             const std::string_view) const -> bool {
  return true;
}

// One way to see the registry means one view, and nothing governs anything, so
// every location is part of what that view shows
auto Authentication::Table::views() const
    -> std::vector<Authentication::RecordedView> {
  return {this->view(VIEW_PUBLIC)};
}

auto Authentication::Table::view(const std::string_view) const
    -> Authentication::RecordedView {
  Authentication::RecordedView result;
  result.name_ = VIEW_PUBLIC;
  return result;
}

auto Authentication::Table::visible(const Authentication::Path &,
                                    const Authentication::RecordedView &) const
    -> bool {
  return true;
}

auto Authentication::Table::governing(const Authentication::Path &) const
    -> std::vector<std::string_view> {
  return {};
}

auto Authentication::Table::reference_permitted(
    const Authentication::Path &, const Authentication::Path &) const -> bool {
  return true;
}

} // namespace sourcemeta::one
