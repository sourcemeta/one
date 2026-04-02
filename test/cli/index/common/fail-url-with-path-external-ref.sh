#!/bin/sh

set -o errexit
set -o nounset

TMP="$(mktemp -d)"
clean() { rm -rf "$TMP"; }
trap clean EXIT

cat << EOF > "$TMP/one.json"
{
  "url": "https://example.com/schemas",
  "html": false,
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
  "$id": "https://example.com/schemas/foo",
  "allOf": [ { "$ref": "https://other.example.com/external" } ]
}
EOF

remove_threads_information() {
  expr='s/ \[[^]]*[^a-z-][^]]*\]//g'
  if [ "$(uname -s)" = "Darwin" ]; then
    sed -i '' "$expr" "$1"
  else
    sed -i "$expr" "$1"
  fi
}

"$1" --skip-banner "$TMP/one.json" "$TMP/output" --concurrency 1 2> "$TMP/output.txt" && CODE="$?" || CODE="$?"
test "$CODE" = "1" || exit 1
remove_threads_information "$TMP/output.txt"

cat << EOF > "$TMP/expected.txt"
Writing output to: $(realpath "$TMP")/output
Using configuration: $(realpath "$TMP")/one.json
Detecting: $(realpath "$TMP")/schemas/foo.json (#1)
(100%) Resolving: foo.json
(  5%) Producing: configuration.json
( 11%) Producing: version.json
( 17%) Producing: schemas/test/foo/%/schema.metapack
( 23%) Producing: schemas/test/foo/%/dependencies.metapack
error: Could not resolve the reference to an external schema
  at identifier https://other.example.com/external
  at path $(realpath "$TMP")/schemas/foo.json

Did you forget to register a schema with such URI?
EOF

diff "$TMP/output.txt" "$TMP/expected.txt"
