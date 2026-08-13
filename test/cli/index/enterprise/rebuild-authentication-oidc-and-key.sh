#!/bin/sh

# A browser policy and a machine policy govern the same registry. Neither can be
# satisfied alongside the other, since one is held as a session and the other
# presented as a key, so the registry is looked at three ways rather than four:
# anonymously and under each of them, with no tree for a pair

set -o errexit
set -o nounset

TMP="$(mktemp -d)"
clean() { rm -rf "$TMP"; }
trap clean EXIT

mkdir "$TMP/schemas"

cat << 'EOF' > "$TMP/schemas/a.json"
{
  "$schema": "https://json-schema.org/draft/2020-12/schema",
  "$id": "https://example.com/a",
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
      "paths": [ "/schemas" ],
      "issuer": "https://login.example.com",
      "clientId": "registry",
      "clientSecret": { "environmentVariable": "ONE_TEST_OIDC_CLIENT" },
      "sessionSecrets": [
        { "environmentVariable": "ONE_TEST_OIDC_SESSION" }
      ]
    },
    {
      "type": "apiKey",
      "algorithm": "identity",
      "name": "vault",
      "paths": [ "/schemas" ],
      "keys": [ { "environmentVariable": "ONE_TEST_KEY_VAULT" } ]
    }
  ],
  "contents": {
    "schemas": {
      "baseUri": "https://example.com/",
      "path": "./schemas"
    }
  }
}
EOF

"$1" --skip-banner --deterministic --concurrency 1 "$TMP/one.json" "$TMP/output" \
  > /dev/null 2>&1

cd "$TMP/output"
find ./explorer -mindepth 1 \
  \( -path './explorer/public/self' -o -path './explorer/corporate/self' \
     -o -path './explorer/vault/self' \) -prune \
  -o -print \
  | LC_ALL=C sort > "$TMP/before.txt"
cd - > /dev/null

cat << 'EOF' > "$TMP/expected-before.txt"
./explorer/corporate
./explorer/corporate/%
./explorer/corporate/%/401.metapack
./explorer/corporate/%/404.metapack
./explorer/corporate/%/directory-html.metapack
./explorer/corporate/%/directory.metapack
./explorer/corporate/%/mcp.metapack
./explorer/corporate/%/search.metapack
./explorer/corporate/schemas
./explorer/corporate/schemas/%
./explorer/corporate/schemas/%/directory-html.metapack
./explorer/corporate/schemas/%/directory.metapack
./explorer/corporate/schemas/a
./explorer/corporate/schemas/a/%
./explorer/corporate/schemas/a/%/dependents.metapack
./explorer/corporate/schemas/a/%/schema-html.metapack
./explorer/corporate/schemas/a/%/schema.metapack
./explorer/public
./explorer/public/%
./explorer/public/%/401.metapack
./explorer/public/%/404.metapack
./explorer/public/%/directory-html.metapack
./explorer/public/%/directory.metapack
./explorer/public/%/login-html.metapack
./explorer/public/%/mcp.metapack
./explorer/public/%/search.metapack
./explorer/vault
./explorer/vault/%
./explorer/vault/%/401.metapack
./explorer/vault/%/404.metapack
./explorer/vault/%/directory-html.metapack
./explorer/vault/%/directory.metapack
./explorer/vault/%/mcp.metapack
./explorer/vault/%/search.metapack
./explorer/vault/schemas
./explorer/vault/schemas/%
./explorer/vault/schemas/%/directory-html.metapack
./explorer/vault/schemas/%/directory.metapack
./explorer/vault/schemas/a
./explorer/vault/schemas/a/%
./explorer/vault/schemas/a/%/dependents.metapack
./explorer/vault/schemas/a/%/schema-html.metapack
./explorer/vault/schemas/a/%/schema.metapack
EOF

diff "$TMP/before.txt" "$TMP/expected-before.txt"

# Retiring the browser policy takes its tree with it and leaves the machine one
# untouched, since neither ever contributed to the other
cat << 'EOF' > "$TMP/one.json"
{
  "url": "https://example.com",
  "authentication": [
    {
      "type": "apiKey",
      "algorithm": "identity",
      "name": "vault",
      "paths": [ "/schemas" ],
      "keys": [ { "environmentVariable": "ONE_TEST_KEY_VAULT" } ]
    }
  ],
  "contents": {
    "schemas": {
      "baseUri": "https://example.com/",
      "path": "./schemas"
    }
  }
}
EOF

"$1" --skip-banner --deterministic --concurrency 1 "$TMP/one.json" "$TMP/output" \
  > /dev/null 2>&1

cd "$TMP/output"
find ./explorer -mindepth 1 \
  \( -path './explorer/public/self' -o -path './explorer/vault/self' \) -prune \
  -o -print \
  | LC_ALL=C sort > "$TMP/after.txt"
cd - > /dev/null

cat << 'EOF' > "$TMP/expected-after.txt"
./explorer/public
./explorer/public/%
./explorer/public/%/401.metapack
./explorer/public/%/404.metapack
./explorer/public/%/directory-html.metapack
./explorer/public/%/directory.metapack
./explorer/public/%/login-html.metapack
./explorer/public/%/mcp.metapack
./explorer/public/%/search.metapack
./explorer/vault
./explorer/vault/%
./explorer/vault/%/401.metapack
./explorer/vault/%/404.metapack
./explorer/vault/%/directory-html.metapack
./explorer/vault/%/directory.metapack
./explorer/vault/%/mcp.metapack
./explorer/vault/%/search.metapack
./explorer/vault/schemas
./explorer/vault/schemas/%
./explorer/vault/schemas/%/directory-html.metapack
./explorer/vault/schemas/%/directory.metapack
./explorer/vault/schemas/a
./explorer/vault/schemas/a/%
./explorer/vault/schemas/a/%/dependents.metapack
./explorer/vault/schemas/a/%/schema-html.metapack
./explorer/vault/schemas/a/%/schema.metapack
EOF

diff "$TMP/after.txt" "$TMP/expected-after.txt"
