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
  "$schema": "https://example.com/my-metaschema",
  "$id": "https://example.com/test"
}
EOF

cat << 'EOF' > "$TMP/schemas/my-metaschema.json"
{
  "$schema": "https://example.com/my-metaschema",
  "$id": "https://example.com/my-metaschema"
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

cat << EOF > "$TMP/expected1.txt"
Writing output to: $(realpath "$TMP")/output
Using configuration: $(realpath "$TMP")/one.json
Detecting: $(realpath "$TMP")/schemas/my-metaschema.json (#1)
Detecting: $(realpath "$TMP")/schemas/test.json (#2)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/api/error.json (#3)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/api/list/response.json (#4)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/api/list/rpc/request.json (#5)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/api/list/rpc/response.json (#6)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/api/schemas/dependencies/response.json (#7)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/api/schemas/dependencies/rpc/request.json (#8)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/api/schemas/dependencies/rpc/response.json (#9)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/api/schemas/dependents/response.json (#10)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/api/schemas/dependents/rpc/request.json (#11)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/api/schemas/dependents/rpc/response.json (#12)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/api/schemas/evaluate/request.json (#13)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/api/schemas/evaluate/response.json (#14)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/api/schemas/evaluate/rpc/request.json (#15)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/api/schemas/evaluate/rpc/response.json (#16)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/api/schemas/health/response.json (#17)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/api/schemas/health/rpc/request.json (#18)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/api/schemas/health/rpc/response.json (#19)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/api/schemas/locations/response.json (#20)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/api/schemas/locations/rpc/request.json (#21)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/api/schemas/locations/rpc/response.json (#22)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/api/schemas/metadata/response.json (#23)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/api/schemas/metadata/rpc/request.json (#24)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/api/schemas/metadata/rpc/response.json (#25)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/api/schemas/position.json (#26)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/api/schemas/positions/response.json (#27)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/api/schemas/positions/rpc/request.json (#28)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/api/schemas/positions/rpc/response.json (#29)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/api/schemas/search/response.json (#30)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/api/schemas/search/rpc/request.json (#31)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/api/schemas/search/rpc/response.json (#32)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/api/schemas/stats/response.json (#33)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/api/schemas/stats/rpc/request.json (#34)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/api/schemas/stats/rpc/response.json (#35)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/api/schemas/trace/request.json (#36)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/api/schemas/trace/response.json (#37)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/api/schemas/trace/rpc/request.json (#38)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/api/schemas/trace/rpc/response.json (#39)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/error.json (#40)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/initialize/request.json (#41)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/initialize/response.json (#42)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/notifications/cancelled.json (#43)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/notifications/initialized.json (#44)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/ping/request.json (#45)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/ping/response.json (#46)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/request.json (#47)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/resources/list/request.json (#48)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/resources/list/response.json (#49)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/resources/read/request.json (#50)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/resources/read/response.json (#51)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/resources/templates/list/request.json (#52)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/resources/templates/list/response.json (#53)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/response.json (#54)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/tools/call/request.json (#55)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/tools/call/response.json (#56)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/tools/list/request.json (#57)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/tools/list/response.json (#58)
(  1%) Resolving: example/schemas/my-metaschema.json
(  3%) Resolving: example/schemas/test.json
(  5%) Resolving: self/v1/schemas/api/error.json
(  6%) Resolving: self/v1/schemas/api/list/response.json
(  8%) Resolving: self/v1/schemas/api/list/rpc/request.json
( 10%) Resolving: self/v1/schemas/api/list/rpc/response.json
( 12%) Resolving: self/v1/schemas/api/schemas/dependencies/response.json
( 13%) Resolving: self/v1/schemas/api/schemas/dependencies/rpc/request.json
( 15%) Resolving: self/v1/schemas/api/schemas/dependencies/rpc/response.json
( 17%) Resolving: self/v1/schemas/api/schemas/dependents/response.json
( 18%) Resolving: self/v1/schemas/api/schemas/dependents/rpc/request.json
( 20%) Resolving: self/v1/schemas/api/schemas/dependents/rpc/response.json
( 22%) Resolving: self/v1/schemas/api/schemas/evaluate/request.json
( 24%) Resolving: self/v1/schemas/api/schemas/evaluate/response.json
( 25%) Resolving: self/v1/schemas/api/schemas/evaluate/rpc/request.json
( 27%) Resolving: self/v1/schemas/api/schemas/evaluate/rpc/response.json
( 29%) Resolving: self/v1/schemas/api/schemas/health/response.json
( 31%) Resolving: self/v1/schemas/api/schemas/health/rpc/request.json
( 32%) Resolving: self/v1/schemas/api/schemas/health/rpc/response.json
( 34%) Resolving: self/v1/schemas/api/schemas/locations/response.json
( 36%) Resolving: self/v1/schemas/api/schemas/locations/rpc/request.json
( 37%) Resolving: self/v1/schemas/api/schemas/locations/rpc/response.json
( 39%) Resolving: self/v1/schemas/api/schemas/metadata/response.json
( 41%) Resolving: self/v1/schemas/api/schemas/metadata/rpc/request.json
( 43%) Resolving: self/v1/schemas/api/schemas/metadata/rpc/response.json
( 44%) Resolving: self/v1/schemas/api/schemas/position.json
( 46%) Resolving: self/v1/schemas/api/schemas/positions/response.json
( 48%) Resolving: self/v1/schemas/api/schemas/positions/rpc/request.json
( 50%) Resolving: self/v1/schemas/api/schemas/positions/rpc/response.json
( 51%) Resolving: self/v1/schemas/api/schemas/search/response.json
( 53%) Resolving: self/v1/schemas/api/schemas/search/rpc/request.json
( 55%) Resolving: self/v1/schemas/api/schemas/search/rpc/response.json
( 56%) Resolving: self/v1/schemas/api/schemas/stats/response.json
( 58%) Resolving: self/v1/schemas/api/schemas/stats/rpc/request.json
( 60%) Resolving: self/v1/schemas/api/schemas/stats/rpc/response.json
( 62%) Resolving: self/v1/schemas/api/schemas/trace/request.json
( 63%) Resolving: self/v1/schemas/api/schemas/trace/response.json
( 65%) Resolving: self/v1/schemas/api/schemas/trace/rpc/request.json
( 67%) Resolving: self/v1/schemas/api/schemas/trace/rpc/response.json
( 68%) Resolving: self/v1/schemas/mcp/error.json
( 70%) Resolving: self/v1/schemas/mcp/initialize/request.json
( 72%) Resolving: self/v1/schemas/mcp/initialize/response.json
( 74%) Resolving: self/v1/schemas/mcp/notifications/cancelled.json
( 75%) Resolving: self/v1/schemas/mcp/notifications/initialized.json
( 77%) Resolving: self/v1/schemas/mcp/ping/request.json
( 79%) Resolving: self/v1/schemas/mcp/ping/response.json
( 81%) Resolving: self/v1/schemas/mcp/request.json
( 82%) Resolving: self/v1/schemas/mcp/resources/list/request.json
( 84%) Resolving: self/v1/schemas/mcp/resources/list/response.json
( 86%) Resolving: self/v1/schemas/mcp/resources/read/request.json
( 87%) Resolving: self/v1/schemas/mcp/resources/read/response.json
( 89%) Resolving: self/v1/schemas/mcp/resources/templates/list/request.json
( 91%) Resolving: self/v1/schemas/mcp/resources/templates/list/response.json
( 93%) Resolving: self/v1/schemas/mcp/response.json
( 94%) Resolving: self/v1/schemas/mcp/tools/call/request.json
( 96%) Resolving: self/v1/schemas/mcp/tools/call/response.json
( 98%) Resolving: self/v1/schemas/mcp/tools/list/request.json
(100%) Resolving: self/v1/schemas/mcp/tools/list/response.json
(  0%) Producing: configuration.json
(  0%) Producing: version.json
(  0%) Producing: routes.bin
(  0%) Producing: explorer/%/404.metapack
(  0%) Producing: schemas/example/schemas/my-metaschema/%/schema.metapack
error: Could not resolve the metaschema of the schema
  at identifier https://sourcemeta.com/example/schemas/my-metaschema
  at path $(realpath "$TMP")/schemas/my-metaschema.json

Did you forget to register a schema with such URI?
EOF

diff "$TMP/output.txt" "$TMP/expected1.txt"
