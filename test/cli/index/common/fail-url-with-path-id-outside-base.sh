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
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/api/schemas/dependencies/response.json (#3)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/api/schemas/dependents/response.json (#4)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/api/schemas/evaluate/request.json (#5)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/api/schemas/evaluate/response.json (#6)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/api/schemas/health/response.json (#7)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/api/schemas/locations/response.json (#8)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/api/schemas/metadata/response.json (#9)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/api/schemas/position.json (#10)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/api/schemas/positions/response.json (#11)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/api/schemas/search/response.json (#12)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/api/schemas/stats/response.json (#13)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/api/schemas/trace/request.json (#14)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/api/schemas/trace/response.json (#15)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/error.json (#16)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/initialize/request.json (#17)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/initialize/response.json (#18)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/notifications/cancelled.json (#19)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/notifications/initialized.json (#20)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/ping/request.json (#21)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/ping/response.json (#22)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/request.json (#23)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/resources/list/request.json (#24)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/resources/list/response.json (#25)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/resources/read/request.json (#26)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/resources/read/response.json (#27)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/resources/templates/list/request.json (#28)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/resources/templates/list/response.json (#29)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/response.json (#30)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/tools/call/evaluate-schema/request.json (#31)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/tools/call/evaluate-schema/response.json (#32)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/tools/call/get-schema-dependencies/request.json (#33)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/tools/call/get-schema-dependencies/response.json (#34)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/tools/call/get-schema-dependents/request.json (#35)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/tools/call/get-schema-dependents/response.json (#36)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/tools/call/get-schema-health/request.json (#37)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/tools/call/get-schema-health/response.json (#38)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/tools/call/get-schema-locations/request.json (#39)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/tools/call/get-schema-locations/response.json (#40)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/tools/call/get-schema-metadata/request.json (#41)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/tools/call/get-schema-metadata/response.json (#42)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/tools/call/get-schema-positions/request.json (#43)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/tools/call/get-schema-positions/response.json (#44)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/tools/call/get-schema-stats/request.json (#45)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/tools/call/get-schema-stats/response.json (#46)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/tools/call/list-directory/request.json (#47)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/tools/call/list-directory/response.json (#48)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/tools/call/request.json (#49)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/tools/call/response.json (#50)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/tools/call/search-schemas/request.json (#51)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/tools/call/search-schemas/response.json (#52)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/tools/call/trace-schema-evaluation/request.json (#53)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/tools/call/trace-schema-evaluation/response.json (#54)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/tools/list/request.json (#55)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/tools/list/response.json (#56)
Detecting: $(realpath "$TMP")/schemas/foo.json (#57)
(  1%) Resolving: self/v1/schemas/api/error.json
(  3%) Resolving: self/v1/schemas/api/list/response.json
(  5%) Resolving: self/v1/schemas/api/schemas/dependencies/response.json
(  7%) Resolving: self/v1/schemas/api/schemas/dependents/response.json
(  8%) Resolving: self/v1/schemas/api/schemas/evaluate/request.json
( 10%) Resolving: self/v1/schemas/api/schemas/evaluate/response.json
( 12%) Resolving: self/v1/schemas/api/schemas/health/response.json
( 14%) Resolving: self/v1/schemas/api/schemas/locations/response.json
( 15%) Resolving: self/v1/schemas/api/schemas/metadata/response.json
( 17%) Resolving: self/v1/schemas/api/schemas/position.json
( 19%) Resolving: self/v1/schemas/api/schemas/positions/response.json
( 21%) Resolving: self/v1/schemas/api/schemas/search/response.json
( 22%) Resolving: self/v1/schemas/api/schemas/stats/response.json
( 24%) Resolving: self/v1/schemas/api/schemas/trace/request.json
( 26%) Resolving: self/v1/schemas/api/schemas/trace/response.json
( 28%) Resolving: self/v1/schemas/mcp/error.json
( 29%) Resolving: self/v1/schemas/mcp/initialize/request.json
( 31%) Resolving: self/v1/schemas/mcp/initialize/response.json
( 33%) Resolving: self/v1/schemas/mcp/notifications/cancelled.json
( 35%) Resolving: self/v1/schemas/mcp/notifications/initialized.json
( 36%) Resolving: self/v1/schemas/mcp/ping/request.json
( 38%) Resolving: self/v1/schemas/mcp/ping/response.json
( 40%) Resolving: self/v1/schemas/mcp/request.json
( 42%) Resolving: self/v1/schemas/mcp/resources/list/request.json
( 43%) Resolving: self/v1/schemas/mcp/resources/list/response.json
( 45%) Resolving: self/v1/schemas/mcp/resources/read/request.json
( 47%) Resolving: self/v1/schemas/mcp/resources/read/response.json
( 49%) Resolving: self/v1/schemas/mcp/resources/templates/list/request.json
( 50%) Resolving: self/v1/schemas/mcp/resources/templates/list/response.json
( 52%) Resolving: self/v1/schemas/mcp/response.json
( 54%) Resolving: self/v1/schemas/mcp/tools/call/evaluate-schema/request.json
( 56%) Resolving: self/v1/schemas/mcp/tools/call/evaluate-schema/response.json
( 57%) Resolving: self/v1/schemas/mcp/tools/call/get-schema-dependencies/request.json
( 59%) Resolving: self/v1/schemas/mcp/tools/call/get-schema-dependencies/response.json
( 61%) Resolving: self/v1/schemas/mcp/tools/call/get-schema-dependents/request.json
( 63%) Resolving: self/v1/schemas/mcp/tools/call/get-schema-dependents/response.json
( 64%) Resolving: self/v1/schemas/mcp/tools/call/get-schema-health/request.json
( 66%) Resolving: self/v1/schemas/mcp/tools/call/get-schema-health/response.json
( 68%) Resolving: self/v1/schemas/mcp/tools/call/get-schema-locations/request.json
( 70%) Resolving: self/v1/schemas/mcp/tools/call/get-schema-locations/response.json
( 71%) Resolving: self/v1/schemas/mcp/tools/call/get-schema-metadata/request.json
( 73%) Resolving: self/v1/schemas/mcp/tools/call/get-schema-metadata/response.json
( 75%) Resolving: self/v1/schemas/mcp/tools/call/get-schema-positions/request.json
( 77%) Resolving: self/v1/schemas/mcp/tools/call/get-schema-positions/response.json
( 78%) Resolving: self/v1/schemas/mcp/tools/call/get-schema-stats/request.json
( 80%) Resolving: self/v1/schemas/mcp/tools/call/get-schema-stats/response.json
( 82%) Resolving: self/v1/schemas/mcp/tools/call/list-directory/request.json
( 84%) Resolving: self/v1/schemas/mcp/tools/call/list-directory/response.json
( 85%) Resolving: self/v1/schemas/mcp/tools/call/request.json
( 87%) Resolving: self/v1/schemas/mcp/tools/call/response.json
( 89%) Resolving: self/v1/schemas/mcp/tools/call/search-schemas/request.json
( 91%) Resolving: self/v1/schemas/mcp/tools/call/search-schemas/response.json
( 92%) Resolving: self/v1/schemas/mcp/tools/call/trace-schema-evaluation/request.json
( 94%) Resolving: self/v1/schemas/mcp/tools/call/trace-schema-evaluation/response.json
( 96%) Resolving: self/v1/schemas/mcp/tools/list/request.json
( 98%) Resolving: self/v1/schemas/mcp/tools/list/response.json
(100%) Resolving: test/foo.json
error: The schema identifier is not relative to the corresponding base
  at path $(realpath "$TMP")/schemas/foo.json
  at identifier https://example.com/other/foo
  with base https://example.com/schemas
EOF

diff "$TMP/output.txt" "$TMP/expected.txt"
