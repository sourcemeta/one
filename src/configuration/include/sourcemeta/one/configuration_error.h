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

} // namespace sourcemeta::one

#endif
