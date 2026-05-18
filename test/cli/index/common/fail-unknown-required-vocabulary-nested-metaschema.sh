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

# A custom metaschema that only uses known vocabularies
cat << 'EOF' > "$TMP/schemas/meta-a.json"
{
  "$schema": "https://json-schema.org/draft/2020-12/schema",
  "$id": "https://example.com/meta-a",
  "$vocabulary": {
    "https://json-schema.org/draft/2020-12/vocab/core": true,
    "https://json-schema.org/draft/2020-12/vocab/applicator": true,
    "https://json-schema.org/draft/2020-12/vocab/validation": true
  }
}
EOF

# A custom metaschema that uses meta-a as its dialect
# and declares an unknown required vocabulary
cat << 'EOF' > "$TMP/schemas/meta-b.json"
{
  "$schema": "https://example.com/meta-a",
  "$id": "https://example.com/meta-b",
  "$vocabulary": {
    "https://json-schema.org/draft/2020-12/vocab/core": true,
    "https://example.com/vocab/totally-unknown": true
  }
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
test "$CODE" = "1"
remove_threads_information "$TMP/output.txt"

cat << EOF > "$TMP/expected.txt"
Writing output to: $(realpath "$TMP")/output
Using configuration: $(realpath "$TMP")/one.json
Detecting: $(realpath "$TMP")/schemas/meta-a.json (#1)
Detecting: $(realpath "$TMP")/schemas/meta-b.json (#2)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/api/error.json (#3)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/api/list/response.json (#4)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/api/list/rpc.json (#5)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/api/schemas/dependencies/response.json (#6)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/api/schemas/dependencies/rpc.json (#7)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/api/schemas/dependents/response.json (#8)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/api/schemas/dependents/rpc.json (#9)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/api/schemas/evaluate/request.json (#10)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/api/schemas/evaluate/response.json (#11)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/api/schemas/evaluate/rpc.json (#12)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/api/schemas/health/response.json (#13)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/api/schemas/health/rpc.json (#14)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/api/schemas/locations/response.json (#15)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/api/schemas/locations/rpc.json (#16)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/api/schemas/metadata/response.json (#17)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/api/schemas/metadata/rpc.json (#18)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/api/schemas/position.json (#19)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/api/schemas/positions/response.json (#20)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/api/schemas/positions/rpc.json (#21)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/api/schemas/search/response.json (#22)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/api/schemas/search/rpc.json (#23)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/api/schemas/stats/response.json (#24)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/api/schemas/stats/rpc.json (#25)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/api/schemas/trace/request.json (#26)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/api/schemas/trace/response.json (#27)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/api/schemas/trace/rpc.json (#28)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/error.json (#29)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/initialize/request.json (#30)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/initialize/response.json (#31)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/notifications/cancelled.json (#32)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/notifications/initialized.json (#33)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/ping/request.json (#34)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/ping/response.json (#35)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/request.json (#36)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/resources/list/request.json (#37)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/resources/list/response.json (#38)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/resources/read/request.json (#39)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/resources/read/response.json (#40)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/resources/templates/list/request.json (#41)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/resources/templates/list/response.json (#42)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/response.json (#43)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/tools/call/request.json (#44)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/tools/call/response.json (#45)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/tools/list/request.json (#46)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/tools/list/response.json (#47)
(  2%) Resolving: example/schemas/meta-a.json
(  4%) Resolving: example/schemas/meta-b.json
(  6%) Resolving: self/v1/schemas/api/error.json
(  8%) Resolving: self/v1/schemas/api/list/response.json
( 10%) Resolving: self/v1/schemas/api/list/rpc.json
( 12%) Resolving: self/v1/schemas/api/schemas/dependencies/response.json
( 14%) Resolving: self/v1/schemas/api/schemas/dependencies/rpc.json
( 17%) Resolving: self/v1/schemas/api/schemas/dependents/response.json
( 19%) Resolving: self/v1/schemas/api/schemas/dependents/rpc.json
( 21%) Resolving: self/v1/schemas/api/schemas/evaluate/request.json
( 23%) Resolving: self/v1/schemas/api/schemas/evaluate/response.json
( 25%) Resolving: self/v1/schemas/api/schemas/evaluate/rpc.json
( 27%) Resolving: self/v1/schemas/api/schemas/health/response.json
( 29%) Resolving: self/v1/schemas/api/schemas/health/rpc.json
( 31%) Resolving: self/v1/schemas/api/schemas/locations/response.json
( 34%) Resolving: self/v1/schemas/api/schemas/locations/rpc.json
( 36%) Resolving: self/v1/schemas/api/schemas/metadata/response.json
( 38%) Resolving: self/v1/schemas/api/schemas/metadata/rpc.json
( 40%) Resolving: self/v1/schemas/api/schemas/position.json
( 42%) Resolving: self/v1/schemas/api/schemas/positions/response.json
( 44%) Resolving: self/v1/schemas/api/schemas/positions/rpc.json
( 46%) Resolving: self/v1/schemas/api/schemas/search/response.json
( 48%) Resolving: self/v1/schemas/api/schemas/search/rpc.json
( 51%) Resolving: self/v1/schemas/api/schemas/stats/response.json
( 53%) Resolving: self/v1/schemas/api/schemas/stats/rpc.json
( 55%) Resolving: self/v1/schemas/api/schemas/trace/request.json
( 57%) Resolving: self/v1/schemas/api/schemas/trace/response.json
( 59%) Resolving: self/v1/schemas/api/schemas/trace/rpc.json
( 61%) Resolving: self/v1/schemas/mcp/error.json
( 63%) Resolving: self/v1/schemas/mcp/initialize/request.json
( 65%) Resolving: self/v1/schemas/mcp/initialize/response.json
( 68%) Resolving: self/v1/schemas/mcp/notifications/cancelled.json
( 70%) Resolving: self/v1/schemas/mcp/notifications/initialized.json
( 72%) Resolving: self/v1/schemas/mcp/ping/request.json
( 74%) Resolving: self/v1/schemas/mcp/ping/response.json
( 76%) Resolving: self/v1/schemas/mcp/request.json
( 78%) Resolving: self/v1/schemas/mcp/resources/list/request.json
( 80%) Resolving: self/v1/schemas/mcp/resources/list/response.json
( 82%) Resolving: self/v1/schemas/mcp/resources/read/request.json
( 85%) Resolving: self/v1/schemas/mcp/resources/read/response.json
( 87%) Resolving: self/v1/schemas/mcp/resources/templates/list/request.json
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
(  0%) Producing: schemas/example/schemas/meta-a/%/schema.metapack
(  0%) Producing: schemas/example/schemas/meta-b/%/schema.metapack
error: The metaschema requires an unrecognised vocabulary
  at vocabulary https://example.com/vocab/totally-unknown
  at path $(realpath "$TMP")/schemas/meta-b.json
EOF

diff "$TMP/output.txt" "$TMP/expected.txt"
