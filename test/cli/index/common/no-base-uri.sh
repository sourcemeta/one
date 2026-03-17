#!/bin/sh

set -o errexit
set -o nounset

TMP="$(mktemp -d)"
clean() { rm -rf "$TMP"; }
trap clean EXIT

cat << EOF > "$TMP/one.json"
{
  "url": "https://example.com",
  "html": {},
  "contents": {
    "test": {
      "contents": {
        "schemas": {
          "path": "./schemas"
        }
      }
    }
  }
}
EOF

mkdir "$TMP/schemas"

cat << 'EOF' > "$TMP/schemas/test-1.json"
{
  "$schema": "https://json-schema.org/draft/2020-12/schema",
  "$id": "https://example.com/test-1",
  "type": "string"
}
EOF

"$1" "$TMP/one.json" "$TMP/output"

cd "$TMP/output"
find . -mindepth 1 | LC_ALL=C sort > "$TMP/output.txt"
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
./explorer/test/schemas
./explorer/test/schemas/%
./explorer/test/schemas/%/directory-html.metapack
./explorer/test/schemas/%/directory.metapack
./explorer/test/schemas/test-1
./explorer/test/schemas/test-1/%
./explorer/test/schemas/test-1/%/schema-html.metapack
./explorer/test/schemas/test-1/%/schema.metapack
./routes.bin
./schemas
./schemas/test
./schemas/test/schemas
./schemas/test/schemas/test-1
./schemas/test/schemas/test-1/%
./schemas/test/schemas/test-1/%/blaze-exhaustive.metapack
./schemas/test/schemas/test-1/%/blaze-fast.metapack
./schemas/test/schemas/test-1/%/bundle.metapack
./schemas/test/schemas/test-1/%/dependencies.metapack
./schemas/test/schemas/test-1/%/dependents.metapack
./schemas/test/schemas/test-1/%/editor.metapack
./schemas/test/schemas/test-1/%/health.metapack
./schemas/test/schemas/test-1/%/locations.metapack
./schemas/test/schemas/test-1/%/positions.metapack
./schemas/test/schemas/test-1/%/schema.metapack
./schemas/test/schemas/test-1/%/stats.metapack
./state.bin
./version.json
EOF

diff "$TMP/output.txt" "$TMP/expected.txt"
