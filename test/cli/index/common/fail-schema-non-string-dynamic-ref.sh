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
  "$schema": "https://json-schema.org/draft/2020-12/schema",
  "$id": "https://example.com/test",
  "$dynamicRef": 123
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

cat << EOF > "$TMP/expected.txt"
Writing output to: $(realpath "$TMP")/output
Using configuration: $(realpath "$TMP")/one.json
Detecting: $(realpath "$TMP")/schemas/test.json (#1)
(100%) Resolving: test.json
(  4%) Producing: configuration.json
(  8%) Producing: version.json
( 13%) Producing: explorer/%/404.metapack
( 17%) Producing: schemas/example/schemas/test/%/schema.metapack
error: The schema does not adhere to its metaschema
The value was expected to be of type string but it was of type integer
  at instance location "/\$dynamicRef"
  at evaluate path "/allOf/0/\$ref/properties/\$dynamicRef/\$ref/type"
The integer value was expected to validate against the referenced schema
  at instance location "/\$dynamicRef"
  at evaluate path "/allOf/0/\$ref/properties/\$dynamicRef/\$ref"
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
