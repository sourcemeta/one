#ifndef SOURCEMETA_ONE_WEB_H_
#define SOURCEMETA_ONE_WEB_H_

#include <sourcemeta/one/build.h>
#include <sourcemeta/one/configuration.h>
#include <sourcemeta/one/resolver.h>

#include <filesystem> // std::filesystem

namespace sourcemeta::one {

struct GENERATE_WEB_DIRECTORY {
  static auto handler(const std::filesystem::path &destination,
                      const sourcemeta::one::BuildDependencies &dependencies,
                      const sourcemeta::one::BuildDynamicCallback &,
                      const sourcemeta::one::Resolver &,
                      const sourcemeta::one::Configuration &configuration)
      -> void;
};

struct GENERATE_WEB_NOT_FOUND {
  static auto handler(const std::filesystem::path &destination,
                      const sourcemeta::one::BuildDependencies &,
                      const sourcemeta::one::BuildDynamicCallback &,
                      const sourcemeta::one::Resolver &,
                      const sourcemeta::one::Configuration &configuration)
      -> void;
};

struct GENERATE_WEB_INDEX {
  static auto handler(const std::filesystem::path &destination,
                      const sourcemeta::one::BuildDependencies &dependencies,
                      const sourcemeta::one::BuildDynamicCallback &,
                      const sourcemeta::one::Resolver &,
                      const sourcemeta::one::Configuration &configuration)
      -> void;
};

struct GENERATE_WEB_SCHEMA {
  static auto handler(const std::filesystem::path &destination,
                      const sourcemeta::one::BuildDependencies &dependencies,
                      const sourcemeta::one::BuildDynamicCallback &,
                      const sourcemeta::one::Resolver &,
                      const sourcemeta::one::Configuration &configuration)
      -> void;
};

} // namespace sourcemeta::one

#endif
