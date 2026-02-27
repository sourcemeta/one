#!/bin/sh

set -o errexit
set -o nounset

TMP="$(mktemp -d)"
clean() { rm -rf "$TMP"; }
trap clean EXIT

"$1" --skip-banner 2> "$TMP/output.txt" && CODE="$?" || CODE="$?"
test "$CODE" = "1" || exit 1

cat << EOF > "$TMP/expected.txt"
Usage: sourcemeta-one-index <one.json> <path/to/output/directory>
EOF

diff "$TMP/output.txt" "$TMP/expected.txt"
