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
    "test": {
      "path": "./schemas"
    }
  }
}
EOF

mkdir "$TMP/schemas"

cat << 'EOF' > "$TMP/schemas/foo.json"
{
  "$schema": "https://json-schema.org/draft/2020-12/schema",
  "$id": "https://example.com/schemas/foo",
  "allOf": [ { "$ref": "https://other.example.com/external" } ]
}
EOF

remove_threads_information() {
  expr='s/ \[[^]]*[^a-z-][^]]*\]//g'
  if [ "$(uname -s)" = "Darwin" ]; then
    sed -i '' "$expr" "$1"
  else
    sed -i "$expr" "$1"
  fi
}

"$1" --skip-banner --deterministic "$TMP/one.json" "$TMP/output" --concurrency 1 2> "$TMP/output.txt" && CODE="$?" || CODE="$?"
test "$CODE" = "1" || exit 1
remove_threads_information "$TMP/output.txt"

cat << EOF > "$TMP/expected.txt"
Writing output to: $(realpath "$TMP")/output
Using configuration: $(realpath "$TMP")/one.json
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/api/error.json (#1)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/api/list/response.json (#2)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/api/schemas/dependencies/response.json (#3)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/api/schemas/dependents/response.json (#4)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/api/schemas/evaluate/response.json (#5)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/api/schemas/health/response.json (#6)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/api/schemas/locations/response.json (#7)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/api/schemas/metadata/response.json (#8)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/api/schemas/position.json (#9)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/api/schemas/positions/response.json (#10)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/api/schemas/search/response.json (#11)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/api/schemas/stats/response.json (#12)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/api/schemas/trace/response.json (#13)
Detecting: $(realpath "$TMP")/schemas/foo.json (#14)
(  7%) Resolving: self/v1/schemas/api/error.json
( 14%) Resolving: self/v1/schemas/api/list/response.json
( 21%) Resolving: self/v1/schemas/api/schemas/dependencies/response.json
( 28%) Resolving: self/v1/schemas/api/schemas/dependents/response.json
( 35%) Resolving: self/v1/schemas/api/schemas/evaluate/response.json
( 42%) Resolving: self/v1/schemas/api/schemas/health/response.json
( 50%) Resolving: self/v1/schemas/api/schemas/locations/response.json
( 57%) Resolving: self/v1/schemas/api/schemas/metadata/response.json
( 64%) Resolving: self/v1/schemas/api/schemas/position.json
( 71%) Resolving: self/v1/schemas/api/schemas/positions/response.json
( 78%) Resolving: self/v1/schemas/api/schemas/search/response.json
( 85%) Resolving: self/v1/schemas/api/schemas/stats/response.json
( 92%) Resolving: self/v1/schemas/api/schemas/trace/response.json
(100%) Resolving: test/foo.json
(  0%) Producing: configuration.json
(  0%) Producing: version.json
(  1%) Producing: explorer/%/404.metapack
(  1%) Producing: schemas/self/v1/schemas/api/error/%/schema.metapack
(  2%) Producing: schemas/self/v1/schemas/api/list/response/%/schema.metapack
(  2%) Producing: schemas/self/v1/schemas/api/schemas/dependencies/response/%/schema.metapack
(  3%) Producing: schemas/self/v1/schemas/api/schemas/dependents/response/%/schema.metapack
(  3%) Producing: schemas/self/v1/schemas/api/schemas/evaluate/response/%/schema.metapack
(  4%) Producing: schemas/self/v1/schemas/api/schemas/health/response/%/schema.metapack
(  4%) Producing: schemas/self/v1/schemas/api/schemas/locations/response/%/schema.metapack
(  5%) Producing: schemas/self/v1/schemas/api/schemas/metadata/response/%/schema.metapack
(  5%) Producing: schemas/self/v1/schemas/api/schemas/position/%/schema.metapack
(  6%) Producing: schemas/self/v1/schemas/api/schemas/positions/response/%/schema.metapack
(  6%) Producing: schemas/self/v1/schemas/api/schemas/search/response/%/schema.metapack
(  7%) Producing: schemas/self/v1/schemas/api/schemas/stats/response/%/schema.metapack
(  7%) Producing: schemas/self/v1/schemas/api/schemas/trace/response/%/schema.metapack
(  8%) Producing: schemas/test/foo/%/schema.metapack
(  8%) Producing: schemas/self/v1/schemas/api/error/%/dependencies.metapack
(  9%) Producing: schemas/self/v1/schemas/api/error/%/locations.metapack
(  9%) Producing: schemas/self/v1/schemas/api/error/%/positions.metapack
( 10%) Producing: schemas/self/v1/schemas/api/error/%/stats.metapack
( 10%) Producing: schemas/self/v1/schemas/api/list/response/%/dependencies.metapack
( 11%) Producing: schemas/self/v1/schemas/api/list/response/%/locations.metapack
( 11%) Producing: schemas/self/v1/schemas/api/list/response/%/positions.metapack
( 11%) Producing: schemas/self/v1/schemas/api/list/response/%/stats.metapack
( 12%) Producing: schemas/self/v1/schemas/api/schemas/dependencies/response/%/dependencies.metapack
( 12%) Producing: schemas/self/v1/schemas/api/schemas/dependencies/response/%/locations.metapack
( 13%) Producing: schemas/self/v1/schemas/api/schemas/dependencies/response/%/positions.metapack
( 13%) Producing: schemas/self/v1/schemas/api/schemas/dependencies/response/%/stats.metapack
( 14%) Producing: schemas/self/v1/schemas/api/schemas/dependents/response/%/dependencies.metapack
( 14%) Producing: schemas/self/v1/schemas/api/schemas/dependents/response/%/locations.metapack
( 15%) Producing: schemas/self/v1/schemas/api/schemas/dependents/response/%/positions.metapack
( 15%) Producing: schemas/self/v1/schemas/api/schemas/dependents/response/%/stats.metapack
( 16%) Producing: schemas/self/v1/schemas/api/schemas/evaluate/response/%/dependencies.metapack
( 16%) Producing: schemas/self/v1/schemas/api/schemas/evaluate/response/%/locations.metapack
( 17%) Producing: schemas/self/v1/schemas/api/schemas/evaluate/response/%/positions.metapack
( 17%) Producing: schemas/self/v1/schemas/api/schemas/evaluate/response/%/stats.metapack
( 18%) Producing: schemas/self/v1/schemas/api/schemas/health/response/%/dependencies.metapack
( 18%) Producing: schemas/self/v1/schemas/api/schemas/health/response/%/locations.metapack
( 19%) Producing: schemas/self/v1/schemas/api/schemas/health/response/%/positions.metapack
( 19%) Producing: schemas/self/v1/schemas/api/schemas/health/response/%/stats.metapack
( 20%) Producing: schemas/self/v1/schemas/api/schemas/locations/response/%/dependencies.metapack
( 20%) Producing: schemas/self/v1/schemas/api/schemas/locations/response/%/locations.metapack
( 21%) Producing: schemas/self/v1/schemas/api/schemas/locations/response/%/positions.metapack
( 21%) Producing: schemas/self/v1/schemas/api/schemas/locations/response/%/stats.metapack
( 22%) Producing: schemas/self/v1/schemas/api/schemas/metadata/response/%/dependencies.metapack
( 22%) Producing: schemas/self/v1/schemas/api/schemas/metadata/response/%/locations.metapack
( 22%) Producing: schemas/self/v1/schemas/api/schemas/metadata/response/%/positions.metapack
( 23%) Producing: schemas/self/v1/schemas/api/schemas/metadata/response/%/stats.metapack
( 23%) Producing: schemas/self/v1/schemas/api/schemas/position/%/dependencies.metapack
( 24%) Producing: schemas/self/v1/schemas/api/schemas/position/%/locations.metapack
( 24%) Producing: schemas/self/v1/schemas/api/schemas/position/%/positions.metapack
( 25%) Producing: schemas/self/v1/schemas/api/schemas/position/%/stats.metapack
( 25%) Producing: schemas/self/v1/schemas/api/schemas/positions/response/%/dependencies.metapack
( 26%) Producing: schemas/self/v1/schemas/api/schemas/positions/response/%/locations.metapack
( 26%) Producing: schemas/self/v1/schemas/api/schemas/positions/response/%/positions.metapack
( 27%) Producing: schemas/self/v1/schemas/api/schemas/positions/response/%/stats.metapack
( 27%) Producing: schemas/self/v1/schemas/api/schemas/search/response/%/dependencies.metapack
( 28%) Producing: schemas/self/v1/schemas/api/schemas/search/response/%/locations.metapack
( 28%) Producing: schemas/self/v1/schemas/api/schemas/search/response/%/positions.metapack
( 29%) Producing: schemas/self/v1/schemas/api/schemas/search/response/%/stats.metapack
( 29%) Producing: schemas/self/v1/schemas/api/schemas/stats/response/%/dependencies.metapack
( 30%) Producing: schemas/self/v1/schemas/api/schemas/stats/response/%/locations.metapack
( 30%) Producing: schemas/self/v1/schemas/api/schemas/stats/response/%/positions.metapack
( 31%) Producing: schemas/self/v1/schemas/api/schemas/stats/response/%/stats.metapack
( 31%) Producing: schemas/self/v1/schemas/api/schemas/trace/response/%/dependencies.metapack
( 32%) Producing: schemas/self/v1/schemas/api/schemas/trace/response/%/locations.metapack
( 32%) Producing: schemas/self/v1/schemas/api/schemas/trace/response/%/positions.metapack
( 33%) Producing: schemas/self/v1/schemas/api/schemas/trace/response/%/stats.metapack
( 33%) Producing: schemas/test/foo/%/dependencies.metapack
error: Could not resolve the reference to an external schema
  at identifier https://other.example.com/external
  at path $(realpath "$TMP")/schemas/foo.json

Did you forget to register a schema with such URI?
EOF

diff "$TMP/output.txt" "$TMP/expected.txt"
