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
// route served by the registry
class SOURCEMETA_ONE_AUTHENTICATION_EXPORT AuthenticationUnknownPathError
    : public std::exception {
public:
  AuthenticationUnknownPathError(std::filesystem::path path, std::string scope)
      : path_{std::move(path)}, scope_{std::move(scope)} {}

  [[nodiscard]] auto what() const noexcept -> const char * override {
    return "An authentication policy matches no known route";
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

// Raised when two authentication policies share a name
class SOURCEMETA_ONE_AUTHENTICATION_EXPORT AuthenticationDuplicateNameError
    : public std::exception {
public:
  AuthenticationDuplicateNameError(std::filesystem::path path, std::string name)
      : path_{std::move(path)}, name_{std::move(name)} {}

  [[nodiscard]] auto what() const noexcept -> const char * override {
    return "An authentication policy name is used more than once";
  }

  [[nodiscard]] auto path() const noexcept -> const std::filesystem::path & {
    return this->path_;
  }

  [[nodiscard]] auto name() const noexcept -> const std::string & {
    return this->name_;
  }

private:
  std::filesystem::path path_;
  std::string name_;
};

// Raised when an authentication policy takes a name reserved for a policy type
class SOURCEMETA_ONE_AUTHENTICATION_EXPORT AuthenticationReservedNameError
    : public std::exception {
public:
  AuthenticationReservedNameError(std::filesystem::path path, std::string name)
      : path_{std::move(path)}, name_{std::move(name)} {}

  [[nodiscard]] auto what() const noexcept -> const char * override {
    return "An authentication policy may not use a reserved name";
  }

  [[nodiscard]] auto path() const noexcept -> const std::filesystem::path & {
    return this->path_;
  }

  [[nodiscard]] auto name() const noexcept -> const std::string & {
    return this->name_;
  }

private:
  std::filesystem::path path_;
  std::string name_;
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

  // A non-empty base path is stripped off the input before matching, turning a
  // request URL path into a registry path
  [[nodiscard]] auto admits(std::string_view registry_path,
                            std::string_view credential,
                            std::string_view base_path = {}) const -> Verdict;

  [[nodiscard]] auto reference_permitted(std::string_view referrer_path,
                                         std::string_view referent_path) const
      -> bool;

private:
  // The implementation differs by edition and owns the memory-mapped artifact,
  // so it is hidden behind a pointer to keep the binary format out of the
  // shared interface
  struct Impl;
  std::unique_ptr<Impl> impl_;
};

} // namespace sourcemeta::one

#endif
