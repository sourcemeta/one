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
    "x": { "include": "./other.json" }
  }
}
EOF

cat << 'EOF' > "$TMP/other.json"
{ "include": "./one.json" }
EOF

"$1" --skip-banner "$TMP/one.json" "$TMP/output" 2> "$TMP/output.txt" && CODE="$?" || CODE="$?"
test "$CODE" = "1" || exit 1

cat << EOF > "$TMP/expected.txt"
Writing output to: $(realpath "$TMP")/output
Using configuration: $(realpath "$TMP")/one.json
error: Circular reference detected in configuration
  from path $(realpath "$TMP")/other.json
  at location "/contents/x/include/include"
  to path $(realpath "$TMP")/one.json
EOF

diff "$TMP/output.txt" "$TMP/expected.txt"
