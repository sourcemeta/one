#!/bin/sh

# The build state records that a build materialised a schema, and an
# incremental build trusts it. The output directory belongs to the indexer, so
# a recorded artifact that is gone from disk means something else modified the
# directory, and the build must refuse loudly instead of continuing over a
# record it cannot honour

set -o errexit
set -o nounset

TMP="$(mktemp -d)"
clean() { rm -rf "$TMP"; }
trap clean EXIT

cat << 'EOF' > "$TMP/one.json"
{
  "url": "https://example.com/",
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

"$1" --skip-banner "$TMP/one.json" "$TMP/output" > /dev/null 2>&1

rm "$TMP/output/schemas/schemas/a/%/schema.metapack"

"$1" --skip-banner "$TMP/one.json" "$TMP/output" > "$TMP/output.txt" 2> /dev/null \
  && CODE="$?" || CODE="$?"
test "$CODE" = "1" || exit 1

cat << EOF > "$TMP/expected.txt"
error: The build state references an artifact that no longer exists on disk
  at path $(realpath "$TMP")/output/schemas/schemas/a/%/schema.metapack

Something other than the indexer modified the output directory, so delete it and index again from scratch
EOF

diff "$TMP/output.txt" "$TMP/expected.txt"
