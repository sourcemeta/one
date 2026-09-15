#include <sourcemeta/one/enterprise_index.h>

#include <sourcemeta/blaze/convert.h>
#include <sourcemeta/blaze/format.h>

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
                    const sourcemeta::core::SchemaResolver &resolver) -> void {
  sourcemeta::blaze::convert(schema, sourcemeta::core::schema_walker, resolver,
                             convert_target(target), "", "", is_metaschema);
  sourcemeta::blaze::format(schema, sourcemeta::core::schema_walker, resolver,
                            conversion_uri(target));
}

} // namespace sourcemeta::one
