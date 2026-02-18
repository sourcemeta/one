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

"$1" "$TMP/one.json" "$TMP/output" -g | tail -n +2 > "$TMP/output.txt"
cat << EOF > "$TMP/expected.txt"
{
  "url": "https://sourcemeta.com/",
  "contents": {
    "example": {
      "contents": {
        "schemas": {
          "baseUri": "https://example.com/",
          "path": "$(realpath "$TMP")/schemas",
          "x-sourcemeta-one:path": "$(realpath "$TMP")/one.json"
        }
      }
    }
  },
  "html": {
    "name": "Sourcemeta",
    "description": "The next-generation JSON Schema platform"
  }
}
EOF
diff "$TMP/output.txt" "$TMP/expected.txt"
