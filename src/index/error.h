#ifndef SOURCEMETA_ONE_INDEX_ERROR_H_
#define SOURCEMETA_ONE_INDEX_ERROR_H_

#include <sourcemeta/blaze/output.h>

#include <sourcemeta/core/jsonpointer.h>

#include <exception> // std::exception
#include <sstream>   // std::ostringstream
#include <string>    // std::string

namespace sourcemeta::one {

class MetaschemaError : public std::exception {
public:
  MetaschemaError(const sourcemeta::blaze::SimpleOutput &output) {
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
    return "The schema does not adhere to its metaschema";
  }

  [[nodiscard]] auto stacktrace() const noexcept -> const std::string & {
    return this->stacktrace_;
  }

private:
  std::string stacktrace_;
};

class CustomRuleError : public std::exception {
public:
  [[nodiscard]] auto what() const noexcept -> const char * override {
    return "Custom linter rules are only available on the enterprise "
           "edition";
  }
};

} // namespace sourcemeta::one

#endif
