#!/bin/sh

set -o errexit
set -o nounset

TMP="$(mktemp -d)"
clean() { rm -rf "$TMP"; }
trap clean EXIT

"$1" /tmp "65536" > "$TMP/output.txt" 2>&1 && CODE="$?" || CODE="$?"
test "$CODE" = "1" || exit 1

cat << EOF > "$TMP/expected.txt"
Sourcemeta One $2 v$3
Usage: $(basename "$1") <path/to/output/directory> <port>
error: The port must be a valid TCP port
EOF

diff "$TMP/output.txt" "$TMP/expected.txt"
