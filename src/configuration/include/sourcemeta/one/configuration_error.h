#ifndef SOURCEMETA_ONE_CONFIGURATION_ERROR_H_
#define SOURCEMETA_ONE_CONFIGURATION_ERROR_H_

#include <sourcemeta/blaze/output.h>

#include <sourcemeta/core/jsonpointer.h>

#include <exception>  // std::exception
#include <filesystem> // std::filesystem::path
#include <format>     // std::format
#include <sstream>    // std::ostringstream
#include <string>     // std::string
#include <utility>    // std::move

namespace sourcemeta::one {

class ConfigurationValidationError : public std::exception {
public:
  ConfigurationValidationError(std::filesystem::path path,
                               const sourcemeta::blaze::SimpleOutput &output)
      : path_{std::move(path)} {
    std::string result;
    for (const auto &entry : output) {
      std::ostringstream instance_location_stream;
      sourcemeta::core::stringify(entry.instance_location,
                                  instance_location_stream);
      std::ostringstream evaluate_path_stream;
      sourcemeta::core::stringify(entry.evaluate_path, evaluate_path_stream);
      result += std::format("{}\n  at instance location \"{}\"\n"
                            "  at evaluate path \"{}\"\n",
                            entry.message, instance_location_stream.str(),
                            evaluate_path_stream.str());
    }
    this->stacktrace_ = std::move(result);
  }

  [[nodiscard]] auto what() const noexcept -> const char * override {
    return "Invalid configuration";
  }

  [[nodiscard]] auto path() const noexcept -> const std::filesystem::path & {
    return this->path_;
  }

  [[nodiscard]] auto stacktrace() const noexcept -> const auto & {
    return this->stacktrace_;
  }

private:
  std::filesystem::path path_;
  std::string stacktrace_;
};

class ConfigurationReadError : public std::exception {
public:
  ConfigurationReadError(std::filesystem::path from,
                         sourcemeta::core::Pointer location,
                         std::filesystem::path target)
      : from_{std::move(from)}, location_{std::move(location)},
        target_{std::move(target)} {}

  [[nodiscard]] auto what() const noexcept -> const char * override {
    return "Could not read referenced file";
  }

  [[nodiscard]] auto from() const noexcept -> const auto & {
    return this->from_;
  }

  [[nodiscard]] auto target() const noexcept -> const auto & {
    return this->target_;
  }

  [[nodiscard]] auto location() const noexcept -> const auto & {
    return this->location_;
  }

private:
  std::filesystem::path from_;
  sourcemeta::core::Pointer location_;
  std::filesystem::path target_;
};

class ConfigurationDuplicateAuthenticationNameError : public std::exception {
public:
  ConfigurationDuplicateAuthenticationNameError(std::filesystem::path path,
                                                std::string name)
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

class ConfigurationSharedAuthenticationKeyError : public std::exception {
public:
  ConfigurationSharedAuthenticationKeyError(std::filesystem::path path,
                                            std::string name,
                                            std::string variable)
      : path_{std::move(path)}, name_{std::move(name)},
        variable_{std::move(variable)} {}

  [[nodiscard]] auto what() const noexcept -> const char * override {
    return "An authentication policy key is used more than once";
  }

  [[nodiscard]] auto path() const noexcept -> const std::filesystem::path & {
    return this->path_;
  }

  [[nodiscard]] auto name() const noexcept -> const std::string & {
    return this->name_;
  }

  [[nodiscard]] auto variable() const noexcept -> const std::string & {
    return this->variable_;
  }

private:
  std::filesystem::path path_;
  std::string name_;
  std::string variable_;
};

class ConfigurationReservedAuthenticationNameError : public std::exception {
public:
  ConfigurationReservedAuthenticationNameError(std::filesystem::path path,
                                               std::string name)
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

class ConfigurationCyclicReferenceError : public std::exception {
public:
  ConfigurationCyclicReferenceError(std::filesystem::path from,
                                    sourcemeta::core::Pointer location,
                                    std::filesystem::path target)
      : from_{std::move(from)}, location_{std::move(location)},
        target_{std::move(target)} {}

  [[nodiscard]] auto what() const noexcept -> const char * override {
    return "Circular reference detected in configuration";
  }

  [[nodiscard]] auto from() const noexcept -> const std::filesystem::path & {
    return this->from_;
  }

  [[nodiscard]] auto target() const noexcept -> const std::filesystem::path & {
    return this->target_;
  }

  [[nodiscard]] auto location() const noexcept
      -> const sourcemeta::core::Pointer & {
    return this->location_;
  }

private:
  std::filesystem::path from_;
  sourcemeta::core::Pointer location_;
  std::filesystem::path target_;
};

// Raised when an interactive authentication policy is declared on an instance
// a browser would not treat as a trustworthy origin, which means https, or
// plain HTTP on a loopback address or the special-use localhost name
class ConfigurationInsecureAuthenticationURLError : public std::exception {
public:
  ConfigurationInsecureAuthenticationURLError(std::filesystem::path path,
                                              std::string name, std::string url)
      : path_{std::move(path)}, name_{std::move(name)}, url_{std::move(url)} {}

  [[nodiscard]] auto what() const noexcept -> const char * override {
    return "An interactive authentication policy requires an https instance, "
           "or a loopback one for local development";
  }

  [[nodiscard]] auto path() const noexcept -> const std::filesystem::path & {
    return this->path_;
  }

  [[nodiscard]] auto name() const noexcept -> const std::string & {
    return this->name_;
  }

  [[nodiscard]] auto url() const noexcept -> const std::string & {
    return this->url_;
  }

private:
  std::filesystem::path path_;
  std::string name_;
  std::string url_;
};

// Raised when the instance URL names anything below the origin. A well-known
// URI is rooted at the top of a path hierarchy and is not well-known anywhere
// else (RFC 8615 Section 3), so an instance mounted under a path cannot serve
// the locations standards derive from its own identifiers, and cannot take
// part in any mechanism built on them
class ConfigurationInstancePathError : public std::exception {
public:
  ConfigurationInstancePathError(std::filesystem::path path, std::string url)
      : path_{std::move(path)}, url_{std::move(url)} {}

  [[nodiscard]] auto what() const noexcept -> const char * override {
    return "The instance URL must contain no path";
  }

  [[nodiscard]] auto path() const noexcept -> const std::filesystem::path & {
    return this->path_;
  }

  [[nodiscard]] auto url() const noexcept -> const std::string & {
    return this->url_;
  }

private:
  std::filesystem::path path_;
  std::string url_;
};

// Raised when the instance URL carries a component that identifies something
// other than where the instance is served. Every schema identifier is composed
// against this URL, so a query or a fragment would travel into identifiers
// that name no resource, and credentials would travel into every identifier
// the instance publishes
class ConfigurationInstanceOriginError : public std::exception {
public:
  ConfigurationInstanceOriginError(std::filesystem::path path, std::string url)
      : path_{std::move(path)}, url_{std::move(url)} {}

  [[nodiscard]] auto what() const noexcept -> const char * override {
    return "The instance URL must contain no credentials, query, or fragment";
  }

  [[nodiscard]] auto path() const noexcept -> const std::filesystem::path & {
    return this->path_;
  }

  [[nodiscard]] auto url() const noexcept -> const std::string & {
    return this->url_;
  }

private:
  std::filesystem::path path_;
  std::string url_;
};

// Raised when an authentication policy names an issuer that this instance
// could never complete a discovery exchange against
class ConfigurationInvalidAuthenticationIssuerError : public std::exception {
public:
  ConfigurationInvalidAuthenticationIssuerError(std::filesystem::path path,
                                                std::string name,
                                                std::string issuer)
      : path_{std::move(path)}, name_{std::move(name)},
        issuer_{std::move(issuer)} {}

  [[nodiscard]] auto what() const noexcept -> const char * override {
    return "An authentication policy issuer must be an https URL";
  }

  [[nodiscard]] auto path() const noexcept -> const std::filesystem::path & {
    return this->path_;
  }

  [[nodiscard]] auto name() const noexcept -> const std::string & {
    return this->name_;
  }

  [[nodiscard]] auto issuer() const noexcept -> const std::string & {
    return this->issuer_;
  }

private:
  std::filesystem::path path_;
  std::string name_;
  std::string issuer_;
};

} // namespace sourcemeta::one

#endif
