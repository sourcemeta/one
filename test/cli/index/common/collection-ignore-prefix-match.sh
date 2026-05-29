#!/bin/sh

set -o errexit
set -o nounset

TMP="$(mktemp -d)"
clean() { rm -rf "$TMP"; }
trap clean EXIT

mkdir -p "$TMP/schemas/foo" "$TMP/schemas/foobar"

cat << 'EOF' > "$TMP/schemas/foo/junk.json"
[1, 2, 3]
EOF

cat << 'EOF' > "$TMP/schemas/foobar/baz.json"
{
  "$schema": "http://json-schema.org/draft-07/schema#",
  "$id": "https://example.com/foobar/baz",
  "type": "object"
}
EOF

cat << EOF > "$TMP/one.json"
{
  "url": "http://localhost:8000",
  "html": false,
  "contents": {
    "example": {
      "baseUri": "https://example.com",
      "path": "./schemas",
      "ignore": [
        "./schemas/foo"
      ]
    }
  }
}
EOF

"$1" "$TMP/one.json" "$TMP/output"

cd "$TMP/output"
find . -mindepth 1 | LC_ALL=C sort \
  | sed -E -e '/^\.\/explorer\/self(\/|$)/d' \
           -e '/^\.\/schemas\/self(\/|$)/d' \
  > "$TMP/manifest.txt"
cd - > /dev/null

cat << 'EOF' > "$TMP/expected.txt"
./configuration.json
./explorer
./explorer/%
./explorer/%/directory.metapack
./explorer/%/mcp.metapack
./explorer/%/search.metapack
./explorer/example
./explorer/example/%
./explorer/example/%/directory.metapack
./explorer/example/foobar
./explorer/example/foobar/%
./explorer/example/foobar/%/directory.metapack
./explorer/example/foobar/baz
./explorer/example/foobar/baz/%
./explorer/example/foobar/baz/%/schema.metapack
./routes.bin
./schemas
./schemas/example
./schemas/example/foobar
./schemas/example/foobar/baz
./schemas/example/foobar/baz/%
./schemas/example/foobar/baz/%/blaze-exhaustive.metapack
./schemas/example/foobar/baz/%/blaze-fast.metapack
./schemas/example/foobar/baz/%/bundle.metapack
./schemas/example/foobar/baz/%/dependencies.metapack
./schemas/example/foobar/baz/%/dependents.metapack
./schemas/example/foobar/baz/%/editor.metapack
./schemas/example/foobar/baz/%/health.metapack
./schemas/example/foobar/baz/%/locations.metapack
./schemas/example/foobar/baz/%/positions.metapack
./schemas/example/foobar/baz/%/schema.metapack
./schemas/example/foobar/baz/%/stats.metapack
./state.bin
./version.json
EOF

diff "$TMP/manifest.txt" "$TMP/expected.txt"
