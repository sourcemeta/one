#ifndef SOURCEMETA_ONE_AUTHENTICATION_H
#define SOURCEMETA_ONE_AUTHENTICATION_H

#ifndef SOURCEMETA_ONE_AUTHENTICATION_EXPORT
#include <sourcemeta/one/authentication_export.h>
#endif

#include <sourcemeta/core/io.h>

#include <cstddef>     // std::size_t
#include <cstdint>     // std::uint64_t, std::uint8_t
#include <filesystem>  // std::filesystem::path
#include <memory>      // std::unique_ptr
#include <span>        // std::span
#include <string_view> // std::string_view

namespace sourcemeta::one {

// A view over the compiled authentication policy. An absent policy
// means no authentication is configured: matching yields the empty set
// and validation admits everyone. That is the permanent behaviour of
// unconfigured instances, not a placeholder
class SOURCEMETA_ONE_AUTHENTICATION_EXPORT Authentication {
public:
  static constexpr std::size_t MAXIMUM_POLICIES{64};

  // The kind of access a policy grants. Only anonymous public access
  // exists for now
  enum class Type : std::uint8_t { Public };

  // A single policy as declared in the configuration. It governs every
  // registry path nested under any of its prefixes. The paths are borrowed
  // for the duration of the call to save, so the caller owns their storage
  struct Policy {
    Type type;
    std::span<const std::string_view> paths;
  };

  // The result of validating a caller against the policies that govern a
  // path. The key name is for audit logging and is empty for anonymous access
  struct Verdict {
    bool allowed;
    std::string_view key_name;
  };

  // Compile a set of policies into a memory-mappable artifact at the given
  // path. Entries are assigned identifiers in declaration order, so at most
  // MAXIMUM_POLICIES may be provided
  static auto save(std::span<const Policy> policies,
                   const std::filesystem::path &path) -> void;

  explicit Authentication(const std::filesystem::path &path);
  ~Authentication();

  // The artifact is memory-mapped and owned for the lifetime of the view
  Authentication(const Authentication &) = delete;
  Authentication(Authentication &&) = delete;
  auto operator=(const Authentication &) -> Authentication & = delete;
  auto operator=(Authentication &&) -> Authentication & = delete;

  // Validate a caller against the policies that govern a registry path. The
  // credential is the bearer token presented by the caller, empty for
  // anonymous access. It is borrowed for the duration of the call, so a
  // caller that validates across an asynchronous boundary must own it
  [[nodiscard]] auto admits(std::string_view registry_path,
                            std::string_view credential) const noexcept
      -> Verdict;

private:
  // A set of policy entries, one bit per entry. Entries are assigned
  // monotonically increasing identifiers in configuration declaration
  // order, so a governing set is a single machine word
  using PolicySet = std::uint64_t;

  // The policies that govern a registry path, accumulated from every prefix
  // covering it. An unconfigured instance yields the empty set
  [[nodiscard]] auto match(std::string_view registry_path) const noexcept
      -> PolicySet;

  std::unique_ptr<sourcemeta::core::FileView> view_;
};

} // namespace sourcemeta::one

#endif
