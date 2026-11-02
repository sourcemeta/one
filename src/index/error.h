#ifndef SOURCEMETA_ONE_INDEX_ERROR_H_
#define SOURCEMETA_ONE_INDEX_ERROR_H_

#include <sourcemeta/blaze/output.h>

#include <sourcemeta/core/error.h>
#include <sourcemeta/core/jsonpointer.h>

#include <format>  // std::format
#include <sstream> // std::ostringstream
#include <string>  // std::string

namespace sourcemeta::one {

class MetaschemaError : public sourcemeta::core::Error {
public:
  MetaschemaError(const sourcemeta::blaze::SimpleOutput &output)
      : sourcemeta::core::Error{
            "The schema does not adhere to its metaschema"} {
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

  [[nodiscard]] auto stacktrace() const noexcept -> const std::string & {
    return this->stacktrace_;
  }

private:
  std::string stacktrace_;
};

class OptionInvalidNumericValueError : public sourcemeta::core::Error {
public:
  OptionInvalidNumericValueError(std::string option, std::string value)
      : sourcemeta::core::Error{"Expected a valid numeric value for option"},
        option_{std::move(option)}, value_{std::move(value)} {}

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

class OptionInvalidURIValueError : public sourcemeta::core::Error {
public:
  OptionInvalidURIValueError(std::string option, std::string value)
      : sourcemeta::core::Error{"Expected a valid URI value for option"},
        option_{std::move(option)}, value_{std::move(value)} {}

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

class CrossPolicyReferenceError : public sourcemeta::core::Error {
public:
  CrossPolicyReferenceError(std::filesystem::path path, std::string referrer,
                            std::string referent)
      : sourcemeta::core::Error{"A schema cannot reference a schema behind a "
                                "stricter authentication policy"},
        path_{std::move(path)}, referrer_{std::move(referrer)},
        referent_{std::move(referent)} {}

  [[nodiscard]] auto path() const noexcept -> const std::filesystem::path & {
    return this->path_;
  }

  [[nodiscard]] auto referrer() const noexcept -> const std::string & {
    return this->referrer_;
  }

  [[nodiscard]] auto referent() const noexcept -> const std::string & {
    return this->referent_;
  }

private:
  std::filesystem::path path_;
  std::string referrer_;
  std::string referent_;
};

} // namespace sourcemeta::one

#endif
