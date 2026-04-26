#!/bin/sh

set -o errexit
set -o nounset

TMP="$(mktemp -d)"
clean() { rm -rf "$TMP"; }
trap clean EXIT

mkdir "$TMP/schemas"

cat << 'EOF' > "$TMP/schemas/test.json"
{
  "$schema": "https://json-schema.org/draft/2020-12/schema",
  "$id": "https://example.com/test"
}
EOF

cat << EOF > "$TMP/one.json"
{
  "url": "http://localhost:8000",
  "api": false,
  "html": false,
  "contents": {
    "example": {
      "baseUri": "https://example.com",
      "path": "./schemas"
    }
  }
}
EOF

remove_threads_information() {
  expr='s/ \[[^]]*[^a-z-][^]]*\]//g'
  if [ "$(uname -s)" = "Darwin" ]; then
    sed -i '' "$expr" "$1"
  else
    sed -i "$expr" "$1"
  fi
}

"$1" --skip-banner --deterministic "$TMP/one.json" "$TMP/output" --concurrency 1 2> "$TMP/output.txt"
remove_threads_information "$TMP/output.txt"

cat << EOF > "$TMP/expected.txt"
Writing output to: $(realpath "$TMP")/output
Using configuration: $(realpath "$TMP")/one.json
Detecting: $(realpath "$TMP")/schemas/test.json (#1)
(100%) Resolving: example/test.json
(  5%) Producing: configuration.json
( 11%) Producing: version.json
( 17%) Producing: schemas/example/test/%/schema.metapack
( 23%) Producing: schemas/example/test/%/dependencies.metapack
( 29%) Producing: schemas/example/test/%/locations.metapack
( 35%) Producing: schemas/example/test/%/positions.metapack
( 41%) Producing: schemas/example/test/%/stats.metapack
( 47%) Producing: schemas/example/test/%/bundle.metapack
( 52%) Producing: schemas/example/test/%/health.metapack
( 58%) Producing: explorer/example/test/%/schema.metapack
( 64%) Producing: schemas/example/test/%/blaze-exhaustive.metapack
( 70%) Producing: schemas/example/test/%/blaze-fast.metapack
( 76%) Producing: schemas/example/test/%/editor.metapack
( 82%) Producing: explorer/example/%/directory.metapack
( 88%) Producing: explorer/%/directory.metapack
( 94%) Producing: explorer/%/search.metapack
(100%) Producing: routes.bin
(100%) Combining: schemas/example/test/%/dependents.metapack
EOF

diff "$TMP/output.txt" "$TMP/expected.txt"

cd "$TMP/output"
find . -mindepth 1 | LC_ALL=C sort > "$TMP/manifest.txt"
cd - > /dev/null

cat << 'EOF' > "$TMP/expected_manifest.txt"
./configuration.json
./explorer
./explorer/%
./explorer/%/directory.metapack
./explorer/%/search.metapack
./explorer/example
./explorer/example/%
./explorer/example/%/directory.metapack
./explorer/example/test
./explorer/example/test/%
./explorer/example/test/%/schema.metapack
./routes.bin
./schemas
./schemas/example
./schemas/example/test
./schemas/example/test/%
./schemas/example/test/%/blaze-exhaustive.metapack
./schemas/example/test/%/blaze-fast.metapack
./schemas/example/test/%/bundle.metapack
./schemas/example/test/%/dependencies.metapack
./schemas/example/test/%/dependents.metapack
./schemas/example/test/%/editor.metapack
./schemas/example/test/%/health.metapack
./schemas/example/test/%/locations.metapack
./schemas/example/test/%/positions.metapack
./schemas/example/test/%/schema.metapack
./schemas/example/test/%/stats.metapack
./state.bin
./version.json
EOF

diff "$TMP/manifest.txt" "$TMP/expected_manifest.txt"
