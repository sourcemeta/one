#!/bin/sh

set -o errexit
set -o nounset

TMP="$(mktemp -d)"
clean() { rm -rf "$TMP"; }
trap clean EXIT

mkdir -p "$TMP/schemas" "$TMP/rules"

cat << 'EOF' > "$TMP/schemas/test.json"
{ "$schema": "https://json-schema.org/draft/2020-12/schema" }
EOF

cat << 'EOF' > "$TMP/rules/rule.json"
{ "$schema": "https://json-schema.org/draft/2020-12/schema", "title": "custom/my_rule" }
EOF

cat << EOF > "$TMP/one.json"
{
  "url": "https://example.com",
  "contents": {
    "test": {
      "path": "./schemas",
      "lint": { "rules": [ "../rules/rule.json" ] }
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
( 10%) Producing: configuration.json
( 20%) Producing: version.json
( 30%) Producing: explorer/%/404.metapack
( 40%) Producing: schemas/test/test/%/schema.metapack
( 50%) Producing: schemas/test/test/%/dependencies.metapack
( 60%) Producing: schemas/test/test/%/locations.metapack
( 70%) Producing: schemas/test/test/%/positions.metapack
( 80%) Producing: schemas/test/test/%/stats.metapack
( 90%) Producing: schemas/test/test/%/bundle.metapack
(100%) Producing: schemas/test/test/%/health.metapack
error: Custom linter rules are only available on the enterprise edition
EOF

diff "$TMP/output.txt" "$TMP/expected.txt"
