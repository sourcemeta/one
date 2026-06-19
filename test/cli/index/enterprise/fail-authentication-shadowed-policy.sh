#!/bin/sh

set -o errexit
set -o nounset

TMP="$(mktemp -d)"
clean() { rm -rf "$TMP"; }
trap clean EXIT

mkdir "$TMP/internal"

cat << 'EOF' > "$TMP/internal/secret.json"
{
  "$schema": "https://json-schema.org/draft/2020-12/schema",
  "type": "object"
}
EOF

cat << 'EOF' > "$TMP/one.json"
{
  "url": "http://localhost:8000",
  "authentication": [
    { "type": "public", "paths": [ "/" ] },
    {
      "type": "apiKey",
      "algorithm": "identity",
      "paths": [ "/internal" ],
      "keys": [ { "environmentVariable": "ONE_TEST_KEY_INTERNAL" } ]
    }
  ],
  "contents": {
    "internal": { "path": "./internal" }
  }
}
EOF

"$1" --skip-banner "$TMP/one.json" "$TMP/output" \
  > "$TMP/output.txt" 2>/dev/null && CODE="$?" || CODE="$?"
test "$CODE" = "1" || exit 1

cat << EOF > "$TMP/expected.txt"
error: An apiKey authentication policy is shadowed by a public policy
  at scope /internal
  shadowed by /
  at path $(realpath "$TMP")/one.json
EOF

diff "$TMP/output.txt" "$TMP/expected.txt"
