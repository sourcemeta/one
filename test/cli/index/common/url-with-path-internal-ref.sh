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
    "test": {
      "path": "./schemas"
    }
  }
}
EOF

mkdir "$TMP/schemas"

cat << 'EOF' > "$TMP/schemas/foo.json"
{
  "$schema": "https://json-schema.org/draft/2020-12/schema",
  "$id": "https://example.com/schemas/foo",
  "allOf": [ { "$ref": "https://example.com/schemas/bar" } ]
}
EOF

cat << 'EOF' > "$TMP/schemas/bar.json"
{
  "$schema": "https://json-schema.org/draft/2020-12/schema",
  "$id": "https://example.com/schemas/bar",
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
./explorer/test
./explorer/test/%
./explorer/test/%/directory-html.metapack
./explorer/test/%/directory.metapack
./explorer/test/bar
./explorer/test/bar/%
./explorer/test/bar/%/schema-html.metapack
./explorer/test/bar/%/schema.metapack
./explorer/test/foo
./explorer/test/foo/%
./explorer/test/foo/%/schema-html.metapack
./explorer/test/foo/%/schema.metapack
./routes.bin
./schemas
./schemas/test
./schemas/test/bar
./schemas/test/bar/%
./schemas/test/bar/%/blaze-exhaustive.metapack
./schemas/test/bar/%/blaze-fast.metapack
./schemas/test/bar/%/bundle.metapack
./schemas/test/bar/%/dependencies.metapack
./schemas/test/bar/%/dependents.metapack
./schemas/test/bar/%/editor.metapack
./schemas/test/bar/%/health.metapack
./schemas/test/bar/%/locations.metapack
./schemas/test/bar/%/positions.metapack
./schemas/test/bar/%/schema.metapack
./schemas/test/bar/%/stats.metapack
./schemas/test/foo
./schemas/test/foo/%
./schemas/test/foo/%/blaze-exhaustive.metapack
./schemas/test/foo/%/blaze-fast.metapack
./schemas/test/foo/%/bundle.metapack
./schemas/test/foo/%/dependencies.metapack
./schemas/test/foo/%/dependents.metapack
./schemas/test/foo/%/editor.metapack
./schemas/test/foo/%/health.metapack
./schemas/test/foo/%/locations.metapack
./schemas/test/foo/%/positions.metapack
./schemas/test/foo/%/schema.metapack
./schemas/test/foo/%/stats.metapack
./state.bin
./version.json
EOF

diff "$TMP/manifest.txt" "$TMP/expected.txt"
