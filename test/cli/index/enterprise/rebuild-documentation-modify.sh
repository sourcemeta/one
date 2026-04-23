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
# Original
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

remove_threads_information() {
  expr='s/ \[[^]]*[^a-z-][^]]*\]//g'
  if [ "$(uname -s)" = "Darwin" ]; then
    sed -i '' "$expr" "$1"
  else
    sed -i "$expr" "$1"
  fi
}

RESOLVED_DOCS_PATH="$(realpath "$TMP/docs/readme.md")"
HASH="$(printf '%s' "$RESOLVED_DOCS_PATH" | shasum -a 256 | cut -d' ' -f1)"

# Run 1: Fresh build
"$1" --skip-banner "$TMP/one.json" "$TMP/output" --concurrency 1 2> "$TMP/output1.txt"
remove_threads_information "$TMP/output1.txt"

cat << EOF > "$TMP/expected1.txt"
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
( 77%) Producing: explorer/test/test/%/schema-html.metapack
( 81%) Producing: explorer/%/directory.metapack
( 86%) Producing: explorer/test/%/directory-html.metapack
( 90%) Producing: explorer/%/directory-html.metapack
( 95%) Producing: explorer/%/search.metapack
(100%) Producing: routes.bin
(100%) Combining: schemas/test/test/%/dependents.metapack
EOF

diff "$TMP/output1.txt" "$TMP/expected1.txt"

# Run 2: No changes - cache hit
"$1" --skip-banner "$TMP/one.json" "$TMP/output" --concurrency 1 2> "$TMP/output2.txt"
remove_threads_information "$TMP/output2.txt"

cat << EOF > "$TMP/expected2.txt"
Writing output to: $(realpath "$TMP")/output
Using configuration: $(realpath "$TMP")/one.json
Detecting: $(realpath "$TMP")/schemas/test.json (#1)
EOF

diff "$TMP/output2.txt" "$TMP/expected2.txt"

# Run 3: Modify documentation file
echo '# Updated Content' > "$TMP/docs/readme.md"

"$1" --skip-banner "$TMP/one.json" "$TMP/output" --concurrency 1 2> "$TMP/output3.txt"
remove_threads_information "$TMP/output3.txt"

cat << EOF > "$TMP/expected3.txt"
Writing output to: $(realpath "$TMP")/output
Using configuration: $(realpath "$TMP")/one.json
Detecting: $(realpath "$TMP")/schemas/test.json (#1)
( 16%) Producing: static/${HASH}.metapack
( 33%) Producing: explorer/test/%/directory.metapack
( 50%) Producing: explorer/%/directory.metapack
( 66%) Producing: explorer/test/%/directory-html.metapack
( 83%) Producing: explorer/%/directory-html.metapack
(100%) Producing: explorer/%/search.metapack
EOF

diff "$TMP/output3.txt" "$TMP/expected3.txt"

# Verify the full output file listing
cd "$TMP/output"
find . -mindepth 1 | LC_ALL=C sort > "$TMP/manifest.txt"
cd - > /dev/null

cat << EOF > "$TMP/expected_manifest.txt"
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

diff "$TMP/manifest.txt" "$TMP/expected_manifest.txt"
