#ifndef SOURCEMETA_ONE_INDEX_ERROR_H_
#define SOURCEMETA_ONE_INDEX_ERROR_H_

#include <sourcemeta/blaze/output.h>

#include <sourcemeta/core/jsonpointer.h>

#include <exception> // std::exception
#include <format>    // std::format
#include <sstream>   // std::ostringstream
#include <string>    // std::string

namespace sourcemeta::one {

class MetaschemaError : public std::exception {
public:
  MetaschemaError(const sourcemeta::blaze::SimpleOutput &output) {
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
    return "The schema does not adhere to its metaschema";
  }

  [[nodiscard]] auto stacktrace() const noexcept -> const std::string & {
    return this->stacktrace_;
  }

private:
  std::string stacktrace_;
};

class OptionInvalidNumericValueError : public std::exception {
public:
  OptionInvalidNumericValueError(std::string option, std::string value)
      : option_{std::move(option)}, value_{std::move(value)} {}

  [[nodiscard]] auto what() const noexcept -> const char * override {
    return "Expected a valid numeric value for option";
  }

  [[nodiscard]] auto option() const noexcept -> const std::string & {
    return this->option_;
  }

  [[nodiscard]] auto value() const noexcept -> const std::string & {
    return this->value_;
  }

private:
  std::string option_;
  std::string value_;
};

class OptionInvalidURIValueError : public std::exception {
public:
  OptionInvalidURIValueError(std::string option, std::string value)
      : option_{std::move(option)}, value_{std::move(value)} {}

  [[nodiscard]] auto what() const noexcept -> const char * override {
    return "Expected a valid URI value for option";
  }

  [[nodiscard]] auto option() const noexcept -> const std::string & {
    return this->option_;
  }

  [[nodiscard]] auto value() const noexcept -> const std::string & {
    return this->value_;
  }

private:
  std::string option_;
  std::string value_;
};

class CustomRuleError : public std::exception {
public:
  CustomRuleError(std::filesystem::path path) : path_{std::move(path)} {}

  [[nodiscard]] auto what() const noexcept -> const char * override {
    return "Custom linter rules are only available on the enterprise "
           "edition";
  }

  [[nodiscard]] auto path() const noexcept -> const std::filesystem::path & {
    return this->path_;
  }

private:
  std::filesystem::path path_;
};

class DocumentationNotSupportedError : public std::exception {
public:
  DocumentationNotSupportedError(std::filesystem::path path)
      : path_{std::move(path)} {}

  [[nodiscard]] auto what() const noexcept -> const char * override {
    return "Collection documentation is only available on the enterprise "
           "edition";
  }

  [[nodiscard]] auto path() const noexcept -> const std::filesystem::path & {
    return this->path_;
  }

private:
  std::filesystem::path path_;
};

class StaticFileTooLargeError : public std::exception {
public:
  StaticFileTooLargeError(std::filesystem::path path, std::size_t size)
      : path_{std::move(path)}, size_{size} {}

  [[nodiscard]] auto what() const noexcept -> const char * override {
    return "Static file exceeds the maximum allowed size";
  }

  [[nodiscard]] auto path() const noexcept -> const std::filesystem::path & {
    return this->path_;
  }

  [[nodiscard]] auto size() const noexcept -> std::size_t {
    return this->size_;
  }

private:
  std::filesystem::path path_;
  std::size_t size_;
};

} // namespace sourcemeta::one

#endif
