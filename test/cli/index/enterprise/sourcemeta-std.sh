#!/bin/sh

set -o errexit
set -o nounset

TMP="$(mktemp -d)"
clean() { rm -rf "$TMP"; }
trap clean EXIT

cat << EOF > "$TMP/one.json"
{
  "url": "https://example.com/",
  "extends": [ "@sourcemeta/std/v0" ]
}
EOF

"$1" "$TMP/one.json" "$TMP/output" --configuration | tail -n +2 > "$TMP/output.txt"

cat << EOF > "$TMP/expected.txt"
{
  "contents": {
    "sourcemeta": {
      "title": "Sourcemeta",
      "description": "A provider of premium tooling and services to support JSON Schema use in production",
      "email": "hello@sourcemeta.com",
      "github": "sourcemeta",
      "website": "https://www.sourcemeta.com",
      "contents": {
        "std": {
          "title": "The JSON Schema Standard Library",
          "description": "A growing collection of hand-crafted high-quality schemas by Sourcemeta",
          "email": "hello@sourcemeta.com",
          "github": "sourcemeta/std",
          "website": "https://www.sourcemeta/products/std",
          "contents": {
            "v0": {
              "x-sourcemeta-one:provenance": "@sourcemeta/std/v0",
              "path": "$ONE_PREFIX/share/sourcemeta/one/collections/sourcemeta/std/v0/schemas/2020-12",
              "baseUri": "https://example.com/"
            }
          }
        }
      }
    }
  },
  "url": "https://example.com/",
  "html": {
    "name": "Sourcemeta",
    "description": "The next-generation JSON Schema platform"
  }
}
EOF

diff "$TMP/output.txt" "$TMP/expected.txt"
