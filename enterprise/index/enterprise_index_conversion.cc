#include <sourcemeta/one/enterprise_index.h>

#include <sourcemeta/blaze/convert.h>
#include <sourcemeta/blaze/convert_error.h>

#include <sourcemeta/core/json.h>
#include <sourcemeta/core/jsonschema.h>

#include <string>      // std::string
#include <string_view> // std::string_view
#include <utility>     // std::move, std::unreachable

namespace {

auto convert_target(const sourcemeta::one::SchemaDialect dialect)
    -> sourcemeta::blaze::ConvertTarget {
  switch (dialect) {
    case sourcemeta::one::SchemaDialect::Draft4:
      return sourcemeta::blaze::ConvertTarget::Draft4;
    case sourcemeta::one::SchemaDialect::Draft6:
      return sourcemeta::blaze::ConvertTarget::Draft6;
    case sourcemeta::one::SchemaDialect::Draft7:
      return sourcemeta::blaze::ConvertTarget::Draft7;
    case sourcemeta::one::SchemaDialect::Draft201909:
      return sourcemeta::blaze::ConvertTarget::Draft201909;
    case sourcemeta::one::SchemaDialect::Draft202012:
      return sourcemeta::blaze::ConvertTarget::Draft202012;
    case sourcemeta::one::SchemaDialect::Draft3:
      break;
  }

  // Nothing is ever converted into the oldest dialect
  std::unreachable();
}

} // namespace

namespace sourcemeta::one {

auto conversions_metadata(const std::string_view dialect,
                          const bool is_metaschema) -> sourcemeta::core::JSON {
  auto result{sourcemeta::core::JSON::make_object()};
  const auto official{official_dialect(dialect)};
  if (is_metaschema || !official.has_value()) {
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
                    const std::string_view identifier,
                    const sourcemeta::core::SchemaResolver &resolver) -> bool {
  // A meta-schema names the keywords that conversion renames, and a dialect
  // off the ladder has no rules to move a schema from, so a closure holding
  // either is one this catalog has no conversion to offer for. Every other
  // way conversion can fail is a fault to report rather than a schema to
  // quietly pass over
  try {
    sourcemeta::blaze::convert(schema, sourcemeta::core::schema_walker,
                               resolver, convert_target(target), "",
                               identifier);
  } catch (const sourcemeta::blaze::ConvertUnsupportedMetaschemaError &) {
    return false;
  } catch (const sourcemeta::blaze::ConvertUnsupportedDialectError &) {
    return false;
  }

  const sourcemeta::core::SchemaFrame frame{
      sourcemeta::core::SchemaFrame::Mode::Locations, schema,
      sourcemeta::core::schema_walker, resolver, conversion_uri(target)};
  sourcemeta::core::schema_format(schema, frame);
  return true;
}

} // namespace sourcemeta::one
