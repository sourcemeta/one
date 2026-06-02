#!/bin/sh

set -o errexit
set -o nounset

TMP="$(mktemp -d)"
clean() { rm -rf "$TMP"; }
trap clean EXIT

cat << EOF > "$TMP/one.json"
{
  "url": "http://localhost:8000",
  "html": false,
  "contents": {
    "example": {
      "baseUri": "https://example.com",
      "path": "./schemas",
      "x-sourcemeta-one:evaluate": false
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
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/api/schemas/dependencies/response.json (#5)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/api/schemas/dependents/response.json (#6)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/api/schemas/evaluate/request.json (#7)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/api/schemas/evaluate/response.json (#8)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/api/schemas/health/response.json (#9)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/api/schemas/locations/response.json (#10)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/api/schemas/metadata/response.json (#11)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/api/schemas/position.json (#12)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/api/schemas/positions/response.json (#13)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/api/schemas/search/response.json (#14)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/api/schemas/stats/response.json (#15)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/api/schemas/trace/request.json (#16)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/api/schemas/trace/response.json (#17)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/error.json (#18)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/initialize/request.json (#19)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/initialize/response.json (#20)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/notifications/cancelled.json (#21)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/notifications/initialized.json (#22)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/ping/request.json (#23)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/ping/response.json (#24)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/request.json (#25)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/resources/list/request.json (#26)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/resources/list/response.json (#27)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/resources/read/request.json (#28)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/resources/read/response.json (#29)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/resources/templates/list/request.json (#30)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/resources/templates/list/response.json (#31)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/response.json (#32)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/tools/call/evaluate-schema/request.json (#33)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/tools/call/evaluate-schema/response.json (#34)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/tools/call/get-schema-dependencies/request.json (#35)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/tools/call/get-schema-dependencies/response.json (#36)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/tools/call/get-schema-dependents/request.json (#37)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/tools/call/get-schema-dependents/response.json (#38)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/tools/call/get-schema-health/request.json (#39)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/tools/call/get-schema-health/response.json (#40)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/tools/call/get-schema-locations/request.json (#41)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/tools/call/get-schema-locations/response.json (#42)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/tools/call/get-schema-metadata/request.json (#43)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/tools/call/get-schema-metadata/response.json (#44)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/tools/call/get-schema-positions/request.json (#45)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/tools/call/get-schema-positions/response.json (#46)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/tools/call/get-schema-stats/request.json (#47)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/tools/call/get-schema-stats/response.json (#48)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/tools/call/list-directory/request.json (#49)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/tools/call/list-directory/response.json (#50)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/tools/call/request.json (#51)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/tools/call/response.json (#52)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/tools/call/search-schemas/request.json (#53)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/tools/call/search-schemas/response.json (#54)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/tools/call/trace-schema-evaluation/request.json (#55)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/tools/call/trace-schema-evaluation/response.json (#56)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/tools/list/request.json (#57)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/tools/list/response.json (#58)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/meta.json (#59)
(  1%) Resolving: example/custom-meta.json
(  3%) Resolving: example/test.json
(  5%) Resolving: self/v1/schemas/api/error.json
(  6%) Resolving: self/v1/schemas/api/list/response.json
(  8%) Resolving: self/v1/schemas/api/schemas/dependencies/response.json
( 10%) Resolving: self/v1/schemas/api/schemas/dependents/response.json
( 11%) Resolving: self/v1/schemas/api/schemas/evaluate/request.json
( 13%) Resolving: self/v1/schemas/api/schemas/evaluate/response.json
( 15%) Resolving: self/v1/schemas/api/schemas/health/response.json
( 16%) Resolving: self/v1/schemas/api/schemas/locations/response.json
( 18%) Resolving: self/v1/schemas/api/schemas/metadata/response.json
( 20%) Resolving: self/v1/schemas/api/schemas/position.json
( 22%) Resolving: self/v1/schemas/api/schemas/positions/response.json
( 23%) Resolving: self/v1/schemas/api/schemas/search/response.json
( 25%) Resolving: self/v1/schemas/api/schemas/stats/response.json
( 27%) Resolving: self/v1/schemas/api/schemas/trace/request.json
( 28%) Resolving: self/v1/schemas/api/schemas/trace/response.json
( 30%) Resolving: self/v1/schemas/mcp/error.json
( 32%) Resolving: self/v1/schemas/mcp/initialize/request.json
( 33%) Resolving: self/v1/schemas/mcp/initialize/response.json
( 35%) Resolving: self/v1/schemas/mcp/notifications/cancelled.json
( 37%) Resolving: self/v1/schemas/mcp/notifications/initialized.json
( 38%) Resolving: self/v1/schemas/mcp/ping/request.json
( 40%) Resolving: self/v1/schemas/mcp/ping/response.json
( 42%) Resolving: self/v1/schemas/mcp/request.json
( 44%) Resolving: self/v1/schemas/mcp/resources/list/request.json
( 45%) Resolving: self/v1/schemas/mcp/resources/list/response.json
( 47%) Resolving: self/v1/schemas/mcp/resources/read/request.json
( 49%) Resolving: self/v1/schemas/mcp/resources/read/response.json
( 50%) Resolving: self/v1/schemas/mcp/resources/templates/list/request.json
( 52%) Resolving: self/v1/schemas/mcp/resources/templates/list/response.json
( 54%) Resolving: self/v1/schemas/mcp/response.json
( 55%) Resolving: self/v1/schemas/mcp/tools/call/evaluate-schema/request.json
( 57%) Resolving: self/v1/schemas/mcp/tools/call/evaluate-schema/response.json
( 59%) Resolving: self/v1/schemas/mcp/tools/call/get-schema-dependencies/request.json
( 61%) Resolving: self/v1/schemas/mcp/tools/call/get-schema-dependencies/response.json
( 62%) Resolving: self/v1/schemas/mcp/tools/call/get-schema-dependents/request.json
( 64%) Resolving: self/v1/schemas/mcp/tools/call/get-schema-dependents/response.json
( 66%) Resolving: self/v1/schemas/mcp/tools/call/get-schema-health/request.json
( 67%) Resolving: self/v1/schemas/mcp/tools/call/get-schema-health/response.json
( 69%) Resolving: self/v1/schemas/mcp/tools/call/get-schema-locations/request.json
( 71%) Resolving: self/v1/schemas/mcp/tools/call/get-schema-locations/response.json
( 72%) Resolving: self/v1/schemas/mcp/tools/call/get-schema-metadata/request.json
( 74%) Resolving: self/v1/schemas/mcp/tools/call/get-schema-metadata/response.json
( 76%) Resolving: self/v1/schemas/mcp/tools/call/get-schema-positions/request.json
( 77%) Resolving: self/v1/schemas/mcp/tools/call/get-schema-positions/response.json
( 79%) Resolving: self/v1/schemas/mcp/tools/call/get-schema-stats/request.json
( 81%) Resolving: self/v1/schemas/mcp/tools/call/get-schema-stats/response.json
( 83%) Resolving: self/v1/schemas/mcp/tools/call/list-directory/request.json
( 84%) Resolving: self/v1/schemas/mcp/tools/call/list-directory/response.json
( 86%) Resolving: self/v1/schemas/mcp/tools/call/request.json
( 88%) Resolving: self/v1/schemas/mcp/tools/call/response.json
( 89%) Resolving: self/v1/schemas/mcp/tools/call/search-schemas/request.json
( 91%) Resolving: self/v1/schemas/mcp/tools/call/search-schemas/response.json
( 93%) Resolving: self/v1/schemas/mcp/tools/call/trace-schema-evaluation/request.json
( 94%) Resolving: self/v1/schemas/mcp/tools/call/trace-schema-evaluation/response.json
( 96%) Resolving: self/v1/schemas/mcp/tools/list/request.json
( 98%) Resolving: self/v1/schemas/mcp/tools/list/response.json
(100%) Resolving: self/v1/schemas/meta.json
(  0%) Producing: configuration.json
(  0%) Producing: version.json
(  0%) Producing: routes.bin
(  0%) Producing: schemas/example/custom-meta/%/schema.metapack
error: The metaschema requires an unrecognised vocabulary
  at vocabulary https://example.com/vocab/totally-unknown
  at path $(realpath "$TMP")/schemas/custom-meta.json
EOF


diff "$TMP/output.txt" "$TMP/expected.txt"
