#ifndef SOURCEMETA_ONE_AUTHENTICATION_H
#define SOURCEMETA_ONE_AUTHENTICATION_H

#ifndef SOURCEMETA_ONE_AUTHENTICATION_EXPORT
#include <sourcemeta/one/authentication_export.h>
#endif

#include <cstddef>     // std::size_t
#include <cstdint>     // std::uint64_t
#include <filesystem>  // std::filesystem::path
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

  // The result of validating a caller against a set of policy entries.
  // The key name is for audit logging and is empty for anonymous access
  struct Verdict {
    bool allowed;
    std::string_view key_name;
  };

  explicit Authentication(const std::filesystem::path &path);

  [[nodiscard]] auto match(std::string_view registry_path) const noexcept
      -> PolicySet;

  // The credential is the bearer token presented by the caller, empty for
  // anonymous access. It is borrowed for the duration of the call, so a
  // caller that validates across an asynchronous boundary must own it
  [[nodiscard]] auto admits(PolicySet policies,
                            std::string_view credential) const noexcept
      -> Verdict;
};

} // namespace sourcemeta::one

#endif
