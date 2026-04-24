#!/bin/sh

set -o errexit
set -o nounset

TMP="$(mktemp -d)"
clean() { rm -rf "$TMP"; }
trap clean EXIT

cat << EOF > "$TMP/one.json"
{ "extends": [ "@self/v1" ], "url": "https://sourcemeta.com/" }
EOF

"$1" --skip-banner "$TMP/one.json" "$TMP/output" --concurrency abc 2> "$TMP/output.txt" && CODE="$?" || CODE="$?"
test "$CODE" = "1" || exit 1

cat << EOF > "$TMP/expected.txt"
Writing output to: $(realpath "$TMP")/output
Using configuration: $(realpath "$TMP")/one.json
error: Expected a valid numeric value for option
  at option concurrency
  with value abc
EOF

diff "$TMP/output.txt" "$TMP/expected.txt"
