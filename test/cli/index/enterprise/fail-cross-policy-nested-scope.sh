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
      "name": "broad",
      "paths": [ "/p" ],
      "keys": [ { "environmentVariable": "ONE_TEST_KEY_BROAD" } ]
    },
    {
      "type": "apiKey",
      "algorithm": "identity",
      "name": "inner",
      "paths": [ "/p/inner" ],
      "keys": [ { "environmentVariable": "ONE_TEST_KEY_INNER" } ]
    }
  ],
  "contents": { "p": { "path": "./p" } }
}
EOF

mkdir "$TMP/p"
mkdir "$TMP/p/inner"

cat << 'EOF' > "$TMP/p/shallow.json"
{
  "$schema": "https://json-schema.org/draft/2020-12/schema",
  "type": "object"
}
EOF

cat << 'EOF' > "$TMP/p/inner/deep.json"
{
  "$schema": "https://json-schema.org/draft/2020-12/schema",
  "properties": { "other": { "$ref": "../shallow" } }
}
EOF

"$1" --skip-banner --concurrency 1 "$TMP/one.json" "$TMP/output" \
  > "$TMP/output.txt" 2>/dev/null && CODE="$?" || CODE="$?"
test "$CODE" = "1" || exit 1

cat << EOF > "$TMP/expected.txt"
error: A schema cannot reference a schema behind a stricter authentication policy
  at schema http://localhost:8000/p/inner/deep
  with reference http://localhost:8000/p/shallow
  at path $(realpath "$TMP")/one.json
EOF

diff "$TMP/output.txt" "$TMP/expected.txt"
