#!/bin/sh

# The build state records that a build wrote a file, not that the file is still
# there, and an incremental build trusts it. So an artifact removed since, by a
# stray deletion or a sync that went wrong, is never produced again: indexing
# reports success and leaves the instance without it.
#
# The authentication artifact is the one where that hurts most. The gate reads
# it on every request and a missing one denies every path, so the instance is
# down while the build that was supposed to repair it exits zero. Routing is the
# same shape.
#
# Each artifact is removed on its own, so that recreating it cannot be a side
# effect of one of the others being gone. Removing the configuration anchor or
# the version would be noticed at startup, which is why neither is the case
# under test here.

set -o errexit
set -o nounset

TMP="$(mktemp -d)"
clean() { rm -rf "$TMP"; }
trap clean EXIT

cat << 'EOF' > "$TMP/one.json"
{
  "url": "https://example.com",
  "authentication": [
    {
      "type": "apiKey",
      "name": "alpha",
      "paths": [ "/schemas" ],
      "algorithm": "identity",
      "keys": [ { "environmentVariable": "ONE_KEY_ALPHA" } ]
    }
  ],
  "contents": {
    "schemas": { "baseUri": "https://example.com/", "path": "./schemas" }
  }
}
EOF

mkdir "$TMP/schemas"

cat << 'EOF' > "$TMP/schemas/a.json"
{
  "$schema": "http://json-schema.org/draft-07/schema#",
  "$id": "https://example.com/a"
}
EOF

cat << 'EOF' > "$TMP/expected.txt"
./authentication.bin
./configuration.json
./explorer
./explorer/%
./explorer/%/401.metapack
./explorer/%/404.metapack
./explorer/%/directory-html.metapack
./explorer/%/directory.metapack
./explorer/%/login-html.metapack
./explorer/%/mcp.metapack
./explorer/%/search.metapack
./explorer/schemas
./explorer/schemas/%
./explorer/schemas/%/directory-html.metapack
./explorer/schemas/%/directory.metapack
./explorer/schemas/%/login-html.metapack
./explorer/schemas/a
./explorer/schemas/a/%
./explorer/schemas/a/%/dependents.metapack
./explorer/schemas/a/%/schema-html.metapack
./explorer/schemas/a/%/schema.metapack
./routes.bin
./schemas
./schemas/schemas
./schemas/schemas/a
./schemas/schemas/a/%
./schemas/schemas/a/%/blaze-exhaustive.metapack
./schemas/schemas/a/%/blaze-fast.metapack
./schemas/schemas/a/%/bundle.metapack
./schemas/schemas/a/%/dependencies.metapack
./schemas/schemas/a/%/editor.metapack
./schemas/schemas/a/%/health.metapack
./schemas/schemas/a/%/locations.metapack
./schemas/schemas/a/%/positions.metapack
./schemas/schemas/a/%/schema.metapack
./schemas/schemas/a/%/stats.metapack
./state.bin
./version.json
EOF

"$1" "$TMP/one.json" "$TMP/output"

rm "$TMP/output/authentication.bin"
"$1" "$TMP/one.json" "$TMP/output"

cd "$TMP/output"
find . -mindepth 1 \
  \( -path './schemas/self' -o -path './explorer/self' \) -prune \
  -o -print \
  | LC_ALL=C sort > "$TMP/manifest.txt"
cd - > /dev/null

diff "$TMP/manifest.txt" "$TMP/expected.txt"

rm "$TMP/output/routes.bin"
"$1" "$TMP/one.json" "$TMP/output"

cd "$TMP/output"
find . -mindepth 1 \
  \( -path './schemas/self' -o -path './explorer/self' \) -prune \
  -o -print \
  | LC_ALL=C sort > "$TMP/manifest.txt"
cd - > /dev/null

diff "$TMP/manifest.txt" "$TMP/expected.txt"
