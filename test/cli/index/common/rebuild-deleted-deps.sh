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

# Run 1: index one schema from scratch
"$1" --skip-banner "$TMP/one.json" "$TMP/output" --concurrency 1 > /dev/null 2>&1

# Assert the full list of .deps files
cd "$TMP/output"
find . -name "*.deps" | LC_ALL=C sort > "$TMP/deps_actual.txt"
cd - > /dev/null

cat << 'EOF' > "$TMP/deps_expected.txt"
./dependency-tree.metapack.deps
./explorer/%/404.metapack.deps
./explorer/%/directory-html.metapack.deps
./explorer/%/directory.metapack.deps
./explorer/%/search.metapack.deps
./explorer/example/%/directory-html.metapack.deps
./explorer/example/%/directory.metapack.deps
./explorer/example/schemas/%/directory-html.metapack.deps
./explorer/example/schemas/%/directory.metapack.deps
./explorer/example/schemas/a/%/schema-html.metapack.deps
./explorer/example/schemas/a/%/schema.metapack.deps
./routes.bin.deps
./schemas/example/schemas/a/%/blaze-exhaustive.metapack.deps
./schemas/example/schemas/a/%/blaze-fast.metapack.deps
./schemas/example/schemas/a/%/bundle.metapack.deps
./schemas/example/schemas/a/%/dependencies.metapack.deps
./schemas/example/schemas/a/%/dependents.metapack.deps
./schemas/example/schemas/a/%/editor.metapack.deps
./schemas/example/schemas/a/%/health.metapack.deps
./schemas/example/schemas/a/%/locations.metapack.deps
./schemas/example/schemas/a/%/positions.metapack.deps
./schemas/example/schemas/a/%/schema.metapack.deps
./schemas/example/schemas/a/%/stats.metapack.deps
EOF

diff "$TMP/deps_actual.txt" "$TMP/deps_expected.txt"

# Delete every .deps file
rm "$TMP/output/dependency-tree.metapack.deps"
rm "$TMP/output/explorer/%/404.metapack.deps"
rm "$TMP/output/explorer/%/directory-html.metapack.deps"
rm "$TMP/output/explorer/%/directory.metapack.deps"
rm "$TMP/output/explorer/%/search.metapack.deps"
rm "$TMP/output/explorer/example/%/directory-html.metapack.deps"
rm "$TMP/output/explorer/example/%/directory.metapack.deps"
rm "$TMP/output/explorer/example/schemas/%/directory-html.metapack.deps"
rm "$TMP/output/explorer/example/schemas/%/directory.metapack.deps"
rm "$TMP/output/explorer/example/schemas/a/%/schema-html.metapack.deps"
rm "$TMP/output/explorer/example/schemas/a/%/schema.metapack.deps"
rm "$TMP/output/routes.bin.deps"
rm "$TMP/output/schemas/example/schemas/a/%/blaze-exhaustive.metapack.deps"
rm "$TMP/output/schemas/example/schemas/a/%/blaze-fast.metapack.deps"
rm "$TMP/output/schemas/example/schemas/a/%/bundle.metapack.deps"
rm "$TMP/output/schemas/example/schemas/a/%/dependencies.metapack.deps"
rm "$TMP/output/schemas/example/schemas/a/%/dependents.metapack.deps"
rm "$TMP/output/schemas/example/schemas/a/%/editor.metapack.deps"
rm "$TMP/output/schemas/example/schemas/a/%/health.metapack.deps"
rm "$TMP/output/schemas/example/schemas/a/%/locations.metapack.deps"
rm "$TMP/output/schemas/example/schemas/a/%/positions.metapack.deps"
rm "$TMP/output/schemas/example/schemas/a/%/schema.metapack.deps"
rm "$TMP/output/schemas/example/schemas/a/%/stats.metapack.deps"

# Run 2: re-index. All .deps files should be restored.
"$1" --skip-banner "$TMP/one.json" "$TMP/output" --concurrency 1 > /dev/null 2>&1

cd "$TMP/output"
find . -name "*.deps" | LC_ALL=C sort > "$TMP/deps_after.txt"
cd - > /dev/null

diff "$TMP/deps_after.txt" "$TMP/deps_expected.txt"
