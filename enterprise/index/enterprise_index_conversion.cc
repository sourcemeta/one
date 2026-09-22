#include <sourcemeta/one/enterprise_index.h>

#include <sourcemeta/blaze/convert.h>

#include <sourcemeta/core/json.h>
#include <sourcemeta/core/jsonschema.h>
#include <sourcemeta/core/uri.h>

#include <exception>     // std::exception
#include <string>        // std::string
#include <string_view>   // std::string_view
#include <unordered_map> // std::unordered_map
#include <unordered_set> // std::unordered_set
#include <utility>       // std::move, std::unreachable
#include <vector>        // std::vector

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

auto conversion_selections(const Resolver &resolver)
    -> std::unordered_map<sourcemeta::core::JSON::String, std::uint32_t> {
  std::unordered_map<sourcemeta::core::JSON::String,
                     std::vector<sourcemeta::core::JSON::String>>
      referrers;
  std::vector<sourcemeta::core::JSON::String> pending;
  std::unordered_set<sourcemeta::core::JSON::String> result;
  result.reserve(resolver.size());

  for (const auto &[uri, entry] : resolver.data()) {
    // A meta-schema is no more convertible inside a bundle than it is on its
    // own, as converting one changes the dialect it is written in without
    // restating the dialect it describes
    auto convertible{official_dialect(entry.dialect).has_value() &&
                     !is_metaschema(entry.dialect, entry.vocabularies,
                                    resolver.is_dialect(uri))};

    // Whatever stops a schema from being read here is reported where it always
    // was, further into the build and with the context that belongs to it. All
    // this pass concludes is that such a schema cannot be converted
    try {
      const auto schema{resolver(uri)};
      if (schema.has_value()) {
        const sourcemeta::core::SchemaFrame frame{
            sourcemeta::core::SchemaFrame::Mode::References, schema.value(),
            sourcemeta::core::schema_walker,
            [&resolver](const auto identifier) {
              return resolver(identifier);
            }};
        frame.for_each_reference(
            [&referrers,
             &uri](const sourcemeta::core::SchemaReferenceType,
                   const sourcemeta::core::WeakPointer &,
                   const sourcemeta::core::SchemaFrame::Reference &reference)
                -> void {
              const sourcemeta::core::URI destination{reference.destination};
              const auto referent{destination.recompose_without_fragment()};
              if (referent.has_value()) {
                referrers[referent.value()].push_back(uri);
              }
            });
      } else {
        convertible = false;
      }
    } catch (const std::exception &) {
      convertible = false;
    }

    if (convertible) {
      result.emplace(uri);
    } else {
      pending.push_back(uri);
    }
  }

  // What a schema reaches is only as convertible as the least convertible
  // thing in it, which settles however the references are tangled
  while (!pending.empty()) {
    const auto current{std::move(pending.back())};
    pending.pop_back();
    const auto match{referrers.find(current)};
    if (match == referrers.cend()) {
      continue;
    }

    for (const auto &referrer : match->second) {
      if (result.erase(referrer) > 0) {
        pending.push_back(referrer);
      }
    }
  }

  std::unordered_map<sourcemeta::core::JSON::String, std::uint32_t> selections;
  selections.reserve(resolver.size());
  for (const auto &[uri, entry] : resolver.data()) {
    selections.emplace(
        uri, result.contains(uri)
                 ? conversion_selection(entry.dialect,
                                        is_metaschema(entry.dialect,
                                                      entry.vocabularies,
                                                      resolver.is_dialect(uri)))
                 : 0);
  }

  return selections;
}

auto convert_schema(sourcemeta::core::JSON &schema, const SchemaDialect target,
                    const std::string_view identifier,
                    const sourcemeta::core::SchemaResolver &resolver) -> void {
  sourcemeta::blaze::convert(schema, sourcemeta::core::schema_walker, resolver,
                             convert_target(target), "", identifier);
  const sourcemeta::core::SchemaFrame frame{
      sourcemeta::core::SchemaFrame::Mode::Locations, schema,
      sourcemeta::core::schema_walker, resolver, conversion_uri(target)};
  sourcemeta::core::schema_format(schema, frame);
}

} // namespace sourcemeta::one
