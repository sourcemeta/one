#ifndef SOURCEMETA_ONE_INDEX_ERROR_H_
#define SOURCEMETA_ONE_INDEX_ERROR_H_

#include <sourcemeta/blaze/output.h>

#include <sourcemeta/core/error.h>
#include <sourcemeta/core/jsonpointer.h>

#include <filesystem> // std::filesystem::path
#include <format>     // std::format
#include <sstream>    // std::ostringstream
#include <string>     // std::string

namespace sourcemeta::one {

// Where every evaluation that did not hold sits, as a block to print under the
// message naming what was being validated
static auto describe_output(const sourcemeta::blaze::SimpleOutput &output)
    -> std::string {
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

  return result;
}

class MetaschemaError : public sourcemeta::core::Error {
public:
  MetaschemaError(const sourcemeta::blaze::SimpleOutput &output)
      : sourcemeta::core::Error{"The schema does not adhere to its metaschema"},
        stacktrace_{describe_output(output)} {}

  [[nodiscard]] auto stacktrace() const noexcept -> const std::string & {
    return this->stacktrace_;
  }

private:
  std::string stacktrace_;
};

// Which of the schemas an OpenAPI description holds is at fault, as one may
// hold hundreds and saying only that one of them is invalid is unusable
class OpenAPISchemaObjectError : public sourcemeta::core::Error {
public:
  OpenAPISchemaObjectError(const sourcemeta::blaze::SimpleOutput &output,
                           std::filesystem::path path,
                           sourcemeta::core::Pointer location,
                           std::string dialect)
      : sourcemeta::core::Error{"The OpenAPI description holds a schema that "
                                "does not adhere to its dialect"},
        path_{std::move(path)}, location_{std::move(location)},
        dialect_{std::move(dialect)}, stacktrace_{describe_output(output)} {}

  [[nodiscard]] auto path() const noexcept -> const std::filesystem::path & {
    return this->path_;
  }

  [[nodiscard]] auto location() const noexcept
      -> const sourcemeta::core::Pointer & {
    return this->location_;
  }

  [[nodiscard]] auto dialect() const noexcept -> const std::string & {
    return this->dialect_;
  }

  [[nodiscard]] auto stacktrace() const noexcept -> const std::string & {
    return this->stacktrace_;
  }

private:
  std::filesystem::path path_;
  sourcemeta::core::Pointer location_;
  std::string dialect_;
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
