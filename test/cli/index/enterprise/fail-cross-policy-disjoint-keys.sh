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
      "paths": [ "/alpha" ],
      "keys": [ { "environmentVariable": "ONE_TEST_KEY_ALPHA" } ]
    },
    {
      "type": "apiKey",
      "algorithm": "identity",
      "paths": [ "/beta" ],
      "keys": [ { "environmentVariable": "ONE_TEST_KEY_BETA" } ]
    }
  ],
  "contents": {
    "alpha": { "path": "./alpha" },
    "beta": { "path": "./beta" }
  }
}
EOF

mkdir "$TMP/alpha"
mkdir "$TMP/beta"

cat << 'EOF' > "$TMP/beta/two.json"
{
  "$schema": "https://json-schema.org/draft/2020-12/schema",
  "type": "object"
}
EOF

cat << 'EOF' > "$TMP/alpha/one.json"
{
  "$schema": "https://json-schema.org/draft/2020-12/schema",
  "properties": { "other": { "$ref": "../beta/two" } }
}
EOF

"$1" --skip-banner --concurrency 1 "$TMP/one.json" "$TMP/output" \
  > "$TMP/output.txt" 2>/dev/null && CODE="$?" || CODE="$?"
test "$CODE" = "1" || exit 1

cat << EOF > "$TMP/expected.txt"
error: A schema cannot reference a schema behind a stricter authentication policy
  at schema http://localhost:8000/alpha/one
  with reference http://localhost:8000/beta/two
EOF

diff "$TMP/output.txt" "$TMP/expected.txt"
