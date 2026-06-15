#include <sourcemeta/one/authentication.h>

#include <sourcemeta/core/http.h> // sourcemeta::core::http_parse_bearer

namespace sourcemeta::one {

auto Authentication::Context::from_header(const std::string_view authorization)
    -> Authentication::Context {
  Context result;
  const auto token{sourcemeta::core::http_parse_bearer(authorization)};
  if (!token.empty()) {
    result.credential_.emplace(token);
  }

  return result;
}

Authentication::Authentication(const std::filesystem::path &) {}

auto Authentication::active() const noexcept -> bool { return false; }

auto Authentication::match(const std::string_view) const noexcept
    -> Authentication::PolicySet {
  return 0;
}

auto Authentication::admits(const Authentication::PolicySet,
                            const Authentication::Context &) const noexcept
    -> Authentication::Verdict {
  return {.allowed = true, .key_name = {}};
}

} // namespace sourcemeta::one
