#include <sourcemeta/one/authentication_session.h>

#include <chrono>      // std::chrono::sys_seconds
#include <optional>    // std::optional, std::nullopt
#include <span>        // std::span
#include <string>      // std::string
#include <string_view> // std::string_view

namespace sourcemeta::one {

// Sessions only arise from interactive authentication, which is an enterprise
// feature, so this edition never produces a sealed value and never accepts one
auto session_seal(const std::string_view, const std::string_view,
                  const std::chrono::sys_seconds) -> std::string {
  return {};
}

auto session_open(const std::string_view,
                  const std::span<const std::string_view>,
                  const std::chrono::sys_seconds)
    -> std::optional<std::string> {
  return std::nullopt;
}

} // namespace sourcemeta::one
