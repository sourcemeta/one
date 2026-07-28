#!/bin/sh

set -o errexit
set -o nounset

TMP="$(mktemp -d)"
clean() { rm -rf "$TMP"; }
trap clean EXIT

mkdir "$TMP/private"

cat << 'EOF' > "$TMP/private/secret.json"
{
  "$schema": "https://json-schema.org/draft/2020-12/schema",
  "type": "object"
}
EOF

cat << 'EOF' > "$TMP/one.json"
{
  "url": "https://example.com",
  "authentication": [
    {
      "type": "jwt",
      "name": "machine",
      "paths": [ "/private" ],
      "issuer": "http://accounts.example.com",
      "audience": "https://example.com",
      "algorithms": [ "RS256" ]
    }
  ],
  "contents": {
    "private": { "path": "./private" }
  }
}
EOF

"$1" --skip-banner "$TMP/one.json" "$TMP/output" \
  > "$TMP/output.txt" && CODE="$?" || CODE="$?"
test "$CODE" = "1" || exit 1

cat << EOF > "$TMP/expected.txt"
error: An authentication policy issuer must be an https URL
  at issuer http://accounts.example.com
  at name machine
  at path $(realpath "$TMP")/one.json
EOF

diff "$TMP/output.txt" "$TMP/expected.txt"
