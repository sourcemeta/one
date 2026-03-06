#ifndef SOURCEMETA_ONE_WEB_H_
#define SOURCEMETA_ONE_WEB_H_

#include <sourcemeta/one/build.h>
#include <sourcemeta/one/configuration.h>

#include <filesystem> // std::filesystem

namespace sourcemeta::one {

struct GENERATE_WEB_DIRECTORY {
  using Context = Configuration;
  static auto handler(const std::filesystem::path &destination,
                      const sourcemeta::one::Build::Dependencies &dependencies,
                      const sourcemeta::one::Build::DynamicCallback &,
                      const Context &configuration) -> void;
};

struct GENERATE_WEB_NOT_FOUND {
  using Context = Configuration;
  static auto handler(const std::filesystem::path &destination,
                      const sourcemeta::one::Build::Dependencies &,
                      const sourcemeta::one::Build::DynamicCallback &,
                      const Context &configuration) -> void;
};

struct GENERATE_WEB_INDEX {
  using Context = Configuration;
  static auto handler(const std::filesystem::path &destination,
                      const sourcemeta::one::Build::Dependencies &dependencies,
                      const sourcemeta::one::Build::DynamicCallback &,
                      const Context &configuration) -> void;
};

struct GENERATE_WEB_SCHEMA {
  using Context = Configuration;
  static auto handler(const std::filesystem::path &destination,
                      const sourcemeta::one::Build::Dependencies &dependencies,
                      const sourcemeta::one::Build::DynamicCallback &,
                      const Context &configuration) -> void;
};

} // namespace sourcemeta::one

#endif
