#!/bin/sh

set -o errexit
set -o nounset

TMP="$(mktemp -d)"
clean() { rm -rf "$TMP"; }
trap clean EXIT

mkdir -p "$TMP/schemas/baz/baz"

cat << 'EOF' > "$TMP/schemas/baz/baz/test.json"
{
  "$schema": "https://json-schema.org/draft/2020-12/schema",
  "type": "array"
}
EOF

cat << EOF > "$TMP/one.json"
{
  "url": "http://localhost:8000",
  "contents": {
    "baz": {
      "path": "./schemas/baz"
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
./explorer/baz
./explorer/baz/%
./explorer/baz/%/directory-html.metapack
./explorer/baz/%/directory.metapack
./explorer/baz/baz
./explorer/baz/baz/%
./explorer/baz/baz/%/directory-html.metapack
./explorer/baz/baz/%/directory.metapack
./explorer/baz/baz/test
./explorer/baz/baz/test/%
./explorer/baz/baz/test/%/schema-html.metapack
./explorer/baz/baz/test/%/schema.metapack
./routes.bin
./schemas
./schemas/baz
./schemas/baz/baz
./schemas/baz/baz/test
./schemas/baz/baz/test/%
./schemas/baz/baz/test/%/blaze-exhaustive.metapack
./schemas/baz/baz/test/%/blaze-fast.metapack
./schemas/baz/baz/test/%/bundle.metapack
./schemas/baz/baz/test/%/dependencies.metapack
./schemas/baz/baz/test/%/dependents.metapack
./schemas/baz/baz/test/%/editor.metapack
./schemas/baz/baz/test/%/health.metapack
./schemas/baz/baz/test/%/locations.metapack
./schemas/baz/baz/test/%/positions.metapack
./schemas/baz/baz/test/%/schema.metapack
./schemas/baz/baz/test/%/stats.metapack
./state.bin
./version.json
EOF

diff "$TMP/manifest.txt" "$TMP/expected.txt"
