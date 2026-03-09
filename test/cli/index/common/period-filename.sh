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
./deps.bin
./explorer
./explorer/%
./explorer/%/directory.metapack
./explorer/%/search.metapack
./explorer/example
./explorer/example/%
./explorer/example/%/directory.metapack
./explorer/example/.period
./explorer/example/.period/%
./explorer/example/.period/%/schema.metapack
./routes.bin
./schemas
./schemas/example
./schemas/example/.period
./schemas/example/.period/%
./schemas/example/.period/%/blaze-exhaustive.metapack
./schemas/example/.period/%/blaze-fast.metapack
./schemas/example/.period/%/bundle.metapack
./schemas/example/.period/%/dependencies.metapack
./schemas/example/.period/%/dependents.metapack
./schemas/example/.period/%/editor.metapack
./schemas/example/.period/%/health.metapack
./schemas/example/.period/%/locations.metapack
./schemas/example/.period/%/positions.metapack
./schemas/example/.period/%/schema.metapack
./schemas/example/.period/%/stats.metapack
./version.json
EOF

diff "$TMP/manifest.txt" "$TMP/expected.txt"
