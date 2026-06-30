#include <sourcemeta/one/authentication.h>

#include <sourcemeta/core/io.h>

#include <cstddef> // std::byte, std::size_t
#include <vector>  // std::vector

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

Authentication::Authentication(const std::filesystem::path &,
                               Authentication::JWKSFetcher) {}

Authentication::~Authentication() = default;

auto Authentication::admits(const std::string_view, const std::string_view,
                            const std::string_view) const
    -> Authentication::Verdict {
  return {.allowed = true};
}

auto Authentication::governing(const std::string_view,
                               const std::string_view) const
    -> std::vector<std::size_t> {
  return {};
}

auto Authentication::reference_permitted(const std::string_view,
                                         const std::string_view) const -> bool {
  return true;
}

} // namespace sourcemeta::one
