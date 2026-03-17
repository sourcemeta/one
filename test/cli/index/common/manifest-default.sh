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
./explorer/%/404.metapack
./explorer/%/directory-html.metapack
./explorer/%/directory.metapack
./explorer/%/search.metapack
./explorer/example
./explorer/example/%
./explorer/example/%/directory-html.metapack
./explorer/example/%/directory.metapack
./explorer/example/test
./explorer/example/test/%
./explorer/example/test/%/schema-html.metapack
./explorer/example/test/%/schema.metapack
./routes.bin
./schemas
./schemas/example
./schemas/example/test
./schemas/example/test/%
./schemas/example/test/%/blaze-exhaustive.metapack
./schemas/example/test/%/blaze-fast.metapack
./schemas/example/test/%/bundle.metapack
./schemas/example/test/%/dependencies.metapack
./schemas/example/test/%/dependents.metapack
./schemas/example/test/%/editor.metapack
./schemas/example/test/%/health.metapack
./schemas/example/test/%/locations.metapack
./schemas/example/test/%/positions.metapack
./schemas/example/test/%/schema.metapack
./schemas/example/test/%/stats.metapack
./state.bin
./version.json
EOF

diff "$TMP/manifest.txt" "$TMP/expected.txt"
