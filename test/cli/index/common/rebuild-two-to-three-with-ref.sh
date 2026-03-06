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

grep '(skip)' "$TMP/output.txt" | LC_ALL=C sort > "$TMP/actual_skips.txt"
cat << 'EOF' | LC_ALL=C sort > "$TMP/expected_skips.txt"
(skip) Ingesting: https://sourcemeta.com/example/schemas/a [materialise]
(skip) Ingesting: https://sourcemeta.com/example/schemas/b [materialise]
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
(skip) Analysing: https://sourcemeta.com/example/schemas/b [positions]
(skip) Analysing: https://sourcemeta.com/example/schemas/b [locations]
(skip) Analysing: https://sourcemeta.com/example/schemas/b [dependencies]
(skip) Analysing: https://sourcemeta.com/example/schemas/b [stats]
(skip) Analysing: https://sourcemeta.com/example/schemas/b [health]
(skip) Analysing: https://sourcemeta.com/example/schemas/b [bundle]
(skip) Analysing: https://sourcemeta.com/example/schemas/b [editor]
(skip) Analysing: https://sourcemeta.com/example/schemas/b [blaze-exhaustive]
(skip) Analysing: https://sourcemeta.com/example/schemas/b [blaze-fast]
(skip) Analysing: https://sourcemeta.com/example/schemas/b [metadata]
(skip) Reworking: https://sourcemeta.com/example/schemas/b [dependents]
(skip) Rendering: . [not-found]
(skip) Rendering: example/schemas/b [schema]
(skip) Producing: routes.bin [routes]
EOF

diff "$TMP/actual_skips.txt" "$TMP/expected_skips.txt"
