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

test -f "$TMP/output/deps.txt"
awk '/^t /{target=substr($0,3)} /^[sd] /{print target "|" $0}' \
  "$TMP/output/deps.txt" | sed "s|$(realpath "$TMP")|REAL|g" \
  | LC_ALL=C sort > "$TMP/deps.txt"

cat << 'EOF' > "$TMP/expected.txt"
REAL/output/dependency-tree.metapack|s REAL/output/schemas/example/test/%/dependencies.metapack
REAL/output/explorer/%/directory.metapack|d REAL/output/explorer/example/%/directory.metapack
REAL/output/explorer/%/directory.metapack|s REAL/output/configuration.json
REAL/output/explorer/%/directory.metapack|s REAL/output/explorer/example/%/directory.metapack
REAL/output/explorer/%/directory.metapack|s REAL/output/version.json
REAL/output/explorer/%/search.metapack|s REAL/output/explorer/example/test/%/schema.metapack
REAL/output/explorer/example/%/directory.metapack|s REAL/output/configuration.json
REAL/output/explorer/example/%/directory.metapack|s REAL/output/explorer/example/test/%/schema.metapack
REAL/output/explorer/example/%/directory.metapack|s REAL/output/version.json
REAL/output/explorer/example/test/%/schema.metapack|s REAL/output/configuration.json
REAL/output/explorer/example/test/%/schema.metapack|s REAL/output/schemas/example/test/%/dependencies.metapack
REAL/output/explorer/example/test/%/schema.metapack|s REAL/output/schemas/example/test/%/health.metapack
REAL/output/explorer/example/test/%/schema.metapack|s REAL/output/schemas/example/test/%/schema.metapack
REAL/output/explorer/example/test/%/schema.metapack|s REAL/output/version.json
REAL/output/routes.bin|s REAL/output/configuration.json
REAL/output/routes.bin|s REAL/output/version.json
REAL/output/schemas/example/test/%/blaze-exhaustive.metapack|s REAL/output/schemas/example/test/%/bundle.metapack
REAL/output/schemas/example/test/%/blaze-exhaustive.metapack|s REAL/output/version.json
REAL/output/schemas/example/test/%/blaze-fast.metapack|s REAL/output/schemas/example/test/%/bundle.metapack
REAL/output/schemas/example/test/%/blaze-fast.metapack|s REAL/output/version.json
REAL/output/schemas/example/test/%/bundle.metapack|s REAL/output/schemas/example/test/%/dependencies.metapack
REAL/output/schemas/example/test/%/bundle.metapack|s REAL/output/schemas/example/test/%/schema.metapack
REAL/output/schemas/example/test/%/bundle.metapack|s REAL/output/version.json
REAL/output/schemas/example/test/%/dependencies.metapack|s REAL/output/schemas/example/test/%/schema.metapack
REAL/output/schemas/example/test/%/dependencies.metapack|s REAL/output/version.json
REAL/output/schemas/example/test/%/dependents.metapack|s REAL/output/dependency-tree.metapack
REAL/output/schemas/example/test/%/editor.metapack|s REAL/output/schemas/example/test/%/bundle.metapack
REAL/output/schemas/example/test/%/editor.metapack|s REAL/output/version.json
REAL/output/schemas/example/test/%/health.metapack|s REAL/output/schemas/example/test/%/dependencies.metapack
REAL/output/schemas/example/test/%/health.metapack|s REAL/output/schemas/example/test/%/schema.metapack
REAL/output/schemas/example/test/%/health.metapack|s REAL/output/version.json
REAL/output/schemas/example/test/%/locations.metapack|s REAL/output/schemas/example/test/%/schema.metapack
REAL/output/schemas/example/test/%/locations.metapack|s REAL/output/version.json
REAL/output/schemas/example/test/%/positions.metapack|s REAL/output/schemas/example/test/%/schema.metapack
REAL/output/schemas/example/test/%/positions.metapack|s REAL/output/version.json
REAL/output/schemas/example/test/%/schema.metapack|s REAL/output/configuration.json
REAL/output/schemas/example/test/%/schema.metapack|s REAL/output/version.json
REAL/output/schemas/example/test/%/schema.metapack|s REAL/schemas/test.json
REAL/output/schemas/example/test/%/stats.metapack|s REAL/output/schemas/example/test/%/schema.metapack
REAL/output/schemas/example/test/%/stats.metapack|s REAL/output/version.json
EOF

diff "$TMP/deps.txt" "$TMP/expected.txt"
