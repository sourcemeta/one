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
    { "type": "public", "paths": [ "/public" ] },
    {
      "type": "apiKey",
      "algorithm": "identity",
      "name": "pub",
      "paths": [ "/pub" ],
      "keys": [ { "environmentVariable": "ONE_TEST_KEY_PUB" } ]
    }
  ],
  "contents": {
    "public": { "path": "./public" },
    "pub": { "path": "./pub" }
  }
}
EOF

mkdir "$TMP/public"
mkdir "$TMP/pub"

cat << 'EOF' > "$TMP/pub/b.json"
{
  "$schema": "https://json-schema.org/draft/2020-12/schema",
  "type": "object"
}
EOF

cat << 'EOF' > "$TMP/public/a.json"
{
  "$schema": "https://json-schema.org/draft/2020-12/schema",
  "properties": { "other": { "$ref": "../pub/b" } }
}
EOF

"$1" --skip-banner --concurrency 1 "$TMP/one.json" "$TMP/output" \
  > "$TMP/output.txt" 2>/dev/null && CODE="$?" || CODE="$?"
test "$CODE" = "1" || exit 1

cat << EOF > "$TMP/expected.txt"
error: A schema cannot reference a schema behind a stricter authentication policy
  at schema http://localhost:8000/public/a
  with reference http://localhost:8000/pub/b
  at path $(realpath "$TMP")/one.json
EOF

diff "$TMP/output.txt" "$TMP/expected.txt"
