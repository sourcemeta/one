#!/bin/sh

set -o errexit
set -o nounset

TMP="$(mktemp -d)"
clean() { rm -rf "$TMP"; }
trap clean EXIT

mkdir -p "$TMP/schemas/bar"

cat << 'EOF' > "$TMP/schemas/bar/test.json"
{
  "$schema": "https://json-schema.org/draft/2020-12/schema",
  "$id": "http://localhost:8000/bar/test",
  "type": "string"
}
EOF

cat << EOF > "$TMP/one.json"
{
  "url": "http://localhost:8000",
  "contents": {
    "bar": {
      "path": "./schemas/bar"
    }
  }
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
./explorer/bar
./explorer/bar/%
./explorer/bar/%/directory-html.metapack
./explorer/bar/%/directory.metapack
./explorer/bar/bar
./explorer/bar/bar/%
./explorer/bar/bar/%/directory-html.metapack
./explorer/bar/bar/%/directory.metapack
./explorer/bar/bar/test
./explorer/bar/bar/test/%
./explorer/bar/bar/test/%/schema-html.metapack
./explorer/bar/bar/test/%/schema.metapack
./routes.bin
./schemas
./schemas/bar
./schemas/bar/bar
./schemas/bar/bar/test
./schemas/bar/bar/test/%
./schemas/bar/bar/test/%/blaze-exhaustive.metapack
./schemas/bar/bar/test/%/blaze-fast.metapack
./schemas/bar/bar/test/%/bundle.metapack
./schemas/bar/bar/test/%/dependencies.metapack
./schemas/bar/bar/test/%/dependents.metapack
./schemas/bar/bar/test/%/editor.metapack
./schemas/bar/bar/test/%/health.metapack
./schemas/bar/bar/test/%/locations.metapack
./schemas/bar/bar/test/%/positions.metapack
./schemas/bar/bar/test/%/schema.metapack
./schemas/bar/bar/test/%/stats.metapack
./state.bin
./version.json
EOF

diff "$TMP/manifest.txt" "$TMP/expected.txt"
