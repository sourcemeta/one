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

cat << 'EOF' > "$TMP/expected.txt"
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

grep "Producing" "$TMP/output.txt" > "$TMP/output_producing.txt"
diff "$TMP/output_producing.txt" "$TMP/expected.txt"
