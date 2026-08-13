#!/bin/sh

set -o errexit
set -o nounset

# A directory that holds both a gated and an ungated collection survives in
# every view, but only the view that reaches the gated collection sees the
# subtree underneath it

TMP="$(mktemp -d)"
clean() { rm -rf "$TMP"; }
trap clean EXIT

mkdir "$TMP/open" "$TMP/open/nested" "$TMP/closed" "$TMP/closed/nested" \
  "$TMP/loose"

cat << 'EOF' > "$TMP/open/one.json"
{
  "$schema": "https://json-schema.org/draft/2020-12/schema",
  "type": "string"
}
EOF

cat << 'EOF' > "$TMP/open/nested/two.json"
{
  "$schema": "https://json-schema.org/draft/2020-12/schema",
  "type": "integer"
}
EOF

cat << 'EOF' > "$TMP/closed/secret.json"
{
  "$schema": "https://json-schema.org/draft/2020-12/schema",
  "type": "object"
}
EOF

cat << 'EOF' > "$TMP/closed/nested/deeper.json"
{
  "$schema": "https://json-schema.org/draft/2020-12/schema",
  "type": "array"
}
EOF

cat << 'EOF' > "$TMP/loose/plain.json"
{
  "$schema": "https://json-schema.org/draft/2020-12/schema",
  "type": "boolean"
}
EOF

cat << 'EOF' > "$TMP/one.json"
{
  "url": "https://example.com",
  "html": {},
  "authentication": [
    {
      "type": "apiKey",
      "algorithm": "identity",
      "name": "vault",
      "paths": [ "/group/closed" ],
      "keys": [ { "environmentVariable": "ONE_KEY_VAULT" } ]
    }
  ],
  "contents": {
    "group": {
      "contents": {
        "open": {
          "baseUri": "https://example.com/group/open/",
          "path": "./open"
        },
        "closed": {
          "baseUri": "https://example.com/group/closed/",
          "path": "./closed"
        }
      }
    },
    "loose": {
      "baseUri": "https://example.com/loose/",
      "path": "./loose"
    }
  }
}
EOF

ONE_KEY_VAULT="secret" \
  "$1" --skip-banner --deterministic --concurrency 1 "$TMP/one.json" \
  "$TMP/output" > /dev/null

cd "$TMP/output"
find . -mindepth 1 \
  \( -path './schemas/self' -o -path './explorer/public/self' \
     -o -path './explorer/vault/self' \) -prune \
  -o -print \
  | LC_ALL=C sort > "$TMP/manifest.txt"
cd - > /dev/null

cat << 'EOF' > "$TMP/expected.txt"
./authentication.bin
./configuration.json
./explorer
./explorer/public
./explorer/public/%
./explorer/public/%/401.metapack
./explorer/public/%/404.metapack
./explorer/public/%/directory-html.metapack
./explorer/public/%/directory.metapack
./explorer/public/%/login-html.metapack
./explorer/public/%/mcp.metapack
./explorer/public/%/search.metapack
./explorer/public/group
./explorer/public/group/%
./explorer/public/group/%/directory-html.metapack
./explorer/public/group/%/directory.metapack
./explorer/public/group/open
./explorer/public/group/open/%
./explorer/public/group/open/%/directory-html.metapack
./explorer/public/group/open/%/directory.metapack
./explorer/public/group/open/nested
./explorer/public/group/open/nested/%
./explorer/public/group/open/nested/%/directory-html.metapack
./explorer/public/group/open/nested/%/directory.metapack
./explorer/public/group/open/nested/two
./explorer/public/group/open/nested/two/%
./explorer/public/group/open/nested/two/%/dependents.metapack
./explorer/public/group/open/nested/two/%/schema-html.metapack
./explorer/public/group/open/nested/two/%/schema.metapack
./explorer/public/group/open/one
./explorer/public/group/open/one/%
./explorer/public/group/open/one/%/dependents.metapack
./explorer/public/group/open/one/%/schema-html.metapack
./explorer/public/group/open/one/%/schema.metapack
./explorer/public/loose
./explorer/public/loose/%
./explorer/public/loose/%/directory-html.metapack
./explorer/public/loose/%/directory.metapack
./explorer/public/loose/plain
./explorer/public/loose/plain/%
./explorer/public/loose/plain/%/dependents.metapack
./explorer/public/loose/plain/%/schema-html.metapack
./explorer/public/loose/plain/%/schema.metapack
./explorer/vault
./explorer/vault/%
./explorer/vault/%/401.metapack
./explorer/vault/%/404.metapack
./explorer/vault/%/directory-html.metapack
./explorer/vault/%/directory.metapack
./explorer/vault/%/mcp.metapack
./explorer/vault/%/search.metapack
./explorer/vault/group
./explorer/vault/group/%
./explorer/vault/group/%/directory-html.metapack
./explorer/vault/group/%/directory.metapack
./explorer/vault/group/closed
./explorer/vault/group/closed/%
./explorer/vault/group/closed/%/directory-html.metapack
./explorer/vault/group/closed/%/directory.metapack
./explorer/vault/group/closed/nested
./explorer/vault/group/closed/nested/%
./explorer/vault/group/closed/nested/%/directory-html.metapack
./explorer/vault/group/closed/nested/%/directory.metapack
./explorer/vault/group/closed/nested/deeper
./explorer/vault/group/closed/nested/deeper/%
./explorer/vault/group/closed/nested/deeper/%/dependents.metapack
./explorer/vault/group/closed/nested/deeper/%/schema-html.metapack
./explorer/vault/group/closed/nested/deeper/%/schema.metapack
./explorer/vault/group/closed/secret
./explorer/vault/group/closed/secret/%
./explorer/vault/group/closed/secret/%/dependents.metapack
./explorer/vault/group/closed/secret/%/schema-html.metapack
./explorer/vault/group/closed/secret/%/schema.metapack
./explorer/vault/group/open
./explorer/vault/group/open/%
./explorer/vault/group/open/%/directory-html.metapack
./explorer/vault/group/open/%/directory.metapack
./explorer/vault/group/open/nested
./explorer/vault/group/open/nested/%
./explorer/vault/group/open/nested/%/directory-html.metapack
./explorer/vault/group/open/nested/%/directory.metapack
./explorer/vault/group/open/nested/two
./explorer/vault/group/open/nested/two/%
./explorer/vault/group/open/nested/two/%/dependents.metapack
./explorer/vault/group/open/nested/two/%/schema-html.metapack
./explorer/vault/group/open/nested/two/%/schema.metapack
./explorer/vault/group/open/one
./explorer/vault/group/open/one/%
./explorer/vault/group/open/one/%/dependents.metapack
./explorer/vault/group/open/one/%/schema-html.metapack
./explorer/vault/group/open/one/%/schema.metapack
./explorer/vault/loose
./explorer/vault/loose/%
./explorer/vault/loose/%/directory-html.metapack
./explorer/vault/loose/%/directory.metapack
./explorer/vault/loose/plain
./explorer/vault/loose/plain/%
./explorer/vault/loose/plain/%/dependents.metapack
./explorer/vault/loose/plain/%/schema-html.metapack
./explorer/vault/loose/plain/%/schema.metapack
./routes.bin
./schemas
./schemas/group
./schemas/group/closed
./schemas/group/closed/nested
./schemas/group/closed/nested/deeper
./schemas/group/closed/nested/deeper/%
./schemas/group/closed/nested/deeper/%/blaze-exhaustive.metapack
./schemas/group/closed/nested/deeper/%/blaze-fast.metapack
./schemas/group/closed/nested/deeper/%/bundle.metapack
./schemas/group/closed/nested/deeper/%/dependencies.metapack
./schemas/group/closed/nested/deeper/%/editor.metapack
./schemas/group/closed/nested/deeper/%/health.metapack
./schemas/group/closed/nested/deeper/%/locations.metapack
./schemas/group/closed/nested/deeper/%/positions.metapack
./schemas/group/closed/nested/deeper/%/schema.metapack
./schemas/group/closed/nested/deeper/%/stats.metapack
./schemas/group/closed/secret
./schemas/group/closed/secret/%
./schemas/group/closed/secret/%/blaze-exhaustive.metapack
./schemas/group/closed/secret/%/blaze-fast.metapack
./schemas/group/closed/secret/%/bundle.metapack
./schemas/group/closed/secret/%/dependencies.metapack
./schemas/group/closed/secret/%/editor.metapack
./schemas/group/closed/secret/%/health.metapack
./schemas/group/closed/secret/%/locations.metapack
./schemas/group/closed/secret/%/positions.metapack
./schemas/group/closed/secret/%/schema.metapack
./schemas/group/closed/secret/%/stats.metapack
./schemas/group/open
./schemas/group/open/nested
./schemas/group/open/nested/two
./schemas/group/open/nested/two/%
./schemas/group/open/nested/two/%/blaze-exhaustive.metapack
./schemas/group/open/nested/two/%/blaze-fast.metapack
./schemas/group/open/nested/two/%/bundle.metapack
./schemas/group/open/nested/two/%/dependencies.metapack
./schemas/group/open/nested/two/%/editor.metapack
./schemas/group/open/nested/two/%/health.metapack
./schemas/group/open/nested/two/%/locations.metapack
./schemas/group/open/nested/two/%/positions.metapack
./schemas/group/open/nested/two/%/schema.metapack
./schemas/group/open/nested/two/%/stats.metapack
./schemas/group/open/one
./schemas/group/open/one/%
./schemas/group/open/one/%/blaze-exhaustive.metapack
./schemas/group/open/one/%/blaze-fast.metapack
./schemas/group/open/one/%/bundle.metapack
./schemas/group/open/one/%/dependencies.metapack
./schemas/group/open/one/%/editor.metapack
./schemas/group/open/one/%/health.metapack
./schemas/group/open/one/%/locations.metapack
./schemas/group/open/one/%/positions.metapack
./schemas/group/open/one/%/schema.metapack
./schemas/group/open/one/%/stats.metapack
./schemas/loose
./schemas/loose/plain
./schemas/loose/plain/%
./schemas/loose/plain/%/blaze-exhaustive.metapack
./schemas/loose/plain/%/blaze-fast.metapack
./schemas/loose/plain/%/bundle.metapack
./schemas/loose/plain/%/dependencies.metapack
./schemas/loose/plain/%/editor.metapack
./schemas/loose/plain/%/health.metapack
./schemas/loose/plain/%/locations.metapack
./schemas/loose/plain/%/positions.metapack
./schemas/loose/plain/%/schema.metapack
./schemas/loose/plain/%/stats.metapack
./state.bin
./version.json
EOF

diff "$TMP/manifest.txt" "$TMP/expected.txt"
