#!/bin/sh

# A policy is one of the ways the registry can be looked at, so removing it
# removes a view, and the tree written for that view has nothing left to serve
# it. Nothing else about the catalog changes, which is what makes the tree
# either disappearing or lingering the only thing this observes

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

test -d "$TMP/output/explorer/public" || exit 1
test -d "$TMP/output/explorer/alpha" || exit 1

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

(cd "$TMP/output" && find ./explorer -maxdepth 1 | sort) > "$TMP/views.txt"

cat << 'EOF' > "$TMP/expected.txt"
./explorer
./explorer/public
EOF

diff "$TMP/views.txt" "$TMP/expected.txt"
