#ifndef SOURCEMETA_ONE_AUTHENTICATION_ERROR_H
#define SOURCEMETA_ONE_AUTHENTICATION_ERROR_H

#ifndef SOURCEMETA_ONE_AUTHENTICATION_EXPORT
#include <sourcemeta/one/authentication_export.h>
#endif

#include <exception>  // std::exception
#include <filesystem> // std::filesystem::path
#include <string>     // std::string
#include <utility>    // std::move

namespace sourcemeta::one {

// Raised when a configured feature is only available on the enterprise edition
class SOURCEMETA_ONE_AUTHENTICATION_EXPORT EnterpriseOnlyFeatureError
    : public std::exception {
public:
  EnterpriseOnlyFeatureError(std::filesystem::path path, const char *message)
      : path_{std::move(path)}, message_{message} {}

  [[nodiscard]] auto what() const noexcept -> const char * override {
    return this->message_;
  }

  [[nodiscard]] auto path() const noexcept -> const std::filesystem::path & {
    return this->path_;
  }

private:
  std::filesystem::path path_;
  const char *message_;
};

// Raised when an authentication policy is scoped to a path that is neither a
// declared collection or page (nor a namespace above one) nor a known route
class SOURCEMETA_ONE_AUTHENTICATION_EXPORT AuthenticationUnknownPathError
    : public std::exception {
public:
  AuthenticationUnknownPathError(std::filesystem::path path, std::string scope)
      : path_{std::move(path)}, scope_{std::move(scope)} {}

  [[nodiscard]] auto what() const noexcept -> const char * override {
    return "An authentication policy matches no known collection, page, or "
           "route";
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

} // namespace sourcemeta::one

#endif
