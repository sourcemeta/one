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

# A custom metaschema that requires an unknown vocabulary
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

# A schema that uses the custom metaschema
cat << 'EOF' > "$TMP/schemas/test.json"
{
  "$schema": "https://example.com/custom-meta",
  "$id": "https://example.com/test",
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
Detecting: $(realpath "$TMP")/schemas/custom-meta.json (#1)
Detecting: $(realpath "$TMP")/schemas/test.json (#2)
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
(  2%) Resolving: example/schemas/custom-meta.json
(  4%) Resolving: example/schemas/test.json
(  6%) Resolving: self/v1/schemas/api/error.json
(  9%) Resolving: self/v1/schemas/api/list/response.json
( 11%) Resolving: self/v1/schemas/api/list/rpc.json
( 13%) Resolving: self/v1/schemas/api/schemas/dependencies/response.json
( 16%) Resolving: self/v1/schemas/api/schemas/dependencies/rpc.json
( 18%) Resolving: self/v1/schemas/api/schemas/dependents/response.json
( 20%) Resolving: self/v1/schemas/api/schemas/dependents/rpc.json
( 23%) Resolving: self/v1/schemas/api/schemas/evaluate/request.json
( 25%) Resolving: self/v1/schemas/api/schemas/evaluate/response.json
( 27%) Resolving: self/v1/schemas/api/schemas/evaluate/rpc.json
( 30%) Resolving: self/v1/schemas/api/schemas/health/response.json
( 32%) Resolving: self/v1/schemas/api/schemas/health/rpc.json
( 34%) Resolving: self/v1/schemas/api/schemas/locations/response.json
( 37%) Resolving: self/v1/schemas/api/schemas/locations/rpc.json
( 39%) Resolving: self/v1/schemas/api/schemas/metadata/response.json
( 41%) Resolving: self/v1/schemas/api/schemas/metadata/rpc.json
( 44%) Resolving: self/v1/schemas/api/schemas/position.json
( 46%) Resolving: self/v1/schemas/api/schemas/positions/response.json
( 48%) Resolving: self/v1/schemas/api/schemas/positions/rpc.json
( 51%) Resolving: self/v1/schemas/api/schemas/search/response.json
( 53%) Resolving: self/v1/schemas/api/schemas/search/rpc.json
( 55%) Resolving: self/v1/schemas/api/schemas/stats/response.json
( 58%) Resolving: self/v1/schemas/api/schemas/stats/rpc.json
( 60%) Resolving: self/v1/schemas/api/schemas/trace/request.json
( 62%) Resolving: self/v1/schemas/api/schemas/trace/response.json
( 65%) Resolving: self/v1/schemas/api/schemas/trace/rpc.json
( 67%) Resolving: self/v1/schemas/mcp/error.json
( 69%) Resolving: self/v1/schemas/mcp/initialize/request.json
( 72%) Resolving: self/v1/schemas/mcp/initialize/response.json
( 74%) Resolving: self/v1/schemas/mcp/notifications/cancelled.json
( 76%) Resolving: self/v1/schemas/mcp/notifications/initialized.json
( 79%) Resolving: self/v1/schemas/mcp/ping/request.json
( 81%) Resolving: self/v1/schemas/mcp/ping/response.json
( 83%) Resolving: self/v1/schemas/mcp/request.json
( 86%) Resolving: self/v1/schemas/mcp/resources/list/request.json
( 88%) Resolving: self/v1/schemas/mcp/resources/list/response.json
( 90%) Resolving: self/v1/schemas/mcp/resources/read/request.json
( 93%) Resolving: self/v1/schemas/mcp/resources/read/response.json
( 95%) Resolving: self/v1/schemas/mcp/resources/templates/list/request.json
( 97%) Resolving: self/v1/schemas/mcp/resources/templates/list/response.json
(100%) Resolving: self/v1/schemas/mcp/response.json
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
