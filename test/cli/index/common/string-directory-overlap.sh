#!/bin/sh

set -o errexit
set -o nounset

TMP="$(mktemp -d)"
clean() { rm -rf "$TMP"; }
trap clean EXIT

mkdir "$TMP/schemas"

cat << 'EOF' > "$TMP/schemas/string.json"
{
  "$schema": "https://json-schema.org/draft/2020-12/schema",
  "$id": "https://example.com/string"
}
EOF

cat << 'EOF' > "$TMP/schemas/overlap.json"
{
  "$schema": "https://json-schema.org/draft/2020-12/schema",
  "$id": "https://example.com/string/overlap"
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
./explorer/example/string
./explorer/example/string/%
./explorer/example/string/%/directory.metapack
./explorer/example/string/%/schema.metapack
./explorer/example/string/overlap
./explorer/example/string/overlap/%
./explorer/example/string/overlap/%/schema.metapack
./routes.bin
./schemas
./schemas/example
./schemas/example/string
./schemas/example/string/%
./schemas/example/string/%/blaze-exhaustive.metapack
./schemas/example/string/%/blaze-fast.metapack
./schemas/example/string/%/bundle.metapack
./schemas/example/string/%/dependencies.metapack
./schemas/example/string/%/dependents.metapack
./schemas/example/string/%/editor.metapack
./schemas/example/string/%/health.metapack
./schemas/example/string/%/locations.metapack
./schemas/example/string/%/positions.metapack
./schemas/example/string/%/schema.metapack
./schemas/example/string/%/stats.metapack
./schemas/example/string/overlap
./schemas/example/string/overlap/%
./schemas/example/string/overlap/%/blaze-exhaustive.metapack
./schemas/example/string/overlap/%/blaze-fast.metapack
./schemas/example/string/overlap/%/bundle.metapack
./schemas/example/string/overlap/%/dependencies.metapack
./schemas/example/string/overlap/%/dependents.metapack
./schemas/example/string/overlap/%/editor.metapack
./schemas/example/string/overlap/%/health.metapack
./schemas/example/string/overlap/%/locations.metapack
./schemas/example/string/overlap/%/positions.metapack
./schemas/example/string/overlap/%/schema.metapack
./schemas/example/string/overlap/%/stats.metapack
./state.bin
./version.json
EOF

diff "$TMP/manifest.txt" "$TMP/expected.txt"
