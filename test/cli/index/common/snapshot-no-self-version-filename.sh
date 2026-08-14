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
./explorer/public/%/login.metapack
./explorer/public/%/mcp.metapack
./explorer/public/%/search.metapack
./explorer/public/example
./explorer/public/example/%
./explorer/public/example/%/directory.metapack
./explorer/public/example/v1.2.3
./explorer/public/example/v1.2.3/%
./explorer/public/example/v1.2.3/%/dependents.metapack
./explorer/public/example/v1.2.3/%/schema.metapack
./routes.bin
./schemas
./schemas/example
./schemas/example/v1.2.3
./schemas/example/v1.2.3/%
./schemas/example/v1.2.3/%/blaze-exhaustive.metapack
./schemas/example/v1.2.3/%/blaze-fast.metapack
./schemas/example/v1.2.3/%/bundle.metapack
./schemas/example/v1.2.3/%/dependencies.metapack
./schemas/example/v1.2.3/%/editor.metapack
./schemas/example/v1.2.3/%/health.metapack
./schemas/example/v1.2.3/%/locations.metapack
./schemas/example/v1.2.3/%/positions.metapack
./schemas/example/v1.2.3/%/schema.metapack
./schemas/example/v1.2.3/%/stats.metapack
./state.bin
./version.json
EOF

diff "$TMP/manifest.txt" "$TMP/expected.txt"
