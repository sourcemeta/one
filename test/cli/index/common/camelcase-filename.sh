#!/bin/sh

set -o errexit
set -o nounset

TMP="$(mktemp -d)"
clean() { rm -rf "$TMP"; }
trap clean EXIT

mkdir "$TMP/schemas"

cat << 'EOF' > "$TMP/schemas/CamelCase.json"
{
  "$schema": "https://json-schema.org/draft/2020-12/schema",
  "$id": "https://example.com/CamelCase.json"
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
./explorer/example/camelcase
./explorer/example/camelcase/%
./explorer/example/camelcase/%/schema.metapack
./explorer/example/camelcase/%/schema.metapack.deps
./routes.bin
./routes.bin.deps
./schemas
./schemas/example
./schemas/example/camelcase
./schemas/example/camelcase/%
./schemas/example/camelcase/%/blaze-exhaustive.metapack
./schemas/example/camelcase/%/blaze-exhaustive.metapack.deps
./schemas/example/camelcase/%/blaze-fast.metapack
./schemas/example/camelcase/%/blaze-fast.metapack.deps
./schemas/example/camelcase/%/bundle.metapack
./schemas/example/camelcase/%/bundle.metapack.deps
./schemas/example/camelcase/%/dependencies.metapack
./schemas/example/camelcase/%/dependencies.metapack.deps
./schemas/example/camelcase/%/dependents.metapack
./schemas/example/camelcase/%/dependents.metapack.deps
./schemas/example/camelcase/%/editor.metapack
./schemas/example/camelcase/%/editor.metapack.deps
./schemas/example/camelcase/%/health.metapack
./schemas/example/camelcase/%/health.metapack.deps
./schemas/example/camelcase/%/locations.metapack
./schemas/example/camelcase/%/locations.metapack.deps
./schemas/example/camelcase/%/positions.metapack
./schemas/example/camelcase/%/positions.metapack.deps
./schemas/example/camelcase/%/schema.metapack
./schemas/example/camelcase/%/schema.metapack.deps
./schemas/example/camelcase/%/stats.metapack
./schemas/example/camelcase/%/stats.metapack.deps
./version.json
EOF

diff "$TMP/manifest.txt" "$TMP/expected.txt"
