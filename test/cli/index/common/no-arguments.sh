#!/bin/sh

set -o errexit
set -o nounset

TMP="$(mktemp -d)"
clean() { rm -rf "$TMP"; }
trap clean EXIT

"$1" > "$TMP/output.txt" 2>/dev/null && CODE="$?" || CODE="$?"
test "$CODE" = "1" || exit 1

# Skip the version line as the edition varies between Community and Enterprise
tail -n +2 "$TMP/output.txt" > "$TMP/output_trimmed.txt"

cat << EOF > "$TMP/expected.txt"
Usage: $(basename "$1") <one.json> <path/to/output/directory>
EOF

diff "$TMP/output_trimmed.txt" "$TMP/expected.txt"
