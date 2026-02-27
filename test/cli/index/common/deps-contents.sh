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
s schemas/example/test/%/dependencies.metapack
--- ./explorer/%/directory.metapack.deps
d explorer/example/%/directory.metapack
s configuration.json
s explorer/example/test/%/schema.metapack
s version.json
--- ./explorer/%/search.metapack.deps
s explorer/example/test/%/schema.metapack
--- ./explorer/example/%/directory.metapack.deps
s configuration.json
s explorer/example/test/%/schema.metapack
s version.json
--- ./explorer/example/test/%/schema.metapack.deps
s configuration.json
s schemas/example/test/%/dependencies.metapack
s schemas/example/test/%/health.metapack
s schemas/example/test/%/schema.metapack
s version.json
--- ./routes.bin.deps
s configuration.json
s version.json
--- ./schemas/example/test/%/blaze-exhaustive.metapack.deps
s schemas/example/test/%/bundle.metapack
s version.json
--- ./schemas/example/test/%/blaze-fast.metapack.deps
s schemas/example/test/%/bundle.metapack
s version.json
--- ./schemas/example/test/%/bundle.metapack.deps
s schemas/example/test/%/dependencies.metapack
s schemas/example/test/%/schema.metapack
s version.json
--- ./schemas/example/test/%/dependencies.metapack.deps
s schemas/example/test/%/schema.metapack
s version.json
--- ./schemas/example/test/%/dependents.metapack.deps
s dependency-tree.metapack
--- ./schemas/example/test/%/editor.metapack.deps
s schemas/example/test/%/bundle.metapack
s version.json
--- ./schemas/example/test/%/health.metapack.deps
s schemas/example/test/%/dependencies.metapack
s schemas/example/test/%/schema.metapack
s version.json
--- ./schemas/example/test/%/locations.metapack.deps
s schemas/example/test/%/schema.metapack
s version.json
--- ./schemas/example/test/%/positions.metapack.deps
s schemas/example/test/%/schema.metapack
s version.json
--- ./schemas/example/test/%/schema.metapack.deps
s $REAL/schemas/test.json
s configuration.json
s version.json
--- ./schemas/example/test/%/stats.metapack.deps
s schemas/example/test/%/schema.metapack
s version.json
EOF

diff "$TMP/deps.txt" "$TMP/expected.txt"
