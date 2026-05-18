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
  "$id": "https://example.com/other/foo",
  "type": "string"
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
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/api/list/rpc.json (#3)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/api/schemas/dependencies/response.json (#4)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/api/schemas/dependencies/rpc.json (#5)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/api/schemas/dependents/response.json (#6)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/api/schemas/dependents/rpc.json (#7)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/api/schemas/evaluate/request.json (#8)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/api/schemas/evaluate/response.json (#9)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/api/schemas/evaluate/rpc.json (#10)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/api/schemas/health/response.json (#11)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/api/schemas/health/rpc.json (#12)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/api/schemas/locations/response.json (#13)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/api/schemas/locations/rpc.json (#14)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/api/schemas/metadata/response.json (#15)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/api/schemas/metadata/rpc.json (#16)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/api/schemas/position.json (#17)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/api/schemas/positions/response.json (#18)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/api/schemas/positions/rpc.json (#19)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/api/schemas/search/response.json (#20)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/api/schemas/search/rpc.json (#21)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/api/schemas/stats/response.json (#22)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/api/schemas/stats/rpc.json (#23)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/api/schemas/trace/request.json (#24)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/api/schemas/trace/response.json (#25)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/api/schemas/trace/rpc.json (#26)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/error.json (#27)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/initialize/request.json (#28)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/initialize/response.json (#29)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/notifications/cancelled.json (#30)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/notifications/initialized.json (#31)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/ping/request.json (#32)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/ping/response.json (#33)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/request.json (#34)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/resources/list/request.json (#35)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/resources/list/response.json (#36)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/resources/read/request.json (#37)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/resources/read/response.json (#38)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/resources/templates/list/request.json (#39)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/resources/templates/list/response.json (#40)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/response.json (#41)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/tools/call/request.json (#42)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/tools/call/response.json (#43)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/tools/list/request.json (#44)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/tools/list/response.json (#45)
Detecting: $(realpath "$TMP")/schemas/foo.json (#46)
(  2%) Resolving: self/v1/schemas/api/error.json
(  4%) Resolving: self/v1/schemas/api/list/response.json
(  6%) Resolving: self/v1/schemas/api/list/rpc.json
(  8%) Resolving: self/v1/schemas/api/schemas/dependencies/response.json
( 10%) Resolving: self/v1/schemas/api/schemas/dependencies/rpc.json
( 13%) Resolving: self/v1/schemas/api/schemas/dependents/response.json
( 15%) Resolving: self/v1/schemas/api/schemas/dependents/rpc.json
( 17%) Resolving: self/v1/schemas/api/schemas/evaluate/request.json
( 19%) Resolving: self/v1/schemas/api/schemas/evaluate/response.json
( 21%) Resolving: self/v1/schemas/api/schemas/evaluate/rpc.json
( 23%) Resolving: self/v1/schemas/api/schemas/health/response.json
( 26%) Resolving: self/v1/schemas/api/schemas/health/rpc.json
( 28%) Resolving: self/v1/schemas/api/schemas/locations/response.json
( 30%) Resolving: self/v1/schemas/api/schemas/locations/rpc.json
( 32%) Resolving: self/v1/schemas/api/schemas/metadata/response.json
( 34%) Resolving: self/v1/schemas/api/schemas/metadata/rpc.json
( 36%) Resolving: self/v1/schemas/api/schemas/position.json
( 39%) Resolving: self/v1/schemas/api/schemas/positions/response.json
( 41%) Resolving: self/v1/schemas/api/schemas/positions/rpc.json
( 43%) Resolving: self/v1/schemas/api/schemas/search/response.json
( 45%) Resolving: self/v1/schemas/api/schemas/search/rpc.json
( 47%) Resolving: self/v1/schemas/api/schemas/stats/response.json
( 50%) Resolving: self/v1/schemas/api/schemas/stats/rpc.json
( 52%) Resolving: self/v1/schemas/api/schemas/trace/request.json
( 54%) Resolving: self/v1/schemas/api/schemas/trace/response.json
( 56%) Resolving: self/v1/schemas/api/schemas/trace/rpc.json
( 58%) Resolving: self/v1/schemas/mcp/error.json
( 60%) Resolving: self/v1/schemas/mcp/initialize/request.json
( 63%) Resolving: self/v1/schemas/mcp/initialize/response.json
( 65%) Resolving: self/v1/schemas/mcp/notifications/cancelled.json
( 67%) Resolving: self/v1/schemas/mcp/notifications/initialized.json
( 69%) Resolving: self/v1/schemas/mcp/ping/request.json
( 71%) Resolving: self/v1/schemas/mcp/ping/response.json
( 73%) Resolving: self/v1/schemas/mcp/request.json
( 76%) Resolving: self/v1/schemas/mcp/resources/list/request.json
( 78%) Resolving: self/v1/schemas/mcp/resources/list/response.json
( 80%) Resolving: self/v1/schemas/mcp/resources/read/request.json
( 82%) Resolving: self/v1/schemas/mcp/resources/read/response.json
( 84%) Resolving: self/v1/schemas/mcp/resources/templates/list/request.json
( 86%) Resolving: self/v1/schemas/mcp/resources/templates/list/response.json
( 89%) Resolving: self/v1/schemas/mcp/response.json
( 91%) Resolving: self/v1/schemas/mcp/tools/call/request.json
( 93%) Resolving: self/v1/schemas/mcp/tools/call/response.json
( 95%) Resolving: self/v1/schemas/mcp/tools/list/request.json
( 97%) Resolving: self/v1/schemas/mcp/tools/list/response.json
(100%) Resolving: test/foo.json
error: The schema identifier is not relative to the corresponding base
  at path $(realpath "$TMP")/schemas/foo.json
  at identifier https://example.com/other/foo
  with base https://example.com/schemas
EOF

diff "$TMP/output.txt" "$TMP/expected.txt"
