#!/bin/sh

# A route is named where it begins, not at something it matches. A scope
# reaching into an expansion asks for a gate that could only hold where the
# path is compared literally, and not on the surfaces that reach the same
# endpoint by name

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
  "url": "http://localhost:8000",
  "authentication": [
    {
      "type": "apiKey",
      "algorithm": "identity",
      "name": "trace",
      "paths": [ "/self/v1/api/schemas/trace/private/secret" ],
      "keys": [ { "environmentVariable": "ONE_TEST_KEY_INTERNAL" } ]
    }
  ],
  "contents": {
    "private": { "path": "./private" }
  }
}
EOF

"$1" --skip-banner "$TMP/one.json" "$TMP/output" \
  > "$TMP/output.txt" 2>/dev/null && CODE="$?" || CODE="$?"
test "$CODE" = "1" || exit 1

cat << EOF > "$TMP/expected.txt"
error: An authentication policy matches no known collection, page, or route
  at scope /self/v1/api/schemas/trace/private/secret
  at path $(realpath "$TMP")/one.json
EOF

diff "$TMP/output.txt" "$TMP/expected.txt"
