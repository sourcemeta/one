#!/bin/sh

set -o errexit
set -o nounset

TMP="$(mktemp -d)"
clean() { rm -rf "$TMP"; }
trap clean EXIT

mkdir "$TMP/schemas"

cat << 'EOF' > "$TMP/schemas/custom.json"
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
    "example": {
      "path": "./schemas",
      "resolve": {
        "urn:example:custom-meta": "http://localhost:8000/example/custom"
      }
    }
  }
}
EOF
"$1" "$TMP/one.json" "$TMP/output"

cd "$TMP/output"
find . -mindepth 1 \
  \( -path './schemas/self' -o -path './explorer/self' \) -prune \
  -o -print \
  | LC_ALL=C sort > "$TMP/manifest.txt"
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
./explorer/example/custom
./explorer/example/custom/%
./explorer/example/custom/%/schema.metapack
./explorer/example/sample
./explorer/example/sample/%
./explorer/example/sample/%/schema.metapack
./routes.bin
./schemas
./schemas/example
./schemas/example/custom
./schemas/example/custom/%
./schemas/example/custom/%/blaze-exhaustive.metapack
./schemas/example/custom/%/blaze-fast.metapack
./schemas/example/custom/%/bundle.metapack
./schemas/example/custom/%/dependencies.metapack
./schemas/example/custom/%/dependents.metapack
./schemas/example/custom/%/editor.metapack
./schemas/example/custom/%/health.metapack
./schemas/example/custom/%/locations.metapack
./schemas/example/custom/%/positions.metapack
./schemas/example/custom/%/schema.metapack
./schemas/example/custom/%/stats.metapack
./schemas/example/sample
./schemas/example/sample/%
./schemas/example/sample/%/blaze-exhaustive.metapack
./schemas/example/sample/%/blaze-fast.metapack
./schemas/example/sample/%/bundle.metapack
./schemas/example/sample/%/dependencies.metapack
./schemas/example/sample/%/dependents.metapack
./schemas/example/sample/%/editor.metapack
./schemas/example/sample/%/health.metapack
./schemas/example/sample/%/locations.metapack
./schemas/example/sample/%/positions.metapack
./schemas/example/sample/%/schema.metapack
./schemas/example/sample/%/stats.metapack
./state.bin
./version.json
EOF

diff "$TMP/manifest.txt" "$TMP/expected.txt"
