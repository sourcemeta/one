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
find . -mindepth 1 \
  \( -path './schemas/self' -o -path './explorer/public/self' \) -prune \
  -o -print \
  | LC_ALL=C sort > "$TMP/manifest.txt"
cd - > /dev/null

cat << 'EOF' > "$TMP/expected.txt"
./authentication.bin
./configuration.json
./explorer
./explorer/public
./explorer/public/%
./explorer/public/%/401.metapack
./explorer/public/%/404.metapack
./explorer/public/%/directory-html.metapack
./explorer/public/%/directory.metapack
./explorer/public/%/login-html.metapack
./explorer/public/%/mcp.metapack
./explorer/public/%/search.metapack
./explorer/public/foo
./explorer/public/foo/%
./explorer/public/foo/%/directory-html.metapack
./explorer/public/foo/%/directory.metapack
./explorer/public/foo/%/login-html.metapack
./explorer/public/foo/foo
./explorer/public/foo/foo/%
./explorer/public/foo/foo/%/dependents.metapack
./explorer/public/foo/foo/%/schema-html.metapack
./explorer/public/foo/foo/%/schema.metapack
./routes.bin
./schemas
./schemas/foo
./schemas/foo/foo
./schemas/foo/foo/%
./schemas/foo/foo/%/blaze-exhaustive.metapack
./schemas/foo/foo/%/blaze-fast.metapack
./schemas/foo/foo/%/bundle.metapack
./schemas/foo/foo/%/dependencies.metapack
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
