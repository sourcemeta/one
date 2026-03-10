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
    "left": {
      "contents": {
        "left-a": {
          "baseUri": "https://example.com/left/left-a/",
          "path": "./schemas-left-a"
        },
        "left-b": {
          "baseUri": "https://example.com/left/left-b/",
          "path": "./schemas-left-b"
        }
      }
    },
    "right": {
      "contents": {
        "right-a": {
          "baseUri": "https://example.com/right/right-a/",
          "path": "./schemas-right-a"
        },
        "right-b": {
          "baseUri": "https://example.com/right/right-b/",
          "path": "./schemas-right-b"
        }
      }
    }
  }
}
EOF

mkdir "$TMP/schemas-left-a" "$TMP/schemas-left-b" \
      "$TMP/schemas-right-a" "$TMP/schemas-right-b"

cat << 'EOF' > "$TMP/schemas-left-a/s1.json"
{
  "$schema": "http://json-schema.org/draft-07/schema#",
  "$id": "https://example.com/left/left-a/s1"
}
EOF

cat << 'EOF' > "$TMP/schemas-left-b/s2.json"
{
  "$schema": "http://json-schema.org/draft-07/schema#",
  "$id": "https://example.com/left/left-b/s2"
}
EOF

cat << 'EOF' > "$TMP/schemas-right-a/s3.json"
{
  "$schema": "http://json-schema.org/draft-07/schema#",
  "$id": "https://example.com/right/right-a/s3"
}
EOF

cat << 'EOF' > "$TMP/schemas-right-b/s4.json"
{
  "$schema": "http://json-schema.org/draft-07/schema#",
  "$id": "https://example.com/right/right-b/s4"
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

# Run 1: index four schemas across four directories
"$1" --skip-banner "$TMP/one.json" "$TMP/output" --concurrency 1 > /dev/null 2>&1

# Run 2: add a fifth schema to left/left-a
# Sibling directories (left-b, right, right-a, right-b) should be fully cached
# Only left/left-a, left, and root directory listings should rebuild
cat << 'EOF' > "$TMP/schemas-left-a/s5.json"
{
  "$schema": "http://json-schema.org/draft-07/schema#",
  "$id": "https://example.com/left/left-a/s5"
}
EOF
"$1" --skip-banner "$TMP/one.json" "$TMP/output" --concurrency 1 --verbose 2> "$TMP/output.txt"
remove_threads_information "$TMP/output.txt"

grep '(skip)' "$TMP/output.txt" | LC_ALL=C sort > "$TMP/actual_skips.txt"
cat << 'EOF' | LC_ALL=C sort > "$TMP/expected_skips.txt"
(skip) Ingesting: https://sourcemeta.com/left/left-a/s1 [materialise]
(skip) Ingesting: https://sourcemeta.com/left/left-b/s2 [materialise]
(skip) Ingesting: https://sourcemeta.com/right/right-a/s3 [materialise]
(skip) Ingesting: https://sourcemeta.com/right/right-b/s4 [materialise]
(skip) Analysing: https://sourcemeta.com/left/left-a/s1 [positions]
(skip) Analysing: https://sourcemeta.com/left/left-a/s1 [locations]
(skip) Analysing: https://sourcemeta.com/left/left-a/s1 [dependencies]
(skip) Analysing: https://sourcemeta.com/left/left-a/s1 [stats]
(skip) Analysing: https://sourcemeta.com/left/left-a/s1 [health]
(skip) Analysing: https://sourcemeta.com/left/left-a/s1 [bundle]
(skip) Analysing: https://sourcemeta.com/left/left-a/s1 [editor]
(skip) Analysing: https://sourcemeta.com/left/left-a/s1 [blaze-exhaustive]
(skip) Analysing: https://sourcemeta.com/left/left-a/s1 [blaze-fast]
(skip) Analysing: https://sourcemeta.com/left/left-a/s1 [metadata]
(skip) Analysing: https://sourcemeta.com/left/left-b/s2 [positions]
(skip) Analysing: https://sourcemeta.com/left/left-b/s2 [locations]
(skip) Analysing: https://sourcemeta.com/left/left-b/s2 [dependencies]
(skip) Analysing: https://sourcemeta.com/left/left-b/s2 [stats]
(skip) Analysing: https://sourcemeta.com/left/left-b/s2 [health]
(skip) Analysing: https://sourcemeta.com/left/left-b/s2 [bundle]
(skip) Analysing: https://sourcemeta.com/left/left-b/s2 [editor]
(skip) Analysing: https://sourcemeta.com/left/left-b/s2 [blaze-exhaustive]
(skip) Analysing: https://sourcemeta.com/left/left-b/s2 [blaze-fast]
(skip) Analysing: https://sourcemeta.com/left/left-b/s2 [metadata]
(skip) Analysing: https://sourcemeta.com/right/right-a/s3 [positions]
(skip) Analysing: https://sourcemeta.com/right/right-a/s3 [locations]
(skip) Analysing: https://sourcemeta.com/right/right-a/s3 [dependencies]
(skip) Analysing: https://sourcemeta.com/right/right-a/s3 [stats]
(skip) Analysing: https://sourcemeta.com/right/right-a/s3 [health]
(skip) Analysing: https://sourcemeta.com/right/right-a/s3 [bundle]
(skip) Analysing: https://sourcemeta.com/right/right-a/s3 [editor]
(skip) Analysing: https://sourcemeta.com/right/right-a/s3 [blaze-exhaustive]
(skip) Analysing: https://sourcemeta.com/right/right-a/s3 [blaze-fast]
(skip) Analysing: https://sourcemeta.com/right/right-a/s3 [metadata]
(skip) Analysing: https://sourcemeta.com/right/right-b/s4 [positions]
(skip) Analysing: https://sourcemeta.com/right/right-b/s4 [locations]
(skip) Analysing: https://sourcemeta.com/right/right-b/s4 [dependencies]
(skip) Analysing: https://sourcemeta.com/right/right-b/s4 [stats]
(skip) Analysing: https://sourcemeta.com/right/right-b/s4 [health]
(skip) Analysing: https://sourcemeta.com/right/right-b/s4 [bundle]
(skip) Analysing: https://sourcemeta.com/right/right-b/s4 [editor]
(skip) Analysing: https://sourcemeta.com/right/right-b/s4 [blaze-exhaustive]
(skip) Analysing: https://sourcemeta.com/right/right-b/s4 [blaze-fast]
(skip) Analysing: https://sourcemeta.com/right/right-b/s4 [metadata]
(skip) Producing: left/left-b [directory]
(skip) Producing: right [directory]
(skip) Producing: right/right-a [directory]
(skip) Producing: right/right-b [directory]
(skip) Producing: routes.bin [routes]
(skip) Rendering: . [not-found]
(skip) Rendering: left/left-a/s1 [schema]
(skip) Rendering: left/left-b [directory]
(skip) Rendering: left/left-b/s2 [schema]
(skip) Rendering: right [directory]
(skip) Rendering: right/right-a [directory]
(skip) Rendering: right/right-a/s3 [schema]
(skip) Rendering: right/right-b [directory]
(skip) Rendering: right/right-b/s4 [schema]
EOF

diff "$TMP/actual_skips.txt" "$TMP/expected_skips.txt"
