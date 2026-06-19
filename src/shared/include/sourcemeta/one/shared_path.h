#ifndef SOURCEMETA_ONE_SHARED_PATH_H_
#define SOURCEMETA_ONE_SHARED_PATH_H_

#include <cstddef>     // std::size_t
#include <string_view> // std::string_view

namespace sourcemeta::one {

// TODO: Upstream these slash-delimited logical path helpers to sourcemeta::core
// alongside is_under_path, as a non-filesystem variant that tolerates leading,
// trailing, and repeated slashes

// Advance the cursor to the next non-empty path segment and return it. The
// returned view is empty once the path is exhausted. Leading, trailing, and
// repeated slashes are ignored, so "/a/b", "a/b", and "a/b/" all yield the
// segments "a" then "b"
inline auto path_next_segment(const std::string_view path,
                              std::size_t &cursor) noexcept
    -> std::string_view {
  while (cursor < path.size() && path[cursor] == '/') {
    cursor += 1;
  }

  if (cursor >= path.size()) {
    return {};
  }

  const auto start{cursor};
  while (cursor < path.size() && path[cursor] != '/') {
    cursor += 1;
  }

  return {path.data() + start, cursor - start};
}

// Whether the first path equals the second or is one of its parent segments, so
// that a policy on the first governs everything under the second. The
// comparison is segment by segment, so trailing or repeated slashes do not
// change the outcome
inline auto path_covers(const std::string_view ancestor,
                        const std::string_view descendant) noexcept -> bool {
  std::size_t ancestor_cursor{0};
  std::size_t descendant_cursor{0};
  while (true) {
    const auto ancestor_segment{path_next_segment(ancestor, ancestor_cursor)};
    if (ancestor_segment.empty()) {
      return true;
    }

    if (path_next_segment(descendant, descendant_cursor) != ancestor_segment) {
      return false;
    }
  }
}

} // namespace sourcemeta::one

#endif
