#ifndef SOURCEMETA_ONE_SHARED_URI_H_
#define SOURCEMETA_ONE_SHARED_URI_H_

#include <string_view> // std::string_view

namespace sourcemeta::one {

// Whether a redirect target is a safe same-origin local path, so that a login
// return cannot be turned into an open redirect to another origin. Only a
// rooted path is accepted, and the protocol-relative and backslash-escaped
// forms a browser would resolve against a different host are rejected, along
// with the space and control characters that would make the target an invalid
// URI-reference in the Location header
[[nodiscard]] inline auto is_local_path(const std::string_view value) -> bool {
  if (value.empty() || value.front() != '/') {
    return false;
  }
  if (value.size() >= 2 && (value[1] == '/' || value[1] == '\\')) {
    return false;
  }
  for (const auto character : value) {
    const auto code{static_cast<unsigned char>(character)};
    if (code <= 0x20 || code == 0x7f || character == '\\') {
      return false;
    }
  }
  return true;
}

} // namespace sourcemeta::one

#endif
