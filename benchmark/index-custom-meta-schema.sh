#!/bin/sh

set -o errexit
set -o nounset

if [ "$#" -ne 2 ]
then
  echo "Usage: $0 <path/to/sourcemeta-one-index> <schema-count>" 1>&2
  exit 1
fi

INDEX="$1"
COUNT="$2"

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

cat << 'EOF' > "$TMP/schemas/meta.json"
{
  "$schema": "https://json-schema.org/draft/2020-12/schema",
  "$id": "https://example.com/meta",
  "$vocabulary": {
    "https://json-schema.org/draft/2020-12/vocab/core": true,
    "https://json-schema.org/draft/2020-12/vocab/applicator": true,
    "https://json-schema.org/draft/2020-12/vocab/unevaluated": true,
    "https://json-schema.org/draft/2020-12/vocab/validation": true,
    "https://json-schema.org/draft/2020-12/vocab/meta-data": true,
    "https://json-schema.org/draft/2020-12/vocab/format-assertion": true,
    "https://json-schema.org/draft/2020-12/vocab/content": true
  },
  "$dynamicAnchor": "meta",
  "title": "Benchmark Meta-Schema",
  "description": "A custom meta-schema used to stress the indexer's resolver",
  "type": [ "object", "boolean" ],
  "allOf": [
    { "$ref": "https://json-schema.org/draft/2020-12/meta/core" },
    { "$ref": "https://json-schema.org/draft/2020-12/meta/applicator" },
    { "$ref": "https://json-schema.org/draft/2020-12/meta/unevaluated" },
    { "$ref": "https://json-schema.org/draft/2020-12/meta/validation" },
    { "$ref": "https://json-schema.org/draft/2020-12/meta/meta-data" },
    { "$ref": "https://json-schema.org/draft/2020-12/meta/format-assertion" },
    { "$ref": "https://json-schema.org/draft/2020-12/meta/content" }
  ]
}
EOF

echo "Filling registry to ${COUNT} schemas (custom meta-schema)..." >&2
index=0
while [ "$index" -lt "$COUNT" ]
do
  cat << EOF > "$TMP/schemas/schema-$index.json"
{
  "\$schema": "https://example.com/meta",
  "\$id": "https://example.com/schema-$index"
}
EOF
  index=$((index + 1))
done

echo "Measuring: index ${COUNT} schemas (custom meta-schema)..." >&2
START="$(nanoseconds)"
"$INDEX" --skip-banner --maximum-direct-directory-entries 0 \
  "$TMP/one.json" "$TMP/output" --time >&2 2>/dev/null
END="$(nanoseconds)"
RESULT="$(( (END - START) / 1000000 ))"
echo "  Result: ${RESULT}ms" >&2

cat << EOF
[
  {
    "name": "Index ${COUNT} schemas (custom meta-schema)",
    "unit": "ms",
    "value": ${RESULT}
  }
]
EOF
