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
      "type": "oidc",
      "name": "corporate",
      "paths": [ "/private" ],
      "issuer": "https://accounts.example.com/",
      "clientId": "registry",
      "clientSecret": { "environmentVariable": "ONE_TEST_OIDC_SECRET" },
      "sessionSecret": { "environmentVariable": "ONE_TEST_OIDC_SESSION" }
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
error: An authentication policy issuer must be an https URL without a trailing slash
  at issuer https://accounts.example.com/
  at name corporate
  at path $(realpath "$TMP")/one.json
EOF

diff "$TMP/output.txt" "$TMP/expected.txt"
