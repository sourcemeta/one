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
./dependency-tree.metapack
./dependency-tree.metapack.deps
./explorer
./explorer/%
./explorer/%/404.metapack
./explorer/%/404.metapack.deps
./explorer/%/directory-html.metapack
./explorer/%/directory-html.metapack.deps
./explorer/%/directory.metapack
./explorer/%/directory.metapack.deps
./explorer/%/search.metapack
./explorer/%/search.metapack.deps
./explorer/test
./explorer/test/%
./explorer/test/%/directory-html.metapack
./explorer/test/%/directory-html.metapack.deps
./explorer/test/%/directory.metapack
./explorer/test/%/directory.metapack.deps
./explorer/test/schemas
./explorer/test/schemas/%
./explorer/test/schemas/%/directory-html.metapack
./explorer/test/schemas/%/directory-html.metapack.deps
./explorer/test/schemas/%/directory.metapack
./explorer/test/schemas/%/directory.metapack.deps
./explorer/test/schemas/test-1
./explorer/test/schemas/test-1/%
./explorer/test/schemas/test-1/%/schema-html.metapack
./explorer/test/schemas/test-1/%/schema-html.metapack.deps
./explorer/test/schemas/test-1/%/schema.metapack
./explorer/test/schemas/test-1/%/schema.metapack.deps
./routes.bin
./routes.bin.deps
./schemas
./schemas/test
./schemas/test/schemas
./schemas/test/schemas/test-1
./schemas/test/schemas/test-1/%
./schemas/test/schemas/test-1/%/blaze-exhaustive.metapack
./schemas/test/schemas/test-1/%/blaze-exhaustive.metapack.deps
./schemas/test/schemas/test-1/%/blaze-fast.metapack
./schemas/test/schemas/test-1/%/blaze-fast.metapack.deps
./schemas/test/schemas/test-1/%/bundle.metapack
./schemas/test/schemas/test-1/%/bundle.metapack.deps
./schemas/test/schemas/test-1/%/dependencies.metapack
./schemas/test/schemas/test-1/%/dependencies.metapack.deps
./schemas/test/schemas/test-1/%/dependents.metapack
./schemas/test/schemas/test-1/%/dependents.metapack.deps
./schemas/test/schemas/test-1/%/editor.metapack
./schemas/test/schemas/test-1/%/editor.metapack.deps
./schemas/test/schemas/test-1/%/health.metapack
./schemas/test/schemas/test-1/%/health.metapack.deps
./schemas/test/schemas/test-1/%/locations.metapack
./schemas/test/schemas/test-1/%/locations.metapack.deps
./schemas/test/schemas/test-1/%/positions.metapack
./schemas/test/schemas/test-1/%/positions.metapack.deps
./schemas/test/schemas/test-1/%/schema.metapack
./schemas/test/schemas/test-1/%/schema.metapack.deps
./schemas/test/schemas/test-1/%/stats.metapack
./schemas/test/schemas/test-1/%/stats.metapack.deps
./version.json
EOF

diff "$TMP/output.txt" "$TMP/expected.txt"
