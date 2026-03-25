#!/bin/sh

set -o errexit
set -o nounset

TMP="$(mktemp -d)"
clean() { rm -rf "$TMP"; }
trap clean EXIT

cat << EOF > "$TMP/one.json"
{
  "url": "http://localhost:8000",
  "html": false,
  "contents": {
    "example": {
      "baseUri": "https://example.com",
      "path": "./schemas",
      "x-sourcemeta-one:evaluate": false
    }
  }
}
EOF

mkdir "$TMP/schemas"

# A custom metaschema that requires an unknown vocabulary
cat << 'EOF' > "$TMP/schemas/custom-meta.json"
{
  "$schema": "https://json-schema.org/draft/2020-12/schema",
  "$id": "https://example.com/custom-meta",
  "$vocabulary": {
    "https://json-schema.org/draft/2020-12/vocab/core": true,
    "https://json-schema.org/draft/2020-12/vocab/applicator": true,
    "https://json-schema.org/draft/2020-12/vocab/validation": true,
    "https://example.com/vocab/totally-unknown": true
  }
}
EOF

# A schema that uses the custom metaschema
cat << 'EOF' > "$TMP/schemas/test.json"
{
  "$schema": "https://example.com/custom-meta",
  "$id": "https://example.com/test",
  "type": "string"
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

cat << EOF > "$TMP/expected1.txt"
Writing output to: $(realpath "$TMP")/output
Using configuration: $(realpath "$TMP")/one.json
Detecting: $(realpath "$TMP")/schemas/custom-meta.json (#1)
Detecting: $(realpath "$TMP")/schemas/test.json (#2)
( 50%) Resolving: custom-meta.json
(100%) Resolving: test.json
(  4%) Producing: configuration.json
(  8%) Producing: version.json
( 12%) Producing: schemas/example/custom-meta/%/schema.metapack
error: The metaschema requires an unrecognised vocabulary
  at vocabulary https://example.com/vocab/totally-unknown
  at path $(realpath "$TMP")/schemas/custom-meta.json
EOF

cat << EOF > "$TMP/expected2.txt"
Writing output to: $(realpath "$TMP")/output
Using configuration: $(realpath "$TMP")/one.json
Detecting: $(realpath "$TMP")/schemas/test.json (#1)
Detecting: $(realpath "$TMP")/schemas/custom-meta.json (#2)
( 50%) Resolving: test.json
(100%) Resolving: custom-meta.json
(  4%) Producing: configuration.json
(  8%) Producing: version.json
( 12%) Producing: schemas/example/custom-meta/%/schema.metapack
error: The metaschema requires an unrecognised vocabulary
  at vocabulary https://example.com/vocab/totally-unknown
  at path $(realpath "$TMP")/schemas/custom-meta.json
EOF

diff "$TMP/output.txt" "$TMP/expected1.txt" || diff "$TMP/output.txt" "$TMP/expected2.txt"
