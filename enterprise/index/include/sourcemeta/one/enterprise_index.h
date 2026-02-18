#ifndef SOURCEMETA_ONE_ENTERPRISE_INDEX_H_
#define SOURCEMETA_ONE_ENTERPRISE_INDEX_H_

#include <sourcemeta/one/resolver.h>

#include <sourcemeta/blaze/configuration.h>

#include <sourcemeta/core/build.h>
#include <sourcemeta/core/jsonschema.h>

#include <filesystem>    // std::filesystem
#include <string_view>   // std::string_view
#include <unordered_set> // std::unordered_set

namespace sourcemeta::one {

auto load_custom_lint_rules(
    sourcemeta::core::SchemaTransformer &bundle,
    std::unordered_set<std::string_view> &custom_names,
    const sourcemeta::blaze::Configuration &configuration,
    const sourcemeta::one::Resolver &resolver,
    const sourcemeta::core::BuildDynamicCallback<std::filesystem::path>
        &callback) -> void;

} // namespace sourcemeta::one

#endif
