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
      "paths": [ "/private" ],
      "keys": [ { "environmentVariable": "ONE_TEST_KEY_PRIVATE" } ]
    },
    { "type": "public", "paths": [ "/private/open" ] }
  ],
  "contents": { "private": { "path": "./private" } }
}
EOF

mkdir "$TMP/private"
mkdir "$TMP/private/open"

cat << 'EOF' > "$TMP/private/secret.json"
{
  "$schema": "https://json-schema.org/draft/2020-12/schema",
  "type": "object"
}
EOF

cat << 'EOF' > "$TMP/private/open/leak.json"
{
  "$schema": "https://json-schema.org/draft/2020-12/schema",
  "properties": { "secret": { "$ref": "../secret" } }
}
EOF

"$1" --skip-banner --concurrency 1 "$TMP/one.json" "$TMP/output" \
  > "$TMP/output.txt" 2>/dev/null && CODE="$?" || CODE="$?"
test "$CODE" = "1" || exit 1

cat << EOF > "$TMP/expected.txt"
error: A schema cannot reference a schema behind a stricter authentication policy
  at schema http://localhost:8000/private/open/leak
  with reference http://localhost:8000/private/secret
EOF

diff "$TMP/output.txt" "$TMP/expected.txt"
