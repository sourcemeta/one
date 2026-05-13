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
  "$schema": "https://json-schema.org/draft/2020-12/schema",
  "$id": "https://example.com/test",
  "$ref": {"type": "string"}
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
(  2%) Resolving: example/schemas/test.json
(  4%) Resolving: self/v1/schemas/api/error.json
(  7%) Resolving: self/v1/schemas/api/list/response.json
(  9%) Resolving: self/v1/schemas/api/list/rpc.json
( 11%) Resolving: self/v1/schemas/api/schemas/dependencies/response.json
( 14%) Resolving: self/v1/schemas/api/schemas/dependencies/rpc.json
( 16%) Resolving: self/v1/schemas/api/schemas/dependents/response.json
( 19%) Resolving: self/v1/schemas/api/schemas/dependents/rpc.json
( 21%) Resolving: self/v1/schemas/api/schemas/evaluate/request.json
( 23%) Resolving: self/v1/schemas/api/schemas/evaluate/response.json
( 26%) Resolving: self/v1/schemas/api/schemas/evaluate/rpc.json
( 28%) Resolving: self/v1/schemas/api/schemas/health/response.json
( 30%) Resolving: self/v1/schemas/api/schemas/health/rpc.json
( 33%) Resolving: self/v1/schemas/api/schemas/locations/response.json
( 35%) Resolving: self/v1/schemas/api/schemas/locations/rpc.json
( 38%) Resolving: self/v1/schemas/api/schemas/metadata/response.json
( 40%) Resolving: self/v1/schemas/api/schemas/metadata/rpc.json
( 42%) Resolving: self/v1/schemas/api/schemas/position.json
( 45%) Resolving: self/v1/schemas/api/schemas/positions/response.json
( 47%) Resolving: self/v1/schemas/api/schemas/positions/rpc.json
( 50%) Resolving: self/v1/schemas/api/schemas/search/response.json
( 52%) Resolving: self/v1/schemas/api/schemas/search/rpc.json
( 54%) Resolving: self/v1/schemas/api/schemas/stats/response.json
( 57%) Resolving: self/v1/schemas/api/schemas/stats/rpc.json
( 59%) Resolving: self/v1/schemas/api/schemas/trace/request.json
( 61%) Resolving: self/v1/schemas/api/schemas/trace/response.json
( 64%) Resolving: self/v1/schemas/api/schemas/trace/rpc.json
( 66%) Resolving: self/v1/schemas/mcp/error.json
( 69%) Resolving: self/v1/schemas/mcp/initialize/request.json
( 71%) Resolving: self/v1/schemas/mcp/initialize/response.json
( 73%) Resolving: self/v1/schemas/mcp/notifications/cancelled.json
( 76%) Resolving: self/v1/schemas/mcp/notifications/initialized.json
( 78%) Resolving: self/v1/schemas/mcp/ping/request.json
( 80%) Resolving: self/v1/schemas/mcp/ping/response.json
( 83%) Resolving: self/v1/schemas/mcp/request.json
( 85%) Resolving: self/v1/schemas/mcp/resources/list/request.json
( 88%) Resolving: self/v1/schemas/mcp/resources/list/response.json
( 90%) Resolving: self/v1/schemas/mcp/resources/read/request.json
( 92%) Resolving: self/v1/schemas/mcp/resources/read/response.json
( 95%) Resolving: self/v1/schemas/mcp/resources/templates/list/request.json
( 97%) Resolving: self/v1/schemas/mcp/resources/templates/list/response.json
(100%) Resolving: self/v1/schemas/mcp/response.json
(  0%) Producing: configuration.json
(  0%) Producing: metadata.json
(  0%) Producing: version.json
(  0%) Producing: explorer/%/404.metapack
(  0%) Producing: schemas/example/schemas/test/%/schema.metapack
error: The schema does not adhere to its metaschema
The value was expected to be of type string but it was of type object
  at instance location "/\$ref"
  at evaluate path "/allOf/0/\$ref/properties/\$ref/\$ref/type"
The object value was expected to validate against the referenced schema
  at instance location "/\$ref"
  at evaluate path "/allOf/0/\$ref/properties/\$ref/\$ref"
The object value was expected to validate against the 9 defined properties subschemas
  at instance location ""
  at evaluate path "/allOf/0/\$ref/properties"
The object value was expected to validate against the referenced schema
  at instance location ""
  at evaluate path "/allOf/0/\$ref"
The object value was expected to validate against the 7 given subschemas
  at instance location ""
  at evaluate path "/allOf"
EOF

diff "$TMP/output.txt" "$TMP/expected.txt"
