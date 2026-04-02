#!/bin/sh

set -o errexit
set -o nounset

TMP="$(mktemp -d)"
clean() { rm -rf "$TMP"; }
trap clean EXIT

cat << EOF > "$TMP/one.json"
{
  "url": "https://example.com/schemas",
  "contents": {
    "org": {
      "contents": {
        "team-a": {
          "baseUri": "https://example.com/team-a",
          "path": "./schemas-a"
        },
        "team-b": {
          "baseUri": "https://example.com/team-b",
          "path": "./schemas-b"
        }
      }
    }
  }
}
EOF

mkdir "$TMP/schemas-a" "$TMP/schemas-b"

cat << 'EOF' > "$TMP/schemas-a/s1.json"
{
  "$schema": "http://json-schema.org/draft-07/schema#",
  "$id": "https://example.com/team-a/s1"
}
EOF

cat << 'EOF' > "$TMP/schemas-b/s2.json"
{
  "$schema": "http://json-schema.org/draft-07/schema#",
  "$id": "https://example.com/team-b/s2"
}
EOF

"$1" "$TMP/one.json" "$TMP/output"

cd "$TMP/output"
find . -mindepth 1 | LC_ALL=C sort > "$TMP/manifest.txt"
cd - > /dev/null

cat << 'EOF' > "$TMP/expected.txt"
./configuration.json
./explorer
./explorer/%
./explorer/%/404.metapack
./explorer/%/directory-html.metapack
./explorer/%/directory.metapack
./explorer/%/search.metapack
./explorer/org
./explorer/org/%
./explorer/org/%/directory-html.metapack
./explorer/org/%/directory.metapack
./explorer/org/team-a
./explorer/org/team-a/%
./explorer/org/team-a/%/directory-html.metapack
./explorer/org/team-a/%/directory.metapack
./explorer/org/team-a/s1
./explorer/org/team-a/s1/%
./explorer/org/team-a/s1/%/schema-html.metapack
./explorer/org/team-a/s1/%/schema.metapack
./explorer/org/team-b
./explorer/org/team-b/%
./explorer/org/team-b/%/directory-html.metapack
./explorer/org/team-b/%/directory.metapack
./explorer/org/team-b/s2
./explorer/org/team-b/s2/%
./explorer/org/team-b/s2/%/schema-html.metapack
./explorer/org/team-b/s2/%/schema.metapack
./routes.bin
./schemas
./schemas/org
./schemas/org/team-a
./schemas/org/team-a/s1
./schemas/org/team-a/s1/%
./schemas/org/team-a/s1/%/blaze-exhaustive.metapack
./schemas/org/team-a/s1/%/blaze-fast.metapack
./schemas/org/team-a/s1/%/bundle.metapack
./schemas/org/team-a/s1/%/dependencies.metapack
./schemas/org/team-a/s1/%/dependents.metapack
./schemas/org/team-a/s1/%/editor.metapack
./schemas/org/team-a/s1/%/health.metapack
./schemas/org/team-a/s1/%/locations.metapack
./schemas/org/team-a/s1/%/positions.metapack
./schemas/org/team-a/s1/%/schema.metapack
./schemas/org/team-a/s1/%/stats.metapack
./schemas/org/team-b
./schemas/org/team-b/s2
./schemas/org/team-b/s2/%
./schemas/org/team-b/s2/%/blaze-exhaustive.metapack
./schemas/org/team-b/s2/%/blaze-fast.metapack
./schemas/org/team-b/s2/%/bundle.metapack
./schemas/org/team-b/s2/%/dependencies.metapack
./schemas/org/team-b/s2/%/dependents.metapack
./schemas/org/team-b/s2/%/editor.metapack
./schemas/org/team-b/s2/%/health.metapack
./schemas/org/team-b/s2/%/locations.metapack
./schemas/org/team-b/s2/%/positions.metapack
./schemas/org/team-b/s2/%/schema.metapack
./schemas/org/team-b/s2/%/stats.metapack
./state.bin
./version.json
EOF

diff "$TMP/manifest.txt" "$TMP/expected.txt"
