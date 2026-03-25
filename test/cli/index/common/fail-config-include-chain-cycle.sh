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
    "x": { "include": "./b.json" }
  }
}
EOF

cat << 'EOF' > "$TMP/b.json"
{ "include": "./c.json" }
EOF

cat << 'EOF' > "$TMP/c.json"
{ "include": "./b.json" }
EOF

"$1" --skip-banner "$TMP/one.json" "$TMP/output" 2> "$TMP/output.txt" && CODE="$?" || CODE="$?"
test "$CODE" = "1" || exit 1

cat << EOF > "$TMP/expected.txt"
Writing output to: $(realpath "$TMP")/output
Using configuration: $(realpath "$TMP")/one.json
error: Circular reference detected in configuration
  from $(realpath "$TMP")/c.json
  at "/contents/x/include/include/include"
  to $(realpath "$TMP")/b.json
EOF

diff "$TMP/output.txt" "$TMP/expected.txt"
