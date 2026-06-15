#include <sourcemeta/one/authentication.h>

namespace sourcemeta::one {

Authentication::Authentication(const std::filesystem::path &) {}

auto Authentication::match(const std::string_view) const noexcept
    -> Authentication::PolicySet {
  return 0;
}

auto Authentication::admits(const Authentication::PolicySet,
                            const std::string_view) const noexcept
    -> Authentication::Verdict {
  return {.allowed = true, .key_name = {}};
}

} // namespace sourcemeta::one
