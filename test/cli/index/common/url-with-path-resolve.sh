#!/bin/sh

set -o errexit
set -o nounset

TMP="$(mktemp -d)"
clean() { rm -rf "$TMP"; }
trap clean EXIT

cat << EOF > "$TMP/one.json"
{
  "url": "https://example.com/schemas",
  "contents": {
    "example": {
      "baseUri": "https://other.example.com",
      "path": "./schemas",
      "resolve": {
        "https://external.example.com/types.json": "/schemas/example/types.json"
      }
    }
  }
}
EOF

mkdir "$TMP/schemas"

cat << 'EOF' > "$TMP/schemas/foo.json"
{
  "$schema": "https://json-schema.org/draft/2020-12/schema",
  "$id": "https://other.example.com/foo",
  "$ref": "https://external.example.com/types.json",
  "type": "string"
}
EOF

cat << 'EOF' > "$TMP/schemas/types.json"
{
  "$schema": "https://json-schema.org/draft/2020-12/schema",
  "$id": "https://other.example.com/types.json",
  "type": "object"
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
./explorer/example
./explorer/example/%
./explorer/example/%/directory-html.metapack
./explorer/example/%/directory.metapack
./explorer/example/foo
./explorer/example/foo/%
./explorer/example/foo/%/schema-html.metapack
./explorer/example/foo/%/schema.metapack
./explorer/example/types
./explorer/example/types/%
./explorer/example/types/%/schema-html.metapack
./explorer/example/types/%/schema.metapack
./routes.bin
./schemas
./schemas/example
./schemas/example/foo
./schemas/example/foo/%
./schemas/example/foo/%/blaze-exhaustive.metapack
./schemas/example/foo/%/blaze-fast.metapack
./schemas/example/foo/%/bundle.metapack
./schemas/example/foo/%/dependencies.metapack
./schemas/example/foo/%/dependents.metapack
./schemas/example/foo/%/editor.metapack
./schemas/example/foo/%/health.metapack
./schemas/example/foo/%/locations.metapack
./schemas/example/foo/%/positions.metapack
./schemas/example/foo/%/schema.metapack
./schemas/example/foo/%/stats.metapack
./schemas/example/types
./schemas/example/types/%
./schemas/example/types/%/blaze-exhaustive.metapack
./schemas/example/types/%/blaze-fast.metapack
./schemas/example/types/%/bundle.metapack
./schemas/example/types/%/dependencies.metapack
./schemas/example/types/%/dependents.metapack
./schemas/example/types/%/editor.metapack
./schemas/example/types/%/health.metapack
./schemas/example/types/%/locations.metapack
./schemas/example/types/%/positions.metapack
./schemas/example/types/%/schema.metapack
./schemas/example/types/%/stats.metapack
./state.bin
./version.json
EOF

diff "$TMP/manifest.txt" "$TMP/expected.txt"
