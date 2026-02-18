#!/bin/sh

set -o errexit
set -o nounset

TMP="$(mktemp -d)"
clean() { rm -rf "$TMP"; }
trap clean EXIT

mkdir -p "$TMP/schemas/foo"

cat << 'EOF' > "$TMP/schemas/foo.json"
{
  "$schema": "https://json-schema.org/draft/2020-12/schema",
  "$id": "https://example.com/foo"
}
EOF

cat << 'EOF' > "$TMP/schemas/foo/bar.json"
{
  "$schema": "https://json-schema.org/draft/2020-12/schema",
  "$id": "https://example.com/foo/bar"
}
EOF

cat << EOF > "$TMP/one.json"
{
  "url": "http://localhost:8000",
  "html": false,
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
./explorer/%/directory.metapack
./explorer/%/directory.metapack.deps
./explorer/%/search.metapack
./explorer/%/search.metapack.deps
./explorer/example
./explorer/example/%
./explorer/example/%/directory.metapack
./explorer/example/%/directory.metapack.deps
./explorer/example/foo
./explorer/example/foo/%
./explorer/example/foo/%/directory.metapack
./explorer/example/foo/%/directory.metapack.deps
./explorer/example/foo/%/schema.metapack
./explorer/example/foo/%/schema.metapack.deps
./explorer/example/foo/bar
./explorer/example/foo/bar/%
./explorer/example/foo/bar/%/schema.metapack
./explorer/example/foo/bar/%/schema.metapack.deps
./routes.bin
./routes.bin.deps
./schemas
./schemas/example
./schemas/example/foo
./schemas/example/foo/%
./schemas/example/foo/%/blaze-exhaustive.metapack
./schemas/example/foo/%/blaze-exhaustive.metapack.deps
./schemas/example/foo/%/blaze-fast.metapack
./schemas/example/foo/%/blaze-fast.metapack.deps
./schemas/example/foo/%/bundle.metapack
./schemas/example/foo/%/bundle.metapack.deps
./schemas/example/foo/%/dependencies.metapack
./schemas/example/foo/%/dependencies.metapack.deps
./schemas/example/foo/%/dependents.metapack
./schemas/example/foo/%/dependents.metapack.deps
./schemas/example/foo/%/editor.metapack
./schemas/example/foo/%/editor.metapack.deps
./schemas/example/foo/%/health.metapack
./schemas/example/foo/%/health.metapack.deps
./schemas/example/foo/%/locations.metapack
./schemas/example/foo/%/locations.metapack.deps
./schemas/example/foo/%/positions.metapack
./schemas/example/foo/%/positions.metapack.deps
./schemas/example/foo/%/schema.metapack
./schemas/example/foo/%/schema.metapack.deps
./schemas/example/foo/%/stats.metapack
./schemas/example/foo/%/stats.metapack.deps
./schemas/example/foo/bar
./schemas/example/foo/bar/%
./schemas/example/foo/bar/%/blaze-exhaustive.metapack
./schemas/example/foo/bar/%/blaze-exhaustive.metapack.deps
./schemas/example/foo/bar/%/blaze-fast.metapack
./schemas/example/foo/bar/%/blaze-fast.metapack.deps
./schemas/example/foo/bar/%/bundle.metapack
./schemas/example/foo/bar/%/bundle.metapack.deps
./schemas/example/foo/bar/%/dependencies.metapack
./schemas/example/foo/bar/%/dependencies.metapack.deps
./schemas/example/foo/bar/%/dependents.metapack
./schemas/example/foo/bar/%/dependents.metapack.deps
./schemas/example/foo/bar/%/editor.metapack
./schemas/example/foo/bar/%/editor.metapack.deps
./schemas/example/foo/bar/%/health.metapack
./schemas/example/foo/bar/%/health.metapack.deps
./schemas/example/foo/bar/%/locations.metapack
./schemas/example/foo/bar/%/locations.metapack.deps
./schemas/example/foo/bar/%/positions.metapack
./schemas/example/foo/bar/%/positions.metapack.deps
./schemas/example/foo/bar/%/schema.metapack
./schemas/example/foo/bar/%/schema.metapack.deps
./schemas/example/foo/bar/%/stats.metapack
./schemas/example/foo/bar/%/stats.metapack.deps
./version.json
EOF

diff "$TMP/manifest.txt" "$TMP/expected.txt"
