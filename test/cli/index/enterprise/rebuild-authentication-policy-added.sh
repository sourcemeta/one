#!/bin/sh

# Adding a policy adds a way the registry can be looked at, so a tree appears
# for it holding everything the anonymous one holds, since nothing filters yet

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

(cd "$TMP/output" && find ./explorer -maxdepth 1 | sort) > "$TMP/views.txt"

cat << 'EOF' > "$TMP/expected.txt"
./explorer
./explorer/alpha
./explorer/public
EOF

diff "$TMP/views.txt" "$TMP/expected.txt"

# The new tree is complete rather than a shell, so what each view holds is
# compared entry by entry
(cd "$TMP/output/explorer/public" && find . | sort) > "$TMP/public.txt"
(cd "$TMP/output/explorer/alpha" && find . | sort) > "$TMP/alpha.txt"
diff "$TMP/public.txt" "$TMP/alpha.txt"
