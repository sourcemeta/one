#include <sourcemeta/one/authentication.h>

#include <sourcemeta/core/io.h>

#include <chrono>      // std::chrono::sys_seconds
#include <cstddef>     // std::byte, std::size_t
#include <filesystem>  // std::filesystem::path
#include <optional>    // std::optional, std::nullopt
#include <span>        // std::span
#include <string>      // std::string
#include <string_view> // std::string_view
#include <vector>      // std::vector

namespace sourcemeta::one {

// The community edition serves every path publicly. Restricting access by
// policy is an enterprise feature, rejected at index time, so this edition
// never reads the artifact and admits every caller. The artifact is still
// emitted, empty, to keep the build output identical in shape across editions
struct Authentication::Impl {};

auto Authentication::save(
    const std::span<const Authentication::Policy> policies,
    const std::filesystem::path &configuration,
    const std::filesystem::path &destination, const Authentication::PathGuard &)
    -> void {
  if (!policies.empty()) {
    throw EnterpriseOnlyFeatureError(
        configuration,
        "Authentication is only available on the enterprise edition");
  }

  sourcemeta::core::write_file(destination, std::vector<std::byte>{});
}

// NOLINTBEGIN(performance-unnecessary-value-param)
Authentication::Authentication(const std::filesystem::path &,
                               sourcemeta::core::JWKSProvider::Fetcher) {}
// NOLINTEND(performance-unnecessary-value-param)

Authentication::~Authentication() = default;

auto Authentication::admits(const Authentication::Path &,
                            const Credentials &) const
    -> Authentication::Verdict {
  return {.allowed = true, .principal = std::nullopt};
}

auto Authentication::admits_route(const std::string_view,
                                  const std::string_view,
                                  const Credentials &) const
    -> Authentication::Verdict {
  return {.allowed = true, .principal = std::nullopt};
}

auto Authentication::governing(const Authentication::Path &) const
    -> std::vector<std::size_t> {
  return {};
}

auto Authentication::interactive(const std::string_view) const
    -> std::optional<Authentication::InteractivePolicy> {
  return std::nullopt;
}

auto Authentication::client_secret(const std::string_view) const
    -> std::optional<std::string> {
  return std::nullopt;
}

auto Authentication::endpoints(const std::string_view) const
    -> std::optional<Authentication::ProviderEndpoints> {
  return std::nullopt;
}

auto Authentication::open_session(const std::string_view) const
    -> std::optional<std::string> {
  return std::nullopt;
}

auto Authentication::seal(const std::string_view, const Purpose,
                          const std::string_view,
                          const std::chrono::sys_seconds) const
    -> std::optional<std::string> {
  return std::nullopt;
}

auto Authentication::open(const std::string_view, const Purpose,
                          const std::string_view) const
    -> std::optional<std::string> {
  return std::nullopt;
}

auto Authentication::reference_permitted(const Authentication::Path &,
                                         const Authentication::Path &) const
    -> bool {
  return true;
}

// Sessions only arise from interactive authentication, which is an enterprise
// feature, so this edition never produces a sealed value and never accepts one
auto Authentication::seal_value(const std::string_view, const Purpose,
                                const std::string_view,
                                const std::chrono::sys_seconds,
                                const std::chrono::sys_seconds) -> std::string {
  return {};
}

auto Authentication::open_value(const std::string_view, const Purpose,
                                const std::span<const std::string_view>,
                                const std::chrono::sys_seconds)
    -> std::optional<std::string> {
  return std::nullopt;
}

} // namespace sourcemeta::one
