#!/bin/sh

# A schema changing while the policies stay put is the incremental path, and it
# has to reach every view rather than only the first, since each holds its own
# copy of what that schema contributes

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
  "title": "Before",
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

grep -q "Before" "$TMP/output/explorer/public/schemas/a/%/schema.metapack" || exit 1
grep -q "Before" "$TMP/output/explorer/alpha/schemas/a/%/schema.metapack" || exit 1

cat << 'EOF' > "$TMP/schemas/a.json"
{
  "$schema": "https://json-schema.org/draft/2020-12/schema",
  "$id": "https://example.com/a",
  "title": "After",
  "type": "object"
}
EOF

"$1" --skip-banner --deterministic --concurrency 1 "$TMP/one.json" "$TMP/output" \
  > /dev/null 2>&1

# Every view carries the edit, so a view left holding the old title is a view
# the rebuild passed over
grep -q "After" "$TMP/output/explorer/public/schemas/a/%/schema.metapack" || exit 1
grep -q "After" "$TMP/output/explorer/alpha/schemas/a/%/schema.metapack" || exit 1
grep -q "Before" "$TMP/output/explorer/public/schemas/a/%/schema.metapack" && exit 1
grep -q "Before" "$TMP/output/explorer/alpha/schemas/a/%/schema.metapack" && exit 1

exit 0
