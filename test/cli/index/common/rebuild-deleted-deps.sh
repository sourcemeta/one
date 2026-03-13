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

cat << 'EOF' > "$TMP/schemas/a.json"
{
  "$schema": "http://json-schema.org/draft-07/schema#",
  "$id": "https://example.com/a"
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

"$1" --skip-banner "$TMP/one.json" "$TMP/output" --concurrency 1 > /dev/null 2>&1

test -f "$TMP/output/state.bin"
rm "$TMP/output/state.bin"
test ! -f "$TMP/output/state.bin"

"$1" --skip-banner "$TMP/one.json" "$TMP/output" --concurrency 1 2> "$TMP/output.txt"
remove_threads_information "$TMP/output.txt"

cat << EOF > "$TMP/expected.txt"
Writing output to: $(realpath "$TMP")/output
Using configuration: $(realpath "$TMP")/one.json
Detecting: $(realpath "$TMP")/schemas/a.json (#1)
(100%) Resolving: a.json
(  4%) Producing: configuration.json
(  8%) Producing: version.json
( 12%) Producing: explorer/%/404.metapack
( 16%) Producing: schemas/example/schemas/a/%/schema.metapack
( 20%) Producing: schemas/example/schemas/a/%/dependencies.metapack
( 24%) Producing: schemas/example/schemas/a/%/locations.metapack
( 28%) Producing: schemas/example/schemas/a/%/positions.metapack
( 32%) Producing: schemas/example/schemas/a/%/stats.metapack
( 36%) Producing: dependency-tree.metapack
( 40%) Producing: schemas/example/schemas/a/%/bundle.metapack
( 44%) Producing: schemas/example/schemas/a/%/health.metapack
( 48%) Producing: explorer/example/schemas/a/%/schema.metapack
( 52%) Producing: schemas/example/schemas/a/%/blaze-exhaustive.metapack
( 56%) Producing: schemas/example/schemas/a/%/blaze-fast.metapack
( 60%) Producing: schemas/example/schemas/a/%/dependents.metapack
( 64%) Producing: schemas/example/schemas/a/%/editor.metapack
( 68%) Producing: explorer/%/search.metapack
( 72%) Producing: explorer/example/schemas/%/directory.metapack
( 76%) Producing: explorer/example/schemas/a/%/schema-html.metapack
( 80%) Producing: explorer/example/%/directory.metapack
( 84%) Producing: explorer/example/schemas/%/directory-html.metapack
( 88%) Producing: explorer/%/directory.metapack
( 92%) Producing: explorer/example/%/directory-html.metapack
( 96%) Producing: explorer/%/directory-html.metapack
(100%) Producing: routes.bin
EOF

diff "$TMP/output.txt" "$TMP/expected.txt"

test -f "$TMP/output/state.bin"
