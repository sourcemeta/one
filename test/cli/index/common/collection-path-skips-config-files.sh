#!/bin/sh

set -o errexit
set -o nounset

TMP="$(mktemp -d)"
clean() { rm -rf "$TMP"; }
trap clean EXIT

cat << 'EOF' > "$TMP/defaults.json"
{
  "contents": {
    "example": {
      "contents": {
        "schemas": {
          "include": "./jsonschema.json"
        }
      }
    }
  }
}
EOF

cat << 'EOF' > "$TMP/jsonschema.json"
{
  "baseUri": "https://example.com/",
  "defaultDialect": "http://json-schema.org/draft-07/schema#",
  "path": "."
}
EOF

cat << EOF > "$TMP/one.json"
{
  "url": "https://sourcemeta.com/",
  "extends": [ "./defaults.json" ]
}
EOF

cat << 'EOF' > "$TMP/test.json"
{
  "$schema": "http://json-schema.org/draft-07/schema#",
  "$id": "https://example.com/test",
  "type": "string"
}
EOF

"$1" --skip-banner "$TMP/one.json" "$TMP/output" --concurrency 1 \
  2> "$TMP/output.txt" && CODE="$?" || CODE="$?"
test "$CODE" = "0" || exit 1

# Remove thread information
if [ "$(uname)" = "Darwin" ]
then
  sed -i '' 's/ \[.*\]//g' "$TMP/output.txt"
else
  sed -i 's/ \[.*\]//g' "$TMP/output.txt"
fi

cat << EOF > "$TMP/expected.txt"
Writing output to: $(realpath "$TMP")/output
Using configuration: $(realpath "$TMP")/one.json
Detecting: $(realpath "$TMP")/test.json (#1)
(100%) Resolving: test.json
(  4%) Producing: configuration.json
(  8%) Producing: version.json
( 13%) Producing: explorer/%/404.metapack
( 17%) Producing: schemas/example/schemas/test/%/schema.metapack
( 21%) Producing: schemas/example/schemas/test/%/dependencies.metapack
( 26%) Producing: schemas/example/schemas/test/%/locations.metapack
( 30%) Producing: schemas/example/schemas/test/%/positions.metapack
( 34%) Producing: schemas/example/schemas/test/%/stats.metapack
( 39%) Producing: schemas/example/schemas/test/%/bundle.metapack
( 43%) Producing: schemas/example/schemas/test/%/health.metapack
( 47%) Producing: explorer/example/schemas/test/%/schema.metapack
( 52%) Producing: schemas/example/schemas/test/%/blaze-exhaustive.metapack
( 56%) Producing: schemas/example/schemas/test/%/blaze-fast.metapack
( 60%) Producing: schemas/example/schemas/test/%/editor.metapack
( 65%) Producing: explorer/example/schemas/%/directory.metapack
( 69%) Producing: explorer/example/schemas/test/%/schema-html.metapack
( 73%) Producing: explorer/example/%/directory.metapack
( 78%) Producing: explorer/example/schemas/%/directory-html.metapack
( 82%) Producing: explorer/%/directory.metapack
( 86%) Producing: explorer/example/%/directory-html.metapack
( 91%) Producing: explorer/%/directory-html.metapack
( 95%) Producing: explorer/%/search.metapack
(100%) Producing: routes.bin
(100%) Combining: schemas/example/schemas/test/%/dependents.metapack
EOF

diff "$TMP/output.txt" "$TMP/expected.txt"
