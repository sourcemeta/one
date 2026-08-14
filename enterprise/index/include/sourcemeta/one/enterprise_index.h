#ifndef SOURCEMETA_ONE_ENTERPRISE_INDEX_H_
#define SOURCEMETA_ONE_ENTERPRISE_INDEX_H_

#include <sourcemeta/one/authentication.h>
#include <sourcemeta/one/build.h>
#include <sourcemeta/one/configuration.h>
#include <sourcemeta/one/resolver.h>

#include <sourcemeta/blaze/alterschema.h>
#include <sourcemeta/blaze/configuration.h>

#include <sourcemeta/core/json.h>
#include <sourcemeta/core/uritemplate.h>

#include <cstddef>       // std::size_t
#include <string_view>   // std::string_view
#include <unordered_set> // std::unordered_set

namespace sourcemeta::one {

auto load_custom_lint_rules(
    sourcemeta::blaze::SchemaTransformer &bundle,
    std::unordered_set<std::string_view> &custom_names,
    const sourcemeta::blaze::Configuration &configuration,
    const sourcemeta::one::Resolver &resolver,
    const sourcemeta::one::BuildDynamicCallback &callback) -> void;

// The tools one view is offered, which are those whose route it may reach. A
// tool naming a route a policy keeps from this view could only ever be refused,
// so offering it would be describing a surface that is not there
auto generate_mcp_tools(const sourcemeta::core::URITemplateRouterView &router,
                        const sourcemeta::one::Authentication &authentication,
                        std::size_t view, sourcemeta::core::JSON &tools,
                        sourcemeta::core::JSON &tool_routes) -> void;

// What this instance's MCP endpoint is called as an OAuth protected resource.
// The metadata document names it and the endpoint requires a presented token
// to name it too, so both read it from here rather than composing their own
auto mcp_resource_identifier(
    const sourcemeta::one::Configuration &configuration,
    std::string_view endpoint) -> std::string;

// RFC 9728 metadata naming where a token for the MCP endpoint comes from, left
// untouched when no policy can honestly answer that
// https://datatracker.ietf.org/doc/html/rfc9728
auto generate_protected_resource_metadata(
    const sourcemeta::one::Authentication &authentication,
    const sourcemeta::one::Configuration &configuration,
    std::string_view endpoint, sourcemeta::core::JSON &result) -> void;

} // namespace sourcemeta::one

#endif
