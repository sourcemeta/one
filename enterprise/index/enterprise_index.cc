#include <sourcemeta/one/enterprise_index.h>

#include <sourcemeta/blaze/alterschema.h>
#include <sourcemeta/blaze/compiler.h>
#include <sourcemeta/blaze/foundation.h>

#include <sourcemeta/core/error.h>
#include <sourcemeta/core/mcp.h>
#include <sourcemeta/core/oauth.h>
#include <sourcemeta/core/text.h>
#include <sourcemeta/core/uri.h>
#include <sourcemeta/core/yaml.h>

#include <sourcemeta/one/actions.h>

#include <algorithm>   // std::ranges::find
#include <array>       // std::array
#include <cassert>     // assert
#include <cstddef>     // std::size_t
#include <cstdint>     // std::int64_t
#include <optional>    // std::optional
#include <string>      // std::string
#include <string_view> // std::string_view
#include <utility>     // std::move
#include <variant>     // std::get
#include <vector>      // std::vector

namespace sourcemeta::one {

auto load_custom_lint_rules(
    sourcemeta::blaze::SchemaTransformer &bundle,
    std::unordered_set<std::string_view> &custom_names,
    const sourcemeta::blaze::Configuration &configuration,
    const sourcemeta::one::Resolver &resolver,
    const sourcemeta::one::BuildDynamicCallback &callback) -> void {
  const auto default_dialect{
      configuration.default_dialect.value_or(std::string{})};
  for (const auto &rule : configuration.lint.rules) {
    auto rule_schema{sourcemeta::core::read_yaml_or_json(rule.path)};
    try {
      custom_names.emplace(bundle.add<sourcemeta::blaze::SchemaRule>(
          rule_schema, sourcemeta::blaze::schema_walker,
          [&callback, &resolver](
              const auto identifier) -> std::optional<sourcemeta::core::JSON> {
            return resolver(identifier, callback);
          },
          sourcemeta::blaze::default_schema_compiler, default_dialect,
          std::nullopt,
          rule.top_level ? sourcemeta::blaze::SchemaRule::Scope::TopLevel
                         : sourcemeta::blaze::SchemaRule::Scope::All));
    } catch (
        const sourcemeta::blaze::SchemaRuleInvalidNamePatternError &error) {
      throw sourcemeta::core::FileError<
          sourcemeta::blaze::SchemaRuleInvalidNamePatternError>(
          rule.path, error.identifier(), error.regex());
    } catch (const sourcemeta::blaze::SchemaRuleInvalidNameError &error) {
      throw sourcemeta::core::FileError<
          sourcemeta::blaze::SchemaRuleInvalidNameError>(
          rule.path, error.identifier(), error.what());
    } catch (const sourcemeta::blaze::SchemaRuleMissingNameError &) {
      throw sourcemeta::core::FileError<
          sourcemeta::blaze::SchemaRuleMissingNameError>(rule.path);
    } catch (const sourcemeta::blaze::SchemaUnknownBaseDialectError &) {
      throw sourcemeta::core::FileError<
          sourcemeta::blaze::SchemaUnknownBaseDialectError>(rule.path);
    }
  }
}

auto generate_mcp_tools(const sourcemeta::core::URITemplateRouterView &router,
                        const RouteGuard &reachable,
                        sourcemeta::core::JSON &tools,
                        sourcemeta::core::JSON &tool_routes) -> void {
  const auto base_url{router.base_url()};
  for (std::size_t index{0}; index < router.size(); index++) {
    const auto identifier{router.at(index)};
    const auto context{router.context(identifier)};

    if (!reachable(router.path(identifier))) {
      continue;
    }

    std::string_view rpc_request_schema;
    std::string_view rpc_response_schema;
    router.arguments(identifier,
                     [&rpc_request_schema, &rpc_response_schema](
                         const auto &key, const auto &value) -> void {
                       if (key == "mcpRequestSchema") {
                         rpc_request_schema = std::get<std::string_view>(value);
                       } else if (key == "mcpResponseSchema") {
                         rpc_response_schema =
                             std::get<std::string_view>(value);
                       }
                     });

    // TODO: Don't infer tool-eligibility from the presence of an
    // `mcpRequestSchema` argument. The action system itself should expose
    // whether a given context is tool-eligible, so the
    // indexer doesn't have to reach into router argument bags
    if (rpc_request_schema.empty()) {
      continue;
    }

    const auto operation_id{router.operation_id(identifier)};
    assert(!operation_id.empty());

    const auto description{sourcemeta::one::action_description(context)};
    assert(!description.empty());

    auto input_schema_url{
        sourcemeta::core::URI::rebase_path(rpc_request_schema, {}, base_url)};
    assert(input_schema_url.has_value());

    auto input_schema_ref{sourcemeta::core::JSON::make_object()};
    // The MCP schema requires `type: "object"` on tool input schemas
    input_schema_ref.assign("type", sourcemeta::core::JSON{"object"});
    input_schema_ref.assign(
        "$ref", sourcemeta::core::JSON{std::move(input_schema_url).value()});
    input_schema_ref.assign(
        "$comment",
        sourcemeta::core::JSON{
            "MCP clients should resolve `$ref` references in input "
            "schemas by default. If yours does not, call `resources/read` "
            "on the `$ref` URL above to fetch the full input schema that "
            "defines this tool's parameters. This server is a JSON Schema "
            "registry, so every `$ref` URL is itself reachable as an MCP "
            "resource"});

    std::optional<sourcemeta::core::JSON> output_schema_ref;
    if (!rpc_response_schema.empty()) {
      auto output_schema_url{sourcemeta::core::URI::rebase_path(
          rpc_response_schema, {}, base_url)};
      assert(output_schema_url.has_value());

      auto ref{sourcemeta::core::JSON::make_object()};
      // The MCP schema requires `type: "object"` on tool output schemas
      ref.assign("type", sourcemeta::core::JSON{"object"});
      ref.assign("$ref",
                 sourcemeta::core::JSON{std::move(output_schema_url).value()});
      output_schema_ref.emplace(std::move(ref));
    }

    std::string title{operation_id};
    sourcemeta::core::to_title_case(title);

    tool_routes.assign(
        std::string{operation_id},
        sourcemeta::core::JSON{static_cast<std::int64_t>(identifier)});

    auto tool_entry{sourcemeta::core::JSON::make_array()};
    tool_entry.push_back(sourcemeta::core::JSON{operation_id});
    tool_entry.push_back(sourcemeta::core::JSON{description});
    tool_entry.push_back(std::move(input_schema_ref));
    if (output_schema_ref.has_value()) {
      tool_entry.push_back(std::move(output_schema_ref).value());
    } else {
      tool_entry.push_back(sourcemeta::core::JSON{nullptr});
    }
    tool_entry.push_back(sourcemeta::core::JSON{title});
    tool_entry.push_back(
        sourcemeta::core::JSON{sourcemeta::one::is_read_only(context)});
    tool_entry.push_back(
        sourcemeta::core::JSON{sourcemeta::one::is_destructive(context)});
    tool_entry.push_back(
        sourcemeta::core::JSON{sourcemeta::one::is_idempotent(context)});
    tool_entry.push_back(
        sourcemeta::core::JSON{sourcemeta::one::is_open_world(context)});
    tools.push_back(std::move(tool_entry));
  }
}

namespace {

// The values a policy's `scope` rule names, gathered without repeating one.
// The views point into the configuration, which outlives the document built
// from them
auto collect_scope_rule(const sourcemeta::core::JSON &claims,
                        std::vector<std::string_view> &result) -> void {
  if (!claims.is_object()) {
    return;
  }

  const auto *rule{claims.try_at("scope")};
  if (rule == nullptr || !rule->is_object()) {
    return;
  }

  const auto *values{rule->try_at("values")};
  if (values == nullptr || !values->is_array()) {
    return;
  }

  for (const auto &value : values->as_array()) {
    if (!value.is_string()) {
      continue;
    }

    const std::string_view entry{value.to_string()};
    if (std::ranges::find(result, entry) == result.cend()) {
      result.push_back(entry);
    }
  }
}

} // namespace

auto mcp_resource_identifier(
    const sourcemeta::one::Configuration &configuration,
    const std::string_view endpoint) -> std::string {
  std::string result{configuration.url};
  if (!result.empty() && result.back() == '/') {
    result.pop_back();
  }

  result.append(endpoint);
  return result;
}

auto generate_protected_resource_metadata(
    const sourcemeta::one::Authentication::Table &authentication,
    const sourcemeta::one::Configuration &configuration,
    const std::string_view endpoint, sourcemeta::core::JSON &result) -> void {
  const auto resource{mcp_resource_identifier(configuration, endpoint)};

  // A client that reads this asks its provider for a token bound to the
  // resource below, so an issuer whose policy expects a different audience
  // would mint one this instance refuses. Only an issuer whose policy accepts
  // that audience can be named without sending the client into a rejection
  std::vector<std::string_view> servers;
  // RFC 9728 Section 2 gives these as the scope values used to request access
  // to this resource, which is exactly what a policy's `scope` rule names. A
  // client reading them learns what to ask its provider for, rather than
  // discovering it by being refused
  std::vector<std::string_view> scopes;
  for (const auto name : authentication.governing(
           sourcemeta::one::Authentication::Path::relative(endpoint))) {
    // What a policy declares is read from the configuration that declared it,
    // which is where the claim rules are already parsed, and the name is what
    // the two agree on
    const auto match{std::ranges::find(
        configuration.authentication, name,
        &sourcemeta::one::Configuration::AuthenticationEntry::name)};
    // A policy the artifact records but this configuration no longer declares
    // is one a build behind this one wrote. It names no client anybody could
    // ask for a token, so it is passed over rather than reported
    if (match == configuration.authentication.cend()) {
      continue;
    }

    const auto &entry{*match};
    if (entry.type !=
            sourcemeta::one::Configuration::AuthenticationEntry::Type::JWT ||
        entry.audience != resource) {
      continue;
    }

    // A policy that names its keys outright is never asked to discover
    // anything, so nothing has established that its issuer is the identifier
    // RFC 8414 defines. The builder refuses a document naming even one such
    // entry, so an unusable issuer is dropped here rather than costing the
    // instance the entries that are usable
    if (!sourcemeta::core::oauth_is_issuer_identifier(entry.issuer)) {
      continue;
    }

    if (std::ranges::find(servers, entry.issuer) == servers.cend()) {
      servers.emplace_back(entry.issuer);
    }

    collect_scope_rule(entry.claims, scopes);
  }

  if (servers.empty()) {
    return;
  }

  std::ranges::sort(scopes);

  static constexpr std::array<std::string_view, 1> BEARER_METHODS{{"header"}};
  sourcemeta::core::OAuthResourceMetadataConfig config;
  config.resource = resource;
  config.authorization_servers = servers;
  config.bearer_methods_supported = BEARER_METHODS;
  config.scopes_supported = scopes;

  // The builder refuses anything a client could not use, which includes a
  // resource identifier that is not an https URL. RFC 9728 Section 1.2 makes
  // no exception for loopback, so an instance served in the clear publishes
  // nothing rather than something a conforming client rejects
  auto document{sourcemeta::core::oauth_make_resource_metadata(config)};
  if (document.has_value()) {
    result = std::move(document).value();
  }
}

} // namespace sourcemeta::one
