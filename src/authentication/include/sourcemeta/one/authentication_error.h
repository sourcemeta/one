#ifndef SOURCEMETA_ONE_AUTHENTICATION_ERROR_H
#define SOURCEMETA_ONE_AUTHENTICATION_ERROR_H

#ifndef SOURCEMETA_ONE_AUTHENTICATION_EXPORT
#include <sourcemeta/one/authentication_export.h>
#endif

#include <cstddef>    // std::size_t
#include <exception>  // std::exception
#include <filesystem> // std::filesystem::path
#include <string>     // std::string
#include <utility>    // std::move

namespace sourcemeta::one {

// Raised when a configured feature is only available on the enterprise edition
class SOURCEMETA_ONE_AUTHENTICATION_EXPORT EnterpriseOnlyFeatureError
    : public std::exception {
public:
  EnterpriseOnlyFeatureError(std::filesystem::path path, std::string message)
      : path_{std::move(path)}, message_{std::move(message)} {}

  [[nodiscard]] auto what() const noexcept -> const char * override {
    return this->message_.c_str();
  }

  [[nodiscard]] auto path() const noexcept -> const std::filesystem::path & {
    return this->path_;
  }

private:
  std::filesystem::path path_;
  std::string message_;
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

// Raised when so many token policies name one issuer that the combinations over
// them cannot be produced. Not a statement about what a build can afford, which
// is known where the views are built rather than where they are named
class SOURCEMETA_ONE_AUTHENTICATION_EXPORT AuthenticationTooManyViewsError
    : public std::exception {
public:
  AuthenticationTooManyViewsError(std::string issuer, const std::size_t count)
      : issuer_{std::move(issuer)}, count_{count} {}

  [[nodiscard]] auto what() const noexcept -> const char * override {
    return "Too many authentication policies share an issuer";
  }

  [[nodiscard]] auto issuer() const noexcept -> const std::string & {
    return this->issuer_;
  }

  [[nodiscard]] auto count() const noexcept -> std::size_t {
    return this->count_;
  }

private:
  std::string issuer_;
  std::size_t count_;
};

// Raised when an authentication policy carries no name, shares one with
// another, or takes the name every caller holding nothing is served under. A
// view is named after the policies it comprises, so any of the three leaves a
// view naming somewhere else or nowhere
class SOURCEMETA_ONE_AUTHENTICATION_EXPORT AuthenticationPolicyNameError
    : public std::exception {
public:
  AuthenticationPolicyNameError(std::filesystem::path path, std::string name)
      : path_{std::move(path)}, name_{std::move(name)} {}

  [[nodiscard]] auto what() const noexcept -> const char * override {
    return "An authentication policy requires a name of its own";
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

// Raised when a policy is named exactly as the view over some combination of
// others is spelled, which would serve two sets of policies from one directory
class SOURCEMETA_ONE_AUTHENTICATION_EXPORT AuthenticationViewNameCollisionError
    : public std::exception {
public:
  AuthenticationViewNameCollisionError(std::filesystem::path path,
                                       std::string name)
      : path_{std::move(path)}, name_{std::move(name)} {}

  [[nodiscard]] auto what() const noexcept -> const char * override {
    return "An authentication policy name collides with a combination of "
           "others";
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

} // namespace sourcemeta::one

#endif
