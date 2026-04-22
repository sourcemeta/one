#ifndef SOURCEMETA_ONE_ENTERPRISE_INDEX_H_
#define SOURCEMETA_ONE_ENTERPRISE_INDEX_H_

#include <sourcemeta/one/build.h>
#include <sourcemeta/one/resolver.h>

#include <sourcemeta/blaze/alterschema.h>
#include <sourcemeta/blaze/configuration.h>

#include <sourcemeta/core/html.h>

#include <filesystem>    // std::filesystem::path
#include <string_view>   // std::string_view
#include <unordered_set> // std::unordered_set

namespace sourcemeta::one {

auto load_custom_lint_rules(
    sourcemeta::blaze::SchemaTransformer &bundle,
    std::unordered_set<std::string_view> &custom_names,
    const sourcemeta::blaze::Configuration &configuration,
    const sourcemeta::one::Resolver &resolver,
    const sourcemeta::one::BuildDynamicCallback &callback) -> void;

auto render_documentation(sourcemeta::core::HTMLWriter &writer,
                          const std::filesystem::path &metapack_path) -> void;

} // namespace sourcemeta::one

#endif
