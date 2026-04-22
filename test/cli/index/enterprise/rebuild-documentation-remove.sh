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
# Docs
EOF

remove_threads_information() {
  expr='s/ \[[^]]*[^a-z-][^]]*\]//g'
  if [ "$(uname -s)" = "Darwin" ]; then
    sed -i '' "$expr" "$1"
  else
    sed -i "$expr" "$1"
  fi
}

# Run 1: Build with documentation
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

"$1" --skip-banner "$TMP/one.json" "$TMP/output" --concurrency 1 2> "$TMP/output1.txt"

# Verify output after run 1
cd "$TMP/output"
find . -mindepth 1 | LC_ALL=C sort > "$TMP/manifest1.txt"
cd - > /dev/null

RESOLVED_DOCS_PATH="$(realpath "$TMP/docs/readme.md")"
HASH="$(printf '%s' "$RESOLVED_DOCS_PATH" | shasum -a 256 | cut -d' ' -f1)"

cat << EOF > "$TMP/expected_manifest1.txt"
./configuration.json
./explorer
./explorer/%
./explorer/%/404.metapack
./explorer/%/directory-html.metapack
./explorer/%/directory.metapack
./explorer/%/search.metapack
./explorer/test
./explorer/test/%
./explorer/test/%/directory-html.metapack
./explorer/test/%/directory.metapack
./explorer/test/test
./explorer/test/test/%
./explorer/test/test/%/schema-html.metapack
./explorer/test/test/%/schema.metapack
./routes.bin
./schemas
./schemas/test
./schemas/test/test
./schemas/test/test/%
./schemas/test/test/%/blaze-exhaustive.metapack
./schemas/test/test/%/blaze-fast.metapack
./schemas/test/test/%/bundle.metapack
./schemas/test/test/%/dependencies.metapack
./schemas/test/test/%/dependents.metapack
./schemas/test/test/%/editor.metapack
./schemas/test/test/%/health.metapack
./schemas/test/test/%/locations.metapack
./schemas/test/test/%/positions.metapack
./schemas/test/test/%/schema.metapack
./schemas/test/test/%/stats.metapack
./state.bin
./static
./static/${HASH}.metapack
./version.json
EOF

diff "$TMP/manifest1.txt" "$TMP/expected_manifest1.txt"

# Run 2: Remove documentation from config
cat << EOF > "$TMP/one.json"
{
  "url": "https://example.com",
  "contents": {
    "test": {
      "path": "./schemas"
    }
  }
}
EOF

"$1" --skip-banner "$TMP/one.json" "$TMP/output" --concurrency 1 2> "$TMP/output2.txt"
remove_threads_information "$TMP/output2.txt"

cat << EOF > "$TMP/expected2.txt"
Writing output to: $(realpath "$TMP")/output
Using configuration: $(realpath "$TMP")/one.json
Detecting: $(realpath "$TMP")/schemas/test.json (#1)
(100%) Resolving: test.json
(  4%) Producing: configuration.json
(  9%) Producing: version.json
( 13%) Producing: explorer/%/404.metapack
( 18%) Producing: schemas/test/test/%/schema.metapack
( 22%) Producing: schemas/test/test/%/dependencies.metapack
( 27%) Producing: schemas/test/test/%/locations.metapack
( 31%) Producing: schemas/test/test/%/positions.metapack
( 36%) Producing: schemas/test/test/%/stats.metapack
( 40%) Producing: schemas/test/test/%/bundle.metapack
( 45%) Producing: schemas/test/test/%/health.metapack
( 50%) Producing: explorer/test/test/%/schema.metapack
( 54%) Producing: schemas/test/test/%/blaze-exhaustive.metapack
( 59%) Producing: schemas/test/test/%/blaze-fast.metapack
( 63%) Producing: schemas/test/test/%/editor.metapack
( 68%) Producing: explorer/test/%/directory.metapack
( 72%) Producing: explorer/test/test/%/schema-html.metapack
( 77%) Producing: explorer/%/directory.metapack
( 81%) Producing: explorer/test/%/directory-html.metapack
( 86%) Producing: explorer/%/directory-html.metapack
( 90%) Producing: explorer/%/search.metapack
( 95%) Producing: routes.bin
(100%) Disposing: static
EOF

diff "$TMP/output2.txt" "$TMP/expected2.txt"

# Verify output after run 2 (static directory removed)
cd "$TMP/output"
find . -mindepth 1 | LC_ALL=C sort > "$TMP/manifest2.txt"
cd - > /dev/null

cat << 'EOF' > "$TMP/expected_manifest2.txt"
./configuration.json
./explorer
./explorer/%
./explorer/%/404.metapack
./explorer/%/directory-html.metapack
./explorer/%/directory.metapack
./explorer/%/search.metapack
./explorer/test
./explorer/test/%
./explorer/test/%/directory-html.metapack
./explorer/test/%/directory.metapack
./explorer/test/test
./explorer/test/test/%
./explorer/test/test/%/schema-html.metapack
./explorer/test/test/%/schema.metapack
./routes.bin
./schemas
./schemas/test
./schemas/test/test
./schemas/test/test/%
./schemas/test/test/%/blaze-exhaustive.metapack
./schemas/test/test/%/blaze-fast.metapack
./schemas/test/test/%/bundle.metapack
./schemas/test/test/%/dependencies.metapack
./schemas/test/test/%/dependents.metapack
./schemas/test/test/%/editor.metapack
./schemas/test/test/%/health.metapack
./schemas/test/test/%/locations.metapack
./schemas/test/test/%/positions.metapack
./schemas/test/test/%/schema.metapack
./schemas/test/test/%/stats.metapack
./state.bin
./version.json
EOF

diff "$TMP/manifest2.txt" "$TMP/expected_manifest2.txt"
