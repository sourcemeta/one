#include <sourcemeta/one/authentication.h>
#include <sourcemeta/one/configuration.h>

#include <sourcemeta/core/io.h>

#include <chrono>      // std::chrono::sys_seconds
#include <cstddef>     // std::byte, std::size_t
#include <filesystem>  // std::filesystem::create_directories
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

auto Authentication::save(const std::span<const Authentication::Policy>,
                          const std::filesystem::path &,
                          const std::filesystem::path &destination) -> void {
  sourcemeta::core::write_file(destination, std::vector<std::byte>{});
}

auto Authentication::save(const Configuration &configuration,
                          const sourcemeta::core::URITemplateRouterView &,
                          const std::filesystem::path &destination) -> void {
  if (!configuration.authentication.empty()) {
    throw EnterpriseOnlyFeatureError(
        configuration.path,
        "Authentication is only available on the enterprise edition");
  }

  std::filesystem::create_directories(destination.parent_path());
  sourcemeta::core::write_file(destination, std::vector<std::byte>{});
}

// NOLINTBEGIN(performance-unnecessary-value-param)
Authentication::Authentication(const std::filesystem::path &,
                               sourcemeta::core::JWKSProvider::Fetcher) {}
// NOLINTEND(performance-unnecessary-value-param)

Authentication::~Authentication() = default;

auto Authentication::admits(const std::string_view, const std::string_view,
                            const std::string_view,
                            const std::string_view) const
    -> Authentication::Verdict {
  return {.allowed = true, .principal = std::nullopt};
}

auto Authentication::governing(const std::string_view,
                               const std::string_view) const
    -> std::vector<std::size_t> {
  return {};
}

auto Authentication::interactive_challenges(const std::string_view,
                                            const std::string_view) const
    -> std::vector<std::string_view> {
  return {};
}

auto Authentication::interactive(const std::string_view) const
    -> std::optional<Authentication::InteractivePolicy> {
  return std::nullopt;
}

auto Authentication::seal(const std::string_view, const std::string_view,
                          const std::chrono::sys_seconds) const
    -> std::optional<std::string> {
  return std::nullopt;
}

auto Authentication::open(const std::string_view, const std::string_view) const
    -> std::optional<std::string> {
  return std::nullopt;
}

auto Authentication::reference_permitted(const std::string_view,
                                         const std::string_view) const -> bool {
  return true;
}

} // namespace sourcemeta::one
