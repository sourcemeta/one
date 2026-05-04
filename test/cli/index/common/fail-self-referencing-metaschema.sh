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
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/api/schemas/dependencies/response.json (#5)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/api/schemas/dependents/response.json (#6)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/api/schemas/evaluate/response.json (#7)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/api/schemas/health/response.json (#8)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/api/schemas/locations/response.json (#9)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/api/schemas/metadata/response.json (#10)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/api/schemas/position.json (#11)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/api/schemas/positions/response.json (#12)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/api/schemas/search/response.json (#13)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/api/schemas/stats/response.json (#14)
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
(  3%) Resolving: example/schemas/my-metaschema.json
(  6%) Resolving: example/schemas/test.json
( 10%) Resolving: self/v1/schemas/api/error.json
( 13%) Resolving: self/v1/schemas/api/list/response.json
( 16%) Resolving: self/v1/schemas/api/schemas/dependencies/response.json
( 20%) Resolving: self/v1/schemas/api/schemas/dependents/response.json
( 23%) Resolving: self/v1/schemas/api/schemas/evaluate/response.json
( 26%) Resolving: self/v1/schemas/api/schemas/health/response.json
( 30%) Resolving: self/v1/schemas/api/schemas/locations/response.json
( 33%) Resolving: self/v1/schemas/api/schemas/metadata/response.json
( 36%) Resolving: self/v1/schemas/api/schemas/position.json
( 40%) Resolving: self/v1/schemas/api/schemas/positions/response.json
( 43%) Resolving: self/v1/schemas/api/schemas/search/response.json
( 46%) Resolving: self/v1/schemas/api/schemas/stats/response.json
( 50%) Resolving: self/v1/schemas/api/schemas/trace/response.json
( 53%) Resolving: self/v1/schemas/mcp/error.json
( 56%) Resolving: self/v1/schemas/mcp/initialize/request.json
( 60%) Resolving: self/v1/schemas/mcp/initialize/response.json
( 63%) Resolving: self/v1/schemas/mcp/notifications/cancelled.json
( 66%) Resolving: self/v1/schemas/mcp/notifications/initialized.json
( 70%) Resolving: self/v1/schemas/mcp/ping/request.json
( 73%) Resolving: self/v1/schemas/mcp/ping/response.json
( 76%) Resolving: self/v1/schemas/mcp/request.json
( 80%) Resolving: self/v1/schemas/mcp/resources/list/request.json
( 83%) Resolving: self/v1/schemas/mcp/resources/list/response.json
( 86%) Resolving: self/v1/schemas/mcp/resources/read/request.json
( 90%) Resolving: self/v1/schemas/mcp/resources/read/response.json
( 93%) Resolving: self/v1/schemas/mcp/resources/templates/list/request.json
( 96%) Resolving: self/v1/schemas/mcp/resources/templates/list/response.json
(100%) Resolving: self/v1/schemas/mcp/response.json
(  0%) Producing: configuration.json
(  0%) Producing: metadata.json
(  0%) Producing: version.json
(  0%) Producing: explorer/%/404.metapack
(  1%) Producing: schemas/example/schemas/my-metaschema/%/schema.metapack
error: Could not resolve the metaschema of the schema
  at identifier https://sourcemeta.com/example/schemas/my-metaschema
  at path $(realpath "$TMP")/schemas/my-metaschema.json

Did you forget to register a schema with such URI?
EOF

diff "$TMP/output.txt" "$TMP/expected1.txt"
