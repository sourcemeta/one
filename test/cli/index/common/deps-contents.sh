#!/bin/sh

set -o errexit
set -o nounset

TMP="$(mktemp -d)"
clean() { rm -rf "$TMP"; }
trap clean EXIT

mkdir "$TMP/schemas"

cat << 'EOF' > "$TMP/schemas/test.json"
{
  "$schema": "https://json-schema.org/draft/2020-12/schema",
  "$id": "https://example.com/test"
}
EOF

cat << EOF > "$TMP/one.json"
{
  "url": "http://localhost:8000",
  "html": false,
  "contents": {
    "example": {
      "baseUri": "https://example.com",
      "path": "./schemas"
    }
  }
}
EOF

"$1" "$TMP/one.json" "$TMP/output" > /dev/null 2>&1

REAL="$(realpath "$TMP")"

cd "$TMP/output"
find . -name "*.deps" -type f | LC_ALL=C sort | while read -r deps_file; do
  echo "--- $deps_file"
  LC_ALL=C sort "$deps_file"
done > "$TMP/deps.txt"
cd - > /dev/null

cat << EOF > "$TMP/expected.txt"
--- ./dependency-tree.metapack.deps
$REAL/output/schemas/example/test/%/dependencies.metapack
--- ./explorer/%/directory.metapack.deps
$REAL/output/configuration.json
$REAL/output/explorer/example/%/directory.metapack
$REAL/output/explorer/example/test/%/schema.metapack
$REAL/output/version.json
--- ./explorer/%/search.metapack.deps
$REAL/output/explorer/example/test/%/schema.metapack
--- ./explorer/example/%/directory.metapack.deps
$REAL/output/configuration.json
$REAL/output/explorer/example/test/%/schema.metapack
$REAL/output/version.json
--- ./explorer/example/test/%/schema.metapack.deps
$REAL/output/configuration.json
$REAL/output/schemas/example/test/%/dependencies.metapack
$REAL/output/schemas/example/test/%/health.metapack
$REAL/output/schemas/example/test/%/schema.metapack
$REAL/output/version.json
--- ./routes.bin.deps
$REAL/output/configuration.json
$REAL/output/version.json
--- ./schemas/example/test/%/blaze-exhaustive.metapack.deps
$REAL/output/schemas/example/test/%/bundle.metapack
$REAL/output/version.json
--- ./schemas/example/test/%/blaze-fast.metapack.deps
$REAL/output/schemas/example/test/%/bundle.metapack
$REAL/output/version.json
--- ./schemas/example/test/%/bundle.metapack.deps
$REAL/output/schemas/example/test/%/dependencies.metapack
$REAL/output/schemas/example/test/%/schema.metapack
$REAL/output/version.json
--- ./schemas/example/test/%/dependencies.metapack.deps
$REAL/output/schemas/example/test/%/schema.metapack
$REAL/output/version.json
--- ./schemas/example/test/%/dependents.metapack.deps
$REAL/output/dependency-tree.metapack
--- ./schemas/example/test/%/editor.metapack.deps
$REAL/output/schemas/example/test/%/bundle.metapack
$REAL/output/version.json
--- ./schemas/example/test/%/health.metapack.deps
$REAL/output/schemas/example/test/%/dependencies.metapack
$REAL/output/schemas/example/test/%/schema.metapack
$REAL/output/version.json
--- ./schemas/example/test/%/locations.metapack.deps
$REAL/output/schemas/example/test/%/schema.metapack
$REAL/output/version.json
--- ./schemas/example/test/%/positions.metapack.deps
$REAL/output/schemas/example/test/%/schema.metapack
$REAL/output/version.json
--- ./schemas/example/test/%/schema.metapack.deps
$REAL/output/configuration.json
$REAL/output/version.json
$REAL/schemas/test.json
--- ./schemas/example/test/%/stats.metapack.deps
$REAL/output/schemas/example/test/%/schema.metapack
$REAL/output/version.json
EOF

diff "$TMP/deps.txt" "$TMP/expected.txt"
