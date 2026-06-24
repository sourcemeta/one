#!/bin/sh

set -o errexit
set -o nounset

TMP="$(mktemp -d)"
clean() { rm -rf "$TMP"; }
trap clean EXIT

cat << EOF > "$TMP/one.json"
{
  "url": "http://localhost:8000",
  "authentication": [
    {
      "type": "apiKey",
      "algorithm": "identity",
      "name": "private",
      "paths": [ "/private" ],
      "keys": [ { "environmentVariable": "ONE_TEST_CROSS_POLICY_KEY" } ]
    }
  ],
  "contents": {
    "public": { "path": "./public" },
    "private": { "path": "./private" }
  }
}
EOF

mkdir "$TMP/public"
mkdir "$TMP/private"

cat << 'EOF' > "$TMP/private/secret.json"
{
  "$schema": "https://json-schema.org/draft/2020-12/schema",
  "type": "object"
}
EOF

cat << 'EOF' > "$TMP/public/leak.json"
{
  "$schema": "https://json-schema.org/draft/2020-12/schema",
  "properties": { "secret": { "$ref": "../private/secret" } }
}
EOF

"$1" --skip-banner --concurrency 1 "$TMP/one.json" "$TMP/output" \
  > "$TMP/output.txt" 2>/dev/null && CODE="$?" || CODE="$?"
test "$CODE" = "1" || exit 1

cat << EOF > "$TMP/expected.txt"
error: A schema cannot reference a schema behind a stricter authentication policy
  at schema http://localhost:8000/public/leak
  with reference http://localhost:8000/private/secret
  at path $(realpath "$TMP")/one.json
EOF

diff "$TMP/output.txt" "$TMP/expected.txt"
