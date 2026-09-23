#include <sourcemeta/one/enterprise_index.h>

#include <sourcemeta/blaze/convert.h>

#include <sourcemeta/core/json.h>
#include <sourcemeta/core/jsonpointer.h>
#include <sourcemeta/core/jsonschema.h>
#include <sourcemeta/core/uri.h>

#include <cassert>     // assert
#include <functional>  // std::function
#include <string>      // std::string
#include <string_view> // std::string_view
#include <utility>     // std::move, std::pair, std::unreachable
#include <vector>      // std::vector

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

// Point every reference that has a matching conversion at that conversion, so
// that what a consumer reaches by following one is written in the dialect it
// asked for
auto rewrite_references(
    sourcemeta::core::JSON &schema, const sourcemeta::one::SchemaDialect target,
    const sourcemeta::core::SchemaResolver &resolver,
    const std::function<bool(std::string_view)> &has_conversion) -> void {
  const sourcemeta::core::SchemaFrame frame{
      sourcemeta::core::SchemaFrame::Mode::References, schema,
      sourcemeta::core::schema_walker, resolver};
  const auto query{sourcemeta::one::conversion_query(target)};
  std::vector<
      std::pair<sourcemeta::core::Pointer, sourcemeta::core::JSON::String>>
      rewrites;

  frame.for_each_reference(
      [&](const sourcemeta::core::SchemaReferenceType,
          const sourcemeta::core::WeakPointer &origin,
          const sourcemeta::core::SchemaFrame::Reference &reference) -> void {
        assert(!origin.empty() && origin.back().is_property());
        // The dialect a schema declares is not one of its references
        if (origin.back().to_property() == "$schema") {
          return;
        }

        const sourcemeta::core::URI destination{reference.destination};
        const auto referent{destination.recompose_without_fragment()};
        if (!referent.has_value() || !has_conversion(referent.value())) {
          return;
        }

        sourcemeta::core::URI result{std::string{reference.original}};
        result.query(query);
        rewrites.emplace_back(sourcemeta::core::to_pointer(origin),
                              result.recompose());
      });

  for (auto &[pointer, reference] : rewrites) {
    sourcemeta::core::get(schema, pointer)
        .into(sourcemeta::core::JSON{std::move(reference)});
  }
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
                    const sourcemeta::core::SchemaResolver &resolver,
                    const std::function<bool(std::string_view)> &has_conversion)
    -> void {
  sourcemeta::blaze::convert(schema, sourcemeta::core::schema_walker, resolver,
                             convert_target(target), "", identifier);
  const sourcemeta::core::SchemaFrame frame{
      sourcemeta::core::SchemaFrame::Mode::Locations, schema,
      sourcemeta::core::schema_walker, resolver, conversion_uri(target)};
  sourcemeta::core::schema_format(schema, frame);
  // A reference that carries a conversion resolves against a schema that
  // identifies itself with that conversion and against nothing else, as a
  // query names a resource of its own where a fragment only addresses into
  // one. Without this, rewriting references to carry a conversion cannot work
  sourcemeta::core::schema_reidentify(schema,
                                      conversion_identifier(identifier, target),
                                      conversion_base_dialect(target));
  rewrite_references(schema, target, resolver, has_conversion);
}

} // namespace sourcemeta::one
