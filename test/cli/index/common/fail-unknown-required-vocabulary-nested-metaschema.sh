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

# A custom metaschema that only uses known vocabularies
cat << 'EOF' > "$TMP/schemas/meta-a.json"
{
  "$schema": "https://json-schema.org/draft/2020-12/schema",
  "$id": "https://example.com/meta-a",
  "$vocabulary": {
    "https://json-schema.org/draft/2020-12/vocab/core": true,
    "https://json-schema.org/draft/2020-12/vocab/applicator": true,
    "https://json-schema.org/draft/2020-12/vocab/validation": true
  }
}
EOF

# A custom metaschema that uses meta-a as its dialect
# and declares an unknown required vocabulary
cat << 'EOF' > "$TMP/schemas/meta-b.json"
{
  "$schema": "https://example.com/meta-a",
  "$id": "https://example.com/meta-b",
  "$vocabulary": {
    "https://json-schema.org/draft/2020-12/vocab/core": true,
    "https://example.com/vocab/totally-unknown": true
  }
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
test "$CODE" = "1"
remove_threads_information "$TMP/output.txt"

cat << EOF > "$TMP/expected1.txt"
Writing output to: $(realpath "$TMP")/output
Using configuration: $(realpath "$TMP")/one.json
Detecting: $(realpath "$TMP")/schemas/meta-a.json (#1)
Detecting: $(realpath "$TMP")/schemas/meta-b.json (#2)
( 50%) Resolving: meta-a.json
(100%) Resolving: meta-b.json
(  2%) Producing: configuration.json
(  5%) Producing: version.json
(  8%) Producing: explorer/%/404.metapack
( 11%) Producing: schemas/example/schemas/meta-a/%/schema.metapack
( 14%) Producing: schemas/example/schemas/meta-b/%/schema.metapack
error: The metaschema requires an unrecognised vocabulary
  at vocabulary https://example.com/vocab/totally-unknown
  at path $(realpath "$TMP")/schemas/meta-b.json
EOF

cat << EOF > "$TMP/expected2.txt"
Writing output to: $(realpath "$TMP")/output
Using configuration: $(realpath "$TMP")/one.json
Detecting: $(realpath "$TMP")/schemas/meta-b.json (#1)
Detecting: $(realpath "$TMP")/schemas/meta-a.json (#2)
( 50%) Resolving: meta-b.json
(100%) Resolving: meta-a.json
(  2%) Producing: configuration.json
(  5%) Producing: version.json
(  8%) Producing: explorer/%/404.metapack
( 11%) Producing: schemas/example/schemas/meta-a/%/schema.metapack
( 14%) Producing: schemas/example/schemas/meta-b/%/schema.metapack
error: The metaschema requires an unrecognised vocabulary
  at vocabulary https://example.com/vocab/totally-unknown
  at path $(realpath "$TMP")/schemas/meta-b.json
EOF

diff "$TMP/output.txt" "$TMP/expected1.txt" || diff "$TMP/output.txt" "$TMP/expected2.txt"
