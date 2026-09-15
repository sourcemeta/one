#ifndef SOURCEMETA_ONE_INDEX_METASCHEMA_H_
#define SOURCEMETA_ONE_INDEX_METASCHEMA_H_

#include <sourcemeta/core/json.h>
#include <sourcemeta/core/jsonschema.h>

// Whether a schema declares the vocabularies of a dialect, which only these
// dialects give meaning to
static auto
declares_vocabulary(const sourcemeta::core::JSON &schema,
                    const sourcemeta::core::SchemaBaseDialect base_dialect)
    -> bool {
  if (base_dialect !=
          sourcemeta::core::SchemaBaseDialect::JSON_SCHEMA_2020_12 &&
      base_dialect !=
          sourcemeta::core::SchemaBaseDialect::JSON_SCHEMA_2020_12_HYPER &&
      base_dialect !=
          sourcemeta::core::SchemaBaseDialect::JSON_SCHEMA_2019_09 &&
      base_dialect !=
          sourcemeta::core::SchemaBaseDialect::JSON_SCHEMA_2019_09_HYPER) {
    return false;
  }

  const auto *vocabulary{schema.is_object() ? schema.try_at("$vocabulary")
                                            : nullptr};
  return vocabulary != nullptr && vocabulary->is_object();
}

#endif
