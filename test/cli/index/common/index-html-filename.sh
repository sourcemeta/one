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
./explorer/example/index.html
./explorer/example/index.html/%
./explorer/example/index.html/%/schema.metapack
./explorer/example/index.html/%/schema.metapack.deps
./routes.bin
./routes.bin.deps
./schemas
./schemas/example
./schemas/example/index.html
./schemas/example/index.html/%
./schemas/example/index.html/%/blaze-exhaustive.metapack
./schemas/example/index.html/%/blaze-exhaustive.metapack.deps
./schemas/example/index.html/%/blaze-fast.metapack
./schemas/example/index.html/%/blaze-fast.metapack.deps
./schemas/example/index.html/%/bundle.metapack
./schemas/example/index.html/%/bundle.metapack.deps
./schemas/example/index.html/%/dependencies.metapack
./schemas/example/index.html/%/dependencies.metapack.deps
./schemas/example/index.html/%/dependents.metapack
./schemas/example/index.html/%/dependents.metapack.deps
./schemas/example/index.html/%/editor.metapack
./schemas/example/index.html/%/editor.metapack.deps
./schemas/example/index.html/%/health.metapack
./schemas/example/index.html/%/health.metapack.deps
./schemas/example/index.html/%/locations.metapack
./schemas/example/index.html/%/locations.metapack.deps
./schemas/example/index.html/%/positions.metapack
./schemas/example/index.html/%/positions.metapack.deps
./schemas/example/index.html/%/schema.metapack
./schemas/example/index.html/%/schema.metapack.deps
./schemas/example/index.html/%/stats.metapack
./schemas/example/index.html/%/stats.metapack.deps
./version.json
EOF

diff "$TMP/manifest.txt" "$TMP/expected.txt"
