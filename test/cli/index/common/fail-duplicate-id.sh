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
  "$id": "https://example.com/test"
}
EOF

cat << 'EOF' > "$TMP/schemas/b.json"
{
  "$schema": "http://json-schema.org/draft-07/schema#",
  "$id": "https://example.com/test"
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

"$1" --skip-banner "$TMP/one.json" "$TMP/output" --concurrency 1 2> "$TMP/output.txt" && CODE="$?" || CODE="$?"
test "$CODE" = "1" || exit 1
remove_threads_information "$TMP/output.txt"

cat << EOF > "$TMP/expected_ab.txt"
Writing output to: $(realpath "$TMP")/output
Using configuration: $(realpath "$TMP")/one.json
Detecting: $(realpath "$TMP")/schemas/a.json (#1)
Detecting: $(realpath "$TMP")/schemas/b.json (#2)
( 50%) Resolving: a.json
(100%) Resolving: b.json
error: Cannot register the same identifier twice
  at path $(realpath "$TMP")/schemas/b.json
  at identifier https://sourcemeta.com/example/schemas/test
EOF

cat << EOF > "$TMP/expected_ba.txt"
Writing output to: $(realpath "$TMP")/output
Using configuration: $(realpath "$TMP")/one.json
Detecting: $(realpath "$TMP")/schemas/b.json (#1)
Detecting: $(realpath "$TMP")/schemas/a.json (#2)
( 50%) Resolving: b.json
(100%) Resolving: a.json
error: Cannot register the same identifier twice
  at path $(realpath "$TMP")/schemas/a.json
  at identifier https://sourcemeta.com/example/schemas/test
EOF

diff "$TMP/output.txt" "$TMP/expected_ab.txt" || \
  diff "$TMP/output.txt" "$TMP/expected_ba.txt"
