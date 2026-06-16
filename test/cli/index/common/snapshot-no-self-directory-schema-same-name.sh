#!/bin/sh

set -o errexit
set -o nounset

TMP="$(mktemp -d)"
clean() { rm -rf "$TMP"; }
trap clean EXIT

cat << EOF > "$TMP/one.json"
{
  "url": "https://sourcemeta.com/",
  "html": {},
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

cat << 'EOF' > "$TMP/schemas/foo.json"
{
  "$schema": "http://json-schema.org/draft-07/schema#",
  "$id": "https://example.com/foo.json"
}
EOF

cat << 'EOF' > "$TMP/schemas/bar.json"
{
  "$schema": "http://json-schema.org/draft-07/schema#",
  "$id": "https://example.com/foo/bar.json"
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
./explorer/%/404.metapack
./explorer/%/directory-html.metapack
./explorer/%/directory.metapack
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
./explorer/example/schemas/foo
./explorer/example/schemas/foo/%
./explorer/example/schemas/foo/%/directory-html.metapack
./explorer/example/schemas/foo/%/directory.metapack
./explorer/example/schemas/foo/%/schema-html.metapack
./explorer/example/schemas/foo/%/schema.metapack
./explorer/example/schemas/foo/bar
./explorer/example/schemas/foo/bar/%
./explorer/example/schemas/foo/bar/%/schema-html.metapack
./explorer/example/schemas/foo/bar/%/schema.metapack
./routes.bin
./schemas
./schemas/example
./schemas/example/schemas
./schemas/example/schemas/foo
./schemas/example/schemas/foo/%
./schemas/example/schemas/foo/%/blaze-exhaustive.metapack
./schemas/example/schemas/foo/%/blaze-fast.metapack
./schemas/example/schemas/foo/%/bundle.metapack
./schemas/example/schemas/foo/%/dependencies.metapack
./schemas/example/schemas/foo/%/dependents.metapack
./schemas/example/schemas/foo/%/editor.metapack
./schemas/example/schemas/foo/%/health.metapack
./schemas/example/schemas/foo/%/locations.metapack
./schemas/example/schemas/foo/%/positions.metapack
./schemas/example/schemas/foo/%/schema.metapack
./schemas/example/schemas/foo/%/stats.metapack
./schemas/example/schemas/foo/bar
./schemas/example/schemas/foo/bar/%
./schemas/example/schemas/foo/bar/%/blaze-exhaustive.metapack
./schemas/example/schemas/foo/bar/%/blaze-fast.metapack
./schemas/example/schemas/foo/bar/%/bundle.metapack
./schemas/example/schemas/foo/bar/%/dependencies.metapack
./schemas/example/schemas/foo/bar/%/dependents.metapack
./schemas/example/schemas/foo/bar/%/editor.metapack
./schemas/example/schemas/foo/bar/%/health.metapack
./schemas/example/schemas/foo/bar/%/locations.metapack
./schemas/example/schemas/foo/bar/%/positions.metapack
./schemas/example/schemas/foo/bar/%/schema.metapack
./schemas/example/schemas/foo/bar/%/stats.metapack
./state.bin
./version.json
EOF

diff "$TMP/manifest.txt" "$TMP/expected.txt"
