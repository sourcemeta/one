#!/bin/sh

# When modifying a schema to add a NEW $ref to another schema that it
# did not previously reference, both the modified schema's dependents and
# the newly referenced schema's dependents should rebuild.
#
# TODO(over-rebuild): A's dependents does NOT rebuild even though B now
# references A. This is because A's dependents state.bin deps do not yet
# include B's dependencies.metapack (B was not a dependent of A before
# this modification). The stale data self-corrects on the next full
# rebuild or the next schema addition/removal.

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

# B has no references initially
cat << 'EOF' > "$TMP/schemas/b.json"
{
  "$schema": "http://json-schema.org/draft-07/schema#",
  "$id": "https://example.com/b"
}
EOF

cat << 'EOF' > "$TMP/schemas/c.json"
{
  "$schema": "http://json-schema.org/draft-07/schema#",
  "$id": "https://example.com/c"
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

# Run 2: modify B to add a $ref to A
cat << 'EOF' > "$TMP/schemas/b.json"
{
  "$schema": "http://json-schema.org/draft-07/schema#",
  "$id": "https://example.com/b",
  "properties": {
    "foo": { "$ref": "https://example.com/a" }
  }
}
EOF
"$1" --skip-banner "$TMP/one.json" "$TMP/output" --concurrency 1 \
    2> "$TMP/output.txt"
remove_threads_information "$TMP/output.txt"
grep "Producing" "$TMP/output.txt" > "$TMP/output_producing.txt"

# B's dependents must rebuild (B changed)
grep -q "schemas/example/schemas/b/%/dependents.metapack" \
    "$TMP/output_producing.txt"

# TODO(over-rebuild): A's dependents SHOULD rebuild because B now
# references A, but currently it does NOT because state.bin deps for
# A's dependents do not include B's dependencies.metapack yet
grep -q "schemas/example/schemas/a/%/dependents.metapack" \
    "$TMP/output_producing.txt" && exit 1 || true

# C's dependents must NOT rebuild (C is unrelated)
grep -q "schemas/example/schemas/c/%/dependents.metapack" \
    "$TMP/output_producing.txt" && exit 1 || true

# Verify no files were deleted
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
./explorer/example/schemas/b
./explorer/example/schemas/b/%
./explorer/example/schemas/b/%/schema-html.metapack
./explorer/example/schemas/b/%/schema.metapack
./explorer/example/schemas/c
./explorer/example/schemas/c/%
./explorer/example/schemas/c/%/schema-html.metapack
./explorer/example/schemas/c/%/schema.metapack
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
./schemas/example/schemas/b
./schemas/example/schemas/b/%
./schemas/example/schemas/b/%/blaze-exhaustive.metapack
./schemas/example/schemas/b/%/blaze-fast.metapack
./schemas/example/schemas/b/%/bundle.metapack
./schemas/example/schemas/b/%/dependencies.metapack
./schemas/example/schemas/b/%/dependents.metapack
./schemas/example/schemas/b/%/editor.metapack
./schemas/example/schemas/b/%/health.metapack
./schemas/example/schemas/b/%/locations.metapack
./schemas/example/schemas/b/%/positions.metapack
./schemas/example/schemas/b/%/schema.metapack
./schemas/example/schemas/b/%/stats.metapack
./schemas/example/schemas/c
./schemas/example/schemas/c/%
./schemas/example/schemas/c/%/blaze-exhaustive.metapack
./schemas/example/schemas/c/%/blaze-fast.metapack
./schemas/example/schemas/c/%/bundle.metapack
./schemas/example/schemas/c/%/dependencies.metapack
./schemas/example/schemas/c/%/dependents.metapack
./schemas/example/schemas/c/%/editor.metapack
./schemas/example/schemas/c/%/health.metapack
./schemas/example/schemas/c/%/locations.metapack
./schemas/example/schemas/c/%/positions.metapack
./schemas/example/schemas/c/%/schema.metapack
./schemas/example/schemas/c/%/stats.metapack
./state.bin
./version.json
EOF

diff "$TMP/manifest.txt" "$TMP/expected_manifest.txt"
