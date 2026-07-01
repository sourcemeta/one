#!/bin/sh

set -o errexit
set -o nounset

TMP="$(mktemp -d)"
clean() { rm -rf "$TMP"; }
trap clean EXIT

cat << 'JSON' > "$TMP/one.json"
{
  "url": "https://example.com",
  "authentication": [
    {
      "type": "jwt",
      "name": "internal",
      "paths": [ "/internal" ],
      "issuer": "https://idp.example.com",
      "audience": "https://schemas.example.com",
      "algorithms": [ "RS256" ]
    }
  ]
}
JSON

"$1" --skip-banner "$TMP/one.json" "$TMP/output" > "$TMP/output.txt" && CODE="$?" || CODE="$?"
test "$CODE" = "1" || exit 1

cat << EOF2 > "$TMP/expected.txt"
error: Authentication is only available on the enterprise edition
  at path $(realpath "$TMP")/one.json
EOF2

diff "$TMP/output.txt" "$TMP/expected.txt"
