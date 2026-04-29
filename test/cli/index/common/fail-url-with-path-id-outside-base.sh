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
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/api/schemas/evaluate/response.json (#5)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/api/schemas/health/response.json (#6)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/api/schemas/locations/response.json (#7)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/api/schemas/metadata/response.json (#8)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/api/schemas/position.json (#9)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/api/schemas/positions/response.json (#10)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/api/schemas/search/response.json (#11)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/api/schemas/stats/response.json (#12)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/api/schemas/trace/response.json (#13)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/error.json (#14)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/initialize/request.json (#15)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/initialize/response.json (#16)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/notifications/cancelled.json (#17)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/notifications/initialized.json (#18)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/ping/request.json (#19)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/ping/response.json (#20)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/request.json (#21)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/response.json (#22)
Detecting: $(realpath "$TMP")/schemas/foo.json (#23)
(  4%) Resolving: self/v1/schemas/api/error.json
(  8%) Resolving: self/v1/schemas/api/list/response.json
( 13%) Resolving: self/v1/schemas/api/schemas/dependencies/response.json
( 17%) Resolving: self/v1/schemas/api/schemas/dependents/response.json
( 21%) Resolving: self/v1/schemas/api/schemas/evaluate/response.json
( 26%) Resolving: self/v1/schemas/api/schemas/health/response.json
( 30%) Resolving: self/v1/schemas/api/schemas/locations/response.json
( 34%) Resolving: self/v1/schemas/api/schemas/metadata/response.json
( 39%) Resolving: self/v1/schemas/api/schemas/position.json
( 43%) Resolving: self/v1/schemas/api/schemas/positions/response.json
( 47%) Resolving: self/v1/schemas/api/schemas/search/response.json
( 52%) Resolving: self/v1/schemas/api/schemas/stats/response.json
( 56%) Resolving: self/v1/schemas/api/schemas/trace/response.json
( 60%) Resolving: self/v1/schemas/mcp/error.json
( 65%) Resolving: self/v1/schemas/mcp/initialize/request.json
( 69%) Resolving: self/v1/schemas/mcp/initialize/response.json
( 73%) Resolving: self/v1/schemas/mcp/notifications/cancelled.json
( 78%) Resolving: self/v1/schemas/mcp/notifications/initialized.json
( 82%) Resolving: self/v1/schemas/mcp/ping/request.json
( 86%) Resolving: self/v1/schemas/mcp/ping/response.json
( 91%) Resolving: self/v1/schemas/mcp/request.json
( 95%) Resolving: self/v1/schemas/mcp/response.json
(100%) Resolving: test/foo.json
error: The schema identifier is not relative to the corresponding base
  at path $(realpath "$TMP")/schemas/foo.json
  at identifier https://example.com/other/foo
  with base https://example.com/schemas
EOF

diff "$TMP/output.txt" "$TMP/expected.txt"
