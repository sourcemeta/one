#!/bin/sh

# Two policies fronting the same provider name the same client secret and the
# same session secret. Neither variable is ever presented by a caller, so
# sharing them aliases nothing, and each policy still gets a tree of its own

set -o errexit
set -o nounset

TMP="$(mktemp -d)"
clean() { rm -rf "$TMP"; }
trap clean EXIT

mkdir "$TMP/alpha" "$TMP/beta"

cat << 'EOF' > "$TMP/alpha/a.json"
{
  "$schema": "https://json-schema.org/draft/2020-12/schema",
  "$id": "https://example.com/alpha/a",
  "type": "object"
}
EOF

cat << 'EOF' > "$TMP/beta/b.json"
{
  "$schema": "https://json-schema.org/draft/2020-12/schema",
  "$id": "https://example.com/beta/b",
  "type": "object"
}
EOF

cat << 'EOF' > "$TMP/one.json"
{
  "url": "https://example.com",
  "authentication": [
    {
      "type": "oidc",
      "name": "engineering",
      "paths": [ "/alpha" ],
      "issuer": "https://login.example.com",
      "clientId": "registry",
      "clientSecret": { "environmentVariable": "ONE_TEST_OIDC_CLIENT" },
      "sessionSecrets": [
        { "environmentVariable": "ONE_TEST_OIDC_SESSION" }
      ]
    },
    {
      "type": "oidc",
      "name": "support",
      "paths": [ "/beta" ],
      "issuer": "https://login.example.com",
      "clientId": "registry",
      "clientSecret": { "environmentVariable": "ONE_TEST_OIDC_CLIENT" },
      "sessionSecrets": [
        { "environmentVariable": "ONE_TEST_OIDC_SESSION" }
      ]
    }
  ],
  "contents": {
    "alpha": {
      "baseUri": "https://example.com/alpha",
      "path": "./alpha"
    },
    "beta": {
      "baseUri": "https://example.com/beta",
      "path": "./beta"
    }
  }
}
EOF

"$1" --skip-banner --deterministic --concurrency 1 "$TMP/one.json" "$TMP/output" \
  > /dev/null 2>&1

cd "$TMP/output"
find ./explorer -mindepth 1 \
  \( -path './explorer/public/self' -o -path './explorer/engineering/self' \
     -o -path './explorer/support/self' -o -name '%' \) -prune \
  -o -type d -print \
  | LC_ALL=C sort > "$TMP/views.txt"
cd - > /dev/null

# Each policy sees its own collection and nothing of the other, and anonymous
# callers see neither
cat << 'EOF' > "$TMP/expected.txt"
./explorer/engineering
./explorer/engineering/alpha
./explorer/engineering/alpha/a
./explorer/public
./explorer/support
./explorer/support/beta
./explorer/support/beta/b
EOF

diff "$TMP/views.txt" "$TMP/expected.txt"
