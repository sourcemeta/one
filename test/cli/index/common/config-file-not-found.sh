#!/bin/sh

set -o errexit
set -o nounset

TMP="$(mktemp -d)"
clean() { rm -rf "$TMP"; }
trap clean EXIT

"$1" "$(realpath "$TMP")/nonexistent.json" "$TMP/output" 2> "$TMP/output.txt" \
  && CODE="$?" || CODE="$?"
test "$CODE" = "1" || exit 1

cat << EOF > "$TMP/expected.txt"
Writing output to: $(realpath "$TMP")/output
error: could not locate the requested file
  at $(realpath "$TMP")/nonexistent.json
EOF

diff "$TMP/output.txt" "$TMP/expected.txt"
