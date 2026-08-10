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
find . -mindepth 1 \
  \( -path './schemas/self' -o -path './explorer/public/self' \) -prune \
  -o -print \
  | LC_ALL=C sort > "$TMP/manifest.txt"
cd - > /dev/null

cat << 'EOF' > "$TMP/expected.txt"
./authentication.bin
./configuration.json
./explorer
./explorer/public
./explorer/public/%
./explorer/public/%/directory.metapack
./explorer/public/%/mcp.metapack
./explorer/public/%/search.metapack
./explorer/public/example
./explorer/public/example/%
./explorer/public/example/%/directory.metapack
./explorer/public/example/%25
./explorer/public/example/%25/%
./explorer/public/example/%25/%/directory.metapack
./explorer/public/example/%25/test
./explorer/public/example/%25/test/%
./explorer/public/example/%25/test/%/dependents.metapack
./explorer/public/example/%25/test/%/schema.metapack
./routes.bin
./schemas
./schemas/example
./schemas/example/%25
./schemas/example/%25/test
./schemas/example/%25/test/%
./schemas/example/%25/test/%/blaze-exhaustive.metapack
./schemas/example/%25/test/%/blaze-fast.metapack
./schemas/example/%25/test/%/bundle.metapack
./schemas/example/%25/test/%/dependencies.metapack
./schemas/example/%25/test/%/editor.metapack
./schemas/example/%25/test/%/health.metapack
./schemas/example/%25/test/%/locations.metapack
./schemas/example/%25/test/%/positions.metapack
./schemas/example/%25/test/%/schema.metapack
./schemas/example/%25/test/%/stats.metapack
./state.bin
./version.json
EOF

diff "$TMP/manifest.txt" "$TMP/expected.txt"
