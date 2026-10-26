#ifndef SOURCEMETA_ONE_ENTERPRISE_SERVER_CONVERSION_H_
#define SOURCEMETA_ONE_ENTERPRISE_SERVER_CONVERSION_H_

#include <sourcemeta/one/enterprise_conversion.h>
#include <sourcemeta/one/router.h>

#include <optional> // std::optional, std::nullopt

namespace sourcemeta::one {

// The dialect on the upgrade ladder a schema declares, which is nothing at
// all when the dialect it declares is not one of them
[[nodiscard]] inline auto
declared_schema_dialect(const RouterAction &action,
                        const ResolvedArtifact &schema)
    -> std::optional<SchemaDialect> {
  const auto contents{action.artifact_read_json(schema)};
  const auto *dialect{contents.has_value() && contents->is_object()
                          ? contents->try_at("$schema")
                          : nullptr};
  if (dialect == nullptr || !dialect->is_string()) {
    return std::nullopt;
  }

  return schema_dialect(dialect->to_string());
}

} // namespace sourcemeta::one

#endif
