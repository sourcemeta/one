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
./explorer/public/%/404.metapack
./explorer/public/%/directory-html.metapack
./explorer/public/%/directory.metapack
./explorer/public/%/login-html.metapack
./explorer/public/%/mcp.metapack
./explorer/public/%/search.metapack
./explorer/public/baz
./explorer/public/baz/%
./explorer/public/baz/%/directory-html.metapack
./explorer/public/baz/%/directory.metapack
./explorer/public/baz/baz
./explorer/public/baz/baz/%
./explorer/public/baz/baz/%/directory-html.metapack
./explorer/public/baz/baz/%/directory.metapack
./explorer/public/baz/baz/test
./explorer/public/baz/baz/test/%
./explorer/public/baz/baz/test/%/dependents.metapack
./explorer/public/baz/baz/test/%/schema-html.metapack
./explorer/public/baz/baz/test/%/schema.metapack
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
