#!/bin/sh

set -o errexit
set -o nounset

TMP="$(mktemp -d)"
clean() { rm -rf "$TMP"; }
trap clean EXIT

cat << EOF > "$TMP/one.json"
{
  "url": "https://example.com/schemas",
  "contents": {
    "example": {
      "baseUri": "https://example.com",
      "path": "./schemas"
    }
  }
}
EOF

mkdir -p "$TMP/schemas/nested"

cat << 'EOF' > "$TMP/schemas/nested/bar.json"
{
  "$schema": "https://json-schema.org/draft/2020-12/schema",
  "$id": "https://example.com/nested/bar",
  "type": "string"
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
./explorer/example/nested
./explorer/example/nested/%
./explorer/example/nested/%/directory-html.metapack
./explorer/example/nested/%/directory.metapack
./explorer/example/nested/bar
./explorer/example/nested/bar/%
./explorer/example/nested/bar/%/schema-html.metapack
./explorer/example/nested/bar/%/schema.metapack
./routes.bin
./schemas
./schemas/example
./schemas/example/nested
./schemas/example/nested/bar
./schemas/example/nested/bar/%
./schemas/example/nested/bar/%/blaze-exhaustive.metapack
./schemas/example/nested/bar/%/blaze-fast.metapack
./schemas/example/nested/bar/%/bundle.metapack
./schemas/example/nested/bar/%/dependencies.metapack
./schemas/example/nested/bar/%/dependents.metapack
./schemas/example/nested/bar/%/editor.metapack
./schemas/example/nested/bar/%/health.metapack
./schemas/example/nested/bar/%/locations.metapack
./schemas/example/nested/bar/%/positions.metapack
./schemas/example/nested/bar/%/schema.metapack
./schemas/example/nested/bar/%/stats.metapack
./state.bin
./version.json
EOF

diff "$TMP/manifest.txt" "$TMP/expected.txt"
