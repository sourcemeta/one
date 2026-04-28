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

"$1" --skip-banner "$TMP/one.json" "$TMP/output" --configuration > "$TMP/output.txt"
cat << EOF > "$TMP/expected.txt"
{
  "contents": {
    "self": {
      "title": "Self",
      "description": "The schemas that define the current version of this instance",
      "email": "hello@sourcemeta.com",
      "github": "sourcemeta/one",
      "website": "https://www.sourcemeta.com",
      "contents": {
        "v1": {
          "contents": {
            "schemas": {
              "path": "$ONE_PREFIX/share/sourcemeta/one/self/v1/schemas",
              "x-sourcemeta-one:path": "$ONE_PREFIX/share/sourcemeta/one/self/v1/one.json",
              "baseUri": "https://sourcemeta.com/"
            }
          }
        }
      }
    },
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
  "url": "https://sourcemeta.com/",
  "html": {
    "name": "Sourcemeta",
    "description": "The next-generation JSON Schema platform"
  },
  "api": {}
}
EOF
diff "$TMP/output.txt" "$TMP/expected.txt"
