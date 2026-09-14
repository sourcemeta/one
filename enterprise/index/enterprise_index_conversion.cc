#include <sourcemeta/one/enterprise_index.h>

#include <sourcemeta/blaze/alterschema.h>
#include <sourcemeta/blaze/foundation.h>

#include <sourcemeta/core/json.h>
#include <sourcemeta/core/jsonpointer.h>

#include <array>       // std::array
#include <cassert>     // assert
#include <cstddef>     // std::size_t
#include <cstdint>     // std::uint32_t
#include <memory>      // std::unique_ptr, std::make_unique
#include <optional>    // std::optional, std::nullopt
#include <span>        // std::span
#include <stdexcept>   // std::runtime_error
#include <string_view> // std::string_view
#include <utility>     // std::unreachable

namespace {

struct OfficialDialectSpelling {
  std::string_view uri;
  sourcemeta::one::SchemaDialect dialect;
};

// Every spelling of an official dialect that Blaze recognises
constexpr std::array<OfficialDialectSpelling, 24> OFFICIAL_DIALECT_SPELLINGS{{
    {.uri = "http://json-schema.org/draft-03/schema#",
     .dialect = sourcemeta::one::SchemaDialect::Draft3},
    {.uri = "http://json-schema.org/draft-03/schema",
     .dialect = sourcemeta::one::SchemaDialect::Draft3},
    {.uri = "https://json-schema.org/draft-03/schema#",
     .dialect = sourcemeta::one::SchemaDialect::Draft3},
    {.uri = "https://json-schema.org/draft-03/schema",
     .dialect = sourcemeta::one::SchemaDialect::Draft3},
    {.uri = "http://json-schema.org/draft-04/schema#",
     .dialect = sourcemeta::one::SchemaDialect::Draft4},
    {.uri = "http://json-schema.org/draft-04/schema",
     .dialect = sourcemeta::one::SchemaDialect::Draft4},
    {.uri = "https://json-schema.org/draft-04/schema#",
     .dialect = sourcemeta::one::SchemaDialect::Draft4},
    {.uri = "https://json-schema.org/draft-04/schema",
     .dialect = sourcemeta::one::SchemaDialect::Draft4},
    {.uri = "http://json-schema.org/draft-06/schema#",
     .dialect = sourcemeta::one::SchemaDialect::Draft6},
    {.uri = "http://json-schema.org/draft-06/schema",
     .dialect = sourcemeta::one::SchemaDialect::Draft6},
    {.uri = "https://json-schema.org/draft-06/schema#",
     .dialect = sourcemeta::one::SchemaDialect::Draft6},
    {.uri = "https://json-schema.org/draft-06/schema",
     .dialect = sourcemeta::one::SchemaDialect::Draft6},
    {.uri = "http://json-schema.org/draft-07/schema#",
     .dialect = sourcemeta::one::SchemaDialect::Draft7},
    {.uri = "http://json-schema.org/draft-07/schema",
     .dialect = sourcemeta::one::SchemaDialect::Draft7},
    {.uri = "https://json-schema.org/draft-07/schema#",
     .dialect = sourcemeta::one::SchemaDialect::Draft7},
    {.uri = "https://json-schema.org/draft-07/schema",
     .dialect = sourcemeta::one::SchemaDialect::Draft7},
    {.uri = "https://json-schema.org/draft/2019-09/schema",
     .dialect = sourcemeta::one::SchemaDialect::Draft201909},
    {.uri = "https://json-schema.org/draft/2019-09/schema#",
     .dialect = sourcemeta::one::SchemaDialect::Draft201909},
    {.uri = "http://json-schema.org/draft/2019-09/schema",
     .dialect = sourcemeta::one::SchemaDialect::Draft201909},
    {.uri = "http://json-schema.org/draft/2019-09/schema#",
     .dialect = sourcemeta::one::SchemaDialect::Draft201909},
    {.uri = "https://json-schema.org/draft/2020-12/schema",
     .dialect = sourcemeta::one::SchemaDialect::Draft202012},
    {.uri = "https://json-schema.org/draft/2020-12/schema#",
     .dialect = sourcemeta::one::SchemaDialect::Draft202012},
    {.uri = "http://json-schema.org/draft/2020-12/schema",
     .dialect = sourcemeta::one::SchemaDialect::Draft202012},
    {.uri = "http://json-schema.org/draft/2020-12/schema#",
     .dialect = sourcemeta::one::SchemaDialect::Draft202012},
}};

// Every dialect a schema can be converted into, oldest first
constexpr std::array<sourcemeta::one::SchemaDialect, 5> CONVERSION_TARGETS{
    {sourcemeta::one::SchemaDialect::Draft4,
     sourcemeta::one::SchemaDialect::Draft6,
     sourcemeta::one::SchemaDialect::Draft7,
     sourcemeta::one::SchemaDialect::Draft201909,
     sourcemeta::one::SchemaDialect::Draft202012}};

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

auto official_dialect(const std::string_view uri)
    -> std::optional<SchemaDialect> {
  for (const auto &spelling : OFFICIAL_DIALECT_SPELLINGS) {
    if (spelling.uri == uri) {
      return spelling.dialect;
    }
  }

  return std::nullopt;
}

auto conversion_name(const SchemaDialect dialect) -> std::string_view {
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

auto conversion_uri(const SchemaDialect dialect) -> std::string_view {
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

auto conversion_base_dialect(const SchemaDialect dialect)
    -> sourcemeta::blaze::SchemaBaseDialect {
  switch (dialect) {
    case SchemaDialect::Draft3:
      return sourcemeta::blaze::SchemaBaseDialect::JSON_SCHEMA_DRAFT_3;
    case SchemaDialect::Draft4:
      return sourcemeta::blaze::SchemaBaseDialect::JSON_SCHEMA_DRAFT_4;
    case SchemaDialect::Draft6:
      return sourcemeta::blaze::SchemaBaseDialect::JSON_SCHEMA_DRAFT_6;
    case SchemaDialect::Draft7:
      return sourcemeta::blaze::SchemaBaseDialect::JSON_SCHEMA_DRAFT_7;
    case SchemaDialect::Draft201909:
      return sourcemeta::blaze::SchemaBaseDialect::JSON_SCHEMA_2019_09;
    case SchemaDialect::Draft202012:
      return sourcemeta::blaze::SchemaBaseDialect::JSON_SCHEMA_2020_12;
  }

  std::unreachable();
}

auto dialect_conversions(const SchemaDialect dialect)
    -> std::span<const SchemaDialect> {
  return std::span<const SchemaDialect>{CONVERSION_TARGETS}.subspan(
      static_cast<std::size_t>(dialect));
}

auto conversion_selection(const std::string_view dialect) -> std::uint32_t {
  const auto official{official_dialect(dialect)};
  if (!official.has_value()) {
    return 0;
  }

  std::uint32_t result{0};
  for (const auto target : dialect_conversions(official.value())) {
    result |= std::uint32_t{1} << conversion_selector(target);
  }

  return result;
}

auto conversion_selector(const SchemaDialect dialect) -> std::uint8_t {
  assert(dialect != SchemaDialect::Draft3);
  return static_cast<std::uint8_t>(static_cast<std::uint8_t>(dialect) - 1);
}

auto convert_schema(sourcemeta::core::JSON &schema, const SchemaDialect target,
                    const bool is_metaschema,
                    const sourcemeta::blaze::SchemaResolver &resolver) -> void {
  // A transformer is never shared between threads
  thread_local std::array<std::unique_ptr<sourcemeta::blaze::SchemaTransformer>,
                          CONVERSION_TARGETS.size()>
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
}

} // namespace sourcemeta::one
