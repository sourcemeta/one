#!/bin/sh

set -o errexit
set -o nounset

TMP="$(mktemp -d)"
clean() { rm -rf "$TMP"; }
trap clean EXIT

cat << EOF > "$TMP/one.json"
{
  "url": "https://sourcemeta.com/",
  "html": false,
  "contents": {
    "example": {
      "contents": {
        "schemas": {
          "baseUri": "https://example.com/",
          "path": "./schemas"
        }
      }
    }
  }
}
EOF

mkdir "$TMP/schemas"

cat << 'EOF' > "$TMP/schemas/foo.json"
{
  "$schema": "http://json-schema.org/draft-07/schema#",
  "$id": "https://example.com/foo"
}
EOF

cat << 'EOF' > "$TMP/schemas/bar.json"
{
  "$schema": "http://json-schema.org/draft-07/schema#",
  "$id": "https://example.com/bar"
}
EOF

cat << 'EOF' > "$TMP/schemas/baz.json"
{
  "$schema": "http://json-schema.org/draft-07/schema#",
  "$id": "https://example.com/baz"
}
EOF

# It should fail with a low limit
"$1" --skip-banner "$TMP/one.json" "$TMP/output" \
  --maximum-direct-directory-entries 2 \
  > "$TMP/output.txt" && CODE="$?" || CODE="$?"
test "$CODE" = "1" || exit 1

cat << EOF > "$TMP/expected.txt"
error: Too many entries in a single directory
  at path $(realpath "$TMP")/output/schemas/self/v1/schemas/mcp/tools/call
  with count 14
EOF

diff "$TMP/output.txt" "$TMP/expected.txt"

# It should succeed without the limit
rm -rf "$TMP/output"
"$1" --skip-banner "$TMP/one.json" "$TMP/output"

# It should succeed when total schemas exceed the limit but are spread
# across a directory hierarchy where no single directory has more than the limit.
# The limit must be above 14 since self/v1's mcp/tools/call directory has 14 entries
# (12 per-tool subdirectories plus the generic request and response schemas)
rm -rf "$TMP/output"
rm -rf "$TMP/schemas"
mkdir -p "$TMP/schemas/a" "$TMP/schemas/b"

cat << 'EOF' > "$TMP/schemas/a/one.json"
{
  "$schema": "http://json-schema.org/draft-07/schema#",
  "$id": "https://example.com/a/one"
}
EOF

cat << 'EOF' > "$TMP/schemas/a/two.json"
{
  "$schema": "http://json-schema.org/draft-07/schema#",
  "$id": "https://example.com/a/two"
}
EOF

cat << 'EOF' > "$TMP/schemas/b/three.json"
{
  "$schema": "http://json-schema.org/draft-07/schema#",
  "$id": "https://example.com/b/three"
}
EOF

"$1" --skip-banner "$TMP/one.json" "$TMP/output" \
  --maximum-direct-directory-entries 15
