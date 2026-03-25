#!/bin/sh

set -o errexit
set -o nounset

TMP="$(mktemp -d)"
clean() { rm -rf "$TMP"; }
trap clean EXIT

cat << 'EOF' > "$TMP/a.json"
{ "extends": ["./b.json"] }
EOF

cat << 'EOF' > "$TMP/b.json"
{ "extends": ["./a.json"] }
EOF

"$1" --skip-banner "$TMP/a.json" "$TMP/output" 2> "$TMP/output.txt" && CODE="$?" || CODE="$?"
test "$CODE" = "1" || exit 1

cat << EOF > "$TMP/expected.txt"
Writing output to: $(realpath "$TMP")/output
Using configuration: $(realpath "$TMP")/a.json
error: Circular reference detected in configuration
  from path $(realpath "$TMP")/b.json
  at location "/extends/extends"
  to path $(realpath "$TMP")/a.json
EOF

diff "$TMP/output.txt" "$TMP/expected.txt"
