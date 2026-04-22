#!/bin/sh

set -o errexit
set -o nounset

TMP="$(mktemp -d)"
clean() { rm -rf "$TMP"; }
trap clean EXIT

mkdir -p "$TMP/schemas" "$TMP/docs"

cat << 'EOF' > "$TMP/schemas/test.json"
{ "$schema": "https://json-schema.org/draft/2020-12/schema" }
EOF

cat << 'EOF' > "$TMP/docs/readme.md"
# Hello
EOF

cat << EOF > "$TMP/one.json"
{
  "url": "https://example.com",
  "contents": {
    "test": {
      "path": "./schemas",
      "x-sourcemeta-one:documentation": "./docs/readme.md"
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

RESOLVED_DOCS_PATH="$(realpath "$TMP/docs/readme.md")"
HASH="$(printf '%s' "$RESOLVED_DOCS_PATH" | shasum -a 256 | cut -d' ' -f1)"

cat << EOF > "$TMP/expected.txt"
Writing output to: $(realpath "$TMP")/output
Using configuration: $(realpath "$TMP")/one.json
Detecting: $(realpath "$TMP")/schemas/test.json (#1)
(100%) Resolving: test.json
(  4%) Producing: configuration.json
(  9%) Producing: version.json
( 13%) Producing: explorer/%/404.metapack
( 18%) Producing: schemas/test/test/%/schema.metapack
( 22%) Producing: static/${HASH}.metapack
( 27%) Producing: schemas/test/test/%/dependencies.metapack
( 31%) Producing: schemas/test/test/%/locations.metapack
( 36%) Producing: schemas/test/test/%/positions.metapack
( 40%) Producing: schemas/test/test/%/stats.metapack
( 45%) Producing: schemas/test/test/%/bundle.metapack
( 50%) Producing: schemas/test/test/%/health.metapack
( 54%) Producing: explorer/test/test/%/schema.metapack
( 59%) Producing: schemas/test/test/%/blaze-exhaustive.metapack
( 63%) Producing: schemas/test/test/%/blaze-fast.metapack
( 68%) Producing: schemas/test/test/%/editor.metapack
( 72%) Producing: explorer/test/%/directory.metapack
error: Collection documentation is only available on the enterprise edition
  at path $(realpath "$TMP")/one.json
EOF

diff "$TMP/output.txt" "$TMP/expected.txt"
