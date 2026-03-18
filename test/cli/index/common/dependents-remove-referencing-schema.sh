#!/bin/sh

# When removing a schema that references another schema, the referenced
# schema's dependents.metapack must be rebuilt in the Combine phase to
# reflect that the removed schema is no longer a dependent.

set -o errexit
set -o nounset

TMP="$(mktemp -d)"
clean() { rm -rf "$TMP"; }
trap clean EXIT

cat << EOF > "$TMP/one.json"
{
  "url": "https://sourcemeta.com/",
  "contents": {
    "example": {
      "contents": {
        "schemas": {
          "baseUri": "https://example.com/",
          "path": "./schemas"
        }
      }
    }
  }
}
EOF

mkdir "$TMP/schemas"

cat << 'EOF' > "$TMP/schemas/a.json"
{
  "$schema": "http://json-schema.org/draft-07/schema#",
  "$id": "https://example.com/a"
}
EOF

cat << 'EOF' > "$TMP/schemas/b.json"
{
  "$schema": "http://json-schema.org/draft-07/schema#",
  "$id": "https://example.com/b",
  "properties": {
    "foo": { "$ref": "https://example.com/a" }
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

# Run 1: full build from scratch
"$1" --skip-banner "$TMP/one.json" "$TMP/output" --concurrency 1 \
    > /dev/null 2>&1

# Run 2: remove schema B (which references A)
rm "$TMP/schemas/b.json"

"$1" --skip-banner "$TMP/one.json" "$TMP/output" --concurrency 1 \
    2> "$TMP/output.txt"
remove_threads_information "$TMP/output.txt"

cat << EOF > "$TMP/expected.txt"
Writing output to: $(realpath "$TMP")/output
Using configuration: $(realpath "$TMP")/one.json
Detecting: $(realpath "$TMP")/schemas/a.json (#1)
( 20%) Producing: explorer/%/directory.metapack
( 40%) Producing: explorer/%/search.metapack
( 60%) Producing: explorer/%/directory-html.metapack
( 80%) Disposing: explorer/example/schemas/b
(100%) Disposing: schemas/example/schemas/b
(100%) Combining: schemas/example/schemas/a/%/dependents.metapack
EOF
diff "$TMP/output.txt" "$TMP/expected.txt"

# Verify final manifest
cd "$TMP/output"
find . -mindepth 1 | LC_ALL=C sort > "$TMP/manifest.txt"
cd - > /dev/null

cat << 'EOF' > "$TMP/expected_manifest.txt"
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
./explorer/example/schemas
./explorer/example/schemas/%
./explorer/example/schemas/%/directory-html.metapack
./explorer/example/schemas/%/directory.metapack
./explorer/example/schemas/a
./explorer/example/schemas/a/%
./explorer/example/schemas/a/%/schema-html.metapack
./explorer/example/schemas/a/%/schema.metapack
./routes.bin
./schemas
./schemas/example
./schemas/example/schemas
./schemas/example/schemas/a
./schemas/example/schemas/a/%
./schemas/example/schemas/a/%/blaze-exhaustive.metapack
./schemas/example/schemas/a/%/blaze-fast.metapack
./schemas/example/schemas/a/%/bundle.metapack
./schemas/example/schemas/a/%/dependencies.metapack
./schemas/example/schemas/a/%/dependents.metapack
./schemas/example/schemas/a/%/editor.metapack
./schemas/example/schemas/a/%/health.metapack
./schemas/example/schemas/a/%/locations.metapack
./schemas/example/schemas/a/%/positions.metapack
./schemas/example/schemas/a/%/schema.metapack
./schemas/example/schemas/a/%/stats.metapack
./state.bin
./version.json
EOF

diff "$TMP/manifest.txt" "$TMP/expected_manifest.txt"
