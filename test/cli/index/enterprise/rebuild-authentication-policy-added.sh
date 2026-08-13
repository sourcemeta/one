#!/bin/sh

# Adding a policy adds a way the registry can be looked at, so a tree appears
# for it, and what the policy covers leaves the anonymous tree at the same time

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

cat << 'EOF' > "$TMP/schemas/b.json"
{
  "$schema": "https://json-schema.org/draft/2020-12/schema",
  "$id": "https://example.com/b",
  "$ref": "./a"
}
EOF

cat << 'EOF' > "$TMP/one.json"
{
  "url": "https://example.com",
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
  \( -path './explorer/public/self' \) -prune \
  -o -print \
  | LC_ALL=C sort > "$TMP/before.txt"
cd - > /dev/null

cat << 'EOF' > "$TMP/expected-before.txt"
./explorer/public
./explorer/public/%
./explorer/public/%/404.metapack
./explorer/public/%/directory-html.metapack
./explorer/public/%/directory.metapack
./explorer/public/%/login-html.metapack
./explorer/public/%/mcp.metapack
./explorer/public/%/search.metapack
./explorer/public/schemas
./explorer/public/schemas/%
./explorer/public/schemas/%/directory-html.metapack
./explorer/public/schemas/%/directory.metapack
./explorer/public/schemas/a
./explorer/public/schemas/a/%
./explorer/public/schemas/a/%/dependents.metapack
./explorer/public/schemas/a/%/schema-html.metapack
./explorer/public/schemas/a/%/schema.metapack
./explorer/public/schemas/b
./explorer/public/schemas/b/%
./explorer/public/schemas/b/%/dependents.metapack
./explorer/public/schemas/b/%/schema-html.metapack
./explorer/public/schemas/b/%/schema.metapack
EOF

diff "$TMP/before.txt" "$TMP/expected-before.txt"

cat << 'EOF' > "$TMP/one.json"
{
  "url": "https://example.com",
  "authentication": [
    {
      "type": "apiKey",
      "algorithm": "identity",
      "name": "alpha",
      "paths": [ "/schemas" ],
      "keys": [ { "environmentVariable": "ONE_TEST_KEY_ALPHA" } ]
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
  \( -path './explorer/public/self' -o -path './explorer/alpha/self' \) -prune \
  -o -print \
  | LC_ALL=C sort > "$TMP/after.txt"
cd - > /dev/null

cat << 'EOF' > "$TMP/expected-after.txt"
./explorer/alpha
./explorer/alpha/%
./explorer/alpha/%/404.metapack
./explorer/alpha/%/directory-html.metapack
./explorer/alpha/%/directory.metapack
./explorer/alpha/%/mcp.metapack
./explorer/alpha/%/search.metapack
./explorer/alpha/schemas
./explorer/alpha/schemas/%
./explorer/alpha/schemas/%/directory-html.metapack
./explorer/alpha/schemas/%/directory.metapack
./explorer/alpha/schemas/a
./explorer/alpha/schemas/a/%
./explorer/alpha/schemas/a/%/dependents.metapack
./explorer/alpha/schemas/a/%/schema-html.metapack
./explorer/alpha/schemas/a/%/schema.metapack
./explorer/alpha/schemas/b
./explorer/alpha/schemas/b/%
./explorer/alpha/schemas/b/%/dependents.metapack
./explorer/alpha/schemas/b/%/schema-html.metapack
./explorer/alpha/schemas/b/%/schema.metapack
./explorer/public
./explorer/public/%
./explorer/public/%/404.metapack
./explorer/public/%/directory-html.metapack
./explorer/public/%/directory.metapack
./explorer/public/%/login-html.metapack
./explorer/public/%/mcp.metapack
./explorer/public/%/search.metapack
./explorer/public/schemas
./explorer/public/schemas/%
./explorer/public/schemas/%/directory-html.metapack
./explorer/public/schemas/%/directory.metapack
EOF

diff "$TMP/after.txt" "$TMP/expected-after.txt"
