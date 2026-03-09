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
./explorer
./explorer/%
./explorer/%/directory.metapack
./explorer/%/search.metapack
./explorer/example
./explorer/example/%
./explorer/example/%/directory.metapack
./explorer/example/with
./explorer/example/with/%
./explorer/example/with/%/schema.metapack
./explorer/example/without
./explorer/example/without/%
./explorer/example/without/%/schema.metapack
./routes.bin
./schemas
./schemas/example
./schemas/example/with
./schemas/example/with/%
./schemas/example/with/%/blaze-exhaustive.metapack
./schemas/example/with/%/blaze-fast.metapack
./schemas/example/with/%/bundle.metapack
./schemas/example/with/%/dependencies.metapack
./schemas/example/with/%/dependents.metapack
./schemas/example/with/%/editor.metapack
./schemas/example/with/%/health.metapack
./schemas/example/with/%/locations.metapack
./schemas/example/with/%/positions.metapack
./schemas/example/with/%/schema.metapack
./schemas/example/with/%/stats.metapack
./schemas/example/without
./schemas/example/without/%
./schemas/example/without/%/blaze-exhaustive.metapack
./schemas/example/without/%/blaze-fast.metapack
./schemas/example/without/%/bundle.metapack
./schemas/example/without/%/dependencies.metapack
./schemas/example/without/%/dependents.metapack
./schemas/example/without/%/editor.metapack
./schemas/example/without/%/health.metapack
./schemas/example/without/%/locations.metapack
./schemas/example/without/%/positions.metapack
./schemas/example/without/%/schema.metapack
./schemas/example/without/%/stats.metapack
./state.bin
./version.json
EOF

diff "$TMP/manifest.txt" "$TMP/expected.txt"
