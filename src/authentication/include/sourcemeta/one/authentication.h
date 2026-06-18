#ifndef SOURCEMETA_ONE_AUTHENTICATION_H
#define SOURCEMETA_ONE_AUTHENTICATION_H

#ifndef SOURCEMETA_ONE_AUTHENTICATION_EXPORT
#include <sourcemeta/one/authentication_export.h>
#endif

#include <cstddef>     // std::size_t
#include <cstdint>     // std::uint8_t
#include <filesystem>  // std::filesystem::path
#include <memory>      // std::unique_ptr
#include <span>        // std::span
#include <string_view> // std::string_view

namespace sourcemeta::one {

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
                   const std::filesystem::path &path) -> void;

  explicit Authentication(const std::filesystem::path &path);
  ~Authentication();

  // The artifact is memory-mapped and owned for the lifetime of the view
  Authentication(const Authentication &) = delete;
  Authentication(Authentication &&) = delete;
  auto operator=(const Authentication &) -> Authentication & = delete;
  auto operator=(Authentication &&) -> Authentication & = delete;

  [[nodiscard]] auto admits(std::string_view registry_path,
                            std::string_view credential) const -> Verdict;

private:
  // The implementation differs by edition and owns the memory-mapped artifact,
  // so it is hidden behind a pointer to keep the binary format out of the
  // shared interface
  struct Impl;
  std::unique_ptr<Impl> impl_;
};

} // namespace sourcemeta::one

#endif
