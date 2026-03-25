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
          "path": "."
        }
      }
    }
  }
}
EOF

"$1" --skip-banner "$TMP/one.json" "$TMP/output" --concurrency 1 \
  2> "$TMP/output.txt" && CODE="$?" || CODE="$?"
test "$CODE" = "0" || exit 1

# Remove thread information
if [ "$(uname)" = "Darwin" ]
then
  sed -i '' 's/ \[.*\]//g' "$TMP/output.txt"
else
  sed -i 's/ \[.*\]//g' "$TMP/output.txt"
fi

cat << EOF > "$TMP/expected.txt"
Writing output to: $(realpath "$TMP")/output
Using configuration: $(realpath "$TMP")/one.json
( 14%) Producing: configuration.json
( 28%) Producing: version.json
( 42%) Producing: explorer/%/404.metapack
( 57%) Producing: explorer/%/directory.metapack
( 71%) Producing: explorer/%/directory-html.metapack
( 85%) Producing: explorer/%/search.metapack
(100%) Producing: routes.bin
EOF

diff "$TMP/output.txt" "$TMP/expected.txt"
