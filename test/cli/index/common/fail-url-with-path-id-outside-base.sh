#!/bin/sh

set -o errexit
set -o nounset

TMP="$(mktemp -d)"
clean() { rm -rf "$TMP"; }
trap clean EXIT

cat << EOF > "$TMP/one.json"
{
  "url": "https://example.com/schemas",
  "contents": {
    "test": {
      "path": "./schemas"
    }
  }
}
EOF

mkdir "$TMP/schemas"

cat << 'EOF' > "$TMP/schemas/foo.json"
{
  "$schema": "https://json-schema.org/draft/2020-12/schema",
  "$id": "https://example.com/other/foo",
  "type": "string"
}
EOF


"$1" --skip-banner --deterministic "$TMP/one.json" "$TMP/output" --concurrency 1 > "$TMP/output.txt" && CODE="$?" || CODE="$?"
test "$CODE" = "1" || exit 1

cat << EOF > "$TMP/expected.txt"
error: The schema identifier is not relative to the corresponding base
  at path $(realpath "$TMP")/schemas/foo.json
  at identifier https://example.com/other/foo
  with base https://example.com/schemas
EOF

diff "$TMP/output.txt" "$TMP/expected.txt"
