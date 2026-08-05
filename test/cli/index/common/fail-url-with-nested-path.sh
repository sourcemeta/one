#!/bin/sh

set -o errexit
set -o nounset

TMP="$(mktemp -d)"
clean() { rm -rf "$TMP"; }
trap clean EXIT

cat << EOF > "$TMP/one.json"
{
  "url": "http://localhost:8000/v1/catalog",
  "contents": {
    "example": {
      "path": "./schemas"
    }
  }
}
EOF

mkdir "$TMP/schemas"

cat << 'EOF' > "$TMP/schemas/example.json"
{
  "$schema": "https://json-schema.org/draft/2020-12/schema",
  "type": "string"
}
EOF

"$1" --skip-banner "$TMP/one.json" "$TMP/output" > "$TMP/output.txt" && CODE="$?" || CODE="$?"
test "$CODE" = "1" || exit 1

cat << EOF > "$TMP/expected.txt"
error: The instance URL must contain no path
  at url http://localhost:8000/v1/catalog
  at path $(realpath "$TMP")/one.json
EOF

diff "$TMP/output.txt" "$TMP/expected.txt"
