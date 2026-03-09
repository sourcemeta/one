#!/bin/sh

set -o errexit
set -o nounset

TMP="$(mktemp -d)"
clean() { rm -rf "$TMP"; }
trap clean EXIT

touch "$TMP/schemas"

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
./explorer
./explorer/%
./explorer/%/directory.metapack
./explorer/%/search.metapack
./routes.bin
./state.bin
./version.json
EOF

diff "$TMP/manifest.txt" "$TMP/expected.txt"
