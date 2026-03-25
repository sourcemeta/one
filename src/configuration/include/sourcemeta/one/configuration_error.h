#ifndef SOURCEMETA_ONE_CONFIGURATION_ERROR_H_
#define SOURCEMETA_ONE_CONFIGURATION_ERROR_H_

#include <sourcemeta/blaze/output.h>

#include <sourcemeta/core/jsonpointer.h>

#include <exception>  // std::exception
#include <filesystem> // std::filesystem::path
#include <sstream>    // std::ostringstream
#include <string>     // std::string
#include <utility>    // std::move

namespace sourcemeta::one {

class ConfigurationValidationError : public std::exception {
public:
  ConfigurationValidationError(std::filesystem::path path,
                               const sourcemeta::blaze::SimpleOutput &output)
      : path_{std::move(path)} {
    std::ostringstream stream;
    for (const auto &entry : output) {
      stream << entry.message << "\n";
      stream << "  at instance location \"";
      sourcemeta::core::stringify(entry.instance_location, stream);
      stream << "\"\n";
      stream << "  at evaluate path \"";
      sourcemeta::core::stringify(entry.evaluate_path, stream);
      stream << "\"\n";
    }
    this->stacktrace_ = stream.str();
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

class ConfigurationUnknownBuiltInCollectionError : public std::exception {
public:
  ConfigurationUnknownBuiltInCollectionError(std::filesystem::path from,
                                             sourcemeta::core::Pointer location,
                                             std::string identifier)
      : from_{std::move(from)}, location_{std::move(location)},
        identifier_{std::move(identifier)} {}

  [[nodiscard]] auto what() const noexcept -> const char * override {
    return "Could not locate built-in collection";
  }

  [[nodiscard]] auto from() const noexcept -> const auto & {
    return this->from_;
  }

  [[nodiscard]] auto identifier() const noexcept -> const auto & {
    return this->identifier_;
  }

  [[nodiscard]] auto location() const noexcept -> const auto & {
    return this->location_;
  }

private:
  std::filesystem::path from_;
  sourcemeta::core::Pointer location_;
  std::string identifier_;
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

} // namespace sourcemeta::one

#endif
