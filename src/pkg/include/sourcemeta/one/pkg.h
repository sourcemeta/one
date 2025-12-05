#ifndef SOURCEMETA_ONE_PKG_H_
#define SOURCEMETA_ONE_PKG_H_

#include <sourcemeta/core/build.h>
#include <sourcemeta/one/configuration.h>
#include <sourcemeta/one/resolver.h>

#include <filesystem> // std::filesystem

namespace sourcemeta::one {

struct GENERATE_PKG_EXTRACT_JSON {
  using Context = sourcemeta::one::Resolver;
  static auto
  handler(const std::filesystem::path &destination,
          const sourcemeta::core::BuildDependencies<std::filesystem::path>
              &dependencies,
          const sourcemeta::core::BuildDynamicCallback<std::filesystem::path> &,
          const Context &resolver) -> void;
};

struct GENERATE_PKG_COPY_FILE {
  using Context = sourcemeta::one::Resolver;
  static auto
  handler(const std::filesystem::path &destination,
          const sourcemeta::core::BuildDependencies<std::filesystem::path>
              &dependencies,
          const sourcemeta::core::BuildDynamicCallback<std::filesystem::path> &,
          const Context &resolver) -> void;
};

} // namespace sourcemeta::one

#endif
