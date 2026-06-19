#ifndef SOURCEMETA_ONE_AUTHENTICATION_H
#define SOURCEMETA_ONE_AUTHENTICATION_H

#ifndef SOURCEMETA_ONE_AUTHENTICATION_EXPORT
#include <sourcemeta/one/authentication_export.h>
#endif

#include <cstddef>     // std::size_t
#include <cstdint>     // std::uint8_t
#include <exception>   // std::exception
#include <filesystem>  // std::filesystem::path
#include <memory>      // std::unique_ptr
#include <span>        // std::span
#include <string>      // std::string
#include <string_view> // std::string_view
#include <utility>     // std::move

namespace sourcemeta::one {

// Raised when saving a policy set in which an apiKey policy can never deny
// anyone because a public policy already covers its entire scope
class SOURCEMETA_ONE_AUTHENTICATION_EXPORT AuthenticationShadowedError
    : public std::exception {
public:
  AuthenticationShadowedError(std::filesystem::path path, std::string scope,
                              std::string shadow)
      : path_{std::move(path)}, scope_{std::move(scope)},
        shadow_{std::move(shadow)} {}

  [[nodiscard]] auto what() const noexcept -> const char * override {
    return "An apiKey authentication policy is shadowed by a public policy";
  }

  [[nodiscard]] auto path() const noexcept -> const std::filesystem::path & {
    return this->path_;
  }

  [[nodiscard]] auto scope() const noexcept -> const std::string & {
    return this->scope_;
  }

  [[nodiscard]] auto shadow() const noexcept -> const std::string & {
    return this->shadow_;
  }

private:
  std::filesystem::path path_;
  std::string scope_;
  std::string shadow_;
};

// Raised when an authentication policy is scoped to a path that matches no
// declared collection or page
class SOURCEMETA_ONE_AUTHENTICATION_EXPORT AuthenticationUnknownPathError
    : public std::exception {
public:
  AuthenticationUnknownPathError(std::filesystem::path path, std::string scope)
      : path_{std::move(path)}, scope_{std::move(scope)} {}

  [[nodiscard]] auto what() const noexcept -> const char * override {
    return "An authentication policy path matches no collection or page";
  }

  [[nodiscard]] auto path() const noexcept -> const std::filesystem::path & {
    return this->path_;
  }

  [[nodiscard]] auto scope() const noexcept -> const std::string & {
    return this->scope_;
  }

private:
  std::filesystem::path path_;
  std::string scope_;
};

class SOURCEMETA_ONE_AUTHENTICATION_EXPORT Authentication {
public:
  static constexpr std::size_t MAXIMUM_POLICIES{64};

  enum class Type : std::uint8_t { Public, ApiKey };

  struct Policy {
    Type type;
    std::span<const std::string_view> paths;
    std::span<const std::string_view> keys{};
  };

  struct Verdict {
    bool allowed;
  };

  static auto save(std::span<const Policy> policies,
                   const std::filesystem::path &configuration,
                   const std::filesystem::path &destination) -> void;

  explicit Authentication(const std::filesystem::path &path);
  ~Authentication();

  // The artifact is memory-mapped and owned for the lifetime of the view
  Authentication(const Authentication &) = delete;
  Authentication(Authentication &&) = delete;
  auto operator=(const Authentication &) -> Authentication & = delete;
  auto operator=(Authentication &&) -> Authentication & = delete;

  [[nodiscard]] auto admits(std::string_view registry_path,
                            std::string_view credential) const -> Verdict;

  [[nodiscard]] auto reference_permitted(std::string_view referrer_path,
                                         std::string_view referent_path) const
      -> bool;

  // Whether the first path equals the second or is one of its parent segments,
  // so that a policy on the first governs everything under the second. The
  // comparison is segment by segment, matching how the policy trie is built, so
  // trailing or repeated slashes do not change the outcome
  [[nodiscard]] static auto
  path_covers(const std::string_view ancestor,
              const std::string_view descendant) noexcept -> bool {
    std::size_t ancestor_cursor{0};
    std::size_t descendant_cursor{0};
    while (true) {
      const auto ancestor_segment{next_segment(ancestor, ancestor_cursor)};
      if (ancestor_segment.empty()) {
        return true;
      }

      if (next_segment(descendant, descendant_cursor) != ancestor_segment) {
        return false;
      }
    }
  }

private:
  // Advance the cursor to the next non-empty path segment and return it. The
  // returned view is empty once the path is exhausted. Leading, trailing, and
  // repeated slashes are ignored, so "/a/b", "a/b", and "a/b/" all yield the
  // segments "a" then "b"
  [[nodiscard]] static auto next_segment(const std::string_view path,
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

  // The implementation differs by edition and owns the memory-mapped artifact,
  // so it is hidden behind a pointer to keep the binary format out of the
  // shared interface
  struct Impl;
  std::unique_ptr<Impl> impl_;
};

} // namespace sourcemeta::one

#endif
