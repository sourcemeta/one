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
(  9%) Producing: version.json
( 14%) Producing: explorer/%/404.metapack
( 19%) Producing: schemas/test/test/%/schema.metapack
( 23%) Producing: schemas/test/test/%/dependencies.metapack
( 28%) Producing: schemas/test/test/%/locations.metapack
( 33%) Producing: schemas/test/test/%/positions.metapack
( 38%) Producing: schemas/test/test/%/stats.metapack
( 42%) Producing: schemas/test/test/%/bundle.metapack
( 47%) Producing: schemas/test/test/%/health.metapack
error: Custom linter rules are only available on the enterprise edition
  at path $(realpath "$TMP")/one.json
EOF

diff "$TMP/output.txt" "$TMP/expected.txt"
