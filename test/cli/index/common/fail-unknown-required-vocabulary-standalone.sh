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
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/api/schemas/dependencies/response.json (#4)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/api/schemas/dependents/response.json (#5)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/api/schemas/evaluate/request.json (#6)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/api/schemas/evaluate/response.json (#7)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/api/schemas/health/response.json (#8)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/api/schemas/locations/response.json (#9)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/api/schemas/metadata/response.json (#10)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/api/schemas/position.json (#11)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/api/schemas/positions/response.json (#12)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/api/schemas/search/response.json (#13)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/api/schemas/stats/response.json (#14)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/api/schemas/trace/request.json (#15)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/api/schemas/trace/response.json (#16)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/error.json (#17)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/initialize/request.json (#18)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/initialize/response.json (#19)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/notifications/cancelled.json (#20)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/notifications/initialized.json (#21)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/ping/request.json (#22)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/ping/response.json (#23)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/request.json (#24)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/resources/list/request.json (#25)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/resources/list/response.json (#26)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/resources/read/request.json (#27)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/resources/read/response.json (#28)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/resources/templates/list/request.json (#29)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/resources/templates/list/response.json (#30)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/response.json (#31)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/tools/call/evaluate-schema/request.json (#32)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/tools/call/evaluate-schema/response.json (#33)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/tools/call/get-schema-dependencies/request.json (#34)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/tools/call/get-schema-dependencies/response.json (#35)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/tools/call/get-schema-dependents/request.json (#36)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/tools/call/get-schema-dependents/response.json (#37)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/tools/call/get-schema-health/request.json (#38)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/tools/call/get-schema-health/response.json (#39)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/tools/call/get-schema-locations/request.json (#40)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/tools/call/get-schema-locations/response.json (#41)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/tools/call/get-schema-metadata/request.json (#42)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/tools/call/get-schema-metadata/response.json (#43)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/tools/call/get-schema-positions/request.json (#44)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/tools/call/get-schema-positions/response.json (#45)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/tools/call/get-schema-stats/request.json (#46)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/tools/call/get-schema-stats/response.json (#47)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/tools/call/list-directory/request.json (#48)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/tools/call/list-directory/response.json (#49)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/tools/call/request.json (#50)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/tools/call/response.json (#51)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/tools/call/search-schemas/request.json (#52)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/tools/call/search-schemas/response.json (#53)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/tools/call/trace-schema-evaluation/request.json (#54)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/tools/call/trace-schema-evaluation/response.json (#55)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/tools/list/request.json (#56)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/tools/list/response.json (#57)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/meta.json (#58)
(  1%) Resolving: example/schemas/custom-meta.json
(  3%) Resolving: self/v1/schemas/api/error.json
(  5%) Resolving: self/v1/schemas/api/list/response.json
(  6%) Resolving: self/v1/schemas/api/schemas/dependencies/response.json
(  8%) Resolving: self/v1/schemas/api/schemas/dependents/response.json
( 10%) Resolving: self/v1/schemas/api/schemas/evaluate/request.json
( 12%) Resolving: self/v1/schemas/api/schemas/evaluate/response.json
( 13%) Resolving: self/v1/schemas/api/schemas/health/response.json
( 15%) Resolving: self/v1/schemas/api/schemas/locations/response.json
( 17%) Resolving: self/v1/schemas/api/schemas/metadata/response.json
( 18%) Resolving: self/v1/schemas/api/schemas/position.json
( 20%) Resolving: self/v1/schemas/api/schemas/positions/response.json
( 22%) Resolving: self/v1/schemas/api/schemas/search/response.json
( 24%) Resolving: self/v1/schemas/api/schemas/stats/response.json
( 25%) Resolving: self/v1/schemas/api/schemas/trace/request.json
( 27%) Resolving: self/v1/schemas/api/schemas/trace/response.json
( 29%) Resolving: self/v1/schemas/mcp/error.json
( 31%) Resolving: self/v1/schemas/mcp/initialize/request.json
( 32%) Resolving: self/v1/schemas/mcp/initialize/response.json
( 34%) Resolving: self/v1/schemas/mcp/notifications/cancelled.json
( 36%) Resolving: self/v1/schemas/mcp/notifications/initialized.json
( 37%) Resolving: self/v1/schemas/mcp/ping/request.json
( 39%) Resolving: self/v1/schemas/mcp/ping/response.json
( 41%) Resolving: self/v1/schemas/mcp/request.json
( 43%) Resolving: self/v1/schemas/mcp/resources/list/request.json
( 44%) Resolving: self/v1/schemas/mcp/resources/list/response.json
( 46%) Resolving: self/v1/schemas/mcp/resources/read/request.json
( 48%) Resolving: self/v1/schemas/mcp/resources/read/response.json
( 50%) Resolving: self/v1/schemas/mcp/resources/templates/list/request.json
( 51%) Resolving: self/v1/schemas/mcp/resources/templates/list/response.json
( 53%) Resolving: self/v1/schemas/mcp/response.json
( 55%) Resolving: self/v1/schemas/mcp/tools/call/evaluate-schema/request.json
( 56%) Resolving: self/v1/schemas/mcp/tools/call/evaluate-schema/response.json
( 58%) Resolving: self/v1/schemas/mcp/tools/call/get-schema-dependencies/request.json
( 60%) Resolving: self/v1/schemas/mcp/tools/call/get-schema-dependencies/response.json
( 62%) Resolving: self/v1/schemas/mcp/tools/call/get-schema-dependents/request.json
( 63%) Resolving: self/v1/schemas/mcp/tools/call/get-schema-dependents/response.json
( 65%) Resolving: self/v1/schemas/mcp/tools/call/get-schema-health/request.json
( 67%) Resolving: self/v1/schemas/mcp/tools/call/get-schema-health/response.json
( 68%) Resolving: self/v1/schemas/mcp/tools/call/get-schema-locations/request.json
( 70%) Resolving: self/v1/schemas/mcp/tools/call/get-schema-locations/response.json
( 72%) Resolving: self/v1/schemas/mcp/tools/call/get-schema-metadata/request.json
( 74%) Resolving: self/v1/schemas/mcp/tools/call/get-schema-metadata/response.json
( 75%) Resolving: self/v1/schemas/mcp/tools/call/get-schema-positions/request.json
( 77%) Resolving: self/v1/schemas/mcp/tools/call/get-schema-positions/response.json
( 79%) Resolving: self/v1/schemas/mcp/tools/call/get-schema-stats/request.json
( 81%) Resolving: self/v1/schemas/mcp/tools/call/get-schema-stats/response.json
( 82%) Resolving: self/v1/schemas/mcp/tools/call/list-directory/request.json
( 84%) Resolving: self/v1/schemas/mcp/tools/call/list-directory/response.json
( 86%) Resolving: self/v1/schemas/mcp/tools/call/request.json
( 87%) Resolving: self/v1/schemas/mcp/tools/call/response.json
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
(  0%) Producing: explorer/%/404.metapack
(  0%) Producing: schemas/example/schemas/custom-meta/%/schema.metapack
error: The metaschema requires an unrecognised vocabulary
  at vocabulary https://example.com/vocab/totally-unknown
  at path $(realpath "$TMP")/schemas/custom-meta.json
EOF

diff "$TMP/output.txt" "$TMP/expected.txt"
