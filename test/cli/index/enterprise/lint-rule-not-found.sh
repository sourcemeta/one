#!/bin/sh

set -o errexit
set -o nounset

TMP="$(mktemp -d)"
clean() { rm -rf "$TMP"; }
trap clean EXIT

mkdir "$TMP/schemas"

cat << 'EOF' > "$TMP/schemas/test.json"
{ "$schema": "https://json-schema.org/draft/2020-12/schema" }
EOF

cat << EOF > "$TMP/one.json"
{
  "url": "https://example.com",
  "contents": {
    "test": {
      "path": "./schemas",
      "lint": { "rules": [ "./rules/does-not-exist.json" ] }
    }
  }
}
EOF

"$1" --skip-banner "$TMP/one.json" "$TMP/output" --concurrency 1 2> "$TMP/output.txt" && CODE="$?" || CODE="$?"
test "$CODE" = "1" || exit 1

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
Detecting: $(realpath "$TMP")/schemas/test.json (#1)
(100%) Resolving: test.json
Building...
(  4%) Producing: configuration.json
(  9%) Producing: version.json
( 14%) Producing: explorer/%/404.metapack
( 19%) Producing: schemas/test/test/%/schema.metapack
( 23%) Producing: schemas/test/test/%/dependencies.metapack
( 28%) Producing: schemas/test/test/%/locations.metapack
( 33%) Producing: schemas/test/test/%/positions.metapack
( 38%) Producing: schemas/test/test/%/stats.metapack
( 42%) Producing: schemas/test/test/%/bundle.metapack
( 47%) Producing: schemas/test/test/%/health.metapack
error: could not locate the requested file
  at $(realpath "$TMP")/rules/does-not-exist.json
EOF

diff "$TMP/output.txt" "$TMP/expected.txt"
