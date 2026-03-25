#ifndef SOURCEMETA_ONE_RESOLVER_ERROR_H_
#define SOURCEMETA_ONE_RESOLVER_ERROR_H_

#include <sourcemeta/core/json.h>

#include <exception>  // std::exception
#include <filesystem> // std::filesystem::path
#include <utility>    // std::move

namespace sourcemeta::one {

class ResolverNotASchemaError : public std::exception {
public:
  ResolverNotASchemaError(std::filesystem::path path)
      : path_{std::move(path)} {}

  [[nodiscard]] auto what() const noexcept -> const char * override {
    return "The file does not contain a valid JSON Schema";
  }

  [[nodiscard]] auto path() const noexcept -> const std::filesystem::path & {
    return this->path_;
  }

private:
  std::filesystem::path path_;
};

class ResolverOutsideBaseError : public std::exception {
public:
  ResolverOutsideBaseError(std::filesystem::path path,
                           sourcemeta::core::JSON::String uri,
                           sourcemeta::core::JSON::String base)
      : path_{std::move(path)}, uri_{std::move(uri)}, base_{std::move(base)} {}

  [[nodiscard]] auto what() const noexcept -> const char * override {
    return "The schema identifier is not relative to the corresponding base";
  }

  [[nodiscard]] auto path() const noexcept -> const std::filesystem::path & {
    return this->path_;
  }

  [[nodiscard]] auto uri() const noexcept
      -> const sourcemeta::core::JSON::String & {
    return this->uri_;
  }

  [[nodiscard]] auto base() const noexcept
      -> const sourcemeta::core::JSON::String & {
    return this->base_;
  }

private:
  std::filesystem::path path_;
  sourcemeta::core::JSON::String uri_;
  sourcemeta::core::JSON::String base_;
};

} // namespace sourcemeta::one

#endif
