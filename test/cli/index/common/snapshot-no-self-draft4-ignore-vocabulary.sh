#!/bin/sh

set -o errexit
set -o nounset

TMP="$(mktemp -d)"
clean() { rm -rf "$TMP"; }
trap clean EXIT

cat << EOF > "$TMP/one.json"
{
  "url": "https://sourcemeta.com",
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
./explorer/public/%/404.metapack
./explorer/public/%/directory-html.metapack
./explorer/public/%/directory.metapack
./explorer/public/%/login-html.metapack
./explorer/public/%/login.metapack
./explorer/public/%/mcp.metapack
./explorer/public/%/search.metapack
./explorer/public/example
./explorer/public/example/%
./explorer/public/example/%/directory-html.metapack
./explorer/public/example/%/directory.metapack
./explorer/public/example/schemas
./explorer/public/example/schemas/%
./explorer/public/example/schemas/%/directory-html.metapack
./explorer/public/example/schemas/%/directory.metapack
./explorer/public/example/schemas/test
./explorer/public/example/schemas/test/%
./explorer/public/example/schemas/test/%/dependents.metapack
./explorer/public/example/schemas/test/%/schema-html.metapack
./explorer/public/example/schemas/test/%/schema.metapack
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
