#!/bin/sh

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
  "$id": "https://example.com/b"
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

# Run 1: index two schemas from scratch
"$1" --skip-banner "$TMP/one.json" "$TMP/output" --concurrency 1 > /dev/null 2>&1

# Run 2: add a third schema
# Only the new schema's artifacts and affected directories should rebuild
cat << 'EOF' > "$TMP/schemas/c.json"
{
  "$schema": "http://json-schema.org/draft-07/schema#",
  "$id": "https://example.com/c"
}
EOF
"$1" --skip-banner "$TMP/one.json" "$TMP/output" --concurrency 1 2> "$TMP/output.txt"
remove_threads_information "$TMP/output.txt"

cat << EOF > "$TMP/expected.txt"
Writing output to: $(realpath "$TMP")/output
Using configuration: $(realpath "$TMP")/one.json
Detecting: $(realpath "$TMP")/schemas/a.json (#1)
Detecting: $(realpath "$TMP")/schemas/c.json (#2)
Detecting: $(realpath "$TMP")/schemas/b.json (#3)
( 33%) Resolving: a.json
( 66%) Resolving: c.json
(100%) Resolving: b.json
(  4%) Producing: schemas/example/schemas/c/%/schema.metapack
(  8%) Producing: schemas/example/schemas/c/%/dependencies.metapack
( 13%) Producing: schemas/example/schemas/c/%/locations.metapack
( 17%) Producing: schemas/example/schemas/c/%/positions.metapack
( 21%) Producing: schemas/example/schemas/c/%/stats.metapack
( 26%) Producing: dependency-tree.metapack
( 30%) Producing: schemas/example/schemas/c/%/bundle.metapack
( 34%) Producing: schemas/example/schemas/c/%/health.metapack
( 39%) Producing: explorer/example/schemas/c/%/schema.metapack
( 43%) Producing: schemas/example/schemas/a/%/dependents.metapack
( 47%) Producing: schemas/example/schemas/b/%/dependents.metapack
( 52%) Producing: schemas/example/schemas/c/%/blaze-exhaustive.metapack
( 56%) Producing: schemas/example/schemas/c/%/blaze-fast.metapack
( 60%) Producing: schemas/example/schemas/c/%/dependents.metapack
( 65%) Producing: schemas/example/schemas/c/%/editor.metapack
( 69%) Producing: explorer/%/search.metapack
( 73%) Producing: explorer/example/schemas/%/directory.metapack
( 78%) Producing: explorer/example/schemas/c/%/schema-html.metapack
( 82%) Producing: explorer/example/%/directory.metapack
( 86%) Producing: explorer/example/schemas/%/directory-html.metapack
( 91%) Producing: explorer/%/directory.metapack
( 95%) Producing: explorer/example/%/directory-html.metapack
(100%) Producing: explorer/%/directory-html.metapack
EOF

diff "$TMP/output.txt" "$TMP/expected.txt"

cd "$TMP/output"
find . -mindepth 1 | LC_ALL=C sort > "$TMP/manifest.txt"
cd - > /dev/null

cat << 'EOF' > "$TMP/expected_manifest.txt"
./configuration.json
./dependency-tree.metapack
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
