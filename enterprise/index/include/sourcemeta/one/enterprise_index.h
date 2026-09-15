#ifndef SOURCEMETA_ONE_ENTERPRISE_INDEX_H_
#define SOURCEMETA_ONE_ENTERPRISE_INDEX_H_

#include <sourcemeta/one/authentication.h>
#include <sourcemeta/one/build.h>
#include <sourcemeta/one/configuration.h>
#include <sourcemeta/one/enterprise_conversion.h>
#include <sourcemeta/one/resolver.h>

#include <sourcemeta/blaze/alterschema.h>
#include <sourcemeta/blaze/configuration.h>
#include <sourcemeta/blaze/foundation.h>

#include <sourcemeta/core/json.h>
#include <sourcemeta/core/uritemplate.h>

#include <cstddef>       // std::size_t
#include <exception>     // std::exception
#include <filesystem>    // std::filesystem::path
#include <functional>    // std::function
#include <string>        // std::string
#include <string_view>   // std::string_view
#include <unordered_set> // std::unordered_set
#include <utility>       // std::move

namespace sourcemeta::one {

// What a schema declaring a dialect can be converted into, as its metadata
// describes it
auto conversions_metadata(std::string_view dialect) -> sourcemeta::core::JSON;

// Convert a schema into a newer official dialect
auto convert_schema(sourcemeta::core::JSON &schema, SchemaDialect target,
                    bool is_metaschema,
                    const sourcemeta::blaze::SchemaResolver &resolver) -> void;

// A schema that could not be converted into a dialect
class SchemaConversionError : public std::exception {
public:
  SchemaConversionError(std::filesystem::path path,
                        const std::string_view dialect, std::string message)
      : path_{std::move(path)}, dialect_{dialect},
        message_{std::move(message)} {}

  [[nodiscard]] auto what() const noexcept -> const char * override {
    return "The schema could not be converted into another dialect";
  }

  [[nodiscard]] auto path() const noexcept -> const std::filesystem::path & {
    return this->path_;
  }

  [[nodiscard]] auto dialect() const noexcept -> std::string_view {
    return this->dialect_;
  }

  [[nodiscard]] auto message() const noexcept -> const std::string & {
    return this->message_;
  }

private:
  std::filesystem::path path_;
  std::string_view dialect_;
  std::string message_;
};

auto load_custom_lint_rules(
    sourcemeta::blaze::SchemaTransformer &bundle,
    std::unordered_set<std::string_view> &custom_names,
    const sourcemeta::blaze::Configuration &configuration,
    const sourcemeta::one::Resolver &resolver,
    const sourcemeta::one::BuildDynamicCallback &callback) -> void;

// Whether the endpoint a URI template describes is one the view being written
// reaches. The template is handed over whole, as which part of it a policy can
// name is settled where the routing table is
using RouteGuard = std::function<bool(std::string_view)>;

// The tools one view is offered, which are those whose route it may reach. A
// tool naming a route a policy keeps from this view could only ever be refused,
// so offering it would be describing a surface that is not there
auto generate_mcp_tools(const sourcemeta::core::URITemplateRouterView &router,
                        const RouteGuard &reachable,
                        sourcemeta::core::JSON &tools,
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
    const sourcemeta::one::Authentication::Table &authentication,
    const sourcemeta::one::Configuration &configuration,
    std::string_view endpoint, sourcemeta::core::JSON &result) -> void;

} // namespace sourcemeta::one

#endif
