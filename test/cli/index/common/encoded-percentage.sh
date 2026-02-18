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
  "$id": "https://example.com/%25/test"
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
./explorer/example/%25
./explorer/example/%25/%
./explorer/example/%25/%/directory.metapack
./explorer/example/%25/%/directory.metapack.deps
./explorer/example/%25/test
./explorer/example/%25/test/%
./explorer/example/%25/test/%/schema.metapack
./explorer/example/%25/test/%/schema.metapack.deps
./routes.bin
./routes.bin.deps
./schemas
./schemas/example
./schemas/example/%25
./schemas/example/%25/test
./schemas/example/%25/test/%
./schemas/example/%25/test/%/blaze-exhaustive.metapack
./schemas/example/%25/test/%/blaze-exhaustive.metapack.deps
./schemas/example/%25/test/%/blaze-fast.metapack
./schemas/example/%25/test/%/blaze-fast.metapack.deps
./schemas/example/%25/test/%/bundle.metapack
./schemas/example/%25/test/%/bundle.metapack.deps
./schemas/example/%25/test/%/dependencies.metapack
./schemas/example/%25/test/%/dependencies.metapack.deps
./schemas/example/%25/test/%/dependents.metapack
./schemas/example/%25/test/%/dependents.metapack.deps
./schemas/example/%25/test/%/editor.metapack
./schemas/example/%25/test/%/editor.metapack.deps
./schemas/example/%25/test/%/health.metapack
./schemas/example/%25/test/%/health.metapack.deps
./schemas/example/%25/test/%/locations.metapack
./schemas/example/%25/test/%/locations.metapack.deps
./schemas/example/%25/test/%/positions.metapack
./schemas/example/%25/test/%/positions.metapack.deps
./schemas/example/%25/test/%/schema.metapack
./schemas/example/%25/test/%/schema.metapack.deps
./schemas/example/%25/test/%/stats.metapack
./schemas/example/%25/test/%/stats.metapack.deps
./version.json
EOF

diff "$TMP/manifest.txt" "$TMP/expected.txt"
