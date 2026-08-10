#!/bin/sh

set -o errexit
set -o nounset

TMP="$(mktemp -d)"
clean() { rm -rf "$TMP"; }
trap clean EXIT

mkdir "$TMP/meta"
mkdir "$TMP/schemas"

cat << 'EOF' > "$TMP/meta/custom.json"
{
  "$schema": "https://json-schema.org/draft/2020-12/schema",
  "$vocabulary": {
    "https://json-schema.org/draft/2020-12/vocab/core": true,
    "https://json-schema.org/draft/2020-12/vocab/applicator": true,
    "https://json-schema.org/draft/2020-12/vocab/validation": true
  },
  "$dynamicAnchor": "meta"
}
EOF

cat << 'EOF' > "$TMP/schemas/sample.json"
{
  "$schema": "urn:example:custom-meta",
  "type": "string"
}
EOF

cat << EOF > "$TMP/one.json"
{
  "url": "http://localhost:8000",
  "html": false,
  "contents": {
    "meta": {
      "path": "./meta"
    },
    "example": {
      "path": "./schemas",
      "resolve": {
        "urn:example:custom-meta": "http://localhost:8000/meta/custom"
      }
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
./explorer/public/%/directory.metapack
./explorer/public/%/mcp.metapack
./explorer/public/%/search.metapack
./explorer/public/example
./explorer/public/example/%
./explorer/public/example/%/directory.metapack
./explorer/public/example/sample
./explorer/public/example/sample/%
./explorer/public/example/sample/%/dependents.metapack
./explorer/public/example/sample/%/schema.metapack
./explorer/public/meta
./explorer/public/meta/%
./explorer/public/meta/%/directory.metapack
./explorer/public/meta/custom
./explorer/public/meta/custom/%
./explorer/public/meta/custom/%/dependents.metapack
./explorer/public/meta/custom/%/schema.metapack
./routes.bin
./schemas
./schemas/example
./schemas/example/sample
./schemas/example/sample/%
./schemas/example/sample/%/blaze-exhaustive.metapack
./schemas/example/sample/%/blaze-fast.metapack
./schemas/example/sample/%/bundle.metapack
./schemas/example/sample/%/dependencies.metapack
./schemas/example/sample/%/editor.metapack
./schemas/example/sample/%/health.metapack
./schemas/example/sample/%/locations.metapack
./schemas/example/sample/%/positions.metapack
./schemas/example/sample/%/schema.metapack
./schemas/example/sample/%/stats.metapack
./schemas/meta
./schemas/meta/custom
./schemas/meta/custom/%
./schemas/meta/custom/%/blaze-exhaustive.metapack
./schemas/meta/custom/%/blaze-fast.metapack
./schemas/meta/custom/%/bundle.metapack
./schemas/meta/custom/%/dependencies.metapack
./schemas/meta/custom/%/editor.metapack
./schemas/meta/custom/%/health.metapack
./schemas/meta/custom/%/locations.metapack
./schemas/meta/custom/%/positions.metapack
./schemas/meta/custom/%/schema.metapack
./schemas/meta/custom/%/stats.metapack
./state.bin
./version.json
EOF

diff "$TMP/manifest.txt" "$TMP/expected.txt"
