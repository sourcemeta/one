#!/bin/sh

set -o errexit
set -o nounset

TMP="$(mktemp -d)"
clean() { rm -rf "$TMP"; }
trap clean EXIT

mkdir -p "$TMP/schemas/foo"

cat << 'EOF' > "$TMP/schemas/foo/foo.json"
{
  "$schema": "https://json-schema.org/draft/2020-12/schema",
  "type": "object"
}
EOF

cat << EOF > "$TMP/one.json"
{
  "url": "http://localhost:8000",
  "contents": {
    "foo": {
      "path": "./schemas/foo"
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
./explorer/foo
./explorer/foo/%
./explorer/foo/%/directory-html.metapack
./explorer/foo/%/directory.metapack
./explorer/foo/foo
./explorer/foo/foo/%
./explorer/foo/foo/%/schema-html.metapack
./explorer/foo/foo/%/schema.metapack
./routes.bin
./schemas
./schemas/foo
./schemas/foo/foo
./schemas/foo/foo/%
./schemas/foo/foo/%/blaze-exhaustive.metapack
./schemas/foo/foo/%/blaze-fast.metapack
./schemas/foo/foo/%/bundle.metapack
./schemas/foo/foo/%/dependencies.metapack
./schemas/foo/foo/%/dependents.metapack
./schemas/foo/foo/%/editor.metapack
./schemas/foo/foo/%/health.metapack
./schemas/foo/foo/%/locations.metapack
./schemas/foo/foo/%/positions.metapack
./schemas/foo/foo/%/schema.metapack
./schemas/foo/foo/%/stats.metapack
./state.bin
./version.json
EOF

diff "$TMP/manifest.txt" "$TMP/expected.txt"
