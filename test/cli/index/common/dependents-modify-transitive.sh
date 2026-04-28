#!/bin/sh

# When modifying a schema in a transitive reference chain (C refs B refs A),
# the dependents of every schema in the chain should rebuild, but unrelated
# schemas' dependents must not be scheduled.

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

# B references A
cat << 'EOF' > "$TMP/schemas/b.json"
{
  "$schema": "http://json-schema.org/draft-07/schema#",
  "$id": "https://example.com/b",
  "properties": {
    "foo": { "$ref": "https://example.com/a" }
  }
}
EOF

# C references B (transitive chain: C -> B -> A)
cat << 'EOF' > "$TMP/schemas/c.json"
{
  "$schema": "http://json-schema.org/draft-07/schema#",
  "$id": "https://example.com/c",
  "properties": {
    "foo": { "$ref": "https://example.com/b" }
  }
}
EOF

# D is standalone
cat << 'EOF' > "$TMP/schemas/d.json"
{
  "$schema": "http://json-schema.org/draft-07/schema#",
  "$id": "https://example.com/d"
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

# Run 2: modify C (add a property, keep the $ref to B)
cat << 'EOF' > "$TMP/schemas/c.json"
{
  "$schema": "http://json-schema.org/draft-07/schema#",
  "$id": "https://example.com/c",
  "properties": {
    "foo": { "$ref": "https://example.com/b" },
    "bar": { "type": "string" }
  }
}
EOF
"$1" --skip-banner "$TMP/one.json" "$TMP/output" --concurrency 1 \
    2> "$TMP/output.txt"
remove_threads_information "$TMP/output.txt"
grep -E "Producing|Combining" "$TMP/output.txt" > "$TMP/output_producing.txt"

# No dependents should rebuild: C's references did not change
# (it still references B the same way), so no schema's dependents are affected
grep -q "dependents.metapack" "$TMP/output_producing.txt" && exit 1

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
./explorer/example/schemas/d
./explorer/example/schemas/d/%
./explorer/example/schemas/d/%/schema-html.metapack
./explorer/example/schemas/d/%/schema.metapack
./explorer/self
./explorer/self/%
./explorer/self/%/directory-html.metapack
./explorer/self/%/directory.metapack
./explorer/self/v1
./explorer/self/v1/%
./explorer/self/v1/%/directory-html.metapack
./explorer/self/v1/%/directory.metapack
./explorer/self/v1/schemas
./explorer/self/v1/schemas/%
./explorer/self/v1/schemas/%/directory-html.metapack
./explorer/self/v1/schemas/%/directory.metapack
./explorer/self/v1/schemas/api
./explorer/self/v1/schemas/api/%
./explorer/self/v1/schemas/api/%/directory-html.metapack
./explorer/self/v1/schemas/api/%/directory.metapack
./explorer/self/v1/schemas/api/error
./explorer/self/v1/schemas/api/error/%
./explorer/self/v1/schemas/api/error/%/schema-html.metapack
./explorer/self/v1/schemas/api/error/%/schema.metapack
./explorer/self/v1/schemas/api/list
./explorer/self/v1/schemas/api/list/%
./explorer/self/v1/schemas/api/list/%/directory-html.metapack
./explorer/self/v1/schemas/api/list/%/directory.metapack
./explorer/self/v1/schemas/api/list/response
./explorer/self/v1/schemas/api/list/response/%
./explorer/self/v1/schemas/api/list/response/%/schema-html.metapack
./explorer/self/v1/schemas/api/list/response/%/schema.metapack
./explorer/self/v1/schemas/api/schemas
./explorer/self/v1/schemas/api/schemas/%
./explorer/self/v1/schemas/api/schemas/%/directory-html.metapack
./explorer/self/v1/schemas/api/schemas/%/directory.metapack
./explorer/self/v1/schemas/api/schemas/dependencies
./explorer/self/v1/schemas/api/schemas/dependencies/%
./explorer/self/v1/schemas/api/schemas/dependencies/%/directory-html.metapack
./explorer/self/v1/schemas/api/schemas/dependencies/%/directory.metapack
./explorer/self/v1/schemas/api/schemas/dependencies/response
./explorer/self/v1/schemas/api/schemas/dependencies/response/%
./explorer/self/v1/schemas/api/schemas/dependencies/response/%/schema-html.metapack
./explorer/self/v1/schemas/api/schemas/dependencies/response/%/schema.metapack
./explorer/self/v1/schemas/api/schemas/dependents
./explorer/self/v1/schemas/api/schemas/dependents/%
./explorer/self/v1/schemas/api/schemas/dependents/%/directory-html.metapack
./explorer/self/v1/schemas/api/schemas/dependents/%/directory.metapack
./explorer/self/v1/schemas/api/schemas/dependents/response
./explorer/self/v1/schemas/api/schemas/dependents/response/%
./explorer/self/v1/schemas/api/schemas/dependents/response/%/schema-html.metapack
./explorer/self/v1/schemas/api/schemas/dependents/response/%/schema.metapack
./explorer/self/v1/schemas/api/schemas/evaluate
./explorer/self/v1/schemas/api/schemas/evaluate/%
./explorer/self/v1/schemas/api/schemas/evaluate/%/directory-html.metapack
./explorer/self/v1/schemas/api/schemas/evaluate/%/directory.metapack
./explorer/self/v1/schemas/api/schemas/evaluate/response
./explorer/self/v1/schemas/api/schemas/evaluate/response/%
./explorer/self/v1/schemas/api/schemas/evaluate/response/%/schema-html.metapack
./explorer/self/v1/schemas/api/schemas/evaluate/response/%/schema.metapack
./explorer/self/v1/schemas/api/schemas/health
./explorer/self/v1/schemas/api/schemas/health/%
./explorer/self/v1/schemas/api/schemas/health/%/directory-html.metapack
./explorer/self/v1/schemas/api/schemas/health/%/directory.metapack
./explorer/self/v1/schemas/api/schemas/health/response
./explorer/self/v1/schemas/api/schemas/health/response/%
./explorer/self/v1/schemas/api/schemas/health/response/%/schema-html.metapack
./explorer/self/v1/schemas/api/schemas/health/response/%/schema.metapack
./explorer/self/v1/schemas/api/schemas/locations
./explorer/self/v1/schemas/api/schemas/locations/%
./explorer/self/v1/schemas/api/schemas/locations/%/directory-html.metapack
./explorer/self/v1/schemas/api/schemas/locations/%/directory.metapack
./explorer/self/v1/schemas/api/schemas/locations/response
./explorer/self/v1/schemas/api/schemas/locations/response/%
./explorer/self/v1/schemas/api/schemas/locations/response/%/schema-html.metapack
./explorer/self/v1/schemas/api/schemas/locations/response/%/schema.metapack
./explorer/self/v1/schemas/api/schemas/metadata
./explorer/self/v1/schemas/api/schemas/metadata/%
./explorer/self/v1/schemas/api/schemas/metadata/%/directory-html.metapack
./explorer/self/v1/schemas/api/schemas/metadata/%/directory.metapack
./explorer/self/v1/schemas/api/schemas/metadata/response
./explorer/self/v1/schemas/api/schemas/metadata/response/%
./explorer/self/v1/schemas/api/schemas/metadata/response/%/schema-html.metapack
./explorer/self/v1/schemas/api/schemas/metadata/response/%/schema.metapack
./explorer/self/v1/schemas/api/schemas/position
./explorer/self/v1/schemas/api/schemas/position/%
./explorer/self/v1/schemas/api/schemas/position/%/schema-html.metapack
./explorer/self/v1/schemas/api/schemas/position/%/schema.metapack
./explorer/self/v1/schemas/api/schemas/positions
./explorer/self/v1/schemas/api/schemas/positions/%
./explorer/self/v1/schemas/api/schemas/positions/%/directory-html.metapack
./explorer/self/v1/schemas/api/schemas/positions/%/directory.metapack
./explorer/self/v1/schemas/api/schemas/positions/response
./explorer/self/v1/schemas/api/schemas/positions/response/%
./explorer/self/v1/schemas/api/schemas/positions/response/%/schema-html.metapack
./explorer/self/v1/schemas/api/schemas/positions/response/%/schema.metapack
./explorer/self/v1/schemas/api/schemas/search
./explorer/self/v1/schemas/api/schemas/search/%
./explorer/self/v1/schemas/api/schemas/search/%/directory-html.metapack
./explorer/self/v1/schemas/api/schemas/search/%/directory.metapack
./explorer/self/v1/schemas/api/schemas/search/response
./explorer/self/v1/schemas/api/schemas/search/response/%
./explorer/self/v1/schemas/api/schemas/search/response/%/schema-html.metapack
./explorer/self/v1/schemas/api/schemas/search/response/%/schema.metapack
./explorer/self/v1/schemas/api/schemas/stats
./explorer/self/v1/schemas/api/schemas/stats/%
./explorer/self/v1/schemas/api/schemas/stats/%/directory-html.metapack
./explorer/self/v1/schemas/api/schemas/stats/%/directory.metapack
./explorer/self/v1/schemas/api/schemas/stats/response
./explorer/self/v1/schemas/api/schemas/stats/response/%
./explorer/self/v1/schemas/api/schemas/stats/response/%/schema-html.metapack
./explorer/self/v1/schemas/api/schemas/stats/response/%/schema.metapack
./explorer/self/v1/schemas/api/schemas/trace
./explorer/self/v1/schemas/api/schemas/trace/%
./explorer/self/v1/schemas/api/schemas/trace/%/directory-html.metapack
./explorer/self/v1/schemas/api/schemas/trace/%/directory.metapack
./explorer/self/v1/schemas/api/schemas/trace/response
./explorer/self/v1/schemas/api/schemas/trace/response/%
./explorer/self/v1/schemas/api/schemas/trace/response/%/schema-html.metapack
./explorer/self/v1/schemas/api/schemas/trace/response/%/schema.metapack
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
./schemas/example/schemas/d
./schemas/example/schemas/d/%
./schemas/example/schemas/d/%/blaze-exhaustive.metapack
./schemas/example/schemas/d/%/blaze-fast.metapack
./schemas/example/schemas/d/%/bundle.metapack
./schemas/example/schemas/d/%/dependencies.metapack
./schemas/example/schemas/d/%/dependents.metapack
./schemas/example/schemas/d/%/editor.metapack
./schemas/example/schemas/d/%/health.metapack
./schemas/example/schemas/d/%/locations.metapack
./schemas/example/schemas/d/%/positions.metapack
./schemas/example/schemas/d/%/schema.metapack
./schemas/example/schemas/d/%/stats.metapack
./schemas/self
./schemas/self/v1
./schemas/self/v1/schemas
./schemas/self/v1/schemas/api
./schemas/self/v1/schemas/api/error
./schemas/self/v1/schemas/api/error/%
./schemas/self/v1/schemas/api/error/%/blaze-exhaustive.metapack
./schemas/self/v1/schemas/api/error/%/blaze-fast.metapack
./schemas/self/v1/schemas/api/error/%/bundle.metapack
./schemas/self/v1/schemas/api/error/%/dependencies.metapack
./schemas/self/v1/schemas/api/error/%/dependents.metapack
./schemas/self/v1/schemas/api/error/%/editor.metapack
./schemas/self/v1/schemas/api/error/%/health.metapack
./schemas/self/v1/schemas/api/error/%/locations.metapack
./schemas/self/v1/schemas/api/error/%/positions.metapack
./schemas/self/v1/schemas/api/error/%/schema.metapack
./schemas/self/v1/schemas/api/error/%/stats.metapack
./schemas/self/v1/schemas/api/list
./schemas/self/v1/schemas/api/list/response
./schemas/self/v1/schemas/api/list/response/%
./schemas/self/v1/schemas/api/list/response/%/blaze-exhaustive.metapack
./schemas/self/v1/schemas/api/list/response/%/blaze-fast.metapack
./schemas/self/v1/schemas/api/list/response/%/bundle.metapack
./schemas/self/v1/schemas/api/list/response/%/dependencies.metapack
./schemas/self/v1/schemas/api/list/response/%/dependents.metapack
./schemas/self/v1/schemas/api/list/response/%/editor.metapack
./schemas/self/v1/schemas/api/list/response/%/health.metapack
./schemas/self/v1/schemas/api/list/response/%/locations.metapack
./schemas/self/v1/schemas/api/list/response/%/positions.metapack
./schemas/self/v1/schemas/api/list/response/%/schema.metapack
./schemas/self/v1/schemas/api/list/response/%/stats.metapack
./schemas/self/v1/schemas/api/schemas
./schemas/self/v1/schemas/api/schemas/dependencies
./schemas/self/v1/schemas/api/schemas/dependencies/response
./schemas/self/v1/schemas/api/schemas/dependencies/response/%
./schemas/self/v1/schemas/api/schemas/dependencies/response/%/blaze-exhaustive.metapack
./schemas/self/v1/schemas/api/schemas/dependencies/response/%/blaze-fast.metapack
./schemas/self/v1/schemas/api/schemas/dependencies/response/%/bundle.metapack
./schemas/self/v1/schemas/api/schemas/dependencies/response/%/dependencies.metapack
./schemas/self/v1/schemas/api/schemas/dependencies/response/%/dependents.metapack
./schemas/self/v1/schemas/api/schemas/dependencies/response/%/editor.metapack
./schemas/self/v1/schemas/api/schemas/dependencies/response/%/health.metapack
./schemas/self/v1/schemas/api/schemas/dependencies/response/%/locations.metapack
./schemas/self/v1/schemas/api/schemas/dependencies/response/%/positions.metapack
./schemas/self/v1/schemas/api/schemas/dependencies/response/%/schema.metapack
./schemas/self/v1/schemas/api/schemas/dependencies/response/%/stats.metapack
./schemas/self/v1/schemas/api/schemas/dependents
./schemas/self/v1/schemas/api/schemas/dependents/response
./schemas/self/v1/schemas/api/schemas/dependents/response/%
./schemas/self/v1/schemas/api/schemas/dependents/response/%/blaze-exhaustive.metapack
./schemas/self/v1/schemas/api/schemas/dependents/response/%/blaze-fast.metapack
./schemas/self/v1/schemas/api/schemas/dependents/response/%/bundle.metapack
./schemas/self/v1/schemas/api/schemas/dependents/response/%/dependencies.metapack
./schemas/self/v1/schemas/api/schemas/dependents/response/%/dependents.metapack
./schemas/self/v1/schemas/api/schemas/dependents/response/%/editor.metapack
./schemas/self/v1/schemas/api/schemas/dependents/response/%/health.metapack
./schemas/self/v1/schemas/api/schemas/dependents/response/%/locations.metapack
./schemas/self/v1/schemas/api/schemas/dependents/response/%/positions.metapack
./schemas/self/v1/schemas/api/schemas/dependents/response/%/schema.metapack
./schemas/self/v1/schemas/api/schemas/dependents/response/%/stats.metapack
./schemas/self/v1/schemas/api/schemas/evaluate
./schemas/self/v1/schemas/api/schemas/evaluate/response
./schemas/self/v1/schemas/api/schemas/evaluate/response/%
./schemas/self/v1/schemas/api/schemas/evaluate/response/%/blaze-exhaustive.metapack
./schemas/self/v1/schemas/api/schemas/evaluate/response/%/blaze-fast.metapack
./schemas/self/v1/schemas/api/schemas/evaluate/response/%/bundle.metapack
./schemas/self/v1/schemas/api/schemas/evaluate/response/%/dependencies.metapack
./schemas/self/v1/schemas/api/schemas/evaluate/response/%/dependents.metapack
./schemas/self/v1/schemas/api/schemas/evaluate/response/%/editor.metapack
./schemas/self/v1/schemas/api/schemas/evaluate/response/%/health.metapack
./schemas/self/v1/schemas/api/schemas/evaluate/response/%/locations.metapack
./schemas/self/v1/schemas/api/schemas/evaluate/response/%/positions.metapack
./schemas/self/v1/schemas/api/schemas/evaluate/response/%/schema.metapack
./schemas/self/v1/schemas/api/schemas/evaluate/response/%/stats.metapack
./schemas/self/v1/schemas/api/schemas/health
./schemas/self/v1/schemas/api/schemas/health/response
./schemas/self/v1/schemas/api/schemas/health/response/%
./schemas/self/v1/schemas/api/schemas/health/response/%/blaze-exhaustive.metapack
./schemas/self/v1/schemas/api/schemas/health/response/%/blaze-fast.metapack
./schemas/self/v1/schemas/api/schemas/health/response/%/bundle.metapack
./schemas/self/v1/schemas/api/schemas/health/response/%/dependencies.metapack
./schemas/self/v1/schemas/api/schemas/health/response/%/dependents.metapack
./schemas/self/v1/schemas/api/schemas/health/response/%/editor.metapack
./schemas/self/v1/schemas/api/schemas/health/response/%/health.metapack
./schemas/self/v1/schemas/api/schemas/health/response/%/locations.metapack
./schemas/self/v1/schemas/api/schemas/health/response/%/positions.metapack
./schemas/self/v1/schemas/api/schemas/health/response/%/schema.metapack
./schemas/self/v1/schemas/api/schemas/health/response/%/stats.metapack
./schemas/self/v1/schemas/api/schemas/locations
./schemas/self/v1/schemas/api/schemas/locations/response
./schemas/self/v1/schemas/api/schemas/locations/response/%
./schemas/self/v1/schemas/api/schemas/locations/response/%/blaze-exhaustive.metapack
./schemas/self/v1/schemas/api/schemas/locations/response/%/blaze-fast.metapack
./schemas/self/v1/schemas/api/schemas/locations/response/%/bundle.metapack
./schemas/self/v1/schemas/api/schemas/locations/response/%/dependencies.metapack
./schemas/self/v1/schemas/api/schemas/locations/response/%/dependents.metapack
./schemas/self/v1/schemas/api/schemas/locations/response/%/editor.metapack
./schemas/self/v1/schemas/api/schemas/locations/response/%/health.metapack
./schemas/self/v1/schemas/api/schemas/locations/response/%/locations.metapack
./schemas/self/v1/schemas/api/schemas/locations/response/%/positions.metapack
./schemas/self/v1/schemas/api/schemas/locations/response/%/schema.metapack
./schemas/self/v1/schemas/api/schemas/locations/response/%/stats.metapack
./schemas/self/v1/schemas/api/schemas/metadata
./schemas/self/v1/schemas/api/schemas/metadata/response
./schemas/self/v1/schemas/api/schemas/metadata/response/%
./schemas/self/v1/schemas/api/schemas/metadata/response/%/blaze-exhaustive.metapack
./schemas/self/v1/schemas/api/schemas/metadata/response/%/blaze-fast.metapack
./schemas/self/v1/schemas/api/schemas/metadata/response/%/bundle.metapack
./schemas/self/v1/schemas/api/schemas/metadata/response/%/dependencies.metapack
./schemas/self/v1/schemas/api/schemas/metadata/response/%/dependents.metapack
./schemas/self/v1/schemas/api/schemas/metadata/response/%/editor.metapack
./schemas/self/v1/schemas/api/schemas/metadata/response/%/health.metapack
./schemas/self/v1/schemas/api/schemas/metadata/response/%/locations.metapack
./schemas/self/v1/schemas/api/schemas/metadata/response/%/positions.metapack
./schemas/self/v1/schemas/api/schemas/metadata/response/%/schema.metapack
./schemas/self/v1/schemas/api/schemas/metadata/response/%/stats.metapack
./schemas/self/v1/schemas/api/schemas/position
./schemas/self/v1/schemas/api/schemas/position/%
./schemas/self/v1/schemas/api/schemas/position/%/blaze-exhaustive.metapack
./schemas/self/v1/schemas/api/schemas/position/%/blaze-fast.metapack
./schemas/self/v1/schemas/api/schemas/position/%/bundle.metapack
./schemas/self/v1/schemas/api/schemas/position/%/dependencies.metapack
./schemas/self/v1/schemas/api/schemas/position/%/dependents.metapack
./schemas/self/v1/schemas/api/schemas/position/%/editor.metapack
./schemas/self/v1/schemas/api/schemas/position/%/health.metapack
./schemas/self/v1/schemas/api/schemas/position/%/locations.metapack
./schemas/self/v1/schemas/api/schemas/position/%/positions.metapack
./schemas/self/v1/schemas/api/schemas/position/%/schema.metapack
./schemas/self/v1/schemas/api/schemas/position/%/stats.metapack
./schemas/self/v1/schemas/api/schemas/positions
./schemas/self/v1/schemas/api/schemas/positions/response
./schemas/self/v1/schemas/api/schemas/positions/response/%
./schemas/self/v1/schemas/api/schemas/positions/response/%/blaze-exhaustive.metapack
./schemas/self/v1/schemas/api/schemas/positions/response/%/blaze-fast.metapack
./schemas/self/v1/schemas/api/schemas/positions/response/%/bundle.metapack
./schemas/self/v1/schemas/api/schemas/positions/response/%/dependencies.metapack
./schemas/self/v1/schemas/api/schemas/positions/response/%/dependents.metapack
./schemas/self/v1/schemas/api/schemas/positions/response/%/editor.metapack
./schemas/self/v1/schemas/api/schemas/positions/response/%/health.metapack
./schemas/self/v1/schemas/api/schemas/positions/response/%/locations.metapack
./schemas/self/v1/schemas/api/schemas/positions/response/%/positions.metapack
./schemas/self/v1/schemas/api/schemas/positions/response/%/schema.metapack
./schemas/self/v1/schemas/api/schemas/positions/response/%/stats.metapack
./schemas/self/v1/schemas/api/schemas/search
./schemas/self/v1/schemas/api/schemas/search/response
./schemas/self/v1/schemas/api/schemas/search/response/%
./schemas/self/v1/schemas/api/schemas/search/response/%/blaze-exhaustive.metapack
./schemas/self/v1/schemas/api/schemas/search/response/%/blaze-fast.metapack
./schemas/self/v1/schemas/api/schemas/search/response/%/bundle.metapack
./schemas/self/v1/schemas/api/schemas/search/response/%/dependencies.metapack
./schemas/self/v1/schemas/api/schemas/search/response/%/dependents.metapack
./schemas/self/v1/schemas/api/schemas/search/response/%/editor.metapack
./schemas/self/v1/schemas/api/schemas/search/response/%/health.metapack
./schemas/self/v1/schemas/api/schemas/search/response/%/locations.metapack
./schemas/self/v1/schemas/api/schemas/search/response/%/positions.metapack
./schemas/self/v1/schemas/api/schemas/search/response/%/schema.metapack
./schemas/self/v1/schemas/api/schemas/search/response/%/stats.metapack
./schemas/self/v1/schemas/api/schemas/stats
./schemas/self/v1/schemas/api/schemas/stats/response
./schemas/self/v1/schemas/api/schemas/stats/response/%
./schemas/self/v1/schemas/api/schemas/stats/response/%/blaze-exhaustive.metapack
./schemas/self/v1/schemas/api/schemas/stats/response/%/blaze-fast.metapack
./schemas/self/v1/schemas/api/schemas/stats/response/%/bundle.metapack
./schemas/self/v1/schemas/api/schemas/stats/response/%/dependencies.metapack
./schemas/self/v1/schemas/api/schemas/stats/response/%/dependents.metapack
./schemas/self/v1/schemas/api/schemas/stats/response/%/editor.metapack
./schemas/self/v1/schemas/api/schemas/stats/response/%/health.metapack
./schemas/self/v1/schemas/api/schemas/stats/response/%/locations.metapack
./schemas/self/v1/schemas/api/schemas/stats/response/%/positions.metapack
./schemas/self/v1/schemas/api/schemas/stats/response/%/schema.metapack
./schemas/self/v1/schemas/api/schemas/stats/response/%/stats.metapack
./schemas/self/v1/schemas/api/schemas/trace
./schemas/self/v1/schemas/api/schemas/trace/response
./schemas/self/v1/schemas/api/schemas/trace/response/%
./schemas/self/v1/schemas/api/schemas/trace/response/%/blaze-exhaustive.metapack
./schemas/self/v1/schemas/api/schemas/trace/response/%/blaze-fast.metapack
./schemas/self/v1/schemas/api/schemas/trace/response/%/bundle.metapack
./schemas/self/v1/schemas/api/schemas/trace/response/%/dependencies.metapack
./schemas/self/v1/schemas/api/schemas/trace/response/%/dependents.metapack
./schemas/self/v1/schemas/api/schemas/trace/response/%/editor.metapack
./schemas/self/v1/schemas/api/schemas/trace/response/%/health.metapack
./schemas/self/v1/schemas/api/schemas/trace/response/%/locations.metapack
./schemas/self/v1/schemas/api/schemas/trace/response/%/positions.metapack
./schemas/self/v1/schemas/api/schemas/trace/response/%/schema.metapack
./schemas/self/v1/schemas/api/schemas/trace/response/%/stats.metapack
./state.bin
./version.json
EOF

diff "$TMP/manifest.txt" "$TMP/expected_manifest.txt"
