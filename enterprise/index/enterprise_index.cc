#include <sourcemeta/one/enterprise_index.h>

#include <sourcemeta/blaze/alterschema.h>
#include <sourcemeta/blaze/compiler.h>

#include <sourcemeta/core/error.h>
#include <sourcemeta/core/jsonschema.h>
#include <sourcemeta/core/yaml.h>

#include <sourcemeta/one/search.h>

#include <cstddef>     // std::size_t
#include <string>      // std::string
#include <string_view> // std::string_view
#include <utility>     // std::move

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
          rule_schema, sourcemeta::core::schema_walker,
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
    } catch (const sourcemeta::core::SchemaUnknownBaseDialectError &) {
      throw sourcemeta::core::FileError<
          sourcemeta::core::SchemaUnknownBaseDialectError>(rule_path);
    }
  }
}

auto enterprise_generate_mcp_resources(
    const std::filesystem::path &search_metapack_path,
    const sourcemeta::one::Configuration &configuration,
    const std::size_t page_size, sourcemeta::core::JSON &resources) -> void {
  sourcemeta::one::SearchView search_view{search_metapack_path};
  const auto total{search_view.count()};

  std::size_t offset{0};
  while (true) {
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

          auto resource{sourcemeta::core::JSON::make_object()};
          resource.assign("uri", sourcemeta::core::JSON{uri});
          resource.assign("name", sourcemeta::core::JSON{name});
          if (!entry.description.empty()) {
            resource.assign("description",
                            sourcemeta::core::JSON{entry.description});
          }
          resource.assign("mimeType",
                          sourcemeta::core::JSON{"application/schema+json"});
          resource.assign("size",
                          sourcemeta::core::JSON{
                              static_cast<std::size_t>(entry.bytes_raw)});
          entries.push_back(std::move(resource));
        });

    page.assign("resources", std::move(entries));
    const auto next_offset{offset + page_size};
    if (next_offset < total) {
      page.assign("nextCursor",
                  sourcemeta::core::JSON{std::to_string(next_offset)});
    }
    resources.assign(std::to_string(offset), std::move(page));

    if (next_offset >= total) {
      break;
    }
    offset = next_offset;
  }
}

} // namespace sourcemeta::one
