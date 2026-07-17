#!/bin/sh

set -o errexit
set -o nounset

TMP="$(mktemp -d)"
clean() { rm -rf "$TMP"; }
trap clean EXIT

cat << EOF > "$TMP/one.json"
{
  "url": "https://sourcemeta.com/",
  "html": false,
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
# The affected directory listings still rebuild, but a headless instance renders
# no web page for any of them, so no login nor directory page is ever produced
cat << 'EOF' > "$TMP/schemas/c.json"
{
  "$schema": "http://json-schema.org/draft-07/schema#",
  "$id": "https://example.com/c"
}
EOF
"$1" --skip-banner "$TMP/one.json" "$TMP/output" --concurrency 1 2> "$TMP/output.txt"
remove_threads_information "$TMP/output.txt"
grep -E "Producing|Combining" "$TMP/output.txt" > "$TMP/output_producing.txt"

cat << 'EOF' > "$TMP/expected.txt"
(  6%) Producing: schemas/example/schemas/c/%/schema.metapack
( 12%) Producing: schemas/example/schemas/c/%/dependencies.metapack
( 18%) Producing: schemas/example/schemas/c/%/locations.metapack
( 25%) Producing: schemas/example/schemas/c/%/positions.metapack
( 31%) Producing: schemas/example/schemas/c/%/stats.metapack
( 37%) Producing: schemas/example/schemas/c/%/bundle.metapack
( 43%) Producing: schemas/example/schemas/c/%/health.metapack
( 50%) Producing: explorer/example/schemas/c/%/schema.metapack
( 56%) Producing: schemas/example/schemas/c/%/blaze-exhaustive.metapack
( 62%) Producing: schemas/example/schemas/c/%/blaze-fast.metapack
( 68%) Producing: schemas/example/schemas/c/%/editor.metapack
( 75%) Producing: explorer/example/schemas/%/directory.metapack
( 81%) Producing: explorer/example/%/directory.metapack
( 87%) Producing: explorer/%/directory.metapack
( 93%) Producing: explorer/%/search.metapack
(100%) Producing: explorer/%/mcp.metapack
(100%) Combining: schemas/example/schemas/c/%/dependents.metapack
EOF

diff "$TMP/output_producing.txt" "$TMP/expected.txt"
