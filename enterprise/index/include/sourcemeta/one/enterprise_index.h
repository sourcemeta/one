#ifndef SOURCEMETA_ONE_ENTERPRISE_INDEX_H_
#define SOURCEMETA_ONE_ENTERPRISE_INDEX_H_

#include <sourcemeta/one/build.h>
#include <sourcemeta/one/configuration.h>
#include <sourcemeta/one/resolver.h>

#include <sourcemeta/blaze/alterschema.h>
#include <sourcemeta/blaze/configuration.h>

#include <sourcemeta/core/json.h>

#include <cstddef>       // std::size_t
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

auto generate_mcp_resources(const std::filesystem::path &search_metapack_path,
                            const sourcemeta::one::Configuration &configuration,
                            const std::size_t page_size,
                            sourcemeta::core::JSON &resources) -> void;

} // namespace sourcemeta::one

#endif
