#ifndef SOURCEMETA_ONE_INDEX_EDGES_H_
#define SOURCEMETA_ONE_INDEX_EDGES_H_

#include <sourcemeta/one/build.h>
#include <sourcemeta/one/resolver.h>
#include <sourcemeta/one/shared.h>

#include <sourcemeta/core/json.h>
#include <sourcemeta/core/jsonschema.h>
#include <sourcemeta/core/openapi.h>

#include <cassert>       // assert
#include <format>        // std::format
#include <string>        // std::string
#include <string_view>   // std::string_view
#include <unordered_map> // std::unordered_map
#include <utility>       // std::move

namespace sourcemeta::one {

// What one end of an edge is called, which is what a listing calls the same
// resource rather than the file behind it
inline auto edge_identifier(const std::string_view uri)
    -> sourcemeta::core::JSON {
  if (uri.ends_with(".json")) {
    return sourcemeta::core::JSON{uri.substr(0, uri.size() - 5)};
  }

  return sourcemeta::core::JSON{uri};
}

// What a dialect rests on, which is what the document declaring that dialect
// says of itself
inline auto
base_dialect_of(const sourcemeta::core::JSON::String &dialect,
                sourcemeta::one::Resolver &resolver,
                const sourcemeta::one::BuildDynamicCallback &callback)
    -> std::string {
  const auto document{resolver(dialect, callback)};
  assert(document.has_value());
  const sourcemeta::core::SchemaFrame frame{
      sourcemeta::core::SchemaFrame::Mode::Root,
      document.value(),
      sourcemeta::core::schema_walker,
      [&callback, &resolver](const auto identifier) {
        return resolver(identifier, callback);
      },
      "",
      dialect};
  return std::format("{}", frame.root_location().value().get().base_dialect);
}

// The cache an edge report keeps, as the same resource commonly sits at more
// than one end across a report
using EdgeEndCache =
    std::unordered_map<sourcemeta::core::JSON::String, sourcemeta::core::JSON>;

// Everything an edge says about the resource at one of its ends. A reference
// reaches an OpenAPI description only through the catalog, so an end this
// instance serves nothing at is a schema it resolved from elsewhere. An
// OpenAPI description is no schema itself, so the base reported for one is
// the base of the dialect its Schema Objects answer to by default
inline auto edge_end(const std::string_view uri,
                     sourcemeta::one::Resolver &resolver,
                     const sourcemeta::one::BuildDynamicCallback &callback,
                     EdgeEndCache &cache) -> sourcemeta::core::JSON {
  sourcemeta::core::JSON::String key{uri};
  const auto cached{cache.find(key)};
  if (cached != cache.cend()) {
    return cached->second;
  }

  // A reference may spell a resource with the file extension its catalog
  // identity leaves out, so the spelling this instance knows it by is tried
  // after the one the reference used, and everything below asks about that one
  sourcemeta::core::JSON::String known{uri};
  auto kind{resolver.kind_of(known)};
  if (!kind.has_value()) {
    sourcemeta::core::JSON::String alternative{
        edge_identifier(uri).to_string()};
    kind = resolver.kind_of(alternative);
    if (kind.has_value()) {
      known = std::move(alternative);
    }
  }

  const auto describes_api{kind == sourcemeta::one::LeafKind::OpenAPI};
  std::string base_dialect;
  if (describes_api) {
    const auto document{resolver.openapi(known, callback)};
    assert(document.has_value());
    const sourcemeta::core::OpenAPIFrame frame{
        document.value(), sourcemeta::core::schema_walker,
        [&callback, &resolver](const auto identifier) {
          return resolver(identifier, callback);
        },
        known};
    const auto *const root{frame.traverse(frame.base())};
    assert(root != nullptr);
    base_dialect = base_dialect_of(root->default_dialect, resolver, callback);
  } else {
    const auto schema{resolver(known, callback)};
    assert(schema.has_value());
    const sourcemeta::core::SchemaFrame frame{
        sourcemeta::core::SchemaFrame::Mode::Root,
        schema.value(),
        sourcemeta::core::schema_walker,
        [&callback, &resolver](const auto identifier) {
          return resolver(identifier, callback);
        },
        resolver.dialect_of(known),
        known};
    base_dialect =
        std::format("{}", frame.root_location().value().get().base_dialect);
  }

  auto result{sourcemeta::core::JSON::make_object()};
  result.assign("identifier", edge_identifier(uri));
  result.assign("type",
                sourcemeta::core::JSON{describes_api ? "openapi" : "schema"});
  result.assign("baseDialect", sourcemeta::core::JSON{std::move(base_dialect)});
  cache.emplace(std::move(key), result);
  return result;
}

// What an edge already recorded about one of its ends, kept apart from the
// location that edge sits at so that another edge may reuse it
inline auto harvest_edge_end(EdgeEndCache &cache,
                             const sourcemeta::core::JSON &end) -> void {
  const auto &identifier{end.at("identifier")};
  if (cache.contains(identifier.to_string())) {
    return;
  }

  auto result{sourcemeta::core::JSON::make_object()};
  result.assign("identifier", identifier);
  result.assign("type", end.at("type"));
  result.assign("baseDialect", end.at("baseDialect"));
  cache.emplace(identifier.to_string(), std::move(result));
}

// One edge, which is the two resources it joins and the position in the first
// of them that names the second
inline auto make_edge(const sourcemeta::core::JSON &origin,
                      const sourcemeta::core::JSON::String &pointer,
                      const sourcemeta::core::JSON &target)
    -> sourcemeta::core::JSON {
  auto from{origin};
  from.assign("location", sourcemeta::core::JSON{pointer});
  auto result{sourcemeta::core::JSON::make_object()};
  result.assign("from", std::move(from));
  result.assign("to", target);
  return result;
}

} // namespace sourcemeta::one

#endif
