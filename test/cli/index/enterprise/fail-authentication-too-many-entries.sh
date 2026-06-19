#!/bin/sh

set -o errexit
set -o nounset

TMP="$(mktemp -d)"
clean() { rm -rf "$TMP"; }
trap clean EXIT

{
  printf '{\n  "url": "http://localhost:8000",\n  "authentication": [\n'
  index=0
  while [ "$index" -lt 65 ]; do
    if [ "$index" -gt 0 ]; then printf ',\n'; fi
    printf '    { "type": "public", "paths": [ "/p%d" ] }' "$index"
    index=$((index + 1))
  done
  printf '\n  ],\n  "contents": {}\n}\n'
} > "$TMP/one.json"

"$1" --skip-banner "$TMP/one.json" "$TMP/output" \
  > "$TMP/output.txt" && CODE="$?" || CODE="$?"
test "$CODE" = "1" || exit 1

cat << EOF > "$TMP/expected.txt"
error: Invalid configuration
  at path $(realpath "$TMP")/one.json
The array value was expected to contain at most 64 items but it contained 65 items
  at instance location "/authentication"
  at evaluate path "/properties/authentication/maxItems"
The object value was expected to validate against the 6 defined properties subschemas
  at instance location ""
  at evaluate path "/properties"
EOF

diff "$TMP/output.txt" "$TMP/expected.txt"
