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

dd if=/dev/zero bs=1048577 count=1 2>/dev/null | tr '\0' 'a' > "$TMP/docs/big.md"

cat << EOF > "$TMP/one.json"
{
  "url": "https://example.com",
  "contents": {
    "test": {
      "path": "./schemas",
      "x-sourcemeta-one:documentation": "./docs/big.md"
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

RESOLVED_DOCS_PATH="$(realpath "$TMP/docs/big.md")"
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
error: Static file exceeds the maximum allowed size
  at path $(realpath "$TMP")/docs/big.md
  with size 1048577
EOF

diff "$TMP/output.txt" "$TMP/expected.txt"
