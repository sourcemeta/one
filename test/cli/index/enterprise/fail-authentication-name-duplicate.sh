#!/bin/sh

set -o errexit
set -o nounset

TMP="$(mktemp -d)"
clean() { rm -rf "$TMP"; }
trap clean EXIT

mkdir "$TMP/alpha" "$TMP/beta"

cat << 'EOF' > "$TMP/alpha/schema.json"
{
  "$schema": "https://json-schema.org/draft/2020-12/schema",
  "type": "object"
}
EOF

cat << 'EOF' > "$TMP/beta/schema.json"
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
      "name": "data-team",
      "paths": [ "/alpha" ],
      "keys": [ { "environmentVariable": "ONE_TEST_KEY_ALPHA" } ]
    },
    {
      "type": "apiKey",
      "algorithm": "identity",
      "name": "data-team",
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

"$1" --skip-banner "$TMP/one.json" "$TMP/output" \
  > "$TMP/output.txt" && CODE="$?" || CODE="$?"
test "$CODE" = "1" || exit 1

cat << EOF > "$TMP/expected.txt"
error: An authentication policy name is used more than once
  at name data-team
  at path $(realpath "$TMP")/one.json
EOF

diff "$TMP/output.txt" "$TMP/expected.txt"
