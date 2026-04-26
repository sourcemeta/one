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
  "allOf": [ { "$ref": "#foo" } ]
}
EOF

"$1" --skip-banner --deterministic "$TMP/one.json" "$TMP/output" --concurrency 1 \
  2> "$TMP/output.txt" && CODE="$?" || CODE="$?"
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
Detecting: $ONE_PREFIX/share/sourcemeta/one/collections/self/v1/schemas/api/error.json (#2)
Detecting: $ONE_PREFIX/share/sourcemeta/one/collections/self/v1/schemas/api/list/response.json (#3)
Detecting: $ONE_PREFIX/share/sourcemeta/one/collections/self/v1/schemas/api/schemas/dependencies/response.json (#4)
Detecting: $ONE_PREFIX/share/sourcemeta/one/collections/self/v1/schemas/api/schemas/dependents/response.json (#5)
Detecting: $ONE_PREFIX/share/sourcemeta/one/collections/self/v1/schemas/api/schemas/evaluate/response.json (#6)
Detecting: $ONE_PREFIX/share/sourcemeta/one/collections/self/v1/schemas/api/schemas/health/response.json (#7)
Detecting: $ONE_PREFIX/share/sourcemeta/one/collections/self/v1/schemas/api/schemas/locations/response.json (#8)
Detecting: $ONE_PREFIX/share/sourcemeta/one/collections/self/v1/schemas/api/schemas/metadata/response.json (#9)
Detecting: $ONE_PREFIX/share/sourcemeta/one/collections/self/v1/schemas/api/schemas/position.json (#10)
Detecting: $ONE_PREFIX/share/sourcemeta/one/collections/self/v1/schemas/api/schemas/positions/response.json (#11)
Detecting: $ONE_PREFIX/share/sourcemeta/one/collections/self/v1/schemas/api/schemas/search/response.json (#12)
Detecting: $ONE_PREFIX/share/sourcemeta/one/collections/self/v1/schemas/api/schemas/stats/response.json (#13)
Detecting: $ONE_PREFIX/share/sourcemeta/one/collections/self/v1/schemas/api/schemas/trace/response.json (#14)
Detecting: $ONE_PREFIX/share/sourcemeta/one/collections/self/v1/schemas/configuration/collection.json (#15)
Detecting: $ONE_PREFIX/share/sourcemeta/one/collections/self/v1/schemas/configuration/configuration.json (#16)
Detecting: $ONE_PREFIX/share/sourcemeta/one/collections/self/v1/schemas/configuration/contents.json (#17)
Detecting: $ONE_PREFIX/share/sourcemeta/one/collections/self/v1/schemas/configuration/extends.json (#18)
Detecting: $ONE_PREFIX/share/sourcemeta/one/collections/self/v1/schemas/configuration/page.json (#19)
Detecting: $ONE_PREFIX/share/sourcemeta/one/collections/self/v1/schemas/configuration/path.json (#20)
(  5%) Resolving: example/schemas/test.json
( 10%) Resolving: self/v1/schemas/api/error.json
( 15%) Resolving: self/v1/schemas/api/list/response.json
( 20%) Resolving: self/v1/schemas/api/schemas/dependencies/response.json
( 25%) Resolving: self/v1/schemas/api/schemas/dependents/response.json
( 30%) Resolving: self/v1/schemas/api/schemas/evaluate/response.json
( 35%) Resolving: self/v1/schemas/api/schemas/health/response.json
( 40%) Resolving: self/v1/schemas/api/schemas/locations/response.json
( 45%) Resolving: self/v1/schemas/api/schemas/metadata/response.json
( 50%) Resolving: self/v1/schemas/api/schemas/position.json
( 55%) Resolving: self/v1/schemas/api/schemas/positions/response.json
( 60%) Resolving: self/v1/schemas/api/schemas/search/response.json
( 65%) Resolving: self/v1/schemas/api/schemas/stats/response.json
( 70%) Resolving: self/v1/schemas/api/schemas/trace/response.json
( 75%) Resolving: self/v1/schemas/configuration/collection.json
( 80%) Resolving: self/v1/schemas/configuration/configuration.json
( 85%) Resolving: self/v1/schemas/configuration/contents.json
( 90%) Resolving: self/v1/schemas/configuration/extends.json
( 95%) Resolving: self/v1/schemas/configuration/page.json
(100%) Resolving: self/v1/schemas/configuration/path.json
(  0%) Producing: configuration.json
(  0%) Producing: version.json
(  1%) Producing: explorer/%/404.metapack
(  1%) Producing: schemas/example/schemas/test/%/schema.metapack
(  1%) Producing: schemas/self/v1/schemas/api/error/%/schema.metapack
(  2%) Producing: schemas/self/v1/schemas/api/list/response/%/schema.metapack
(  2%) Producing: schemas/self/v1/schemas/api/schemas/dependencies/response/%/schema.metapack
(  2%) Producing: schemas/self/v1/schemas/api/schemas/dependents/response/%/schema.metapack
(  3%) Producing: schemas/self/v1/schemas/api/schemas/evaluate/response/%/schema.metapack
(  3%) Producing: schemas/self/v1/schemas/api/schemas/health/response/%/schema.metapack
(  3%) Producing: schemas/self/v1/schemas/api/schemas/locations/response/%/schema.metapack
(  4%) Producing: schemas/self/v1/schemas/api/schemas/metadata/response/%/schema.metapack
(  4%) Producing: schemas/self/v1/schemas/api/schemas/position/%/schema.metapack
(  4%) Producing: schemas/self/v1/schemas/api/schemas/positions/response/%/schema.metapack
(  5%) Producing: schemas/self/v1/schemas/api/schemas/search/response/%/schema.metapack
(  5%) Producing: schemas/self/v1/schemas/api/schemas/stats/response/%/schema.metapack
(  5%) Producing: schemas/self/v1/schemas/api/schemas/trace/response/%/schema.metapack
(  6%) Producing: schemas/self/v1/schemas/configuration/collection/%/schema.metapack
(  6%) Producing: schemas/self/v1/schemas/configuration/configuration/%/schema.metapack
(  7%) Producing: schemas/self/v1/schemas/configuration/contents/%/schema.metapack
(  7%) Producing: schemas/self/v1/schemas/configuration/extends/%/schema.metapack
(  7%) Producing: schemas/self/v1/schemas/configuration/page/%/schema.metapack
(  8%) Producing: schemas/self/v1/schemas/configuration/path/%/schema.metapack
(  8%) Producing: schemas/example/schemas/test/%/dependencies.metapack
error: Could not resolve schema reference
  to identifier https://sourcemeta.com/example/schemas/test#foo
  at path $(realpath "$TMP")/schemas/test.json
  at location "/allOf/0/\$ref"
EOF

diff "$TMP/output.txt" "$TMP/expected.txt"
