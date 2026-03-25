#!/bin/sh

set -o errexit
set -o nounset

TMP="$(mktemp -d)"
clean() { rm -rf "$TMP"; }
trap clean EXIT

cat << EOF > "$TMP/one.json"
{ "url": "https://sourcemeta.com/" }
EOF

"$1" --skip-banner "$TMP/one.json" "$TMP/output" --concurrency abc 2> "$TMP/output.txt" && CODE="$?" || CODE="$?"
test "$CODE" = "1" || exit 1

cat << EOF > "$TMP/expected_1.txt"
Writing output to: $(realpath "$TMP")/output
Using configuration: $(realpath "$TMP")/one.json
unexpected error: stoull: no conversion
EOF

cat << EOF > "$TMP/expected_2.txt"
Writing output to: $(realpath "$TMP")/output
Using configuration: $(realpath "$TMP")/one.json
unexpected error: stoul
EOF

cat << EOF > "$TMP/expected_3.txt"
Writing output to: $(realpath "$TMP")/output
Using configuration: $(realpath "$TMP")/one.json
unexpected error: stoull
EOF

diff "$TMP/output.txt" "$TMP/expected_1.txt" || \
  diff "$TMP/output.txt" "$TMP/expected_2.txt" || \
  diff "$TMP/output.txt" "$TMP/expected_3.txt"
