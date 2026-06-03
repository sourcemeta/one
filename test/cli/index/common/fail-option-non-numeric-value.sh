#!/bin/sh

set -o errexit
set -o nounset

TMP="$(mktemp -d)"
clean() { rm -rf "$TMP"; }
trap clean EXIT

cat << EOF > "$TMP/one.json"
{ "url": "https://sourcemeta.com/" }
EOF

"$1" --skip-banner "$TMP/one.json" "$TMP/output" --concurrency abc > "$TMP/output.txt" && CODE="$?" || CODE="$?"
test "$CODE" = "1" || exit 1

cat << EOF > "$TMP/expected.txt"
error: Expected a valid numeric value for option
  at option concurrency
  with value abc
EOF

diff "$TMP/output.txt" "$TMP/expected.txt"
