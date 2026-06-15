#ifndef SOURCEMETA_ONE_AUTHENTICATION_H
#define SOURCEMETA_ONE_AUTHENTICATION_H

#ifndef SOURCEMETA_ONE_AUTHENTICATION_EXPORT
#include <sourcemeta/one/authentication_export.h>
#endif

#include <cstddef>     // std::size_t
#include <cstdint>     // std::uint64_t
#include <filesystem>  // std::filesystem::path
#include <optional>    // std::optional
#include <string>      // std::string
#include <string_view> // std::string_view

namespace sourcemeta::one {

// A view over the compiled authentication policy. An absent policy
// means no authentication is configured: matching yields the empty set
// and validation admits everyone. That is the permanent behaviour of
// unconfigured instances, not a placeholder
class SOURCEMETA_ONE_AUTHENTICATION_EXPORT Authentication {
public:
  // A set of policy entries, one bit per entry. Entries are assigned
  // monotonically increasing identifiers in configuration declaration
  // order, so a governing set is a single machine word
  using PolicySet = std::uint64_t;

  static constexpr std::size_t MAXIMUM_POLICIES{64};

  class Context {
  public:
    Context() = default;

    [[nodiscard]] static auto from_header(std::string_view authorization)
        -> Context;

    [[nodiscard]] auto credential() const noexcept
        -> const std::optional<std::string> & {
      return this->credential_;
    }

  private:
    std::optional<std::string> credential_;
  };

  // The result of validating a caller against a set of policy entries.
  // The key name is for audit logging and is empty for anonymous access
  struct Verdict {
    bool allowed;
    std::string_view key_name;
  };

  explicit Authentication(const std::filesystem::path &path);

  [[nodiscard]] auto active() const noexcept -> bool;

  [[nodiscard]] auto match(std::string_view registry_path) const noexcept
      -> PolicySet;

  [[nodiscard]] auto admits(PolicySet policies,
                            const Context &context) const noexcept -> Verdict;
};

} // namespace sourcemeta::one

#endif
