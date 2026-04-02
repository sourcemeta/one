#!/bin/sh

set -o errexit
set -o nounset

TMP="$(mktemp -d)"
clean() { rm -rf "$TMP"; }
trap clean EXIT

cat << EOF > "$TMP/one.json"
{
  "url": "https://example.com/schemas",
  "html": false,
  "contents": {
    "first": {
      "baseUri": "https://example.com/first",
      "path": "./schemas-first"
    },
    "second": {
      "baseUri": "https://example.com/second",
      "path": "./schemas-second"
    }
  }
}
EOF

mkdir "$TMP/schemas-first" "$TMP/schemas-second"

cat << 'EOF' > "$TMP/schemas-first/a.json"
{
  "$schema": "https://json-schema.org/draft/2020-12/schema",
  "$id": "https://example.com/first/a",
  "type": "string"
}
EOF

cat << 'EOF' > "$TMP/schemas-second/b.json"
{
  "$schema": "https://json-schema.org/draft/2020-12/schema",
  "$id": "https://example.com/second/b",
  "type": "integer"
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
./explorer/%/directory.metapack
./explorer/%/search.metapack
./explorer/first
./explorer/first/%
./explorer/first/%/directory.metapack
./explorer/first/a
./explorer/first/a/%
./explorer/first/a/%/schema.metapack
./explorer/second
./explorer/second/%
./explorer/second/%/directory.metapack
./explorer/second/b
./explorer/second/b/%
./explorer/second/b/%/schema.metapack
./routes.bin
./schemas
./schemas/first
./schemas/first/a
./schemas/first/a/%
./schemas/first/a/%/blaze-exhaustive.metapack
./schemas/first/a/%/blaze-fast.metapack
./schemas/first/a/%/bundle.metapack
./schemas/first/a/%/dependencies.metapack
./schemas/first/a/%/dependents.metapack
./schemas/first/a/%/editor.metapack
./schemas/first/a/%/health.metapack
./schemas/first/a/%/locations.metapack
./schemas/first/a/%/positions.metapack
./schemas/first/a/%/schema.metapack
./schemas/first/a/%/stats.metapack
./schemas/second
./schemas/second/b
./schemas/second/b/%
./schemas/second/b/%/blaze-exhaustive.metapack
./schemas/second/b/%/blaze-fast.metapack
./schemas/second/b/%/bundle.metapack
./schemas/second/b/%/dependencies.metapack
./schemas/second/b/%/dependents.metapack
./schemas/second/b/%/editor.metapack
./schemas/second/b/%/health.metapack
./schemas/second/b/%/locations.metapack
./schemas/second/b/%/positions.metapack
./schemas/second/b/%/schema.metapack
./schemas/second/b/%/stats.metapack
./state.bin
./version.json
EOF

diff "$TMP/manifest.txt" "$TMP/expected.txt"
