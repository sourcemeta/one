#!/bin/sh

set -o errexit
set -o nounset

TMP="$(mktemp -d)"
clean() { rm -rf "$TMP"; }
trap clean EXIT

cat << EOF > "$TMP/one.json"
{
  "url": "https://sourcemeta.com/",
  "contents": {
    "example": {
      "contents": {
        "schemas": {
          "baseUri": "https://example.com/",
          "path": "./schemas"
        }
      }
    }
  }
}
EOF

mkdir "$TMP/schemas"

cat << 'EOF' > "$TMP/schemas/custom-meta.json"
{
  "$schema": "https://json-schema.org/draft/2020-12/schema",
  "$id": "https://example.com/custom-meta",
  "$vocabulary": "not-an-object"
}
EOF

cat << 'EOF' > "$TMP/schemas/test.json"
{
  "$schema": "https://example.com/custom-meta",
  "$id": "https://example.com/test",
  "type": "string"
}
EOF


"$1" --skip-banner --deterministic "$TMP/one.json" "$TMP/output" --concurrency 1 > "$TMP/output.txt" && CODE="$?" || CODE="$?"
test "$CODE" = "1" || exit 1

cat << EOF > "$TMP/expected.txt"
error: The schema does not adhere to its metaschema
The value was expected to be of type object but it was of type string
  at instance location "/\$vocabulary"
  at evaluate path "/allOf/0/\$ref/properties/\$vocabulary/type"
The object value was expected to validate against the 9 defined properties subschemas
  at instance location ""
  at evaluate path "/allOf/0/\$ref/properties"
The object value was expected to validate against the referenced schema
  at instance location ""
  at evaluate path "/allOf/0/\$ref"
The object value was expected to validate against the 7 given subschemas
  at instance location ""
  at evaluate path "/allOf"
EOF

diff "$TMP/output.txt" "$TMP/expected.txt"
