#!/bin/sh

set -o errexit
set -o nounset

TMP="$(mktemp -d)"
clean() { rm -rf "$TMP"; }
trap clean EXIT

mkdir -p "$TMP/data"

cat << 'EOF' > "$TMP/data/foo.json"
{
  "$schema": "http://json-schema.org/draft-07/schema#",
  "type": "object"
}
EOF

cat << 'EOF' > "$TMP/data/jsonschema.json"
{
  "title": "Example"
}
EOF

cat << EOF > "$TMP/one.json"
{
  "url": "http://localhost:8000",
  "html": false,
  "contents": {
    "example": {
      "include": "./data"
    }
  }
}
EOF
"$1" "$TMP/one.json" "$TMP/output"

cd "$TMP/output"
find . -mindepth 1 \
  \( -path './schemas/self' -o -path './explorer/self' \) -prune \
  -o -print \
  | LC_ALL=C sort > "$TMP/manifest.txt"
cd - > /dev/null

cat << 'EOF' > "$TMP/expected.txt"
./authentication.bin
./configuration.json
./explorer
./explorer/%
./explorer/%/directory.metapack
./explorer/%/mcp.metapack
./explorer/%/search.metapack
./explorer/example
./explorer/example/%
./explorer/example/%/directory.metapack
./explorer/example/foo
./explorer/example/foo/%
./explorer/example/foo/%/schema.metapack
./routes.bin
./schemas
./schemas/example
./schemas/example/foo
./schemas/example/foo/%
./schemas/example/foo/%/blaze-exhaustive.metapack
./schemas/example/foo/%/blaze-fast.metapack
./schemas/example/foo/%/bundle.metapack
./schemas/example/foo/%/dependencies.metapack
./schemas/example/foo/%/dependents.metapack
./schemas/example/foo/%/editor.metapack
./schemas/example/foo/%/health.metapack
./schemas/example/foo/%/locations.metapack
./schemas/example/foo/%/positions.metapack
./schemas/example/foo/%/schema.metapack
./schemas/example/foo/%/stats.metapack
./state.bin
./version.json
EOF

diff "$TMP/manifest.txt" "$TMP/expected.txt"
