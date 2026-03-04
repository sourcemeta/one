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
dependency-tree.metapack|s schemas/example/test/%/dependencies.metapack
explorer/%/directory.metapack|d explorer/example/%/directory.metapack
explorer/%/directory.metapack|s configuration.json
explorer/%/directory.metapack|s explorer/example/%/directory.metapack
explorer/%/directory.metapack|s version.json
explorer/%/search.metapack|s explorer/example/test/%/schema.metapack
explorer/example/%/directory.metapack|s configuration.json
explorer/example/%/directory.metapack|s explorer/example/test/%/schema.metapack
explorer/example/%/directory.metapack|s version.json
explorer/example/test/%/schema.metapack|s configuration.json
explorer/example/test/%/schema.metapack|s schemas/example/test/%/dependencies.metapack
explorer/example/test/%/schema.metapack|s schemas/example/test/%/health.metapack
explorer/example/test/%/schema.metapack|s schemas/example/test/%/schema.metapack
explorer/example/test/%/schema.metapack|s version.json
routes.bin|s configuration.json
routes.bin|s version.json
schemas/example/test/%/blaze-exhaustive.metapack|s schemas/example/test/%/bundle.metapack
schemas/example/test/%/blaze-exhaustive.metapack|s version.json
schemas/example/test/%/blaze-fast.metapack|s schemas/example/test/%/bundle.metapack
schemas/example/test/%/blaze-fast.metapack|s version.json
schemas/example/test/%/bundle.metapack|s schemas/example/test/%/dependencies.metapack
schemas/example/test/%/bundle.metapack|s schemas/example/test/%/schema.metapack
schemas/example/test/%/bundle.metapack|s version.json
schemas/example/test/%/dependencies.metapack|s schemas/example/test/%/schema.metapack
schemas/example/test/%/dependencies.metapack|s version.json
schemas/example/test/%/dependents.metapack|s dependency-tree.metapack
schemas/example/test/%/editor.metapack|s schemas/example/test/%/bundle.metapack
schemas/example/test/%/editor.metapack|s version.json
schemas/example/test/%/health.metapack|s schemas/example/test/%/dependencies.metapack
schemas/example/test/%/health.metapack|s schemas/example/test/%/schema.metapack
schemas/example/test/%/health.metapack|s version.json
schemas/example/test/%/locations.metapack|s schemas/example/test/%/schema.metapack
schemas/example/test/%/locations.metapack|s version.json
schemas/example/test/%/positions.metapack|s schemas/example/test/%/schema.metapack
schemas/example/test/%/positions.metapack|s version.json
schemas/example/test/%/schema.metapack|s REAL/schemas/test.json
schemas/example/test/%/schema.metapack|s configuration.json
schemas/example/test/%/schema.metapack|s version.json
schemas/example/test/%/stats.metapack|s schemas/example/test/%/schema.metapack
schemas/example/test/%/stats.metapack|s version.json
EOF

diff "$TMP/deps.txt" "$TMP/expected.txt"
