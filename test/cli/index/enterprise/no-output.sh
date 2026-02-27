#!/bin/sh

set -o errexit
set -o nounset

TMP="$(mktemp -d)"
clean() { rm -rf "$TMP"; }
trap clean EXIT

cat << EOF > "$TMP/one.json"
{
  "url": "http://localhost:8000",
  "contents": {
    "example": {
      "contents": {
        "schemas": {
          "baseUri": "https://example.com/schemas",
          "path": "./schemas/example/folder"
        }
      }
    }
  }
}
EOF

"$1" --skip-banner "$TMP/one.json" 2> "$TMP/output.txt" && CODE="$?" || CODE="$?"
test "$CODE" = "1" || exit 1

cat << EOF > "$TMP/expected.txt"
Usage: sourcemeta-one-index <one.json> <path/to/output/directory>
EOF

diff "$TMP/output.txt" "$TMP/expected.txt"
