#!/bin/sh

set -o errexit
set -o nounset

TMP="$(mktemp -d)"
clean() { rm -rf "$TMP"; }
trap clean EXIT

mkdir "$TMP/schemas"

cat << 'EOF' > "$TMP/schemas/test.json"
{
  "$schema": "https://json-schema.org/draft/2020-12/schema",
  "$id": "https://example.com/test"
}
EOF

cat << EOF > "$TMP/one.json"
{
  "url": "http://localhost:8000",
  "contents": {
    "example": {
      "baseUri": "https://example.com",
      "path": "./schemas"
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
./explorer/example
./explorer/example/%
./explorer/example/%/directory-html.metapack
./explorer/example/%/directory-html.metapack.deps
./explorer/example/%/directory.metapack
./explorer/example/%/directory.metapack.deps
./explorer/example/test
./explorer/example/test/%
./explorer/example/test/%/schema-html.metapack
./explorer/example/test/%/schema-html.metapack.deps
./explorer/example/test/%/schema.metapack
./explorer/example/test/%/schema.metapack.deps
./routes.bin
./routes.bin.deps
./schemas
./schemas/example
./schemas/example/test
./schemas/example/test/%
./schemas/example/test/%/blaze-exhaustive.metapack
./schemas/example/test/%/blaze-exhaustive.metapack.deps
./schemas/example/test/%/blaze-fast.metapack
./schemas/example/test/%/blaze-fast.metapack.deps
./schemas/example/test/%/bundle.metapack
./schemas/example/test/%/bundle.metapack.deps
./schemas/example/test/%/dependencies.metapack
./schemas/example/test/%/dependencies.metapack.deps
./schemas/example/test/%/dependents.metapack
./schemas/example/test/%/dependents.metapack.deps
./schemas/example/test/%/editor.metapack
./schemas/example/test/%/editor.metapack.deps
./schemas/example/test/%/health.metapack
./schemas/example/test/%/health.metapack.deps
./schemas/example/test/%/locations.metapack
./schemas/example/test/%/locations.metapack.deps
./schemas/example/test/%/positions.metapack
./schemas/example/test/%/positions.metapack.deps
./schemas/example/test/%/schema.metapack
./schemas/example/test/%/schema.metapack.deps
./schemas/example/test/%/stats.metapack
./schemas/example/test/%/stats.metapack.deps
./version.json
EOF

diff "$TMP/manifest.txt" "$TMP/expected.txt"
