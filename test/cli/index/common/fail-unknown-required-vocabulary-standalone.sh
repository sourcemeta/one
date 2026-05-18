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

# A custom metaschema that requires an unknown vocabulary,
# with no other schema using it
cat << 'EOF' > "$TMP/schemas/custom-meta.json"
{
  "$schema": "https://json-schema.org/draft/2020-12/schema",
  "$id": "https://example.com/custom-meta",
  "$vocabulary": {
    "https://json-schema.org/draft/2020-12/vocab/core": true,
    "https://json-schema.org/draft/2020-12/vocab/applicator": true,
    "https://json-schema.org/draft/2020-12/vocab/validation": true,
    "https://example.com/vocab/totally-unknown": true
  }
}
EOF

"$1" --skip-banner --deterministic "$TMP/one.json" "$TMP/output" --concurrency 1 2> "$TMP/output.txt" && CODE="$?" || CODE="$?"
test "$CODE" = "1" || exit 1

# Remove thread information
if [ "$(uname)" = "Darwin" ]
then
  sed -i '' 's/ \[.*\]//g' "$TMP/output.txt"
else
  sed -i 's/ \[.*\]//g' "$TMP/output.txt"
fi

cat << EOF > "$TMP/expected.txt"
Writing output to: $(realpath "$TMP")/output
Using configuration: $(realpath "$TMP")/one.json
Detecting: $(realpath "$TMP")/schemas/custom-meta.json (#1)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/api/error.json (#2)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/api/list/response.json (#3)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/api/list/rpc.json (#4)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/api/schemas/dependencies/response.json (#5)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/api/schemas/dependencies/rpc.json (#6)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/api/schemas/dependents/response.json (#7)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/api/schemas/dependents/rpc.json (#8)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/api/schemas/evaluate/request.json (#9)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/api/schemas/evaluate/response.json (#10)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/api/schemas/evaluate/rpc.json (#11)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/api/schemas/health/response.json (#12)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/api/schemas/health/rpc.json (#13)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/api/schemas/locations/response.json (#14)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/api/schemas/locations/rpc.json (#15)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/api/schemas/metadata/response.json (#16)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/api/schemas/metadata/rpc.json (#17)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/api/schemas/position.json (#18)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/api/schemas/positions/response.json (#19)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/api/schemas/positions/rpc.json (#20)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/api/schemas/search/response.json (#21)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/api/schemas/search/rpc.json (#22)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/api/schemas/stats/response.json (#23)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/api/schemas/stats/rpc.json (#24)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/api/schemas/trace/request.json (#25)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/api/schemas/trace/response.json (#26)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/api/schemas/trace/rpc.json (#27)
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
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/tools/list/request.json (#44)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/tools/list/response.json (#45)
(  2%) Resolving: example/schemas/custom-meta.json
(  4%) Resolving: self/v1/schemas/api/error.json
(  6%) Resolving: self/v1/schemas/api/list/response.json
(  8%) Resolving: self/v1/schemas/api/list/rpc.json
( 11%) Resolving: self/v1/schemas/api/schemas/dependencies/response.json
( 13%) Resolving: self/v1/schemas/api/schemas/dependencies/rpc.json
( 15%) Resolving: self/v1/schemas/api/schemas/dependents/response.json
( 17%) Resolving: self/v1/schemas/api/schemas/dependents/rpc.json
( 20%) Resolving: self/v1/schemas/api/schemas/evaluate/request.json
( 22%) Resolving: self/v1/schemas/api/schemas/evaluate/response.json
( 24%) Resolving: self/v1/schemas/api/schemas/evaluate/rpc.json
( 26%) Resolving: self/v1/schemas/api/schemas/health/response.json
( 28%) Resolving: self/v1/schemas/api/schemas/health/rpc.json
( 31%) Resolving: self/v1/schemas/api/schemas/locations/response.json
( 33%) Resolving: self/v1/schemas/api/schemas/locations/rpc.json
( 35%) Resolving: self/v1/schemas/api/schemas/metadata/response.json
( 37%) Resolving: self/v1/schemas/api/schemas/metadata/rpc.json
( 40%) Resolving: self/v1/schemas/api/schemas/position.json
( 42%) Resolving: self/v1/schemas/api/schemas/positions/response.json
( 44%) Resolving: self/v1/schemas/api/schemas/positions/rpc.json
( 46%) Resolving: self/v1/schemas/api/schemas/search/response.json
( 48%) Resolving: self/v1/schemas/api/schemas/search/rpc.json
( 51%) Resolving: self/v1/schemas/api/schemas/stats/response.json
( 53%) Resolving: self/v1/schemas/api/schemas/stats/rpc.json
( 55%) Resolving: self/v1/schemas/api/schemas/trace/request.json
( 57%) Resolving: self/v1/schemas/api/schemas/trace/response.json
( 60%) Resolving: self/v1/schemas/api/schemas/trace/rpc.json
( 62%) Resolving: self/v1/schemas/mcp/error.json
( 64%) Resolving: self/v1/schemas/mcp/initialize/request.json
( 66%) Resolving: self/v1/schemas/mcp/initialize/response.json
( 68%) Resolving: self/v1/schemas/mcp/notifications/cancelled.json
( 71%) Resolving: self/v1/schemas/mcp/notifications/initialized.json
( 73%) Resolving: self/v1/schemas/mcp/ping/request.json
( 75%) Resolving: self/v1/schemas/mcp/ping/response.json
( 77%) Resolving: self/v1/schemas/mcp/request.json
( 80%) Resolving: self/v1/schemas/mcp/resources/list/request.json
( 82%) Resolving: self/v1/schemas/mcp/resources/list/response.json
( 84%) Resolving: self/v1/schemas/mcp/resources/read/request.json
( 86%) Resolving: self/v1/schemas/mcp/resources/read/response.json
( 88%) Resolving: self/v1/schemas/mcp/resources/templates/list/request.json
( 91%) Resolving: self/v1/schemas/mcp/resources/templates/list/response.json
( 93%) Resolving: self/v1/schemas/mcp/response.json
( 95%) Resolving: self/v1/schemas/mcp/tools/call/request.json
( 97%) Resolving: self/v1/schemas/mcp/tools/list/request.json
(100%) Resolving: self/v1/schemas/mcp/tools/list/response.json
(  0%) Producing: configuration.json
(  0%) Producing: version.json
(  0%) Producing: routes.bin
(  0%) Producing: explorer/%/404.metapack
(  0%) Producing: schemas/example/schemas/custom-meta/%/schema.metapack
error: The metaschema requires an unrecognised vocabulary
  at vocabulary https://example.com/vocab/totally-unknown
  at path $(realpath "$TMP")/schemas/custom-meta.json
EOF

diff "$TMP/output.txt" "$TMP/expected.txt"
