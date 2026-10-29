#ifndef SOURCEMETA_ONE_ENTERPRISE_CONVERSION_H_
#define SOURCEMETA_ONE_ENTERPRISE_CONVERSION_H_

#include <sourcemeta/core/jsonschema.h>

#include <algorithm>   // std::ranges::find
#include <array>       // std::array
#include <cassert>     // assert
#include <cstddef>     // std::size_t
#include <cstdint>     // std::uint8_t, std::uint32_t
#include <iterator>    // std::ranges::distance
#include <optional>    // std::optional, std::nullopt
#include <span>        // std::span
#include <string>      // std::string
#include <string_view> // std::string_view
#include <utility>     // std::unreachable

namespace sourcemeta::one {

// A dialect this catalog can convert between. The official JSON Schema
// dialects run up to 2020-12, and the OpenAPI Schema Object dialects carry on
// from there, each being 2020-12 plus a vocabulary of its own.
//
// The order of these members carries no meaning. What a dialect converts into
// is settled by whether it appears in the list of targets below, and where,
// so a dialect may be added here at any position
enum class SchemaDialect : std::uint8_t {
  Draft3,
  OpenAPI30,
  Draft4,
  Draft6,
  Draft7,
  Draft201909,
  Draft202012,
  OpenAPI31,
  OpenAPI32
};

// One way of writing the identifier of a dialect on the ladder
struct SchemaDialectSpelling {
  std::string_view uri;
  SchemaDialect dialect;
};

// Every spelling of a dialect on the ladder that Blaze recognises
inline constexpr std::array<SchemaDialectSpelling, 32> SCHEMA_DIALECT_SPELLINGS{
    {
        // The OpenAPI Initiative mints a dialect URI per revision of the v3.0
        // Schema Object, and the two it has minted so far describe the same
        // keywords
        {.uri = "tag:spec.openapis.org,2024-10-18:oas/3.0/dialect",
         .dialect = SchemaDialect::OpenAPI30},
        {.uri = "tag:spec.openapis.org,2021-09-28:oas/3.0/dialect",
         .dialect = SchemaDialect::OpenAPI30},
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
        // The OpenAPI Initiative publishes a dated revision of the v3.1
        // dialect alongside the undated one the specification names, and a
        // description is free to name any of them
        {.uri = "https://spec.openapis.org/oas/3.1/dialect/base",
         .dialect = SchemaDialect::OpenAPI31},
        {.uri = "https://spec.openapis.org/oas/3.1/dialect/base#",
         .dialect = SchemaDialect::OpenAPI31},
        {.uri = "https://spec.openapis.org/oas/3.1/dialect/2024-11-10",
         .dialect = SchemaDialect::OpenAPI31},
        {.uri = "https://spec.openapis.org/oas/3.1/dialect/2024-10-25",
         .dialect = SchemaDialect::OpenAPI31},
        {.uri = "https://spec.openapis.org/oas/3.2/dialect/2025-09-17",
         .dialect = SchemaDialect::OpenAPI32},
        {.uri = "https://spec.openapis.org/oas/3.2/dialect/2025-09-17#",
         .dialect = SchemaDialect::OpenAPI32},
    }};

// Every dialect a schema can be converted into, oldest first. This is the one
// place the upgrade order is written down, and a dialect left out of it is one
// nothing converts into
inline constexpr std::array<SchemaDialect, 7> SCHEMA_CONVERSION_TARGETS{
    {SchemaDialect::Draft4, SchemaDialect::Draft6, SchemaDialect::Draft7,
     SchemaDialect::Draft201909, SchemaDialect::Draft202012,
     SchemaDialect::OpenAPI31, SchemaDialect::OpenAPI32}};

// The dialect on the ladder a declared dialect names, in any spelling of it
[[nodiscard]] inline auto schema_dialect(const std::string_view uri)
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
    case SchemaDialect::OpenAPI30:
      return "openapi3.0";
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
    case SchemaDialect::OpenAPI31:
      return "openapi3.1";
    case SchemaDialect::OpenAPI32:
      return "openapi3.2";
  }

  std::unreachable();
}

// The canonical identifier of a dialect
[[nodiscard]] inline auto conversion_uri(const SchemaDialect dialect)
    -> std::string_view {
  switch (dialect) {
    case SchemaDialect::Draft3:
      return "http://json-schema.org/draft-03/schema#";
    case SchemaDialect::OpenAPI30:
      return "tag:spec.openapis.org,2024-10-18:oas/3.0/dialect";
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
    case SchemaDialect::OpenAPI31:
      return "https://spec.openapis.org/oas/3.1/dialect/base";
    case SchemaDialect::OpenAPI32:
      return "https://spec.openapis.org/oas/3.2/dialect/2025-09-17";
  }

  std::unreachable();
}

// The base dialect of a dialect on the ladder
[[nodiscard]] inline auto conversion_base_dialect(const SchemaDialect dialect)
    -> sourcemeta::core::SchemaBaseDialect {
  switch (dialect) {
    case SchemaDialect::Draft3:
      return sourcemeta::core::SchemaBaseDialect::JSON_SCHEMA_DRAFT_3;
    // The OpenAPI v3.0 Schema Object describes itself through a Draft 4
    // meta-schema, so Draft 4 is what it rests on
    case SchemaDialect::OpenAPI30:
    case SchemaDialect::Draft4:
      return sourcemeta::core::SchemaBaseDialect::JSON_SCHEMA_DRAFT_4;
    case SchemaDialect::Draft6:
      return sourcemeta::core::SchemaBaseDialect::JSON_SCHEMA_DRAFT_6;
    case SchemaDialect::Draft7:
      return sourcemeta::core::SchemaBaseDialect::JSON_SCHEMA_DRAFT_7;
    case SchemaDialect::Draft201909:
      return sourcemeta::core::SchemaBaseDialect::JSON_SCHEMA_2019_09;
    case SchemaDialect::Draft202012:
    // An OpenAPI Schema Object dialect declares the vocabularies of 2020-12
    // alongside one of its own, so 2020-12 is what it rests on
    case SchemaDialect::OpenAPI31:
    case SchemaDialect::OpenAPI32:
      return sourcemeta::core::SchemaBaseDialect::JSON_SCHEMA_2020_12;
  }

  std::unreachable();
}

// Where a dialect sits among the targets, which is nothing at all for one that
// is only ever converted from
[[nodiscard]] inline auto conversion_rung(const SchemaDialect dialect)
    -> std::optional<std::size_t> {
  const auto match{std::ranges::find(SCHEMA_CONVERSION_TARGETS, dialect)};
  if (match == SCHEMA_CONVERSION_TARGETS.cend()) {
    return std::nullopt;
  }

  return static_cast<std::size_t>(
      std::ranges::distance(SCHEMA_CONVERSION_TARGETS.cbegin(), match));
}

// The dialects a schema declaring a dialect can be converted into, oldest
// first, including the one it declares, as what it references may still be
// older than that.
//
// A dialect nothing converts into is one the upgrade order starts from rather
// than passes through, so every target is newer than it and all of them are
// reachable. More than one dialect can start the order this way, which is why
// they are not ranked against each other: Draft 3 and the OpenAPI 3.0 Schema
// Object both convert into Draft 4 and upwards, and neither converts into the
// other
[[nodiscard]] inline auto dialect_conversions(const SchemaDialect dialect)
    -> std::span<const SchemaDialect> {
  return std::span<const SchemaDialect>{SCHEMA_CONVERSION_TARGETS}.subspan(
      conversion_rung(dialect).value_or(0));
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

  const auto declared{schema_dialect(dialect)};
  return declares_vocabularies && declared.has_value() &&
         (declared.value() == SchemaDialect::Draft201909 ||
          declared.value() == SchemaDialect::Draft202012 ||
          declared.value() == SchemaDialect::OpenAPI31 ||
          declared.value() == SchemaDialect::OpenAPI32);
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

// The selection bit of a conversion into a dialect, which a dialect nothing
// converts into has none of
[[nodiscard]] inline auto conversion_selector(const SchemaDialect dialect)
    -> std::uint8_t {
  const auto rung{conversion_rung(dialect)};
  assert(rung.has_value());
  return static_cast<std::uint8_t>(rung.value());
}

// The selection bits of every conversion a schema declaring a dialect gets.
// A meta-schema gets none, as a conversion cannot restate the dialect that a
// meta-schema describes
[[nodiscard]] inline auto conversion_selection(const std::string_view dialect,
                                               const bool is_metaschema)
    -> std::uint32_t {
  const auto declared{schema_dialect(dialect)};
  if (is_metaschema || !declared.has_value()) {
    return 0;
  }

  std::uint32_t result{0};
  for (const auto target : dialect_conversions(declared.value())) {
    result |= std::uint32_t{1} << conversion_selector(target);
  }

  return result;
}

// The artifact holding a schema converted into a dialect, which is its closure
// bundled and then converted as one document
[[nodiscard]] inline auto conversion_artifact(const SchemaDialect dialect)
    -> std::string {
  std::string result{"bundle-"};
  result.append(conversion_name(dialect));
  return result;
}

} // namespace sourcemeta::one

#endif
