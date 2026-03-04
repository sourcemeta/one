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

remove_threads_information() {
  expr='s/ \[[^]]*[^a-z-][^]]*\]//g'
  if [ "$(uname -s)" = "Darwin" ]; then
    sed -i '' "$expr" "$1"
  else
    sed -i "$expr" "$1"
  fi
}

# Run 1: index one schema from scratch
"$1" --skip-banner "$TMP/one.json" "$TMP/output" --concurrency 1 > /dev/null 2>&1

# Run 2: modify the schema and re-index
cat << 'EOF' > "$TMP/schemas/a.json"
{
  "$schema": "http://json-schema.org/draft-07/schema#",
  "$id": "https://example.com/a",
  "type": "string"
}
EOF
"$1" --skip-banner "$TMP/one.json" "$TMP/output" --concurrency 1 > /dev/null 2>&1

# Run 3: re-index with no changes. Everything should be fully cached.
"$1" --skip-banner "$TMP/one.json" "$TMP/output" --concurrency 1 2> "$TMP/output.txt"
remove_threads_information "$TMP/output.txt"

cat << EOF > "$TMP/expected.txt"
Writing output to: $(realpath "$TMP")/output
Using configuration: $(realpath "$TMP")/one.json
Detecting: $(realpath "$TMP")/schemas/a.json (#1)
(100%) Resolving: a.json
(100%) Ingesting: https://sourcemeta.com/example/schemas/a
(skip) Ingesting: https://sourcemeta.com/example/schemas/a [materialise]
(100%) Analysing: https://sourcemeta.com/example/schemas/a
(skip) Analysing: https://sourcemeta.com/example/schemas/a [positions]
(skip) Analysing: https://sourcemeta.com/example/schemas/a [locations]
(skip) Analysing: https://sourcemeta.com/example/schemas/a [dependencies]
(skip) Analysing: https://sourcemeta.com/example/schemas/a [stats]
(skip) Analysing: https://sourcemeta.com/example/schemas/a [health]
(skip) Analysing: https://sourcemeta.com/example/schemas/a [bundle]
(skip) Analysing: https://sourcemeta.com/example/schemas/a [editor]
(skip) Analysing: https://sourcemeta.com/example/schemas/a [blaze-exhaustive]
(skip) Analysing: https://sourcemeta.com/example/schemas/a [blaze-fast]
(skip) Analysing: https://sourcemeta.com/example/schemas/a [metadata]
( 50%) Reviewing: schemas
(100%) Reviewing: schemas
(skip) Reviewing: schemas [dependencies]
(100%) Reworking: https://sourcemeta.com/example/schemas/a
(skip) Reworking: https://sourcemeta.com/example/schemas/a [dependents]
(  0%) Producing: explorer
(skip) Producing: explorer [search]
( 33%) Producing: example/schemas
(skip) Producing: example/schemas [directory]
( 66%) Producing: example
(skip) Producing: example [directory]
(100%) Producing: .
(skip) Producing: . [directory]
( 25%) Rendering: example/schemas
(skip) Rendering: example/schemas [directory]
( 50%) Rendering: example
(skip) Rendering: example [directory]
( 75%) Rendering: .
(skip) Rendering: . [index]
(skip) Rendering: . [not-found]
(100%) Rendering: example/schemas/a
(skip) Rendering: example/schemas/a [schema]
(skip) Producing: routes.bin [routes]
EOF

diff "$TMP/output.txt" "$TMP/expected.txt"
