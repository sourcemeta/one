#!/bin/sh

# A schema changing while the policies stay put is the incremental path, and it
# has to reach every view rather than only the first, since each holds its own
# copy of what that schema contributes

set -o errexit
set -o nounset

TMP="$(mktemp -d)"
clean() { rm -rf "$TMP"; }
trap clean EXIT

remove_threads_information() {
  expr='s/ \[[^]]*[^a-z-][^]]*\]//g'
  if [ "$(uname -s)" = "Darwin" ]; then
    sed -i '' "$expr" "$1"
  else
    sed -i "$expr" "$1"
  fi
}

mkdir "$TMP/schemas"

cat << 'EOF' > "$TMP/schemas/a.json"
{
  "$schema": "https://json-schema.org/draft/2020-12/schema",
  "$id": "https://example.com/a",
  "title": "Before",
  "type": "object"
}
EOF

cat << 'EOF' > "$TMP/schemas/b.json"
{
  "$schema": "https://json-schema.org/draft/2020-12/schema",
  "$id": "https://example.com/b",
  "$ref": "./a"
}
EOF

cat << 'EOF' > "$TMP/one.json"
{
  "url": "https://example.com",
  "authentication": [
    {
      "type": "apiKey",
      "algorithm": "identity",
      "name": "alpha",
      "paths": [ "/schemas" ],
      "keys": [ { "environmentVariable": "ONE_TEST_KEY_ALPHA" } ]
    }
  ],
  "contents": {
    "schemas": {
      "baseUri": "https://example.com/",
      "path": "./schemas"
    }
  }
}
EOF

"$1" --skip-banner --deterministic --concurrency 1 "$TMP/one.json" "$TMP/output" \
  > /dev/null 2>&1

cat << 'EOF' > "$TMP/schemas/a.json"
{
  "$schema": "https://json-schema.org/draft/2020-12/schema",
  "$id": "https://example.com/a",
  "title": "After",
  "type": "object"
}
EOF

"$1" --skip-banner --deterministic --concurrency 1 "$TMP/one.json" "$TMP/output" \
  2> "$TMP/output.txt"
remove_threads_information "$TMP/output.txt"
grep -E "Producing|Combining" "$TMP/output.txt" > "$TMP/produced.txt"

# Every artifact the edit reaches is named twice, once under each view, which is
# what a view left holding the previous contents would be missing from
cat << 'EOF' > "$TMP/expected-produced.txt"
(  3%) Producing: explorer/public/%/directory.metapack
(  6%) Producing: schemas/schemas/a/%/schema.metapack
(  9%) Producing: schemas/schemas/b/%/dependencies.metapack
( 12%) Producing: explorer/public/%/directory-html.metapack
( 15%) Producing: explorer/public/%/login-html.metapack
( 18%) Producing: explorer/public/%/search.metapack
( 21%) Producing: schemas/schemas/a/%/dependencies.metapack
( 24%) Producing: schemas/schemas/a/%/locations.metapack
( 27%) Producing: schemas/schemas/a/%/positions.metapack
( 30%) Producing: schemas/schemas/a/%/stats.metapack
( 33%) Producing: schemas/schemas/b/%/bundle.metapack
( 36%) Producing: schemas/schemas/b/%/health.metapack
( 39%) Producing: explorer/alpha/schemas/b/%/schema.metapack
( 42%) Producing: explorer/public/%/mcp.metapack
( 45%) Producing: schemas/schemas/a/%/bundle.metapack
( 48%) Producing: schemas/schemas/a/%/health.metapack
( 51%) Producing: schemas/schemas/b/%/blaze-exhaustive.metapack
( 54%) Producing: schemas/schemas/b/%/blaze-fast.metapack
( 57%) Producing: schemas/schemas/b/%/editor.metapack
( 60%) Producing: explorer/alpha/schemas/a/%/schema.metapack
( 63%) Producing: explorer/alpha/schemas/b/%/schema-html.metapack
( 66%) Producing: schemas/schemas/a/%/blaze-exhaustive.metapack
( 69%) Producing: schemas/schemas/a/%/blaze-fast.metapack
( 72%) Producing: schemas/schemas/a/%/editor.metapack
( 75%) Producing: explorer/alpha/schemas/%/directory.metapack
( 78%) Producing: explorer/alpha/schemas/a/%/schema-html.metapack
( 81%) Producing: explorer/alpha/%/directory.metapack
( 84%) Producing: explorer/alpha/schemas/%/directory-html.metapack
( 87%) Producing: explorer/alpha/schemas/%/login-html.metapack
( 90%) Producing: explorer/alpha/%/directory-html.metapack
( 93%) Producing: explorer/alpha/%/login-html.metapack
( 96%) Producing: explorer/alpha/%/search.metapack
(100%) Producing: explorer/alpha/%/mcp.metapack
EOF

diff "$TMP/produced.txt" "$TMP/expected-produced.txt"

cd "$TMP/output"
find ./explorer -mindepth 1 \
  \( -path './explorer/public/self' -o -path './explorer/alpha/self' \) -prune \
  -o -print \
  | LC_ALL=C sort > "$TMP/manifest.txt"
cd - > /dev/null

cat << 'EOF' > "$TMP/expected-manifest.txt"
./explorer/alpha
./explorer/alpha/%
./explorer/alpha/%/401.metapack
./explorer/alpha/%/404.metapack
./explorer/alpha/%/directory-html.metapack
./explorer/alpha/%/directory.metapack
./explorer/alpha/%/login-html.metapack
./explorer/alpha/%/mcp.metapack
./explorer/alpha/%/search.metapack
./explorer/alpha/schemas
./explorer/alpha/schemas/%
./explorer/alpha/schemas/%/directory-html.metapack
./explorer/alpha/schemas/%/directory.metapack
./explorer/alpha/schemas/%/login-html.metapack
./explorer/alpha/schemas/a
./explorer/alpha/schemas/a/%
./explorer/alpha/schemas/a/%/dependents.metapack
./explorer/alpha/schemas/a/%/schema-html.metapack
./explorer/alpha/schemas/a/%/schema.metapack
./explorer/alpha/schemas/b
./explorer/alpha/schemas/b/%
./explorer/alpha/schemas/b/%/dependents.metapack
./explorer/alpha/schemas/b/%/schema-html.metapack
./explorer/alpha/schemas/b/%/schema.metapack
./explorer/public
./explorer/public/%
./explorer/public/%/401.metapack
./explorer/public/%/404.metapack
./explorer/public/%/directory-html.metapack
./explorer/public/%/directory.metapack
./explorer/public/%/login-html.metapack
./explorer/public/%/mcp.metapack
./explorer/public/%/search.metapack
EOF

diff "$TMP/manifest.txt" "$TMP/expected-manifest.txt"
