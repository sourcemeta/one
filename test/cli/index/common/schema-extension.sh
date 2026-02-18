#!/bin/sh

set -o errexit
set -o nounset

TMP="$(mktemp -d)"
clean() { rm -rf "$TMP"; }
trap clean EXIT

mkdir "$TMP/schemas"

cat << 'EOF' > "$TMP/schemas/with.schema.json"
{
  "$schema": "https://json-schema.org/draft/2020-12/schema",
  "$id": "https://example.com/with.schema.json"
}
EOF

cat << 'EOF' > "$TMP/schemas/without.json"
{
  "$schema": "https://json-schema.org/draft/2020-12/schema",
  "$id": "https://example.com/without"
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
./explorer/example/with
./explorer/example/with/%
./explorer/example/with/%/schema.metapack
./explorer/example/with/%/schema.metapack.deps
./explorer/example/without
./explorer/example/without/%
./explorer/example/without/%/schema.metapack
./explorer/example/without/%/schema.metapack.deps
./routes.bin
./routes.bin.deps
./schemas
./schemas/example
./schemas/example/with
./schemas/example/with/%
./schemas/example/with/%/blaze-exhaustive.metapack
./schemas/example/with/%/blaze-exhaustive.metapack.deps
./schemas/example/with/%/blaze-fast.metapack
./schemas/example/with/%/blaze-fast.metapack.deps
./schemas/example/with/%/bundle.metapack
./schemas/example/with/%/bundle.metapack.deps
./schemas/example/with/%/dependencies.metapack
./schemas/example/with/%/dependencies.metapack.deps
./schemas/example/with/%/dependents.metapack
./schemas/example/with/%/dependents.metapack.deps
./schemas/example/with/%/editor.metapack
./schemas/example/with/%/editor.metapack.deps
./schemas/example/with/%/health.metapack
./schemas/example/with/%/health.metapack.deps
./schemas/example/with/%/locations.metapack
./schemas/example/with/%/locations.metapack.deps
./schemas/example/with/%/positions.metapack
./schemas/example/with/%/positions.metapack.deps
./schemas/example/with/%/schema.metapack
./schemas/example/with/%/schema.metapack.deps
./schemas/example/with/%/stats.metapack
./schemas/example/with/%/stats.metapack.deps
./schemas/example/without
./schemas/example/without/%
./schemas/example/without/%/blaze-exhaustive.metapack
./schemas/example/without/%/blaze-exhaustive.metapack.deps
./schemas/example/without/%/blaze-fast.metapack
./schemas/example/without/%/blaze-fast.metapack.deps
./schemas/example/without/%/bundle.metapack
./schemas/example/without/%/bundle.metapack.deps
./schemas/example/without/%/dependencies.metapack
./schemas/example/without/%/dependencies.metapack.deps
./schemas/example/without/%/dependents.metapack
./schemas/example/without/%/dependents.metapack.deps
./schemas/example/without/%/editor.metapack
./schemas/example/without/%/editor.metapack.deps
./schemas/example/without/%/health.metapack
./schemas/example/without/%/health.metapack.deps
./schemas/example/without/%/locations.metapack
./schemas/example/without/%/locations.metapack.deps
./schemas/example/without/%/positions.metapack
./schemas/example/without/%/positions.metapack.deps
./schemas/example/without/%/schema.metapack
./schemas/example/without/%/schema.metapack.deps
./schemas/example/without/%/stats.metapack
./schemas/example/without/%/stats.metapack.deps
./version.json
EOF

diff "$TMP/manifest.txt" "$TMP/expected.txt"
