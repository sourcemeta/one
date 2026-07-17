#!/bin/sh

set -o errexit
set -o nounset

TMP="$(mktemp -d)"
clean() { rm -rf "$TMP"; }
trap clean EXIT

cat << EOF > "$TMP/one.json"
{
  "url": "https://sourcemeta.com/",
  "contents": {
    "example": {
      "contents": {
        "schemas": {
          "baseUri": "https://example.com/",
          "path": "./schemas"
        }
      }
    }
  }
}
EOF

mkdir "$TMP/schemas"

cat << 'EOF' > "$TMP/schemas/test.json"
{
  "$schema": "http://json-schema.org/draft-04/schema#",
  "id": "https://example.com/test",
  "$vocabulary": {
    "https://example.com/vocab/totally-unknown": true
  },
  "type": "string"
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
./explorer/%/401.metapack
./explorer/%/404.metapack
./explorer/%/directory-html.metapack
./explorer/%/directory.metapack
./explorer/%/login-html.metapack
./explorer/%/mcp.metapack
./explorer/%/search.metapack
./explorer/example
./explorer/example/%
./explorer/example/%/directory-html.metapack
./explorer/example/%/directory.metapack
./explorer/example/schemas
./explorer/example/schemas/%
./explorer/example/schemas/%/directory-html.metapack
./explorer/example/schemas/%/directory.metapack
./explorer/example/schemas/test
./explorer/example/schemas/test/%
./explorer/example/schemas/test/%/schema-html.metapack
./explorer/example/schemas/test/%/schema.metapack
./routes.bin
./schemas
./schemas/example
./schemas/example/schemas
./schemas/example/schemas/test
./schemas/example/schemas/test/%
./schemas/example/schemas/test/%/blaze-exhaustive.metapack
./schemas/example/schemas/test/%/blaze-fast.metapack
./schemas/example/schemas/test/%/bundle.metapack
./schemas/example/schemas/test/%/dependencies.metapack
./schemas/example/schemas/test/%/dependents.metapack
./schemas/example/schemas/test/%/editor.metapack
./schemas/example/schemas/test/%/health.metapack
./schemas/example/schemas/test/%/locations.metapack
./schemas/example/schemas/test/%/positions.metapack
./schemas/example/schemas/test/%/schema.metapack
./schemas/example/schemas/test/%/stats.metapack
./state.bin
./version.json
EOF

diff "$TMP/manifest.txt" "$TMP/expected.txt"
