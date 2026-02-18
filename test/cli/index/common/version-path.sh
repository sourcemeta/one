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
    "v2.0": {
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
./explorer/v2.0
./explorer/v2.0/%
./explorer/v2.0/%/directory.metapack
./explorer/v2.0/%/directory.metapack.deps
./explorer/v2.0/test
./explorer/v2.0/test/%
./explorer/v2.0/test/%/schema.metapack
./explorer/v2.0/test/%/schema.metapack.deps
./routes.bin
./routes.bin.deps
./schemas
./schemas/v2.0
./schemas/v2.0/test
./schemas/v2.0/test/%
./schemas/v2.0/test/%/blaze-exhaustive.metapack
./schemas/v2.0/test/%/blaze-exhaustive.metapack.deps
./schemas/v2.0/test/%/blaze-fast.metapack
./schemas/v2.0/test/%/blaze-fast.metapack.deps
./schemas/v2.0/test/%/bundle.metapack
./schemas/v2.0/test/%/bundle.metapack.deps
./schemas/v2.0/test/%/dependencies.metapack
./schemas/v2.0/test/%/dependencies.metapack.deps
./schemas/v2.0/test/%/dependents.metapack
./schemas/v2.0/test/%/dependents.metapack.deps
./schemas/v2.0/test/%/editor.metapack
./schemas/v2.0/test/%/editor.metapack.deps
./schemas/v2.0/test/%/health.metapack
./schemas/v2.0/test/%/health.metapack.deps
./schemas/v2.0/test/%/locations.metapack
./schemas/v2.0/test/%/locations.metapack.deps
./schemas/v2.0/test/%/positions.metapack
./schemas/v2.0/test/%/positions.metapack.deps
./schemas/v2.0/test/%/schema.metapack
./schemas/v2.0/test/%/schema.metapack.deps
./schemas/v2.0/test/%/stats.metapack
./schemas/v2.0/test/%/stats.metapack.deps
./version.json
EOF

diff "$TMP/manifest.txt" "$TMP/expected.txt"
