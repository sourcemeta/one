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
./explorer/example/v1.2.3
./explorer/example/v1.2.3/%
./explorer/example/v1.2.3/%/schema.metapack
./explorer/example/v1.2.3/%/schema.metapack.deps
./routes.bin
./routes.bin.deps
./schemas
./schemas/example
./schemas/example/v1.2.3
./schemas/example/v1.2.3/%
./schemas/example/v1.2.3/%/blaze-exhaustive.metapack
./schemas/example/v1.2.3/%/blaze-exhaustive.metapack.deps
./schemas/example/v1.2.3/%/blaze-fast.metapack
./schemas/example/v1.2.3/%/blaze-fast.metapack.deps
./schemas/example/v1.2.3/%/bundle.metapack
./schemas/example/v1.2.3/%/bundle.metapack.deps
./schemas/example/v1.2.3/%/dependencies.metapack
./schemas/example/v1.2.3/%/dependencies.metapack.deps
./schemas/example/v1.2.3/%/dependents.metapack
./schemas/example/v1.2.3/%/dependents.metapack.deps
./schemas/example/v1.2.3/%/editor.metapack
./schemas/example/v1.2.3/%/editor.metapack.deps
./schemas/example/v1.2.3/%/health.metapack
./schemas/example/v1.2.3/%/health.metapack.deps
./schemas/example/v1.2.3/%/locations.metapack
./schemas/example/v1.2.3/%/locations.metapack.deps
./schemas/example/v1.2.3/%/positions.metapack
./schemas/example/v1.2.3/%/positions.metapack.deps
./schemas/example/v1.2.3/%/schema.metapack
./schemas/example/v1.2.3/%/schema.metapack.deps
./schemas/example/v1.2.3/%/stats.metapack
./schemas/example/v1.2.3/%/stats.metapack.deps
./version.json
EOF

diff "$TMP/manifest.txt" "$TMP/expected.txt"
