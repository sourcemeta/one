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
  "$dynamicRef": 123
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
error: The schema does not adhere to its metaschema
The value was expected to be of type string but it was of type integer
  at instance location "/\$dynamicRef"
  at evaluate path "/allOf/0/\$ref/properties/\$dynamicRef/\$ref/type"
The integer value was expected to validate against the referenced schema
  at instance location "/\$dynamicRef"
  at evaluate path "/allOf/0/\$ref/properties/\$dynamicRef/\$ref"
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
