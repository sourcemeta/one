#ifndef SOURCEMETA_ONE_ENTERPRISE_SERVER_CONVERSION_H_
#define SOURCEMETA_ONE_ENTERPRISE_SERVER_CONVERSION_H_

#include <sourcemeta/one/enterprise_conversion.h>
#include <sourcemeta/one/router.h>

#include <sourcemeta/core/jsonschema.h>

#include <optional> // std::optional, std::nullopt

namespace sourcemeta::one {

// What this instance knows about the dialect a schema is served as. The
// artifact records it rather than the document, as a schema whose dialect
// reserves no keyword to declare one carries it nowhere in the document
struct ServedSchemaDialect {
  // Nothing at all when the dialect is not a rung of the upgrade ladder
  std::optional<SchemaDialect> ladder;
  // Whether the dialect is one of the catalog's own schemas rather than one
  // this implementation ships, which is what leaves it unconvertible
  bool custom;
};

[[nodiscard]] inline auto served_schema_dialect(const RouterAction &action,
                                                const ResolvedArtifact &schema)
    -> ServedSchemaDialect {
  const auto dialect{action.artifact_dialect(schema)};
  if (!dialect.has_value()) {
    return {.ladder = std::nullopt, .custom = true};
  }

  return {.ladder = schema_dialect(dialect.value()),
          .custom = !sourcemeta::core::schema_is_known(dialect.value())};
}

} // namespace sourcemeta::one

#endif
