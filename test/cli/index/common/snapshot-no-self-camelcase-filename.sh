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
./explorer/public/example/camelcase
./explorer/public/example/camelcase/%
./explorer/public/example/camelcase/%/dependents.metapack
./explorer/public/example/camelcase/%/schema.metapack
./routes.bin
./schemas
./schemas/example
./schemas/example/camelcase
./schemas/example/camelcase/%
./schemas/example/camelcase/%/blaze-exhaustive.metapack
./schemas/example/camelcase/%/blaze-fast.metapack
./schemas/example/camelcase/%/bundle.metapack
./schemas/example/camelcase/%/dependencies.metapack
./schemas/example/camelcase/%/editor.metapack
./schemas/example/camelcase/%/health.metapack
./schemas/example/camelcase/%/locations.metapack
./schemas/example/camelcase/%/positions.metapack
./schemas/example/camelcase/%/schema.metapack
./schemas/example/camelcase/%/stats.metapack
./state.bin
./version.json
EOF

diff "$TMP/manifest.txt" "$TMP/expected.txt"
