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

mkdir "$TMP/schemas"

cat << 'EOF' > "$TMP/schemas/bar.yaml"
$schema: 'https://json-schema.org/draft/2020-12/schema'
$id: https://example.com/bar
type: string
EOF

cat << 'EOF' > "$TMP/schemas/baz.yml"
$schema: 'https://json-schema.org/draft/2020-12/schema'
$id: https://example.com/baz
type: integer
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
./explorer/example/bar
./explorer/example/bar/%
./explorer/example/bar/%/schema-html.metapack
./explorer/example/bar/%/schema.metapack
./explorer/example/baz
./explorer/example/baz/%
./explorer/example/baz/%/schema-html.metapack
./explorer/example/baz/%/schema.metapack
./routes.bin
./schemas
./schemas/example
./schemas/example/bar
./schemas/example/bar/%
./schemas/example/bar/%/blaze-exhaustive.metapack
./schemas/example/bar/%/blaze-fast.metapack
./schemas/example/bar/%/bundle.metapack
./schemas/example/bar/%/dependencies.metapack
./schemas/example/bar/%/dependents.metapack
./schemas/example/bar/%/editor.metapack
./schemas/example/bar/%/health.metapack
./schemas/example/bar/%/locations.metapack
./schemas/example/bar/%/positions.metapack
./schemas/example/bar/%/schema.metapack
./schemas/example/bar/%/stats.metapack
./schemas/example/baz
./schemas/example/baz/%
./schemas/example/baz/%/blaze-exhaustive.metapack
./schemas/example/baz/%/blaze-fast.metapack
./schemas/example/baz/%/bundle.metapack
./schemas/example/baz/%/dependencies.metapack
./schemas/example/baz/%/dependents.metapack
./schemas/example/baz/%/editor.metapack
./schemas/example/baz/%/health.metapack
./schemas/example/baz/%/locations.metapack
./schemas/example/baz/%/positions.metapack
./schemas/example/baz/%/schema.metapack
./schemas/example/baz/%/stats.metapack
./state.bin
./version.json
EOF

diff "$TMP/manifest.txt" "$TMP/expected.txt"
