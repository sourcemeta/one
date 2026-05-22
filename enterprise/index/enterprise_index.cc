#include <sourcemeta/one/enterprise_index.h>

#include <sourcemeta/blaze/alterschema.h>
#include <sourcemeta/blaze/compiler.h>
#include <sourcemeta/blaze/foundation.h>

#include <sourcemeta/core/error.h>
#include <sourcemeta/core/text.h>
#include <sourcemeta/core/uri.h>
#include <sourcemeta/core/yaml.h>

#include <sourcemeta/one/actions.h>
#include <sourcemeta/one/mcp.h>
#include <sourcemeta/one/search.h>

#include <cassert>     // assert
#include <cstddef>     // std::size_t
#include <cstdint>     // std::int64_t
#include <optional>    // std::optional
#include <string>      // std::string
#include <string_view> // std::string_view
#include <utility>     // std::move
#include <variant>     // std::get

namespace sourcemeta::one {

auto load_custom_lint_rules(
    sourcemeta::blaze::SchemaTransformer &bundle,
    std::unordered_set<std::string_view> &custom_names,
    const sourcemeta::blaze::Configuration &configuration,
    const sourcemeta::one::Resolver &resolver,
    const sourcemeta::one::BuildDynamicCallback &callback) -> void {
  const auto default_dialect{
      configuration.default_dialect.value_or(std::string{})};
  for (const auto &rule_path : configuration.lint.rules) {
    auto rule_schema{sourcemeta::core::read_yaml_or_json(rule_path)};
    try {
      custom_names.emplace(bundle.add<sourcemeta::blaze::SchemaRule>(
          rule_schema, sourcemeta::blaze::schema_walker,
          [&callback, &resolver](const auto identifier) {
            return resolver(identifier, callback);
          },
          sourcemeta::blaze::default_schema_compiler, default_dialect));
    } catch (
        const sourcemeta::blaze::SchemaRuleInvalidNamePatternError &error) {
      throw sourcemeta::core::FileError<
          sourcemeta::blaze::SchemaRuleInvalidNamePatternError>(
          rule_path, error.identifier(), error.regex());
    } catch (const sourcemeta::blaze::SchemaRuleInvalidNameError &error) {
      throw sourcemeta::core::FileError<
          sourcemeta::blaze::SchemaRuleInvalidNameError>(
          rule_path, error.identifier(), error.what());
    } catch (const sourcemeta::blaze::SchemaRuleMissingNameError &) {
      throw sourcemeta::core::FileError<
          sourcemeta::blaze::SchemaRuleMissingNameError>(rule_path);
    } catch (const sourcemeta::blaze::SchemaUnknownBaseDialectError &) {
      throw sourcemeta::core::FileError<
          sourcemeta::blaze::SchemaUnknownBaseDialectError>(rule_path);
    }
  }
}

auto generate_mcp_resources(const std::filesystem::path &search_metapack_path,
                            const sourcemeta::one::Configuration &configuration,
                            const std::size_t page_size,
                            sourcemeta::core::JSON &resources) -> void {
  sourcemeta::one::SearchView search_view{search_metapack_path};
  const auto total{search_view.count()};
  const auto page_count{total == 0 ? std::size_t{1}
                                   : (total + page_size - 1) / page_size};

  for (std::size_t page_index{0}; page_index < page_count; ++page_index) {
    const auto offset{page_index * page_size};
    auto page{sourcemeta::core::JSON::make_object()};
    auto entries{sourcemeta::core::JSON::make_array()};

    search_view.for_each(
        offset, page_size,
        [&entries, &configuration](
            const sourcemeta::one::SearchListEntry &entry) -> void {
          std::string_view name{entry.path};
          const auto last_slash{name.rfind('/')};
          if (last_slash != std::string_view::npos) {
            name.remove_prefix(last_slash + 1);
          }

          std::string uri{configuration.origin};
          uri.append(entry.path);

          entries.push_back(sourcemeta::one::mcp_make_resource(
              uri, name, "application/schema+json", entry.description,
              static_cast<std::size_t>(entry.bytes_raw)));
        });

    page.assign("resources", std::move(entries));
    if (page_index + 1 < page_count) {
      page.assign("nextCursor",
                  sourcemeta::core::JSON{std::to_string(offset + page_size)});
    }
    resources.assign(std::to_string(offset), std::move(page));
  }
}

auto generate_mcp_tools(const sourcemeta::core::URITemplateRouterView &router,
                        sourcemeta::core::JSON &tools,
                        sourcemeta::core::JSON &tool_routes) -> void {
  const auto base_url{router.base_url()};
  const auto base_path{router.base_path()};
  for (std::size_t index{0}; index < router.size(); index++) {
    const auto identifier{router.at(index)};
    const auto context{router.context(identifier)};

    std::string_view rpc_schema;
    std::string_view response_schema;
    router.arguments(identifier, [&rpc_schema, &response_schema](
                                     const auto &key, const auto &value) {
      if (key == "rpcSchema") {
        rpc_schema = std::get<std::string_view>(value);
      } else if (key == "responseSchema") {
        response_schema = std::get<std::string_view>(value);
      }
    });

    // TODO: Don't infer tool-eligibility from the presence of an
    // `rpcSchema` argument. The action system itself should expose
    // whether a given context is tool-eligible, so the
    // indexer doesn't have to reach into router argument bags
    if (rpc_schema.empty()) {
      continue;
    }

    const auto operation_id{router.operation_id(identifier)};
    assert(!operation_id.empty());

    const auto description{sourcemeta::one::action_description(context)};
    assert(!description.empty());

    auto input_schema_url{
        sourcemeta::core::URI::rebase_path(rpc_schema, base_path, base_url)};
    assert(input_schema_url.has_value());

    auto input_schema_ref{sourcemeta::core::JSON::make_object()};
    input_schema_ref.assign(
        "$ref", sourcemeta::core::JSON{std::move(input_schema_url).value()});

    std::optional<sourcemeta::core::JSON> output_schema_ref;
    if (!response_schema.empty()) {
      auto output_schema_url{sourcemeta::core::URI::rebase_path(
          response_schema, base_path, base_url)};
      assert(output_schema_url.has_value());

      auto ref{sourcemeta::core::JSON::make_object()};
      ref.assign("$ref",
                 sourcemeta::core::JSON{std::move(output_schema_url).value()});
      output_schema_ref.emplace(std::move(ref));
    }

    auto annotations{sourcemeta::core::JSON::make_object()};
    std::string title{operation_id};
    sourcemeta::core::to_title_case(title);
    annotations.assign("title", sourcemeta::core::JSON{title});

    tool_routes.assign(
        std::string{operation_id},
        sourcemeta::core::JSON{static_cast<std::int64_t>(identifier)});
    tools.push_back(sourcemeta::one::mcp_make_tool_descriptor(
        sourcemeta::one::MCPProtocolVersion::V_2025_11_25, operation_id,
        description, std::move(input_schema_ref), std::move(output_schema_ref),
        std::move(annotations)));
  }
}

} // namespace sourcemeta::one
