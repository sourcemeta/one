#!/bin/sh

set -o errexit
set -o nounset

TMP="$(mktemp -d)"
clean() { rm -rf "$TMP"; }
trap clean EXIT

mkdir "$TMP/schemas"
cat << 'SCHEMA' > "$TMP/schemas/example.json"
{
  "$schema": "https://json-schema.org/draft/2020-12/schema",
  "type": "string"
}
SCHEMA

cat << EOC > "$TMP/one.json"
{
  "url": "http://localhost:8000/",
  "contents": {
    "example": {
      "path": "./schemas"
    }
  }
}
EOC

"$1" --skip-banner "$TMP/one.json" "$TMP/output" > "$TMP/output.txt" && CODE="$?" || CODE="$?"
test "$CODE" = "1" || exit 1

cat << EOE > "$TMP/expected.txt"
error: The instance URL must name an origin and nothing more. Serve the instance on a domain of its own, and name a collection to give its schemas a prefix
  at url http://localhost:8000/
  at path $(realpath "$TMP")/one.json
EOE

diff "$TMP/output.txt" "$TMP/expected.txt"
