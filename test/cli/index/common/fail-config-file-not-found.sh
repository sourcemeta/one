#!/bin/sh

set -o errexit
set -o nounset

TMP="$(mktemp -d)"
clean() { rm -rf "$TMP"; }
trap clean EXIT

"$1" --skip-banner "$(realpath "$TMP")/nonexistent.json" "$TMP/output" > "$TMP/output.txt" \
  && CODE="$?" || CODE="$?"
test "$CODE" = "1" || exit 1

cat << EOF > "$TMP/expected.txt"
error: Could not locate the requested file
  at path $(realpath "$TMP")/nonexistent.json
EOF

diff "$TMP/output.txt" "$TMP/expected.txt"
