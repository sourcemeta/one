#!/bin/sh

set -o errexit
set -o nounset

TMP="$(mktemp -d)"
clean() { rm -rf "$TMP"; }
trap clean EXIT

"$1" --verbose=true 2> "$TMP/output.txt" && CODE="$?" || CODE="$?"
test "$CODE" = "1" || exit 1

cat << 'EOF' > "$TMP/expected.txt"
error: This flag cannot take a value 'verbose'
EOF

diff "$TMP/output.txt" "$TMP/expected.txt"
