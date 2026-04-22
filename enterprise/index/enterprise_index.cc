#include <sourcemeta/one/enterprise_index.h>

#include <sourcemeta/blaze/alterschema.h>
#include <sourcemeta/blaze/compiler.h>

#include <sourcemeta/core/error.h>
#include <sourcemeta/core/jsonschema.h>
#include <sourcemeta/core/markdown.h>
#include <sourcemeta/core/yaml.h>
#include <sourcemeta/one/metapack.h>

#include <string> // std::string

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

auto render_documentation(sourcemeta::core::HTMLWriter &writer,
                          const std::filesystem::path &metapack_path) -> void {
  const auto content{metapack_read_text(metapack_path)};
  if (!content.has_value()) {
    return;
  }

  writer.div().attribute("class", "card mt-4");
  writer.div().attribute("class", "card-body documentation");
  writer.raw(sourcemeta::core::markdown_to_html(content.value()));
  writer.close();
  writer.close();
}

} // namespace sourcemeta::one
