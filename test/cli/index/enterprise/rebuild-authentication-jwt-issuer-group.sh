#!/bin/sh

# Two token policies naming one issuer can both be satisfied by a single token
# carrying both claims, so the registry is looked at four ways rather than
# three: anonymously, under each of them, and under the two together. The tree
# for the pair is named by joining their names sorted rather than in the order
# they were declared, which is what gives one combination one spelling. These
# are declared with the later name first, so the sorting is what the expected
# output below is showing

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
      "type": "jwt",
      "name": "platform",
      "paths": [ "/schemas" ],
      "issuer": "https://idp.example.com",
      "audience": "https://example.com",
      "algorithms": [ "RS256" ],
      "claims": { "groups": [ "platform" ] }
    },
    {
      "type": "jwt",
      "name": "oncall",
      "paths": [ "/schemas" ],
      "issuer": "https://idp.example.com",
      "audience": "https://example.com",
      "algorithms": [ "RS256" ],
      "claims": { "groups": [ "oncall" ] }
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
  \( -path './explorer/public/self' -o -path './explorer/oncall/self' \
     -o -path './explorer/oncall+platform/self' \
     -o -path './explorer/platform/self' \) -prune \
  -o -print \
  | LC_ALL=C sort > "$TMP/before.txt"
cd - > /dev/null

cat << 'EOF' > "$TMP/expected-before.txt"
./explorer/oncall
./explorer/oncall+platform
./explorer/oncall+platform/%
./explorer/oncall+platform/%/404.metapack
./explorer/oncall+platform/%/directory-html.metapack
./explorer/oncall+platform/%/directory.metapack
./explorer/oncall+platform/%/mcp.metapack
./explorer/oncall+platform/%/search.metapack
./explorer/oncall+platform/schemas
./explorer/oncall+platform/schemas/%
./explorer/oncall+platform/schemas/%/directory-html.metapack
./explorer/oncall+platform/schemas/%/directory.metapack
./explorer/oncall+platform/schemas/a
./explorer/oncall+platform/schemas/a/%
./explorer/oncall+platform/schemas/a/%/dependents.metapack
./explorer/oncall+platform/schemas/a/%/schema-html.metapack
./explorer/oncall+platform/schemas/a/%/schema.metapack
./explorer/oncall/%
./explorer/oncall/%/404.metapack
./explorer/oncall/%/directory-html.metapack
./explorer/oncall/%/directory.metapack
./explorer/oncall/%/mcp.metapack
./explorer/oncall/%/search.metapack
./explorer/oncall/schemas
./explorer/oncall/schemas/%
./explorer/oncall/schemas/%/directory-html.metapack
./explorer/oncall/schemas/%/directory.metapack
./explorer/oncall/schemas/a
./explorer/oncall/schemas/a/%
./explorer/oncall/schemas/a/%/dependents.metapack
./explorer/oncall/schemas/a/%/schema-html.metapack
./explorer/oncall/schemas/a/%/schema.metapack
./explorer/platform
./explorer/platform/%
./explorer/platform/%/404.metapack
./explorer/platform/%/directory-html.metapack
./explorer/platform/%/directory.metapack
./explorer/platform/%/mcp.metapack
./explorer/platform/%/search.metapack
./explorer/platform/schemas
./explorer/platform/schemas/%
./explorer/platform/schemas/%/directory-html.metapack
./explorer/platform/schemas/%/directory.metapack
./explorer/platform/schemas/a
./explorer/platform/schemas/a/%
./explorer/platform/schemas/a/%/dependents.metapack
./explorer/platform/schemas/a/%/schema-html.metapack
./explorer/platform/schemas/a/%/schema.metapack
./explorer/public
./explorer/public/%
./explorer/public/%/404.metapack
./explorer/public/%/directory-html.metapack
./explorer/public/%/directory.metapack
./explorer/public/%/login-html.metapack
./explorer/public/%/login.metapack
./explorer/public/%/mcp.metapack
./explorer/public/%/search.metapack
EOF

diff "$TMP/before.txt" "$TMP/expected-before.txt"

# Dropping one of the two leaves nothing for the pair to name, so the tree
# spelled from both goes with it rather than only the one that was removed
cat << 'EOF' > "$TMP/one.json"
{
  "url": "https://example.com",
  "authentication": [
    {
      "type": "jwt",
      "name": "platform",
      "paths": [ "/schemas" ],
      "issuer": "https://idp.example.com",
      "audience": "https://example.com",
      "algorithms": [ "RS256" ],
      "claims": { "groups": [ "platform" ] }
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
  \( -path './explorer/public/self' -o -path './explorer/platform/self' \) -prune \
  -o -print \
  | LC_ALL=C sort > "$TMP/after.txt"
cd - > /dev/null

cat << 'EOF' > "$TMP/expected-after.txt"
./explorer/platform
./explorer/platform/%
./explorer/platform/%/404.metapack
./explorer/platform/%/directory-html.metapack
./explorer/platform/%/directory.metapack
./explorer/platform/%/mcp.metapack
./explorer/platform/%/search.metapack
./explorer/platform/schemas
./explorer/platform/schemas/%
./explorer/platform/schemas/%/directory-html.metapack
./explorer/platform/schemas/%/directory.metapack
./explorer/platform/schemas/a
./explorer/platform/schemas/a/%
./explorer/platform/schemas/a/%/dependents.metapack
./explorer/platform/schemas/a/%/schema-html.metapack
./explorer/platform/schemas/a/%/schema.metapack
./explorer/public
./explorer/public/%
./explorer/public/%/404.metapack
./explorer/public/%/directory-html.metapack
./explorer/public/%/directory.metapack
./explorer/public/%/login-html.metapack
./explorer/public/%/login.metapack
./explorer/public/%/mcp.metapack
./explorer/public/%/search.metapack
EOF

diff "$TMP/after.txt" "$TMP/expected-after.txt"
