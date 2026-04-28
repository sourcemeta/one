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

cat << 'EOF' > "$TMP/schemas/test.json"
{
  "$schema": "http://json-schema.org/draft-07/schema#",
  "$id": "https://example.com/test.json",
  "$ref": "#",
  "type": 1
}
EOF

"$1" --skip-banner --deterministic "$TMP/one.json" "$TMP/output" --concurrency 1 2> "$TMP/output.txt" && CODE="$?" || CODE="$?"
test "$CODE" = "1" || exit 1

remove_threads_information() {
  expr='s/ \[[^]]*[^a-z-][^]]*\]//g'
  if [ "$(uname -s)" = "Darwin" ]; then
    sed -i '' "$expr" "$1"
  else
    sed -i "$expr" "$1"
  fi
}

remove_threads_information "$TMP/output.txt"

cat << EOF > "$TMP/expected.txt"
Writing output to: $(realpath "$TMP")/output
Using configuration: $(realpath "$TMP")/one.json
Detecting: $(realpath "$TMP")/schemas/test.json (#1)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/api/error.json (#2)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/api/list/response.json (#3)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/api/schemas/dependencies/response.json (#4)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/api/schemas/dependents/response.json (#5)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/api/schemas/evaluate/response.json (#6)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/api/schemas/health/response.json (#7)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/api/schemas/locations/response.json (#8)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/api/schemas/metadata/response.json (#9)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/api/schemas/position.json (#10)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/api/schemas/positions/response.json (#11)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/api/schemas/search/response.json (#12)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/api/schemas/stats/response.json (#13)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/api/schemas/trace/response.json (#14)
(  7%) Resolving: example/schemas/test.json
( 14%) Resolving: self/v1/schemas/api/error.json
( 21%) Resolving: self/v1/schemas/api/list/response.json
( 28%) Resolving: self/v1/schemas/api/schemas/dependencies/response.json
( 35%) Resolving: self/v1/schemas/api/schemas/dependents/response.json
( 42%) Resolving: self/v1/schemas/api/schemas/evaluate/response.json
( 50%) Resolving: self/v1/schemas/api/schemas/health/response.json
( 57%) Resolving: self/v1/schemas/api/schemas/locations/response.json
( 64%) Resolving: self/v1/schemas/api/schemas/metadata/response.json
( 71%) Resolving: self/v1/schemas/api/schemas/position.json
( 78%) Resolving: self/v1/schemas/api/schemas/positions/response.json
( 85%) Resolving: self/v1/schemas/api/schemas/search/response.json
( 92%) Resolving: self/v1/schemas/api/schemas/stats/response.json
(100%) Resolving: self/v1/schemas/api/schemas/trace/response.json
(  0%) Producing: configuration.json
(  0%) Producing: version.json
(  1%) Producing: explorer/%/404.metapack
(  1%) Producing: schemas/example/schemas/test/%/schema.metapack
error: A schema with a top-level \`\$ref\` in JSON Schema Draft 7 and older dialects ignores every sibling keywords (like identifiers and meta-schema declarations) and therefore many operations, like bundling, are not possible without undefined behavior
  at path $(realpath "$TMP")/schemas/test.json
  at identifier https://sourcemeta.com/example/schemas/test
EOF

diff "$TMP/output.txt" "$TMP/expected.txt"
