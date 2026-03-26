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

cat << 'EOF' > "$TMP/schemas/test.json"
{
  "$schema": "https://example.com/my-metaschema",
  "$id": "https://example.com/test"
}
EOF

cat << 'EOF' > "$TMP/schemas/my-metaschema.json"
{
  "$schema": "https://example.com/my-metaschema",
  "$id": "https://example.com/my-metaschema"
}
EOF

"$1" --skip-banner "$TMP/one.json" "$TMP/output" --concurrency 1 2> "$TMP/output.txt" && CODE="$?" || CODE="$?"
test "$CODE" = "1" || exit 1

remove_threads_information() {
  expr='s/ \[[^]]*[^a-z-][^]]*\]//g'
  if [ "$(uname -s)" = "Darwin" ]; then
    sed -i '' "$expr" "$1"
  else
    sed -i "$expr" "$1"
  fi
}

remove_threads_information "$TMP/output.txt"

cat << EOF > "$TMP/expected1.txt"
Writing output to: $(realpath "$TMP")/output
Using configuration: $(realpath "$TMP")/one.json
Detecting: $(realpath "$TMP")/schemas/test.json (#1)
Detecting: $(realpath "$TMP")/schemas/my-metaschema.json (#2)
( 50%) Resolving: test.json
(100%) Resolving: my-metaschema.json
(  2%) Producing: configuration.json
(  5%) Producing: version.json
(  8%) Producing: explorer/%/404.metapack
( 11%) Producing: schemas/example/schemas/my-metaschema/%/schema.metapack
error: Could not resolve the metaschema of the schema
  at identifier https://sourcemeta.com/example/schemas/my-metaschema
  at path $(realpath "$TMP")/schemas/my-metaschema.json

Did you forget to register a schema with such URI?
EOF

cat << EOF > "$TMP/expected2.txt"
Writing output to: $(realpath "$TMP")/output
Using configuration: $(realpath "$TMP")/one.json
Detecting: $(realpath "$TMP")/schemas/my-metaschema.json (#1)
Detecting: $(realpath "$TMP")/schemas/test.json (#2)
( 50%) Resolving: my-metaschema.json
(100%) Resolving: test.json
(  2%) Producing: configuration.json
(  5%) Producing: version.json
(  8%) Producing: explorer/%/404.metapack
( 11%) Producing: schemas/example/schemas/my-metaschema/%/schema.metapack
error: Could not resolve the metaschema of the schema
  at identifier https://sourcemeta.com/example/schemas/my-metaschema
  at path $(realpath "$TMP")/schemas/my-metaschema.json

Did you forget to register a schema with such URI?
EOF

diff "$TMP/output.txt" "$TMP/expected1.txt" || diff "$TMP/output.txt" "$TMP/expected2.txt"
