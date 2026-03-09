#!/bin/sh

set -o errexit
set -o nounset

if [ "$(uname -s)" = "Darwin" ]
then
  IS_DARWIN=1
else
  IS_DARWIN=0
fi

nanoseconds() {
  if [ "$IS_DARWIN" = "1" ]
  then
    perl -MTime::HiRes=time -e 'printf "%d\n", time * 1000000000'
  else
    date +%s%N
  fi
}

generate_schema() {
  cat << EOF > "$TMP/schemas/$1.json"
{
  "\$schema": "http://json-schema.org/draft-07/schema#",
  "\$id": "https://example.com/$1"
}
EOF
}

generate_schema_with_ref() {
  cat << EOF > "$TMP/schemas/$1.json"
{
  "\$schema": "http://json-schema.org/draft-07/schema#",
  "\$id": "https://example.com/$1",
  "properties": {
    "foo": {
      "\$ref": "https://example.com/$2"
    }
  }
}
EOF
}

measure() {
  START="$(nanoseconds)"
  "$INDEX" --skip-banner "$TMP/one.json" "$TMP/output" --time >&2 2>/dev/null
  END="$(nanoseconds)"
  echo "$(( (END - START) / 1000000 ))"
}

INDEX="$1"
TMP="$(mktemp -d)"
clean() { rm -rf "$TMP"; }
trap clean EXIT

cat << EOF > "$TMP/one.json"
{
  "url": "https://sourcemeta.com/",
  "contents": {
    "schemas": {
      "baseUri": "https://example.com/",
      "path": "./schemas"
    }
  }
}
EOF

mkdir "$TMP/schemas"

# Measure adding one schema to an empty registry
echo "Measuring: add one schema (0 existing)..." >&2
generate_schema "schema-0"
RESULT_0_TO_1="$(measure)"
echo "  Result: ${RESULT_0_TO_1}ms" >&2

# Measure a fully cached rebuild (nothing changed, 1 schema)
echo "Measuring: cached rebuild (1 existing)..." >&2
RESULT_CACHED_1="$(measure)"
echo "  Result: ${RESULT_CACHED_1}ms" >&2

# Fill up to 100 schemas
echo "Filling registry to 100 schemas..." >&2
index=1
while [ "$index" -lt 100 ]
do
  generate_schema "schema-$index"
  index=$((index + 1))
done
echo "Reindexing outside measurements..." >&2
"$INDEX" --skip-banner "$TMP/one.json" "$TMP/output" > /dev/null 2>&1

# Measure adding one schema (with a $ref) to a 100-schema registry
echo "Measuring: add one schema (100 existing)..." >&2
generate_schema_with_ref "schema-100" "schema-0"
RESULT_100_TO_101="$(measure)"
echo "  Result: ${RESULT_100_TO_101}ms" >&2

# Measure a fully cached rebuild (nothing changed, 101 schemas)
echo "Measuring: cached rebuild (101 existing)..." >&2
RESULT_CACHED_101="$(measure)"
echo "  Result: ${RESULT_CACHED_101}ms" >&2

# Fill up to 1000 schemas
echo "Filling registry to 1000 schemas..." >&2
index=101
while [ "$index" -lt 1000 ]
do
  generate_schema "schema-$index"
  index=$((index + 1))
done
echo "Reindexing outside measurements..." >&2
"$INDEX" --skip-banner "$TMP/one.json" "$TMP/output" > /dev/null 2>&1

# Measure adding one schema (with a $ref) to a 1000-schema registry
echo "Measuring: add one schema (1000 existing)..." >&2
generate_schema_with_ref "schema-1000" "schema-0"
RESULT_1000_TO_1001="$(measure)"
echo "  Result: ${RESULT_1000_TO_1001}ms" >&2

# Measure a fully cached rebuild (nothing changed, 1001 schemas)
echo "Measuring: cached rebuild (1001 existing)..." >&2
RESULT_CACHED_1001="$(measure)"
echo "  Result: ${RESULT_CACHED_1001}ms" >&2

cat << EOF
[
  {
    "name": "Add one schema (0 existing)",
    "unit": "ms",
    "value": $RESULT_0_TO_1
  },
  {
    "name": "Add one schema (100 existing)",
    "unit": "ms",
    "value": $RESULT_100_TO_101
  },
  {
    "name": "Add one schema (1000 existing)",
    "unit": "ms",
    "value": $RESULT_1000_TO_1001
  },
  {
    "name": "Cached rebuild (1 existing)",
    "unit": "ms",
    "value": $RESULT_CACHED_1
  },
  {
    "name": "Cached rebuild (101 existing)",
    "unit": "ms",
    "value": $RESULT_CACHED_101
  },
  {
    "name": "Cached rebuild (1001 existing)",
    "unit": "ms",
    "value": $RESULT_CACHED_1001
  }
]
EOF
