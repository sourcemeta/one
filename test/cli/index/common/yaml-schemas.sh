#!/bin/sh

set -o errexit
set -o nounset

TMP="$(mktemp -d)"
clean() { rm -rf "$TMP"; }
trap clean EXIT

mkdir "$TMP/schemas"

cat << 'EOF' > "$TMP/schemas/yaml.yaml"
$schema: 'https://json-schema.org/draft/2020-12/schema'
$id: https://example.com/yaml.yaml
type: string
EOF

cat << 'EOF' > "$TMP/schemas/yml.yml"
$schema: 'https://json-schema.org/draft/2020-12/schema'
$id: https://example.com/yml.yml
type: string
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
./explorer/example/yaml
./explorer/example/yaml/%
./explorer/example/yaml/%/schema.metapack
./explorer/example/yaml/%/schema.metapack.deps
./explorer/example/yml
./explorer/example/yml/%
./explorer/example/yml/%/schema.metapack
./explorer/example/yml/%/schema.metapack.deps
./routes.bin
./routes.bin.deps
./schemas
./schemas/example
./schemas/example/yaml
./schemas/example/yaml/%
./schemas/example/yaml/%/blaze-exhaustive.metapack
./schemas/example/yaml/%/blaze-exhaustive.metapack.deps
./schemas/example/yaml/%/blaze-fast.metapack
./schemas/example/yaml/%/blaze-fast.metapack.deps
./schemas/example/yaml/%/bundle.metapack
./schemas/example/yaml/%/bundle.metapack.deps
./schemas/example/yaml/%/dependencies.metapack
./schemas/example/yaml/%/dependencies.metapack.deps
./schemas/example/yaml/%/dependents.metapack
./schemas/example/yaml/%/dependents.metapack.deps
./schemas/example/yaml/%/editor.metapack
./schemas/example/yaml/%/editor.metapack.deps
./schemas/example/yaml/%/health.metapack
./schemas/example/yaml/%/health.metapack.deps
./schemas/example/yaml/%/locations.metapack
./schemas/example/yaml/%/locations.metapack.deps
./schemas/example/yaml/%/positions.metapack
./schemas/example/yaml/%/positions.metapack.deps
./schemas/example/yaml/%/schema.metapack
./schemas/example/yaml/%/schema.metapack.deps
./schemas/example/yaml/%/stats.metapack
./schemas/example/yaml/%/stats.metapack.deps
./schemas/example/yml
./schemas/example/yml/%
./schemas/example/yml/%/blaze-exhaustive.metapack
./schemas/example/yml/%/blaze-exhaustive.metapack.deps
./schemas/example/yml/%/blaze-fast.metapack
./schemas/example/yml/%/blaze-fast.metapack.deps
./schemas/example/yml/%/bundle.metapack
./schemas/example/yml/%/bundle.metapack.deps
./schemas/example/yml/%/dependencies.metapack
./schemas/example/yml/%/dependencies.metapack.deps
./schemas/example/yml/%/dependents.metapack
./schemas/example/yml/%/dependents.metapack.deps
./schemas/example/yml/%/editor.metapack
./schemas/example/yml/%/editor.metapack.deps
./schemas/example/yml/%/health.metapack
./schemas/example/yml/%/health.metapack.deps
./schemas/example/yml/%/locations.metapack
./schemas/example/yml/%/locations.metapack.deps
./schemas/example/yml/%/positions.metapack
./schemas/example/yml/%/positions.metapack.deps
./schemas/example/yml/%/schema.metapack
./schemas/example/yml/%/schema.metapack.deps
./schemas/example/yml/%/stats.metapack
./schemas/example/yml/%/stats.metapack.deps
./version.json
EOF

diff "$TMP/manifest.txt" "$TMP/expected.txt"
