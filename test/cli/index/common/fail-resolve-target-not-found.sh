#!/bin/sh

set -o errexit
set -o nounset

TMP="$(mktemp -d)"
clean() { rm -rf "$TMP"; }
trap clean EXIT

mkdir "$TMP/meta"
mkdir "$TMP/schemas"

cat << 'EOF' > "$TMP/meta/custom.json"
{
  "$schema": "https://json-schema.org/draft/2020-12/schema",
  "$vocabulary": {
    "https://json-schema.org/draft/2020-12/vocab/core": true,
    "https://json-schema.org/draft/2020-12/vocab/applicator": true,
    "https://json-schema.org/draft/2020-12/vocab/validation": true
  },
  "$dynamicAnchor": "meta"
}
EOF

cat << 'EOF' > "$TMP/schemas/sample.json"
{
  "$schema": "urn:example:custom-meta",
  "type": "string"
}
EOF

cat << EOF > "$TMP/one.json"
{
  "url": "http://localhost:8000",
  "html": false,
  "contents": {
    "meta": {
      "baseUri": "https://example.com/foo",
      "path": "./meta"
    },
    "example": {
      "baseUri": "https://example.com/foo",
      "path": "./schemas",
      "resolve": {
        "urn:example:custom-meta": "https://example.com/foo/custom"
      }
    }
  }
}
EOF

"$1" --skip-banner "$TMP/one.json" "$TMP/output" > "$TMP/output.txt" && CODE="$?" || CODE="$?"
test "$CODE" = "1" || exit 1

cat << EOF > "$TMP/expected.txt"
error: Could not resolve the metaschema of the schema
  at identifier http://localhost:8000/example/custom
  at path $(realpath "$TMP")/schemas/sample.json

Did you forget to register a schema with such URI?
EOF

diff "$TMP/output.txt" "$TMP/expected.txt"
