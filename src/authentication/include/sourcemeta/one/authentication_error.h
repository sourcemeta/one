#ifndef SOURCEMETA_ONE_AUTHENTICATION_ERROR_H
#define SOURCEMETA_ONE_AUTHENTICATION_ERROR_H

#ifndef SOURCEMETA_ONE_AUTHENTICATION_EXPORT
#include <sourcemeta/one/authentication_export.h>
#endif

#include <sourcemeta/core/error.h>

#include <cstddef>    // std::size_t
#include <filesystem> // std::filesystem::path
#include <string>     // std::string
#include <utility>    // std::move

namespace sourcemeta::one {

SOURCEMETA_CORE_ERROR_WARNINGS_PUSH

// Raised when a configured feature is only available on the enterprise edition
class SOURCEMETA_ONE_AUTHENTICATION_EXPORT EnterpriseOnlyFeatureError
    : public sourcemeta::core::Error {
public:
  EnterpriseOnlyFeatureError(std::filesystem::path path,
                             const sourcemeta::core::StaticString message)
      : sourcemeta::core::Error{message}, path_{std::move(path)} {}

  [[nodiscard]] auto path() const noexcept -> const std::filesystem::path & {
    return this->path_;
  }

private:
  std::filesystem::path path_;
};

// Raised when an authentication policy is scoped to a path that is neither a
// declared collection or page (nor a namespace above one) nor a known route
class SOURCEMETA_ONE_AUTHENTICATION_EXPORT AuthenticationUnknownPathError
    : public sourcemeta::core::Error {
public:
  AuthenticationUnknownPathError(std::filesystem::path path, std::string scope)
      : sourcemeta::core::Error{"An authentication policy matches no known "
                                "collection, page, or route"},
        path_{std::move(path)}, scope_{std::move(scope)} {}

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
    : public sourcemeta::core::Error {
public:
  AuthenticationTooManyViewsError(std::string issuer, const std::size_t count)
      : sourcemeta::core::
            Error{"Too many authentication policies share an issuer"},
        issuer_{std::move(issuer)}, count_{count} {}

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
    : public sourcemeta::core::Error {
public:
  AuthenticationPolicyNameError(std::filesystem::path path, std::string name)
      : sourcemeta::core::
            Error{"An authentication policy requires a name of its own"},
        path_{std::move(path)}, name_{std::move(name)} {}

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

// Raised when more policies are declared than one policy set has bits to name,
// since each occupies one of them
class SOURCEMETA_ONE_AUTHENTICATION_EXPORT AuthenticationTooManyPoliciesError
    : public sourcemeta::core::Error {
public:
  AuthenticationTooManyPoliciesError(std::filesystem::path path,
                                     const std::size_t count)
      : sourcemeta::core::Error{"Too many authentication policies"},
        path_{std::move(path)}, count_{count} {}

  [[nodiscard]] auto path() const noexcept -> const std::filesystem::path & {
    return this->path_;
  }

  [[nodiscard]] auto count() const noexcept -> std::size_t {
    return this->count_;
  }

private:
  std::filesystem::path path_;
  std::size_t count_;
};

// Raised when a policy that signs people in names no secret to seal their
// session with, which could neither mint one nor read one back
class SOURCEMETA_ONE_AUTHENTICATION_EXPORT AuthenticationMissingSecretError
    : public sourcemeta::core::Error {
public:
  AuthenticationMissingSecretError(std::filesystem::path path, std::string name)
      : sourcemeta::core::Error{"An interactive authentication policy requires "
                                "a session secret"},
        path_{std::move(path)}, name_{std::move(name)} {}

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
    : public sourcemeta::core::Error {
public:
  AuthenticationViewNameCollisionError(std::filesystem::path path,
                                       std::string name)
      : sourcemeta::core::Error{"An authentication policy name collides with a "
                                "combination of others"},
        path_{std::move(path)}, name_{std::move(name)} {}

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

SOURCEMETA_CORE_ERROR_WARNINGS_POP

} // namespace sourcemeta::one

#endif
