#include <sourcemeta/one/enterprise_index.h>

#include <sourcemeta/blaze/alterschema.h>
#include <sourcemeta/blaze/format.h>
#include <sourcemeta/blaze/foundation.h>

#include <sourcemeta/core/json.h>
#include <sourcemeta/core/jsonpointer.h>

#include <array>       // std::array
#include <memory>      // std::unique_ptr, std::make_unique
#include <stdexcept>   // std::runtime_error
#include <string>      // std::string
#include <string_view> // std::string_view
#include <utility>     // std::move, std::unreachable

namespace {

auto upgrade_mode(const sourcemeta::one::SchemaDialect dialect)
    -> sourcemeta::blaze::AlterSchemaMode {
  switch (dialect) {
    case sourcemeta::one::SchemaDialect::Draft4:
      return sourcemeta::blaze::AlterSchemaMode::UpgradeDraft4;
    case sourcemeta::one::SchemaDialect::Draft6:
      return sourcemeta::blaze::AlterSchemaMode::UpgradeDraft6;
    case sourcemeta::one::SchemaDialect::Draft7:
      return sourcemeta::blaze::AlterSchemaMode::UpgradeDraft7;
    case sourcemeta::one::SchemaDialect::Draft201909:
      return sourcemeta::blaze::AlterSchemaMode::Upgrade201909;
    case sourcemeta::one::SchemaDialect::Draft202012:
      return sourcemeta::blaze::AlterSchemaMode::Upgrade202012;
    case sourcemeta::one::SchemaDialect::Draft3:
      break;
  }

  // Nothing is ever converted into the oldest dialect
  std::unreachable();
}

} // namespace

namespace sourcemeta::one {

auto conversions_metadata(const std::string_view dialect)
    -> sourcemeta::core::JSON {
  auto result{sourcemeta::core::JSON::make_object()};
  const auto official{official_dialect(dialect)};
  if (!official.has_value()) {
    return result;
  }

  for (const auto target : dialect_conversions(official.value())) {
    auto conversion{sourcemeta::core::JSON::make_object()};
    conversion.assign("mediaType",
                      sourcemeta::core::JSON{"application/schema+json"});
    result.assign(std::string{conversion_name(target)}, std::move(conversion));
  }

  return result;
}

auto convert_schema(sourcemeta::core::JSON &schema, const SchemaDialect target,
                    const bool is_metaschema,
                    const sourcemeta::blaze::SchemaResolver &resolver) -> void {
  // A transformer is never shared between threads
  thread_local std::array<std::unique_ptr<sourcemeta::blaze::SchemaTransformer>,
                          SCHEMA_CONVERSION_TARGETS.size()>
      transformers;
  auto &transformer{transformers[conversion_selector(target)]};
  if (!transformer) {
    transformer = std::make_unique<sourcemeta::blaze::SchemaTransformer>();
    sourcemeta::blaze::add(*transformer, upgrade_mode(target));
  }

  const auto result{transformer->apply(
      schema, sourcemeta::blaze::schema_walker, resolver,
      [](const sourcemeta::core::Pointer &, const std::string_view,
         const std::string_view,
         const sourcemeta::blaze::SchemaTransformRule::Result &,
         const bool) -> void {},
      "", "", "", is_metaschema)};
  if (!result.first) {
    throw std::runtime_error("The schema could not be fully converted");
  }

  sourcemeta::blaze::format(schema, sourcemeta::blaze::schema_walker, resolver,
                            conversion_uri(target));
}

} // namespace sourcemeta::one
