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
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/api/list/rpc/request.json (#4)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/api/schemas/dependencies/response.json (#5)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/api/schemas/dependencies/rpc/request.json (#6)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/api/schemas/dependents/response.json (#7)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/api/schemas/dependents/rpc/request.json (#8)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/api/schemas/evaluate/request.json (#9)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/api/schemas/evaluate/response.json (#10)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/api/schemas/evaluate/rpc/request.json (#11)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/api/schemas/health/response.json (#12)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/api/schemas/health/rpc/request.json (#13)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/api/schemas/locations/response.json (#14)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/api/schemas/locations/rpc/request.json (#15)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/api/schemas/metadata/response.json (#16)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/api/schemas/metadata/rpc/request.json (#17)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/api/schemas/position.json (#18)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/api/schemas/positions/response.json (#19)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/api/schemas/positions/rpc/request.json (#20)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/api/schemas/search/response.json (#21)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/api/schemas/search/rpc/request.json (#22)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/api/schemas/stats/response.json (#23)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/api/schemas/stats/rpc/request.json (#24)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/api/schemas/trace/request.json (#25)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/api/schemas/trace/response.json (#26)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/api/schemas/trace/rpc/request.json (#27)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/error.json (#28)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/initialize/request.json (#29)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/initialize/response.json (#30)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/notifications/cancelled.json (#31)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/notifications/initialized.json (#32)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/ping/request.json (#33)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/ping/response.json (#34)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/request.json (#35)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/resources/list/request.json (#36)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/resources/list/response.json (#37)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/resources/read/request.json (#38)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/resources/read/response.json (#39)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/resources/templates/list/request.json (#40)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/resources/templates/list/response.json (#41)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/response.json (#42)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/tools/call/request.json (#43)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/tools/call/response.json (#44)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/tools/list/request.json (#45)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/tools/list/response.json (#46)
(  2%) Resolving: example/schemas/test.json
(  4%) Resolving: self/v1/schemas/api/error.json
(  6%) Resolving: self/v1/schemas/api/list/response.json
(  8%) Resolving: self/v1/schemas/api/list/rpc/request.json
( 10%) Resolving: self/v1/schemas/api/schemas/dependencies/response.json
( 13%) Resolving: self/v1/schemas/api/schemas/dependencies/rpc/request.json
( 15%) Resolving: self/v1/schemas/api/schemas/dependents/response.json
( 17%) Resolving: self/v1/schemas/api/schemas/dependents/rpc/request.json
( 19%) Resolving: self/v1/schemas/api/schemas/evaluate/request.json
( 21%) Resolving: self/v1/schemas/api/schemas/evaluate/response.json
( 23%) Resolving: self/v1/schemas/api/schemas/evaluate/rpc/request.json
( 26%) Resolving: self/v1/schemas/api/schemas/health/response.json
( 28%) Resolving: self/v1/schemas/api/schemas/health/rpc/request.json
( 30%) Resolving: self/v1/schemas/api/schemas/locations/response.json
( 32%) Resolving: self/v1/schemas/api/schemas/locations/rpc/request.json
( 34%) Resolving: self/v1/schemas/api/schemas/metadata/response.json
( 36%) Resolving: self/v1/schemas/api/schemas/metadata/rpc/request.json
( 39%) Resolving: self/v1/schemas/api/schemas/position.json
( 41%) Resolving: self/v1/schemas/api/schemas/positions/response.json
( 43%) Resolving: self/v1/schemas/api/schemas/positions/rpc/request.json
( 45%) Resolving: self/v1/schemas/api/schemas/search/response.json
( 47%) Resolving: self/v1/schemas/api/schemas/search/rpc/request.json
( 50%) Resolving: self/v1/schemas/api/schemas/stats/response.json
( 52%) Resolving: self/v1/schemas/api/schemas/stats/rpc/request.json
( 54%) Resolving: self/v1/schemas/api/schemas/trace/request.json
( 56%) Resolving: self/v1/schemas/api/schemas/trace/response.json
( 58%) Resolving: self/v1/schemas/api/schemas/trace/rpc/request.json
( 60%) Resolving: self/v1/schemas/mcp/error.json
( 63%) Resolving: self/v1/schemas/mcp/initialize/request.json
( 65%) Resolving: self/v1/schemas/mcp/initialize/response.json
( 67%) Resolving: self/v1/schemas/mcp/notifications/cancelled.json
( 69%) Resolving: self/v1/schemas/mcp/notifications/initialized.json
( 71%) Resolving: self/v1/schemas/mcp/ping/request.json
( 73%) Resolving: self/v1/schemas/mcp/ping/response.json
( 76%) Resolving: self/v1/schemas/mcp/request.json
( 78%) Resolving: self/v1/schemas/mcp/resources/list/request.json
( 80%) Resolving: self/v1/schemas/mcp/resources/list/response.json
( 82%) Resolving: self/v1/schemas/mcp/resources/read/request.json
( 84%) Resolving: self/v1/schemas/mcp/resources/read/response.json
( 86%) Resolving: self/v1/schemas/mcp/resources/templates/list/request.json
( 89%) Resolving: self/v1/schemas/mcp/resources/templates/list/response.json
( 91%) Resolving: self/v1/schemas/mcp/response.json
( 93%) Resolving: self/v1/schemas/mcp/tools/call/request.json
( 95%) Resolving: self/v1/schemas/mcp/tools/call/response.json
( 97%) Resolving: self/v1/schemas/mcp/tools/list/request.json
(100%) Resolving: self/v1/schemas/mcp/tools/list/response.json
(  0%) Producing: configuration.json
(  0%) Producing: version.json
(  0%) Producing: routes.bin
(  0%) Producing: explorer/%/404.metapack
(  0%) Producing: schemas/example/schemas/test/%/schema.metapack
(  0%) Producing: schemas/self/v1/schemas/api/error/%/schema.metapack
(  1%) Producing: schemas/self/v1/schemas/api/list/response/%/schema.metapack
(  1%) Producing: schemas/self/v1/schemas/api/list/rpc/request/%/schema.metapack
(  1%) Producing: schemas/self/v1/schemas/api/schemas/dependencies/response/%/schema.metapack
(  1%) Producing: schemas/self/v1/schemas/api/schemas/dependencies/rpc/request/%/schema.metapack
(  1%) Producing: schemas/self/v1/schemas/api/schemas/dependents/response/%/schema.metapack
(  1%) Producing: schemas/self/v1/schemas/api/schemas/dependents/rpc/request/%/schema.metapack
(  2%) Producing: schemas/self/v1/schemas/api/schemas/evaluate/request/%/schema.metapack
(  2%) Producing: schemas/self/v1/schemas/api/schemas/evaluate/response/%/schema.metapack
(  2%) Producing: schemas/self/v1/schemas/api/schemas/evaluate/rpc/request/%/schema.metapack
(  2%) Producing: schemas/self/v1/schemas/api/schemas/health/response/%/schema.metapack
(  2%) Producing: schemas/self/v1/schemas/api/schemas/health/rpc/request/%/schema.metapack
(  2%) Producing: schemas/self/v1/schemas/api/schemas/locations/response/%/schema.metapack
(  2%) Producing: schemas/self/v1/schemas/api/schemas/locations/rpc/request/%/schema.metapack
(  3%) Producing: schemas/self/v1/schemas/api/schemas/metadata/response/%/schema.metapack
(  3%) Producing: schemas/self/v1/schemas/api/schemas/metadata/rpc/request/%/schema.metapack
(  3%) Producing: schemas/self/v1/schemas/api/schemas/position/%/schema.metapack
(  3%) Producing: schemas/self/v1/schemas/api/schemas/positions/response/%/schema.metapack
(  3%) Producing: schemas/self/v1/schemas/api/schemas/positions/rpc/request/%/schema.metapack
(  3%) Producing: schemas/self/v1/schemas/api/schemas/search/response/%/schema.metapack
(  4%) Producing: schemas/self/v1/schemas/api/schemas/search/rpc/request/%/schema.metapack
(  4%) Producing: schemas/self/v1/schemas/api/schemas/stats/response/%/schema.metapack
(  4%) Producing: schemas/self/v1/schemas/api/schemas/stats/rpc/request/%/schema.metapack
(  4%) Producing: schemas/self/v1/schemas/api/schemas/trace/request/%/schema.metapack
(  4%) Producing: schemas/self/v1/schemas/api/schemas/trace/response/%/schema.metapack
(  4%) Producing: schemas/self/v1/schemas/api/schemas/trace/rpc/request/%/schema.metapack
(  4%) Producing: schemas/self/v1/schemas/mcp/error/%/schema.metapack
(  5%) Producing: schemas/self/v1/schemas/mcp/initialize/request/%/schema.metapack
(  5%) Producing: schemas/self/v1/schemas/mcp/initialize/response/%/schema.metapack
(  5%) Producing: schemas/self/v1/schemas/mcp/notifications/cancelled/%/schema.metapack
(  5%) Producing: schemas/self/v1/schemas/mcp/notifications/initialized/%/schema.metapack
(  5%) Producing: schemas/self/v1/schemas/mcp/ping/request/%/schema.metapack
(  5%) Producing: schemas/self/v1/schemas/mcp/ping/response/%/schema.metapack
(  6%) Producing: schemas/self/v1/schemas/mcp/request/%/schema.metapack
(  6%) Producing: schemas/self/v1/schemas/mcp/resources/list/request/%/schema.metapack
(  6%) Producing: schemas/self/v1/schemas/mcp/resources/list/response/%/schema.metapack
(  6%) Producing: schemas/self/v1/schemas/mcp/resources/read/request/%/schema.metapack
(  6%) Producing: schemas/self/v1/schemas/mcp/resources/read/response/%/schema.metapack
(  6%) Producing: schemas/self/v1/schemas/mcp/resources/templates/list/request/%/schema.metapack
(  7%) Producing: schemas/self/v1/schemas/mcp/resources/templates/list/response/%/schema.metapack
(  7%) Producing: schemas/self/v1/schemas/mcp/response/%/schema.metapack
(  7%) Producing: schemas/self/v1/schemas/mcp/tools/call/request/%/schema.metapack
(  7%) Producing: schemas/self/v1/schemas/mcp/tools/call/response/%/schema.metapack
(  7%) Producing: schemas/self/v1/schemas/mcp/tools/list/request/%/schema.metapack
(  7%) Producing: schemas/self/v1/schemas/mcp/tools/list/response/%/schema.metapack
(  7%) Producing: schemas/example/schemas/test/%/dependencies.metapack
error: Could not resolve the reference to an external schema
  at identifier https://sourcemeta.com/external
  at path $(realpath "$TMP")/schemas/test.json

Did you forget to register a schema with such URI?
EOF

diff "$TMP/output.txt" "$TMP/expected.txt"
