#include <sourcemeta/one/enterprise_index.h>

#include <sourcemeta/blaze/compiler.h>
#include <sourcemeta/blaze/linter.h>

#include <sourcemeta/core/yaml.h>

#include <string> // std::string

namespace sourcemeta::one {

auto load_custom_lint_rules(
    sourcemeta::core::SchemaTransformer &bundle,
    std::unordered_set<std::string_view> &custom_names,
    const sourcemeta::blaze::Configuration &configuration,
    const sourcemeta::one::Resolver &resolver,
    const sourcemeta::core::BuildDynamicCallback<std::filesystem::path>
        &callback) -> void {
  const auto default_dialect{
      configuration.default_dialect.value_or(std::string{})};
  for (const auto &rule_path : configuration.lint.rules) {
    auto rule_schema{sourcemeta::core::read_yaml_or_json(rule_path)};
    custom_names.emplace(bundle.add<sourcemeta::blaze::SchemaRule>(
        rule_schema, sourcemeta::core::schema_walker,
        [&callback, &resolver](const auto identifier) {
          return resolver(identifier, callback);
        },
        sourcemeta::blaze::default_schema_compiler, default_dialect));
  }
}

} // namespace sourcemeta::one
