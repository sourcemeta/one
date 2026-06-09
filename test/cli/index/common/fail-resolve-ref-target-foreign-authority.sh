#!/bin/sh

set -o errexit
set -o nounset

TMP="$(mktemp -d)"
clean() { rm -rf "$TMP"; }
trap clean EXIT

mkdir "$TMP/meta"
mkdir "$TMP/schemas"

cat << 'EOF' > "$TMP/meta/target.json"
{
  "$schema": "https://json-schema.org/draft/2020-12/schema",
  "type": "string"
}
EOF

cat << 'EOF' > "$TMP/schemas/sample.json"
{
  "$schema": "https://json-schema.org/draft/2020-12/schema",
  "$ref": "urn:example:target"
}
EOF

cat << EOF > "$TMP/one.json"
{
  "url": "http://localhost:8000",
  "html": false,
  "contents": {
    "meta": {
      "path": "./meta"
    },
    "example": {
      "path": "./schemas",
      "resolve": {
        "urn:example:target": "https://other.example.com/meta/target.json"
      }
    }
  }
}
EOF

"$1" --skip-banner "$TMP/one.json" "$TMP/output" > "$TMP/output.txt" && CODE="$?" || CODE="$?"
test "$CODE" = "1" || exit 1

cat << EOF > "$TMP/expected.txt"
error: Could not resolve the reference to an external schema
  at identifier https://other.example.com/meta/target
  at path $(realpath "$TMP")/schemas/sample.json

Did you forget to register a schema with such URI?
EOF

diff "$TMP/output.txt" "$TMP/expected.txt"
