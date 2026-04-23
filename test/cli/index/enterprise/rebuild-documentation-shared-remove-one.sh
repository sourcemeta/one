#!/bin/sh

set -o errexit
set -o nounset

TMP="$(mktemp -d)"
clean() { rm -rf "$TMP"; }
trap clean EXIT

mkdir -p "$TMP/schemas-a" "$TMP/schemas-b" "$TMP/docs"

cat << 'EOF' > "$TMP/schemas-a/test.json"
{ "$schema": "https://json-schema.org/draft/2020-12/schema" }
EOF

cat << 'EOF' > "$TMP/schemas-b/test.json"
{ "$schema": "https://json-schema.org/draft/2020-12/schema" }
EOF

cat << 'EOF' > "$TMP/docs/readme.md"
# Shared
EOF

remove_threads_information() {
  expr='s/ \[[^]]*[^a-z-][^]]*\]//g'
  if [ "$(uname -s)" = "Darwin" ]; then
    sed -i '' "$expr" "$1"
  else
    sed -i "$expr" "$1"
  fi
}

# Run 1: Build with both collections having documentation
cat << EOF > "$TMP/one.json"
{
  "url": "https://example.com",
  "contents": {
    "alpha": {
      "path": "./schemas-a",
      "x-sourcemeta-one:documentation": "./docs/readme.md"
    },
    "beta": {
      "path": "./schemas-b",
      "x-sourcemeta-one:documentation": "./docs/readme.md"
    }
  }
}
EOF

"$1" --skip-banner "$TMP/one.json" "$TMP/output" --concurrency 1 2> "$TMP/output1.txt"

RESOLVED_DOCS_PATH="$(realpath "$TMP/docs/readme.md")"
HASH="$(printf '%s' "$RESOLVED_DOCS_PATH" | shasum -a 256 | cut -d' ' -f1)"

# Verify output after run 1
cd "$TMP/output"
find . -mindepth 1 | LC_ALL=C sort > "$TMP/manifest1.txt"
cd - > /dev/null

cat << EOF > "$TMP/expected_manifest1.txt"
./configuration.json
./explorer
./explorer/%
./explorer/%/404.metapack
./explorer/%/directory-html.metapack
./explorer/%/directory.metapack
./explorer/%/search.metapack
./explorer/alpha
./explorer/alpha/%
./explorer/alpha/%/directory-html.metapack
./explorer/alpha/%/directory.metapack
./explorer/alpha/test
./explorer/alpha/test/%
./explorer/alpha/test/%/schema-html.metapack
./explorer/alpha/test/%/schema.metapack
./explorer/beta
./explorer/beta/%
./explorer/beta/%/directory-html.metapack
./explorer/beta/%/directory.metapack
./explorer/beta/test
./explorer/beta/test/%
./explorer/beta/test/%/schema-html.metapack
./explorer/beta/test/%/schema.metapack
./routes.bin
./schemas
./schemas/alpha
./schemas/alpha/test
./schemas/alpha/test/%
./schemas/alpha/test/%/blaze-exhaustive.metapack
./schemas/alpha/test/%/blaze-fast.metapack
./schemas/alpha/test/%/bundle.metapack
./schemas/alpha/test/%/dependencies.metapack
./schemas/alpha/test/%/dependents.metapack
./schemas/alpha/test/%/editor.metapack
./schemas/alpha/test/%/health.metapack
./schemas/alpha/test/%/locations.metapack
./schemas/alpha/test/%/positions.metapack
./schemas/alpha/test/%/schema.metapack
./schemas/alpha/test/%/stats.metapack
./schemas/beta
./schemas/beta/test
./schemas/beta/test/%
./schemas/beta/test/%/blaze-exhaustive.metapack
./schemas/beta/test/%/blaze-fast.metapack
./schemas/beta/test/%/bundle.metapack
./schemas/beta/test/%/dependencies.metapack
./schemas/beta/test/%/dependents.metapack
./schemas/beta/test/%/editor.metapack
./schemas/beta/test/%/health.metapack
./schemas/beta/test/%/locations.metapack
./schemas/beta/test/%/positions.metapack
./schemas/beta/test/%/schema.metapack
./schemas/beta/test/%/stats.metapack
./state.bin
./static
./static/${HASH}.metapack
./version.json
EOF

diff "$TMP/manifest1.txt" "$TMP/expected_manifest1.txt"

# Run 2: Remove documentation from beta only
cat << EOF > "$TMP/one.json"
{
  "url": "https://example.com",
  "contents": {
    "alpha": {
      "path": "./schemas-a",
      "x-sourcemeta-one:documentation": "./docs/readme.md"
    },
    "beta": {
      "path": "./schemas-b"
    }
  }
}
EOF

"$1" --skip-banner "$TMP/one.json" "$TMP/output" --concurrency 1 2> "$TMP/output2.txt"
remove_threads_information "$TMP/output2.txt"

cat << EOF > "$TMP/expected2.txt"
Writing output to: $(realpath "$TMP")/output
Using configuration: $(realpath "$TMP")/one.json
Detecting: $(realpath "$TMP")/schemas-b/test.json (#1)
Detecting: $(realpath "$TMP")/schemas-a/test.json (#2)
( 50%) Resolving: test.json
(100%) Resolving: test.json
(  2%) Producing: configuration.json
(  5%) Producing: version.json
(  8%) Producing: explorer/%/404.metapack
( 11%) Producing: schemas/alpha/test/%/schema.metapack
( 13%) Producing: schemas/beta/test/%/schema.metapack
( 16%) Producing: static/${HASH}.metapack
( 19%) Producing: schemas/alpha/test/%/dependencies.metapack
( 22%) Producing: schemas/alpha/test/%/locations.metapack
( 25%) Producing: schemas/alpha/test/%/positions.metapack
( 27%) Producing: schemas/alpha/test/%/stats.metapack
( 30%) Producing: schemas/beta/test/%/dependencies.metapack
( 33%) Producing: schemas/beta/test/%/locations.metapack
( 36%) Producing: schemas/beta/test/%/positions.metapack
( 38%) Producing: schemas/beta/test/%/stats.metapack
( 41%) Producing: schemas/alpha/test/%/bundle.metapack
( 44%) Producing: schemas/alpha/test/%/health.metapack
( 47%) Producing: schemas/beta/test/%/bundle.metapack
( 50%) Producing: schemas/beta/test/%/health.metapack
( 52%) Producing: explorer/alpha/test/%/schema.metapack
( 55%) Producing: explorer/beta/test/%/schema.metapack
( 58%) Producing: schemas/alpha/test/%/blaze-exhaustive.metapack
( 61%) Producing: schemas/alpha/test/%/blaze-fast.metapack
( 63%) Producing: schemas/alpha/test/%/editor.metapack
( 66%) Producing: schemas/beta/test/%/blaze-exhaustive.metapack
( 69%) Producing: schemas/beta/test/%/blaze-fast.metapack
( 72%) Producing: schemas/beta/test/%/editor.metapack
( 75%) Producing: explorer/alpha/%/directory.metapack
( 77%) Producing: explorer/alpha/test/%/schema-html.metapack
( 80%) Producing: explorer/beta/%/directory.metapack
( 83%) Producing: explorer/beta/test/%/schema-html.metapack
( 86%) Producing: explorer/%/directory.metapack
( 88%) Producing: explorer/alpha/%/directory-html.metapack
( 91%) Producing: explorer/beta/%/directory-html.metapack
( 94%) Producing: explorer/%/directory-html.metapack
( 97%) Producing: explorer/%/search.metapack
(100%) Producing: routes.bin
EOF

diff "$TMP/output2.txt" "$TMP/expected2.txt"

# Verify output after run 2 (static file still exists, alpha still uses it)
cd "$TMP/output"
find . -mindepth 1 | LC_ALL=C sort > "$TMP/manifest2.txt"
cd - > /dev/null

cat << EOF > "$TMP/expected_manifest2.txt"
./configuration.json
./explorer
./explorer/%
./explorer/%/404.metapack
./explorer/%/directory-html.metapack
./explorer/%/directory.metapack
./explorer/%/search.metapack
./explorer/alpha
./explorer/alpha/%
./explorer/alpha/%/directory-html.metapack
./explorer/alpha/%/directory.metapack
./explorer/alpha/test
./explorer/alpha/test/%
./explorer/alpha/test/%/schema-html.metapack
./explorer/alpha/test/%/schema.metapack
./explorer/beta
./explorer/beta/%
./explorer/beta/%/directory-html.metapack
./explorer/beta/%/directory.metapack
./explorer/beta/test
./explorer/beta/test/%
./explorer/beta/test/%/schema-html.metapack
./explorer/beta/test/%/schema.metapack
./routes.bin
./schemas
./schemas/alpha
./schemas/alpha/test
./schemas/alpha/test/%
./schemas/alpha/test/%/blaze-exhaustive.metapack
./schemas/alpha/test/%/blaze-fast.metapack
./schemas/alpha/test/%/bundle.metapack
./schemas/alpha/test/%/dependencies.metapack
./schemas/alpha/test/%/dependents.metapack
./schemas/alpha/test/%/editor.metapack
./schemas/alpha/test/%/health.metapack
./schemas/alpha/test/%/locations.metapack
./schemas/alpha/test/%/positions.metapack
./schemas/alpha/test/%/schema.metapack
./schemas/alpha/test/%/stats.metapack
./schemas/beta
./schemas/beta/test
./schemas/beta/test/%
./schemas/beta/test/%/blaze-exhaustive.metapack
./schemas/beta/test/%/blaze-fast.metapack
./schemas/beta/test/%/bundle.metapack
./schemas/beta/test/%/dependencies.metapack
./schemas/beta/test/%/dependents.metapack
./schemas/beta/test/%/editor.metapack
./schemas/beta/test/%/health.metapack
./schemas/beta/test/%/locations.metapack
./schemas/beta/test/%/positions.metapack
./schemas/beta/test/%/schema.metapack
./schemas/beta/test/%/stats.metapack
./state.bin
./static
./static/${HASH}.metapack
./version.json
EOF

diff "$TMP/manifest2.txt" "$TMP/expected_manifest2.txt"
