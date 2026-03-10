#!/bin/sh

set -o errexit
set -o nounset

TMP="$(mktemp -d)"
clean() { rm -rf "$TMP"; }
trap clean EXIT

mkdir "$TMP/schemas"

cat << 'EOF' > "$TMP/schemas/v1.2.3.json"
{
  "$schema": "https://json-schema.org/draft/2020-12/schema",
  "$id": "https://example.com/v1.2.3.json"
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

"$1" "$TMP/one.json" "$TMP/output"

cd "$TMP/output"
find . -mindepth 1 | LC_ALL=C sort > "$TMP/manifest.txt"
cd - > /dev/null

cat << 'EOF' > "$TMP/expected.txt"
./configuration.json
./dependency-tree.metapack
./explorer
./explorer/%
./explorer/%/directory.metapack
./explorer/%/search.metapack
./explorer/example
./explorer/example/%
./explorer/example/%/directory.metapack
./explorer/example/v1.2.3
./explorer/example/v1.2.3/%
./explorer/example/v1.2.3/%/schema.metapack
./routes.bin
./schemas
./schemas/example
./schemas/example/v1.2.3
./schemas/example/v1.2.3/%
./schemas/example/v1.2.3/%/blaze-exhaustive.metapack
./schemas/example/v1.2.3/%/blaze-fast.metapack
./schemas/example/v1.2.3/%/bundle.metapack
./schemas/example/v1.2.3/%/dependencies.metapack
./schemas/example/v1.2.3/%/dependents.metapack
./schemas/example/v1.2.3/%/editor.metapack
./schemas/example/v1.2.3/%/health.metapack
./schemas/example/v1.2.3/%/locations.metapack
./schemas/example/v1.2.3/%/positions.metapack
./schemas/example/v1.2.3/%/schema.metapack
./schemas/example/v1.2.3/%/stats.metapack
./state.bin
./version.json
EOF

diff "$TMP/manifest.txt" "$TMP/expected.txt"
