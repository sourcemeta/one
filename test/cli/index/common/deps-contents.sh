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
schemas/example/test/%/dependencies.metapack
--- ./explorer/%/directory.metapack.deps
configuration.json
explorer/example/%/directory.metapack
explorer/example/test/%/schema.metapack
version.json
--- ./explorer/%/search.metapack.deps
explorer/example/test/%/schema.metapack
--- ./explorer/example/%/directory.metapack.deps
configuration.json
explorer/example/test/%/schema.metapack
version.json
--- ./explorer/example/test/%/schema.metapack.deps
configuration.json
schemas/example/test/%/dependencies.metapack
schemas/example/test/%/health.metapack
schemas/example/test/%/schema.metapack
version.json
--- ./routes.bin.deps
configuration.json
version.json
--- ./schemas/example/test/%/blaze-exhaustive.metapack.deps
schemas/example/test/%/bundle.metapack
version.json
--- ./schemas/example/test/%/blaze-fast.metapack.deps
schemas/example/test/%/bundle.metapack
version.json
--- ./schemas/example/test/%/bundle.metapack.deps
schemas/example/test/%/dependencies.metapack
schemas/example/test/%/schema.metapack
version.json
--- ./schemas/example/test/%/dependencies.metapack.deps
schemas/example/test/%/schema.metapack
version.json
--- ./schemas/example/test/%/dependents.metapack.deps
dependency-tree.metapack
--- ./schemas/example/test/%/editor.metapack.deps
schemas/example/test/%/bundle.metapack
version.json
--- ./schemas/example/test/%/health.metapack.deps
schemas/example/test/%/dependencies.metapack
schemas/example/test/%/schema.metapack
version.json
--- ./schemas/example/test/%/locations.metapack.deps
schemas/example/test/%/schema.metapack
version.json
--- ./schemas/example/test/%/positions.metapack.deps
schemas/example/test/%/schema.metapack
version.json
--- ./schemas/example/test/%/schema.metapack.deps
$REAL/schemas/test.json
configuration.json
version.json
--- ./schemas/example/test/%/stats.metapack.deps
schemas/example/test/%/schema.metapack
version.json
EOF

diff "$TMP/deps.txt" "$TMP/expected.txt"
