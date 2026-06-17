#include <sourcemeta/one/authentication.h>

#include <sourcemeta/core/io.h>

#include <cstddef> // std::byte
#include <vector>  // std::vector

namespace sourcemeta::one {

// The community edition serves every path publicly. Restricting access by
// policy is an enterprise feature, rejected at index time, so this edition
// never reads the artifact and admits every caller. The artifact is still
// emitted, empty, to keep the build output identical in shape across editions
struct Authentication::Impl {};

auto Authentication::save(const std::span<const Authentication::Policy>,
                          const std::filesystem::path &path) -> void {
  sourcemeta::core::write_file(path, std::vector<std::byte>{});
}

Authentication::Authentication(const std::filesystem::path &) {}

Authentication::~Authentication() = default;

auto Authentication::admits(const std::string_view,
                            const std::string_view) const
    -> Authentication::Verdict {
  return {.allowed = true};
}

} // namespace sourcemeta::one
