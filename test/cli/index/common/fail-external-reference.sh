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
  "$id": "https://example.com/test",
  "allOf": [ { "$ref": "https://sourcemeta.com/external" } ]
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
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/error.json (#15)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/initialize/request.json (#16)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/initialize/response.json (#17)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/notifications/cancelled.json (#18)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/notifications/initialized.json (#19)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/ping/request.json (#20)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/ping/response.json (#21)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/request.json (#22)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/resources/list/request.json (#23)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/resources/list/response.json (#24)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/resources/read/request.json (#25)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/resources/read/response.json (#26)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/resources/templates/list/request.json (#27)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/resources/templates/list/response.json (#28)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/response.json (#29)
(  3%) Resolving: example/schemas/test.json
(  6%) Resolving: self/v1/schemas/api/error.json
( 10%) Resolving: self/v1/schemas/api/list/response.json
( 13%) Resolving: self/v1/schemas/api/schemas/dependencies/response.json
( 17%) Resolving: self/v1/schemas/api/schemas/dependents/response.json
( 20%) Resolving: self/v1/schemas/api/schemas/evaluate/response.json
( 24%) Resolving: self/v1/schemas/api/schemas/health/response.json
( 27%) Resolving: self/v1/schemas/api/schemas/locations/response.json
( 31%) Resolving: self/v1/schemas/api/schemas/metadata/response.json
( 34%) Resolving: self/v1/schemas/api/schemas/position.json
( 37%) Resolving: self/v1/schemas/api/schemas/positions/response.json
( 41%) Resolving: self/v1/schemas/api/schemas/search/response.json
( 44%) Resolving: self/v1/schemas/api/schemas/stats/response.json
( 48%) Resolving: self/v1/schemas/api/schemas/trace/response.json
( 51%) Resolving: self/v1/schemas/mcp/error.json
( 55%) Resolving: self/v1/schemas/mcp/initialize/request.json
( 58%) Resolving: self/v1/schemas/mcp/initialize/response.json
( 62%) Resolving: self/v1/schemas/mcp/notifications/cancelled.json
( 65%) Resolving: self/v1/schemas/mcp/notifications/initialized.json
( 68%) Resolving: self/v1/schemas/mcp/ping/request.json
( 72%) Resolving: self/v1/schemas/mcp/ping/response.json
( 75%) Resolving: self/v1/schemas/mcp/request.json
( 79%) Resolving: self/v1/schemas/mcp/resources/list/request.json
( 82%) Resolving: self/v1/schemas/mcp/resources/list/response.json
( 86%) Resolving: self/v1/schemas/mcp/resources/read/request.json
( 89%) Resolving: self/v1/schemas/mcp/resources/read/response.json
( 93%) Resolving: self/v1/schemas/mcp/resources/templates/list/request.json
( 96%) Resolving: self/v1/schemas/mcp/resources/templates/list/response.json
(100%) Resolving: self/v1/schemas/mcp/response.json
(  0%) Producing: configuration.json
(  0%) Producing: metadata.json
(  0%) Producing: version.json
(  0%) Producing: explorer/%/404.metapack
(  1%) Producing: schemas/example/schemas/test/%/schema.metapack
(  1%) Producing: schemas/self/v1/schemas/api/error/%/schema.metapack
(  1%) Producing: schemas/self/v1/schemas/api/list/response/%/schema.metapack
(  1%) Producing: schemas/self/v1/schemas/api/schemas/dependencies/response/%/schema.metapack
(  2%) Producing: schemas/self/v1/schemas/api/schemas/dependents/response/%/schema.metapack
(  2%) Producing: schemas/self/v1/schemas/api/schemas/evaluate/response/%/schema.metapack
(  2%) Producing: schemas/self/v1/schemas/api/schemas/health/response/%/schema.metapack
(  2%) Producing: schemas/self/v1/schemas/api/schemas/locations/response/%/schema.metapack
(  3%) Producing: schemas/self/v1/schemas/api/schemas/metadata/response/%/schema.metapack
(  3%) Producing: schemas/self/v1/schemas/api/schemas/position/%/schema.metapack
(  3%) Producing: schemas/self/v1/schemas/api/schemas/positions/response/%/schema.metapack
(  3%) Producing: schemas/self/v1/schemas/api/schemas/search/response/%/schema.metapack
(  4%) Producing: schemas/self/v1/schemas/api/schemas/stats/response/%/schema.metapack
(  4%) Producing: schemas/self/v1/schemas/api/schemas/trace/response/%/schema.metapack
(  4%) Producing: schemas/self/v1/schemas/mcp/error/%/schema.metapack
(  4%) Producing: schemas/self/v1/schemas/mcp/initialize/request/%/schema.metapack
(  5%) Producing: schemas/self/v1/schemas/mcp/initialize/response/%/schema.metapack
(  5%) Producing: schemas/self/v1/schemas/mcp/notifications/cancelled/%/schema.metapack
(  5%) Producing: schemas/self/v1/schemas/mcp/notifications/initialized/%/schema.metapack
(  5%) Producing: schemas/self/v1/schemas/mcp/ping/request/%/schema.metapack
(  6%) Producing: schemas/self/v1/schemas/mcp/ping/response/%/schema.metapack
(  6%) Producing: schemas/self/v1/schemas/mcp/request/%/schema.metapack
(  6%) Producing: schemas/self/v1/schemas/mcp/resources/list/request/%/schema.metapack
(  6%) Producing: schemas/self/v1/schemas/mcp/resources/list/response/%/schema.metapack
(  7%) Producing: schemas/self/v1/schemas/mcp/resources/read/request/%/schema.metapack
(  7%) Producing: schemas/self/v1/schemas/mcp/resources/read/response/%/schema.metapack
(  7%) Producing: schemas/self/v1/schemas/mcp/resources/templates/list/request/%/schema.metapack
(  7%) Producing: schemas/self/v1/schemas/mcp/resources/templates/list/response/%/schema.metapack
(  8%) Producing: schemas/self/v1/schemas/mcp/response/%/schema.metapack
(  8%) Producing: schemas/example/schemas/test/%/dependencies.metapack
error: Could not resolve the reference to an external schema
  at identifier https://sourcemeta.com/external
  at path $(realpath "$TMP")/schemas/test.json

Did you forget to register a schema with such URI?
EOF

diff "$TMP/output.txt" "$TMP/expected.txt"
