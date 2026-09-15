#ifndef SOURCEMETA_ONE_ENTERPRISE_SERVER_CONVERSION_H_
#define SOURCEMETA_ONE_ENTERPRISE_SERVER_CONVERSION_H_

#include <sourcemeta/one/enterprise_conversion.h>
#include <sourcemeta/one/router.h>

namespace sourcemeta::one {

// Whether a schema declares a dialect that is not an official one
[[nodiscard]] inline auto
declares_custom_dialect(const RouterAction &action,
                        const ResolvedArtifact &schema) -> bool {
  const auto contents{action.artifact_read_json(schema)};
  const auto *dialect{contents.has_value() && contents->is_object()
                          ? contents->try_at("$schema")
                          : nullptr};
  return dialect != nullptr && dialect->is_string() &&
         !official_dialect(dialect->to_string()).has_value();
}

} // namespace sourcemeta::one

#endif
