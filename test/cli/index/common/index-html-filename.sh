#!/bin/sh

set -o errexit
set -o nounset

TMP="$(mktemp -d)"
clean() { rm -rf "$TMP"; }
trap clean EXIT

mkdir "$TMP/schemas"

cat << 'EOF' > "$TMP/schemas/index.html.json"
{
  "$schema": "https://json-schema.org/draft/2020-12/schema",
  "$id": "https://example.com/index.html.json"
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
./explorer
./explorer/%
./explorer/%/directory.metapack
./explorer/%/search.metapack
./explorer/example
./explorer/example/%
./explorer/example/%/directory.metapack
./explorer/example/index.html
./explorer/example/index.html/%
./explorer/example/index.html/%/schema.metapack
./routes.bin
./schemas
./schemas/example
./schemas/example/index.html
./schemas/example/index.html/%
./schemas/example/index.html/%/blaze-exhaustive.metapack
./schemas/example/index.html/%/blaze-fast.metapack
./schemas/example/index.html/%/bundle.metapack
./schemas/example/index.html/%/dependencies.metapack
./schemas/example/index.html/%/dependents.metapack
./schemas/example/index.html/%/editor.metapack
./schemas/example/index.html/%/health.metapack
./schemas/example/index.html/%/locations.metapack
./schemas/example/index.html/%/positions.metapack
./schemas/example/index.html/%/schema.metapack
./schemas/example/index.html/%/stats.metapack
./state.bin
./version.json
EOF

diff "$TMP/manifest.txt" "$TMP/expected.txt"
