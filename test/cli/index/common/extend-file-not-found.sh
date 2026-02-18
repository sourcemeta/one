#!/bin/sh

set -o errexit
set -o nounset

TMP="$(mktemp -d)"
clean() { rm -rf "$TMP"; }
trap clean EXIT

cat << EOF > "$TMP/one.json"
{
  "url": "https://sourcemeta.com/",
  "extends": [ "./nonexistent.json" ]
}
EOF

"$1" "$TMP/one.json" "$TMP/output" 2> "$TMP/output.txt" && CODE="$?" || CODE="$?"
test "$CODE" = "1" || exit 1

cat << EOF > "$TMP/expected.txt"
Writing output to: $(realpath "$TMP")/output
Using configuration: $(realpath "$TMP")/one.json
error: Could not read referenced file
  from $(realpath "$TMP")/one.json
  at "/extends"
  to $(realpath "$TMP")/nonexistent.json
EOF

diff "$TMP/output.txt" "$TMP/expected.txt"
