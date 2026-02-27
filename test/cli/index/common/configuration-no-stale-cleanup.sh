#!/bin/sh

set -o errexit
set -o nounset

TMP="$(mktemp -d)"
clean() { rm -rf "$TMP"; }
trap clean EXIT

cat << EOF > "$TMP/one.json"
{
  "url": "https://sourcemeta.com/",
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

# Simulate stale staging directories left behind by crashed previous runs
mkdir "$TMP/.sourcemeta-one-aaa"
mkdir "$TMP/.sourcemeta-one-bbb"
mkdir "$TMP/.sourcemeta-one-ccc"

"$1" --skip-banner "$TMP/one.json" "$TMP/output" --configuration > /dev/null 2> "$TMP/output.txt"

cat << EOF > "$TMP/expected.txt"
Writing output to: $(realpath "$TMP")/output
Using configuration: $(realpath "$TMP")/one.json
EOF
diff "$TMP/output.txt" "$TMP/expected.txt"

# Verify the stale directories were NOT cleaned up
test -d "$TMP/.sourcemeta-one-aaa"
test -d "$TMP/.sourcemeta-one-bbb"
test -d "$TMP/.sourcemeta-one-ccc"
