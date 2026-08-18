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

# The same aliasing as the other way around, which the order of the policies
# must not decide
cat << 'EOF' > "$TMP/one.json"
{
  "url": "https://localhost:8000",
  "authentication": [
    {
      "type": "oidc",
      "name": "alpha",
      "paths": [ "/alpha" ],
      "issuer": "https://idp.example.com",
      "clientId": "one",
      "clientSecret": { "environmentVariable": "ONE_TEST_CLIENT_SECRET" },
      "sessionSecrets": [ { "environmentVariable": "ONE_TEST_ALIASED" } ]
    },
    {
      "type": "apiKey",
      "algorithm": "identity",
      "name": "beta",
      "paths": [ "/beta" ],
      "keys": [ { "environmentVariable": "ONE_TEST_ALIASED" } ]
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
error: An authentication policy key is used more than once
  at variable ONE_TEST_ALIASED
  at name beta
  at path $(realpath "$TMP")/one.json
EOF

diff "$TMP/output.txt" "$TMP/expected.txt"
