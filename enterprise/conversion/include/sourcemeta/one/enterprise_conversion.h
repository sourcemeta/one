#ifndef SOURCEMETA_ONE_ENTERPRISE_CONVERSION_H_
#define SOURCEMETA_ONE_ENTERPRISE_CONVERSION_H_

#include <sourcemeta/core/jsonschema.h>
#include <sourcemeta/core/uri.h>

#include <algorithm>   // std::ranges::find
#include <array>       // std::array
#include <cassert>     // assert
#include <cstddef>     // std::size_t
#include <cstdint>     // std::uint8_t, std::uint32_t
#include <optional>    // std::optional, std::nullopt
#include <span>        // std::span
#include <string>      // std::string
#include <string_view> // std::string_view
#include <utility>     // std::unreachable

namespace sourcemeta::one {

// An official JSON Schema dialect, oldest first
enum class SchemaDialect : std::uint8_t {
  Draft3,
  Draft4,
  Draft6,
  Draft7,
  Draft201909,
  Draft202012
};

// One way of writing the identifier of an official dialect
struct SchemaDialectSpelling {
  std::string_view uri;
  SchemaDialect dialect;
};

// Every spelling of an official dialect that Blaze recognises
inline constexpr std::array<SchemaDialectSpelling, 24> SCHEMA_DIALECT_SPELLINGS{
    {
        {.uri = "http://json-schema.org/draft-03/schema#",
         .dialect = SchemaDialect::Draft3},
        {.uri = "http://json-schema.org/draft-03/schema",
         .dialect = SchemaDialect::Draft3},
        {.uri = "https://json-schema.org/draft-03/schema#",
         .dialect = SchemaDialect::Draft3},
        {.uri = "https://json-schema.org/draft-03/schema",
         .dialect = SchemaDialect::Draft3},
        {.uri = "http://json-schema.org/draft-04/schema#",
         .dialect = SchemaDialect::Draft4},
        {.uri = "http://json-schema.org/draft-04/schema",
         .dialect = SchemaDialect::Draft4},
        {.uri = "https://json-schema.org/draft-04/schema#",
         .dialect = SchemaDialect::Draft4},
        {.uri = "https://json-schema.org/draft-04/schema",
         .dialect = SchemaDialect::Draft4},
        {.uri = "http://json-schema.org/draft-06/schema#",
         .dialect = SchemaDialect::Draft6},
        {.uri = "http://json-schema.org/draft-06/schema",
         .dialect = SchemaDialect::Draft6},
        {.uri = "https://json-schema.org/draft-06/schema#",
         .dialect = SchemaDialect::Draft6},
        {.uri = "https://json-schema.org/draft-06/schema",
         .dialect = SchemaDialect::Draft6},
        {.uri = "http://json-schema.org/draft-07/schema#",
         .dialect = SchemaDialect::Draft7},
        {.uri = "http://json-schema.org/draft-07/schema",
         .dialect = SchemaDialect::Draft7},
        {.uri = "https://json-schema.org/draft-07/schema#",
         .dialect = SchemaDialect::Draft7},
        {.uri = "https://json-schema.org/draft-07/schema",
         .dialect = SchemaDialect::Draft7},
        {.uri = "https://json-schema.org/draft/2019-09/schema",
         .dialect = SchemaDialect::Draft201909},
        {.uri = "https://json-schema.org/draft/2019-09/schema#",
         .dialect = SchemaDialect::Draft201909},
        {.uri = "http://json-schema.org/draft/2019-09/schema",
         .dialect = SchemaDialect::Draft201909},
        {.uri = "http://json-schema.org/draft/2019-09/schema#",
         .dialect = SchemaDialect::Draft201909},
        {.uri = "https://json-schema.org/draft/2020-12/schema",
         .dialect = SchemaDialect::Draft202012},
        {.uri = "https://json-schema.org/draft/2020-12/schema#",
         .dialect = SchemaDialect::Draft202012},
        {.uri = "http://json-schema.org/draft/2020-12/schema",
         .dialect = SchemaDialect::Draft202012},
        {.uri = "http://json-schema.org/draft/2020-12/schema#",
         .dialect = SchemaDialect::Draft202012},
    }};

// Every dialect a schema can be converted into, oldest first
inline constexpr std::array<SchemaDialect, 5> SCHEMA_CONVERSION_TARGETS{
    {SchemaDialect::Draft4, SchemaDialect::Draft6, SchemaDialect::Draft7,
     SchemaDialect::Draft201909, SchemaDialect::Draft202012}};

// The official dialect a declared dialect names, in any spelling of it
[[nodiscard]] inline auto official_dialect(const std::string_view uri)
    -> std::optional<SchemaDialect> {
  const auto match{std::ranges::find(SCHEMA_DIALECT_SPELLINGS, uri,
                                     &SchemaDialectSpelling::uri)};
  if (match == SCHEMA_DIALECT_SPELLINGS.cend()) {
    return std::nullopt;
  }

  return match->dialect;
}

// What a conversion into a dialect is called
[[nodiscard]] inline auto conversion_name(const SchemaDialect dialect)
    -> std::string_view {
  switch (dialect) {
    case SchemaDialect::Draft3:
      return "draft3";
    case SchemaDialect::Draft4:
      return "draft4";
    case SchemaDialect::Draft6:
      return "draft6";
    case SchemaDialect::Draft7:
      return "draft7";
    case SchemaDialect::Draft201909:
      return "2019-09";
    case SchemaDialect::Draft202012:
      return "2020-12";
  }

  std::unreachable();
}

// The canonical identifier of a dialect
[[nodiscard]] inline auto conversion_uri(const SchemaDialect dialect)
    -> std::string_view {
  switch (dialect) {
    case SchemaDialect::Draft3:
      return "http://json-schema.org/draft-03/schema#";
    case SchemaDialect::Draft4:
      return "http://json-schema.org/draft-04/schema#";
    case SchemaDialect::Draft6:
      return "http://json-schema.org/draft-06/schema#";
    case SchemaDialect::Draft7:
      return "http://json-schema.org/draft-07/schema#";
    case SchemaDialect::Draft201909:
      return "https://json-schema.org/draft/2019-09/schema";
    case SchemaDialect::Draft202012:
      return "https://json-schema.org/draft/2020-12/schema";
  }

  std::unreachable();
}

// The base dialect of an official dialect
[[nodiscard]] inline auto conversion_base_dialect(const SchemaDialect dialect)
    -> sourcemeta::core::SchemaBaseDialect {
  switch (dialect) {
    case SchemaDialect::Draft3:
      return sourcemeta::core::SchemaBaseDialect::JSON_SCHEMA_DRAFT_3;
    case SchemaDialect::Draft4:
      return sourcemeta::core::SchemaBaseDialect::JSON_SCHEMA_DRAFT_4;
    case SchemaDialect::Draft6:
      return sourcemeta::core::SchemaBaseDialect::JSON_SCHEMA_DRAFT_6;
    case SchemaDialect::Draft7:
      return sourcemeta::core::SchemaBaseDialect::JSON_SCHEMA_DRAFT_7;
    case SchemaDialect::Draft201909:
      return sourcemeta::core::SchemaBaseDialect::JSON_SCHEMA_2019_09;
    case SchemaDialect::Draft202012:
      return sourcemeta::core::SchemaBaseDialect::JSON_SCHEMA_2020_12;
  }

  std::unreachable();
}

// The dialects a schema declaring a dialect can be converted into, oldest first
[[nodiscard]] inline auto dialect_conversions(const SchemaDialect dialect)
    -> std::span<const SchemaDialect> {
  return std::span<const SchemaDialect>{SCHEMA_CONVERSION_TARGETS}.subspan(
      static_cast<std::size_t>(dialect));
}

// Whether a schema is a meta-schema, which is what its metadata reports. A
// schema declares vocabularies only where the dialect it declares gives that
// keyword meaning
[[nodiscard]] inline auto is_metaschema(const std::string_view dialect,
                                        const bool declares_vocabularies,
                                        const bool declared_as_dialect)
    -> bool {
  if (declared_as_dialect) {
    return true;
  }

  const auto official{official_dialect(dialect)};
  return declares_vocabularies && official.has_value() &&
         (official.value() == SchemaDialect::Draft201909 ||
          official.value() == SchemaDialect::Draft202012);
}

// Whether a schema declaring a dialect can be converted into another
[[nodiscard]] inline auto conversion_applies(const SchemaDialect dialect,
                                             const SchemaDialect target)
    -> bool {
  const auto targets{dialect_conversions(dialect)};
  return std::ranges::find(targets, target) != targets.end();
}

// The dialect a conversion is named after, if the name is one at all
[[nodiscard]] inline auto conversion_target(const std::string_view name)
    -> std::optional<SchemaDialect> {
  const auto match{
      std::ranges::find(SCHEMA_CONVERSION_TARGETS, name, conversion_name)};
  if (match == SCHEMA_CONVERSION_TARGETS.cend()) {
    return std::nullopt;
  }

  return *match;
}

// The selection bit of a conversion into a dialect
[[nodiscard]] inline auto conversion_selector(const SchemaDialect dialect)
    -> std::uint8_t {
  assert(dialect != SchemaDialect::Draft3);
  return static_cast<std::uint8_t>(static_cast<std::uint8_t>(dialect) - 1);
}

// Whether a selection includes the conversion into a dialect
[[nodiscard]] inline auto conversion_selected(const std::uint32_t selection,
                                              const SchemaDialect dialect)
    -> bool {
  return (selection & (std::uint32_t{1} << conversion_selector(dialect))) != 0;
}

// The selection bits of every conversion a schema declaring a dialect gets.
// A meta-schema gets none, as a conversion cannot restate the dialect that a
// meta-schema describes
[[nodiscard]] inline auto conversion_selection(const std::string_view dialect,
                                               const bool is_metaschema)
    -> std::uint32_t {
  const auto official{official_dialect(dialect)};
  if (is_metaschema || !official.has_value()) {
    return 0;
  }

  std::uint32_t result{0};
  for (const auto target : dialect_conversions(official.value())) {
    result |= std::uint32_t{1} << conversion_selector(target);
  }

  return result;
}

// The query that asks for a conversion into a dialect
[[nodiscard]] inline auto conversion_query(const SchemaDialect dialect)
    -> std::string {
  std::string result{"as="};
  result.append(conversion_name(dialect));
  return result;
}

// The identifier of a schema converted into a dialect, which is the one the
// schema declares plus the name of the conversion
[[nodiscard]] inline auto
conversion_identifier(const std::string_view identifier,
                      const SchemaDialect dialect) -> std::string {
  sourcemeta::core::URI uri{std::string{identifier}};
  // Every identifier this catalog hands out is built from path components
  assert(!uri.query().has_value());
  uri.query(conversion_query(dialect));
  return uri.recompose();
}

// The artifact holding a schema converted into a dialect
[[nodiscard]] inline auto conversion_artifact(const SchemaDialect dialect)
    -> std::string {
  std::string result{"schema-"};
  result.append(conversion_name(dialect));
  return result;
}

} // namespace sourcemeta::one

#endif
