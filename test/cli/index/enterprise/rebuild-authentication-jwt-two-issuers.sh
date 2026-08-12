#!/bin/sh

# A token carries one issuer, so two policies naming different ones can never be
# satisfied at once however many claims a token holds. The registry is looked at
# three ways rather than four, and no tree is spelled from both names

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
      "name": "staff",
      "paths": [ "/schemas" ],
      "issuer": "https://idp.example.com",
      "audience": "https://example.com",
      "algorithms": [ "RS256" ],
      "claims": { "groups": [ "platform" ] }
    },
    {
      "type": "jwt",
      "name": "partner",
      "paths": [ "/schemas" ],
      "issuer": "https://idp.partner.com",
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
  \( -path './explorer/public/self' -o -path './explorer/partner/self' \
     -o -path './explorer/staff/self' \) -prune \
  -o -print \
  | LC_ALL=C sort > "$TMP/manifest.txt"
cd - > /dev/null

cat << 'EOF' > "$TMP/expected.txt"
./explorer/partner
./explorer/partner/%
./explorer/partner/%/401.metapack
./explorer/partner/%/404.metapack
./explorer/partner/%/directory-html.metapack
./explorer/partner/%/directory.metapack
./explorer/partner/%/login-html.metapack
./explorer/partner/%/mcp.metapack
./explorer/partner/%/search.metapack
./explorer/partner/schemas
./explorer/partner/schemas/%
./explorer/partner/schemas/%/directory-html.metapack
./explorer/partner/schemas/%/directory.metapack
./explorer/partner/schemas/%/login-html.metapack
./explorer/partner/schemas/a
./explorer/partner/schemas/a/%
./explorer/partner/schemas/a/%/dependents.metapack
./explorer/partner/schemas/a/%/schema-html.metapack
./explorer/partner/schemas/a/%/schema.metapack
./explorer/public
./explorer/public/%
./explorer/public/%/401.metapack
./explorer/public/%/404.metapack
./explorer/public/%/directory-html.metapack
./explorer/public/%/directory.metapack
./explorer/public/%/login-html.metapack
./explorer/public/%/mcp.metapack
./explorer/public/%/search.metapack
./explorer/staff
./explorer/staff/%
./explorer/staff/%/401.metapack
./explorer/staff/%/404.metapack
./explorer/staff/%/directory-html.metapack
./explorer/staff/%/directory.metapack
./explorer/staff/%/login-html.metapack
./explorer/staff/%/mcp.metapack
./explorer/staff/%/search.metapack
./explorer/staff/schemas
./explorer/staff/schemas/%
./explorer/staff/schemas/%/directory-html.metapack
./explorer/staff/schemas/%/directory.metapack
./explorer/staff/schemas/%/login-html.metapack
./explorer/staff/schemas/a
./explorer/staff/schemas/a/%
./explorer/staff/schemas/a/%/dependents.metapack
./explorer/staff/schemas/a/%/schema-html.metapack
./explorer/staff/schemas/a/%/schema.metapack
EOF

diff "$TMP/manifest.txt" "$TMP/expected.txt"
