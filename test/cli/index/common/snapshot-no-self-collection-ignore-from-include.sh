#!/bin/sh

set -o errexit
set -o nounset

TMP="$(mktemp -d)"
clean() { rm -rf "$TMP"; }
trap clean EXIT

mkdir -p "$TMP/configs" "$TMP/data/keep" "$TMP/data/skip"

cat << 'EOF' > "$TMP/data/keep/foo.json"
{
  "$schema": "http://json-schema.org/draft-07/schema#",
  "$id": "https://example.com/keep/foo",
  "type": "object"
}
EOF

cat << 'EOF' > "$TMP/data/skip/bar.json"
[1, 2, 3]
EOF

cat << 'EOF' > "$TMP/configs/jsonschema.json"
{
  "baseUri": "https://example.com",
  "path": "../data",
  "ignore": [
    "../data/skip"
  ]
}
EOF

cat << EOF > "$TMP/one.json"
{
  "url": "http://localhost:8000",
  "html": false,
  "contents": {
    "example": {
      "include": "./configs/jsonschema.json"
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
./configuration.json
./explorer
./explorer/%
./explorer/%/directory.metapack
./explorer/%/mcp.metapack
./explorer/%/search.metapack
./explorer/example
./explorer/example/%
./explorer/example/%/directory.metapack
./explorer/example/keep
./explorer/example/keep/%
./explorer/example/keep/%/directory.metapack
./explorer/example/keep/foo
./explorer/example/keep/foo/%
./explorer/example/keep/foo/%/schema.metapack
./routes.bin
./schemas
./schemas/example
./schemas/example/keep
./schemas/example/keep/foo
./schemas/example/keep/foo/%
./schemas/example/keep/foo/%/blaze-exhaustive.metapack
./schemas/example/keep/foo/%/blaze-fast.metapack
./schemas/example/keep/foo/%/bundle.metapack
./schemas/example/keep/foo/%/dependencies.metapack
./schemas/example/keep/foo/%/dependents.metapack
./schemas/example/keep/foo/%/editor.metapack
./schemas/example/keep/foo/%/health.metapack
./schemas/example/keep/foo/%/locations.metapack
./schemas/example/keep/foo/%/positions.metapack
./schemas/example/keep/foo/%/schema.metapack
./schemas/example/keep/foo/%/stats.metapack
./state.bin
./version.json
EOF

diff "$TMP/manifest.txt" "$TMP/expected.txt"
