#!/bin/sh

set -o errexit
set -o nounset

TMP="$(mktemp -d)"
clean() { rm -rf "$TMP"; }
trap clean EXIT

mkdir "$TMP/schemas"

cat << 'EOF' > "$TMP/schemas/.period.json"
{
  "$schema": "https://json-schema.org/draft/2020-12/schema",
  "$id": "https://example.com/doc/.period.json"
}
EOF

cat << EOF > "$TMP/one.json"
{
  "url": "http://localhost:8000",
  "html": false,
  "contents": {
    "example": {
      "baseUri": "https://example.com/doc",
      "path": "./schemas"
    }
  }
}
EOF

"$1" "$TMP/one.json" "$TMP/output"

cd "$TMP/output"
find . -mindepth 1 | LC_ALL=C sort > "$TMP/manifest.txt"
cd - > /dev/null

cat << 'EOF' > "$TMP/expected.txt"
./configuration.json
./dependency-tree.metapack
./dependency-tree.metapack.deps
./explorer
./explorer/%
./explorer/%/directory.metapack
./explorer/%/directory.metapack.deps
./explorer/%/search.metapack
./explorer/%/search.metapack.deps
./explorer/example
./explorer/example/%
./explorer/example/%/directory.metapack
./explorer/example/%/directory.metapack.deps
./explorer/example/.period
./explorer/example/.period/%
./explorer/example/.period/%/schema.metapack
./explorer/example/.period/%/schema.metapack.deps
./routes.bin
./routes.bin.deps
./schemas
./schemas/example
./schemas/example/.period
./schemas/example/.period/%
./schemas/example/.period/%/blaze-exhaustive.metapack
./schemas/example/.period/%/blaze-exhaustive.metapack.deps
./schemas/example/.period/%/blaze-fast.metapack
./schemas/example/.period/%/blaze-fast.metapack.deps
./schemas/example/.period/%/bundle.metapack
./schemas/example/.period/%/bundle.metapack.deps
./schemas/example/.period/%/dependencies.metapack
./schemas/example/.period/%/dependencies.metapack.deps
./schemas/example/.period/%/dependents.metapack
./schemas/example/.period/%/dependents.metapack.deps
./schemas/example/.period/%/editor.metapack
./schemas/example/.period/%/editor.metapack.deps
./schemas/example/.period/%/health.metapack
./schemas/example/.period/%/health.metapack.deps
./schemas/example/.period/%/locations.metapack
./schemas/example/.period/%/locations.metapack.deps
./schemas/example/.period/%/positions.metapack
./schemas/example/.period/%/positions.metapack.deps
./schemas/example/.period/%/schema.metapack
./schemas/example/.period/%/schema.metapack.deps
./schemas/example/.period/%/stats.metapack
./schemas/example/.period/%/stats.metapack.deps
./version.json
EOF

diff "$TMP/manifest.txt" "$TMP/expected.txt"
