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

# Run 2: add a third schema that references schema a
# Only the new schema's artifacts and affected directories should rebuild
cat << 'EOF' > "$TMP/schemas/c.json"
{
  "$schema": "http://json-schema.org/draft-07/schema#",
  "$id": "https://example.com/c",
  "properties": {
    "foo": { "$ref": "https://example.com/a" }
  }
}
EOF
"$1" --skip-banner "$TMP/one.json" "$TMP/output" --concurrency 1 2> "$TMP/output.txt"
remove_threads_information "$TMP/output.txt"
grep "Producing" "$TMP/output.txt" > "$TMP/output_producing.txt"

cat << 'EOF' > "$TMP/expected.txt"
(  4%) Producing: schemas/example/schemas/a/%/dependents.metapack
(  9%) Producing: schemas/example/schemas/b/%/dependents.metapack
( 13%) Producing: schemas/example/schemas/c/%/schema.metapack
( 18%) Producing: schemas/example/schemas/c/%/dependencies.metapack
( 22%) Producing: schemas/example/schemas/c/%/locations.metapack
( 27%) Producing: schemas/example/schemas/c/%/positions.metapack
( 31%) Producing: schemas/example/schemas/c/%/stats.metapack
( 36%) Producing: schemas/example/schemas/c/%/bundle.metapack
( 40%) Producing: schemas/example/schemas/c/%/dependents.metapack
( 45%) Producing: schemas/example/schemas/c/%/health.metapack
( 50%) Producing: explorer/example/schemas/c/%/schema.metapack
( 54%) Producing: schemas/example/schemas/c/%/blaze-exhaustive.metapack
( 59%) Producing: schemas/example/schemas/c/%/blaze-fast.metapack
( 63%) Producing: schemas/example/schemas/c/%/editor.metapack
( 68%) Producing: explorer/%/search.metapack
( 72%) Producing: explorer/example/schemas/%/directory.metapack
( 77%) Producing: explorer/example/schemas/c/%/schema-html.metapack
( 81%) Producing: explorer/example/%/directory.metapack
( 86%) Producing: explorer/example/schemas/%/directory-html.metapack
( 90%) Producing: explorer/%/directory.metapack
( 95%) Producing: explorer/example/%/directory-html.metapack
(100%) Producing: explorer/%/directory-html.metapack
EOF

diff "$TMP/output_producing.txt" "$TMP/expected.txt"
