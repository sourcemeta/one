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
  "$schema": "http://json-schema.org/draft-07/schema#",
  "$id": "https://example.com/old.json"
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

"$1" --skip-banner --deterministic "$TMP/one.json" "$TMP/output" --concurrency 1 2> "$TMP/output.txt"
remove_threads_information "$TMP/output.txt"

cat << EOF > "$TMP/expected.txt"
Writing output to: $(realpath "$TMP")/output
Using configuration: $(realpath "$TMP")/one.json
Detecting: $(realpath "$TMP")/schemas/test.json (#1)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/api/error.json (#2)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/api/list/response.json (#3)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/api/schemas/dependencies/response.json (#4)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/api/schemas/dependents/response.json (#5)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/api/schemas/evaluate/request.json (#6)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/api/schemas/evaluate/response.json (#7)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/api/schemas/health/response.json (#8)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/api/schemas/locations/response.json (#9)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/api/schemas/metadata/response.json (#10)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/api/schemas/output-error.json (#11)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/api/schemas/position.json (#12)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/api/schemas/positions/response.json (#13)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/api/schemas/rdf/request.json (#14)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/api/schemas/rdf/response.json (#15)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/api/schemas/search/response.json (#16)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/api/schemas/stats/response.json (#17)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/api/schemas/trace/request.json (#18)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/api/schemas/trace/response.json (#19)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/error.json (#20)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/initialize/request.json (#21)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/initialize/response.json (#22)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/notifications/cancelled.json (#23)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/notifications/initialized.json (#24)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/ping/request.json (#25)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/ping/response.json (#26)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/request.json (#27)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/resources/list/request.json (#28)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/resources/list/response.json (#29)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/resources/read/request.json (#30)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/resources/read/response.json (#31)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/resources/templates/list/request.json (#32)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/resources/templates/list/response.json (#33)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/response.json (#34)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/tools/call/evaluate-schema/request.json (#35)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/tools/call/evaluate-schema/response.json (#36)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/tools/call/get-schema-dependencies/request.json (#37)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/tools/call/get-schema-dependencies/response.json (#38)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/tools/call/get-schema-dependents/request.json (#39)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/tools/call/get-schema-dependents/response.json (#40)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/tools/call/get-schema-health/request.json (#41)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/tools/call/get-schema-health/response.json (#42)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/tools/call/get-schema-locations/request.json (#43)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/tools/call/get-schema-locations/response.json (#44)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/tools/call/get-schema-metadata/request.json (#45)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/tools/call/get-schema-metadata/response.json (#46)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/tools/call/get-schema-positions/request.json (#47)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/tools/call/get-schema-positions/response.json (#48)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/tools/call/get-schema-stats/request.json (#49)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/tools/call/get-schema-stats/response.json (#50)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/tools/call/instance-to-rdf/request.json (#51)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/tools/call/instance-to-rdf/response.json (#52)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/tools/call/list-directory/request.json (#53)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/tools/call/list-directory/response.json (#54)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/tools/call/request.json (#55)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/tools/call/response.json (#56)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/tools/call/search-schemas/request.json (#57)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/tools/call/search-schemas/response.json (#58)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/tools/call/trace-schema-evaluation/request.json (#59)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/tools/call/trace-schema-evaluation/response.json (#60)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/tools/list/request.json (#61)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/tools/list/response.json (#62)
(  1%) Resolving: example/schemas/test.json
(  3%) Resolving: self/v1/schemas/api/error.json
(  4%) Resolving: self/v1/schemas/api/list/response.json
(  6%) Resolving: self/v1/schemas/api/schemas/dependencies/response.json
(  8%) Resolving: self/v1/schemas/api/schemas/dependents/response.json
(  9%) Resolving: self/v1/schemas/api/schemas/evaluate/request.json
( 11%) Resolving: self/v1/schemas/api/schemas/evaluate/response.json
( 12%) Resolving: self/v1/schemas/api/schemas/health/response.json
( 14%) Resolving: self/v1/schemas/api/schemas/locations/response.json
( 16%) Resolving: self/v1/schemas/api/schemas/metadata/response.json
( 17%) Resolving: self/v1/schemas/api/schemas/output-error.json
( 19%) Resolving: self/v1/schemas/api/schemas/position.json
( 20%) Resolving: self/v1/schemas/api/schemas/positions/response.json
( 22%) Resolving: self/v1/schemas/api/schemas/rdf/request.json
( 24%) Resolving: self/v1/schemas/api/schemas/rdf/response.json
( 25%) Resolving: self/v1/schemas/api/schemas/search/response.json
( 27%) Resolving: self/v1/schemas/api/schemas/stats/response.json
( 29%) Resolving: self/v1/schemas/api/schemas/trace/request.json
( 30%) Resolving: self/v1/schemas/api/schemas/trace/response.json
( 32%) Resolving: self/v1/schemas/mcp/error.json
( 33%) Resolving: self/v1/schemas/mcp/initialize/request.json
( 35%) Resolving: self/v1/schemas/mcp/initialize/response.json
( 37%) Resolving: self/v1/schemas/mcp/notifications/cancelled.json
( 38%) Resolving: self/v1/schemas/mcp/notifications/initialized.json
( 40%) Resolving: self/v1/schemas/mcp/ping/request.json
( 41%) Resolving: self/v1/schemas/mcp/ping/response.json
( 43%) Resolving: self/v1/schemas/mcp/request.json
( 45%) Resolving: self/v1/schemas/mcp/resources/list/request.json
( 46%) Resolving: self/v1/schemas/mcp/resources/list/response.json
( 48%) Resolving: self/v1/schemas/mcp/resources/read/request.json
( 50%) Resolving: self/v1/schemas/mcp/resources/read/response.json
( 51%) Resolving: self/v1/schemas/mcp/resources/templates/list/request.json
( 53%) Resolving: self/v1/schemas/mcp/resources/templates/list/response.json
( 54%) Resolving: self/v1/schemas/mcp/response.json
( 56%) Resolving: self/v1/schemas/mcp/tools/call/evaluate-schema/request.json
( 58%) Resolving: self/v1/schemas/mcp/tools/call/evaluate-schema/response.json
( 59%) Resolving: self/v1/schemas/mcp/tools/call/get-schema-dependencies/request.json
( 61%) Resolving: self/v1/schemas/mcp/tools/call/get-schema-dependencies/response.json
( 62%) Resolving: self/v1/schemas/mcp/tools/call/get-schema-dependents/request.json
( 64%) Resolving: self/v1/schemas/mcp/tools/call/get-schema-dependents/response.json
( 66%) Resolving: self/v1/schemas/mcp/tools/call/get-schema-health/request.json
( 67%) Resolving: self/v1/schemas/mcp/tools/call/get-schema-health/response.json
( 69%) Resolving: self/v1/schemas/mcp/tools/call/get-schema-locations/request.json
( 70%) Resolving: self/v1/schemas/mcp/tools/call/get-schema-locations/response.json
( 72%) Resolving: self/v1/schemas/mcp/tools/call/get-schema-metadata/request.json
( 74%) Resolving: self/v1/schemas/mcp/tools/call/get-schema-metadata/response.json
( 75%) Resolving: self/v1/schemas/mcp/tools/call/get-schema-positions/request.json
( 77%) Resolving: self/v1/schemas/mcp/tools/call/get-schema-positions/response.json
( 79%) Resolving: self/v1/schemas/mcp/tools/call/get-schema-stats/request.json
( 80%) Resolving: self/v1/schemas/mcp/tools/call/get-schema-stats/response.json
( 82%) Resolving: self/v1/schemas/mcp/tools/call/instance-to-rdf/request.json
( 83%) Resolving: self/v1/schemas/mcp/tools/call/instance-to-rdf/response.json
( 85%) Resolving: self/v1/schemas/mcp/tools/call/list-directory/request.json
( 87%) Resolving: self/v1/schemas/mcp/tools/call/list-directory/response.json
( 88%) Resolving: self/v1/schemas/mcp/tools/call/request.json
( 90%) Resolving: self/v1/schemas/mcp/tools/call/response.json
( 91%) Resolving: self/v1/schemas/mcp/tools/call/search-schemas/request.json
( 93%) Resolving: self/v1/schemas/mcp/tools/call/search-schemas/response.json
( 95%) Resolving: self/v1/schemas/mcp/tools/call/trace-schema-evaluation/request.json
( 96%) Resolving: self/v1/schemas/mcp/tools/call/trace-schema-evaluation/response.json
( 98%) Resolving: self/v1/schemas/mcp/tools/list/request.json
(100%) Resolving: self/v1/schemas/mcp/tools/list/response.json
(  0%) Producing: configuration.json
(  0%) Producing: version.json
(  0%) Producing: routes.bin
(  0%) Producing: authentication.bin
(  0%) Producing: explorer/%/401.metapack
(  0%) Producing: explorer/%/404.metapack
(  0%) Producing: explorer/%/login-html.metapack
(  0%) Producing: schemas/example/schemas/old/%/schema.metapack
(  1%) Producing: schemas/self/v1/schemas/api/error/%/schema.metapack
(  1%) Producing: schemas/self/v1/schemas/api/list/response/%/schema.metapack
(  1%) Producing: schemas/self/v1/schemas/api/schemas/dependencies/response/%/schema.metapack
(  1%) Producing: schemas/self/v1/schemas/api/schemas/dependents/response/%/schema.metapack
(  1%) Producing: schemas/self/v1/schemas/api/schemas/evaluate/request/%/schema.metapack
(  1%) Producing: schemas/self/v1/schemas/api/schemas/evaluate/response/%/schema.metapack
(  1%) Producing: schemas/self/v1/schemas/api/schemas/health/response/%/schema.metapack
(  1%) Producing: schemas/self/v1/schemas/api/schemas/locations/response/%/schema.metapack
(  2%) Producing: schemas/self/v1/schemas/api/schemas/metadata/response/%/schema.metapack
(  2%) Producing: schemas/self/v1/schemas/api/schemas/output-error/%/schema.metapack
(  2%) Producing: schemas/self/v1/schemas/api/schemas/position/%/schema.metapack
(  2%) Producing: schemas/self/v1/schemas/api/schemas/positions/response/%/schema.metapack
(  2%) Producing: schemas/self/v1/schemas/api/schemas/rdf/request/%/schema.metapack
(  2%) Producing: schemas/self/v1/schemas/api/schemas/rdf/response/%/schema.metapack
(  2%) Producing: schemas/self/v1/schemas/api/schemas/search/response/%/schema.metapack
(  2%) Producing: schemas/self/v1/schemas/api/schemas/stats/response/%/schema.metapack
(  2%) Producing: schemas/self/v1/schemas/api/schemas/trace/request/%/schema.metapack
(  3%) Producing: schemas/self/v1/schemas/api/schemas/trace/response/%/schema.metapack
(  3%) Producing: schemas/self/v1/schemas/mcp/error/%/schema.metapack
(  3%) Producing: schemas/self/v1/schemas/mcp/initialize/request/%/schema.metapack
(  3%) Producing: schemas/self/v1/schemas/mcp/initialize/response/%/schema.metapack
(  3%) Producing: schemas/self/v1/schemas/mcp/notifications/cancelled/%/schema.metapack
(  3%) Producing: schemas/self/v1/schemas/mcp/notifications/initialized/%/schema.metapack
(  3%) Producing: schemas/self/v1/schemas/mcp/ping/request/%/schema.metapack
(  3%) Producing: schemas/self/v1/schemas/mcp/ping/response/%/schema.metapack
(  4%) Producing: schemas/self/v1/schemas/mcp/request/%/schema.metapack
(  4%) Producing: schemas/self/v1/schemas/mcp/resources/list/request/%/schema.metapack
(  4%) Producing: schemas/self/v1/schemas/mcp/resources/list/response/%/schema.metapack
(  4%) Producing: schemas/self/v1/schemas/mcp/resources/read/request/%/schema.metapack
(  4%) Producing: schemas/self/v1/schemas/mcp/resources/read/response/%/schema.metapack
(  4%) Producing: schemas/self/v1/schemas/mcp/resources/templates/list/request/%/schema.metapack
(  4%) Producing: schemas/self/v1/schemas/mcp/resources/templates/list/response/%/schema.metapack
(  4%) Producing: schemas/self/v1/schemas/mcp/response/%/schema.metapack
(  4%) Producing: schemas/self/v1/schemas/mcp/tools/call/evaluate-schema/request/%/schema.metapack
(  5%) Producing: schemas/self/v1/schemas/mcp/tools/call/evaluate-schema/response/%/schema.metapack
(  5%) Producing: schemas/self/v1/schemas/mcp/tools/call/get-schema-dependencies/request/%/schema.metapack
(  5%) Producing: schemas/self/v1/schemas/mcp/tools/call/get-schema-dependencies/response/%/schema.metapack
(  5%) Producing: schemas/self/v1/schemas/mcp/tools/call/get-schema-dependents/request/%/schema.metapack
(  5%) Producing: schemas/self/v1/schemas/mcp/tools/call/get-schema-dependents/response/%/schema.metapack
(  5%) Producing: schemas/self/v1/schemas/mcp/tools/call/get-schema-health/request/%/schema.metapack
(  5%) Producing: schemas/self/v1/schemas/mcp/tools/call/get-schema-health/response/%/schema.metapack
(  5%) Producing: schemas/self/v1/schemas/mcp/tools/call/get-schema-locations/request/%/schema.metapack
(  6%) Producing: schemas/self/v1/schemas/mcp/tools/call/get-schema-locations/response/%/schema.metapack
(  6%) Producing: schemas/self/v1/schemas/mcp/tools/call/get-schema-metadata/request/%/schema.metapack
(  6%) Producing: schemas/self/v1/schemas/mcp/tools/call/get-schema-metadata/response/%/schema.metapack
(  6%) Producing: schemas/self/v1/schemas/mcp/tools/call/get-schema-positions/request/%/schema.metapack
(  6%) Producing: schemas/self/v1/schemas/mcp/tools/call/get-schema-positions/response/%/schema.metapack
(  6%) Producing: schemas/self/v1/schemas/mcp/tools/call/get-schema-stats/request/%/schema.metapack
(  6%) Producing: schemas/self/v1/schemas/mcp/tools/call/get-schema-stats/response/%/schema.metapack
(  6%) Producing: schemas/self/v1/schemas/mcp/tools/call/instance-to-rdf/request/%/schema.metapack
(  7%) Producing: schemas/self/v1/schemas/mcp/tools/call/instance-to-rdf/response/%/schema.metapack
(  7%) Producing: schemas/self/v1/schemas/mcp/tools/call/list-directory/request/%/schema.metapack
(  7%) Producing: schemas/self/v1/schemas/mcp/tools/call/list-directory/response/%/schema.metapack
(  7%) Producing: schemas/self/v1/schemas/mcp/tools/call/request/%/schema.metapack
(  7%) Producing: schemas/self/v1/schemas/mcp/tools/call/response/%/schema.metapack
(  7%) Producing: schemas/self/v1/schemas/mcp/tools/call/search-schemas/request/%/schema.metapack
(  7%) Producing: schemas/self/v1/schemas/mcp/tools/call/search-schemas/response/%/schema.metapack
(  7%) Producing: schemas/self/v1/schemas/mcp/tools/call/trace-schema-evaluation/request/%/schema.metapack
(  7%) Producing: schemas/self/v1/schemas/mcp/tools/call/trace-schema-evaluation/response/%/schema.metapack
(  8%) Producing: schemas/self/v1/schemas/mcp/tools/list/request/%/schema.metapack
(  8%) Producing: schemas/self/v1/schemas/mcp/tools/list/response/%/schema.metapack
(  8%) Producing: schemas/example/schemas/old/%/dependencies.metapack
(  8%) Producing: schemas/example/schemas/old/%/locations.metapack
(  8%) Producing: schemas/example/schemas/old/%/positions.metapack
(  8%) Producing: schemas/example/schemas/old/%/stats.metapack
(  8%) Producing: schemas/self/v1/schemas/api/error/%/dependencies.metapack
(  8%) Producing: schemas/self/v1/schemas/api/error/%/locations.metapack
(  9%) Producing: schemas/self/v1/schemas/api/error/%/positions.metapack
(  9%) Producing: schemas/self/v1/schemas/api/error/%/stats.metapack
(  9%) Producing: schemas/self/v1/schemas/api/list/response/%/dependencies.metapack
(  9%) Producing: schemas/self/v1/schemas/api/list/response/%/locations.metapack
(  9%) Producing: schemas/self/v1/schemas/api/list/response/%/positions.metapack
(  9%) Producing: schemas/self/v1/schemas/api/list/response/%/stats.metapack
(  9%) Producing: schemas/self/v1/schemas/api/schemas/dependencies/response/%/dependencies.metapack
(  9%) Producing: schemas/self/v1/schemas/api/schemas/dependencies/response/%/locations.metapack
(  9%) Producing: schemas/self/v1/schemas/api/schemas/dependencies/response/%/positions.metapack
( 10%) Producing: schemas/self/v1/schemas/api/schemas/dependencies/response/%/stats.metapack
( 10%) Producing: schemas/self/v1/schemas/api/schemas/dependents/response/%/dependencies.metapack
( 10%) Producing: schemas/self/v1/schemas/api/schemas/dependents/response/%/locations.metapack
( 10%) Producing: schemas/self/v1/schemas/api/schemas/dependents/response/%/positions.metapack
( 10%) Producing: schemas/self/v1/schemas/api/schemas/dependents/response/%/stats.metapack
( 10%) Producing: schemas/self/v1/schemas/api/schemas/evaluate/request/%/dependencies.metapack
( 10%) Producing: schemas/self/v1/schemas/api/schemas/evaluate/request/%/locations.metapack
( 10%) Producing: schemas/self/v1/schemas/api/schemas/evaluate/request/%/positions.metapack
( 11%) Producing: schemas/self/v1/schemas/api/schemas/evaluate/request/%/stats.metapack
( 11%) Producing: schemas/self/v1/schemas/api/schemas/evaluate/response/%/dependencies.metapack
( 11%) Producing: schemas/self/v1/schemas/api/schemas/evaluate/response/%/locations.metapack
( 11%) Producing: schemas/self/v1/schemas/api/schemas/evaluate/response/%/positions.metapack
( 11%) Producing: schemas/self/v1/schemas/api/schemas/evaluate/response/%/stats.metapack
( 11%) Producing: schemas/self/v1/schemas/api/schemas/health/response/%/dependencies.metapack
( 11%) Producing: schemas/self/v1/schemas/api/schemas/health/response/%/locations.metapack
( 11%) Producing: schemas/self/v1/schemas/api/schemas/health/response/%/positions.metapack
( 12%) Producing: schemas/self/v1/schemas/api/schemas/health/response/%/stats.metapack
( 12%) Producing: schemas/self/v1/schemas/api/schemas/locations/response/%/dependencies.metapack
( 12%) Producing: schemas/self/v1/schemas/api/schemas/locations/response/%/locations.metapack
( 12%) Producing: schemas/self/v1/schemas/api/schemas/locations/response/%/positions.metapack
( 12%) Producing: schemas/self/v1/schemas/api/schemas/locations/response/%/stats.metapack
( 12%) Producing: schemas/self/v1/schemas/api/schemas/metadata/response/%/dependencies.metapack
( 12%) Producing: schemas/self/v1/schemas/api/schemas/metadata/response/%/locations.metapack
( 12%) Producing: schemas/self/v1/schemas/api/schemas/metadata/response/%/positions.metapack
( 12%) Producing: schemas/self/v1/schemas/api/schemas/metadata/response/%/stats.metapack
( 13%) Producing: schemas/self/v1/schemas/api/schemas/output-error/%/dependencies.metapack
( 13%) Producing: schemas/self/v1/schemas/api/schemas/output-error/%/locations.metapack
( 13%) Producing: schemas/self/v1/schemas/api/schemas/output-error/%/positions.metapack
( 13%) Producing: schemas/self/v1/schemas/api/schemas/output-error/%/stats.metapack
( 13%) Producing: schemas/self/v1/schemas/api/schemas/position/%/dependencies.metapack
( 13%) Producing: schemas/self/v1/schemas/api/schemas/position/%/locations.metapack
( 13%) Producing: schemas/self/v1/schemas/api/schemas/position/%/positions.metapack
( 13%) Producing: schemas/self/v1/schemas/api/schemas/position/%/stats.metapack
( 14%) Producing: schemas/self/v1/schemas/api/schemas/positions/response/%/dependencies.metapack
( 14%) Producing: schemas/self/v1/schemas/api/schemas/positions/response/%/locations.metapack
( 14%) Producing: schemas/self/v1/schemas/api/schemas/positions/response/%/positions.metapack
( 14%) Producing: schemas/self/v1/schemas/api/schemas/positions/response/%/stats.metapack
( 14%) Producing: schemas/self/v1/schemas/api/schemas/rdf/request/%/dependencies.metapack
( 14%) Producing: schemas/self/v1/schemas/api/schemas/rdf/request/%/locations.metapack
( 14%) Producing: schemas/self/v1/schemas/api/schemas/rdf/request/%/positions.metapack
( 14%) Producing: schemas/self/v1/schemas/api/schemas/rdf/request/%/stats.metapack
( 14%) Producing: schemas/self/v1/schemas/api/schemas/rdf/response/%/dependencies.metapack
( 15%) Producing: schemas/self/v1/schemas/api/schemas/rdf/response/%/locations.metapack
( 15%) Producing: schemas/self/v1/schemas/api/schemas/rdf/response/%/positions.metapack
( 15%) Producing: schemas/self/v1/schemas/api/schemas/rdf/response/%/stats.metapack
( 15%) Producing: schemas/self/v1/schemas/api/schemas/search/response/%/dependencies.metapack
( 15%) Producing: schemas/self/v1/schemas/api/schemas/search/response/%/locations.metapack
( 15%) Producing: schemas/self/v1/schemas/api/schemas/search/response/%/positions.metapack
( 15%) Producing: schemas/self/v1/schemas/api/schemas/search/response/%/stats.metapack
( 15%) Producing: schemas/self/v1/schemas/api/schemas/stats/response/%/dependencies.metapack
( 16%) Producing: schemas/self/v1/schemas/api/schemas/stats/response/%/locations.metapack
( 16%) Producing: schemas/self/v1/schemas/api/schemas/stats/response/%/positions.metapack
( 16%) Producing: schemas/self/v1/schemas/api/schemas/stats/response/%/stats.metapack
( 16%) Producing: schemas/self/v1/schemas/api/schemas/trace/request/%/dependencies.metapack
( 16%) Producing: schemas/self/v1/schemas/api/schemas/trace/request/%/locations.metapack
( 16%) Producing: schemas/self/v1/schemas/api/schemas/trace/request/%/positions.metapack
( 16%) Producing: schemas/self/v1/schemas/api/schemas/trace/request/%/stats.metapack
( 16%) Producing: schemas/self/v1/schemas/api/schemas/trace/response/%/dependencies.metapack
( 17%) Producing: schemas/self/v1/schemas/api/schemas/trace/response/%/locations.metapack
( 17%) Producing: schemas/self/v1/schemas/api/schemas/trace/response/%/positions.metapack
( 17%) Producing: schemas/self/v1/schemas/api/schemas/trace/response/%/stats.metapack
( 17%) Producing: schemas/self/v1/schemas/mcp/error/%/dependencies.metapack
( 17%) Producing: schemas/self/v1/schemas/mcp/error/%/locations.metapack
( 17%) Producing: schemas/self/v1/schemas/mcp/error/%/positions.metapack
( 17%) Producing: schemas/self/v1/schemas/mcp/error/%/stats.metapack
( 17%) Producing: schemas/self/v1/schemas/mcp/initialize/request/%/dependencies.metapack
( 17%) Producing: schemas/self/v1/schemas/mcp/initialize/request/%/locations.metapack
( 18%) Producing: schemas/self/v1/schemas/mcp/initialize/request/%/positions.metapack
( 18%) Producing: schemas/self/v1/schemas/mcp/initialize/request/%/stats.metapack
( 18%) Producing: schemas/self/v1/schemas/mcp/initialize/response/%/dependencies.metapack
( 18%) Producing: schemas/self/v1/schemas/mcp/initialize/response/%/locations.metapack
( 18%) Producing: schemas/self/v1/schemas/mcp/initialize/response/%/positions.metapack
( 18%) Producing: schemas/self/v1/schemas/mcp/initialize/response/%/stats.metapack
( 18%) Producing: schemas/self/v1/schemas/mcp/notifications/cancelled/%/dependencies.metapack
( 18%) Producing: schemas/self/v1/schemas/mcp/notifications/cancelled/%/locations.metapack
( 19%) Producing: schemas/self/v1/schemas/mcp/notifications/cancelled/%/positions.metapack
( 19%) Producing: schemas/self/v1/schemas/mcp/notifications/cancelled/%/stats.metapack
( 19%) Producing: schemas/self/v1/schemas/mcp/notifications/initialized/%/dependencies.metapack
( 19%) Producing: schemas/self/v1/schemas/mcp/notifications/initialized/%/locations.metapack
( 19%) Producing: schemas/self/v1/schemas/mcp/notifications/initialized/%/positions.metapack
( 19%) Producing: schemas/self/v1/schemas/mcp/notifications/initialized/%/stats.metapack
( 19%) Producing: schemas/self/v1/schemas/mcp/ping/request/%/dependencies.metapack
( 19%) Producing: schemas/self/v1/schemas/mcp/ping/request/%/locations.metapack
( 19%) Producing: schemas/self/v1/schemas/mcp/ping/request/%/positions.metapack
( 20%) Producing: schemas/self/v1/schemas/mcp/ping/request/%/stats.metapack
( 20%) Producing: schemas/self/v1/schemas/mcp/ping/response/%/dependencies.metapack
( 20%) Producing: schemas/self/v1/schemas/mcp/ping/response/%/locations.metapack
( 20%) Producing: schemas/self/v1/schemas/mcp/ping/response/%/positions.metapack
( 20%) Producing: schemas/self/v1/schemas/mcp/ping/response/%/stats.metapack
( 20%) Producing: schemas/self/v1/schemas/mcp/request/%/dependencies.metapack
( 20%) Producing: schemas/self/v1/schemas/mcp/request/%/locations.metapack
( 20%) Producing: schemas/self/v1/schemas/mcp/request/%/positions.metapack
( 21%) Producing: schemas/self/v1/schemas/mcp/request/%/stats.metapack
( 21%) Producing: schemas/self/v1/schemas/mcp/resources/list/request/%/dependencies.metapack
( 21%) Producing: schemas/self/v1/schemas/mcp/resources/list/request/%/locations.metapack
( 21%) Producing: schemas/self/v1/schemas/mcp/resources/list/request/%/positions.metapack
( 21%) Producing: schemas/self/v1/schemas/mcp/resources/list/request/%/stats.metapack
( 21%) Producing: schemas/self/v1/schemas/mcp/resources/list/response/%/dependencies.metapack
( 21%) Producing: schemas/self/v1/schemas/mcp/resources/list/response/%/locations.metapack
( 21%) Producing: schemas/self/v1/schemas/mcp/resources/list/response/%/positions.metapack
( 21%) Producing: schemas/self/v1/schemas/mcp/resources/list/response/%/stats.metapack
( 22%) Producing: schemas/self/v1/schemas/mcp/resources/read/request/%/dependencies.metapack
( 22%) Producing: schemas/self/v1/schemas/mcp/resources/read/request/%/locations.metapack
( 22%) Producing: schemas/self/v1/schemas/mcp/resources/read/request/%/positions.metapack
( 22%) Producing: schemas/self/v1/schemas/mcp/resources/read/request/%/stats.metapack
( 22%) Producing: schemas/self/v1/schemas/mcp/resources/read/response/%/dependencies.metapack
( 22%) Producing: schemas/self/v1/schemas/mcp/resources/read/response/%/locations.metapack
( 22%) Producing: schemas/self/v1/schemas/mcp/resources/read/response/%/positions.metapack
( 22%) Producing: schemas/self/v1/schemas/mcp/resources/read/response/%/stats.metapack
( 23%) Producing: schemas/self/v1/schemas/mcp/resources/templates/list/request/%/dependencies.metapack
( 23%) Producing: schemas/self/v1/schemas/mcp/resources/templates/list/request/%/locations.metapack
( 23%) Producing: schemas/self/v1/schemas/mcp/resources/templates/list/request/%/positions.metapack
( 23%) Producing: schemas/self/v1/schemas/mcp/resources/templates/list/request/%/stats.metapack
( 23%) Producing: schemas/self/v1/schemas/mcp/resources/templates/list/response/%/dependencies.metapack
( 23%) Producing: schemas/self/v1/schemas/mcp/resources/templates/list/response/%/locations.metapack
( 23%) Producing: schemas/self/v1/schemas/mcp/resources/templates/list/response/%/positions.metapack
( 23%) Producing: schemas/self/v1/schemas/mcp/resources/templates/list/response/%/stats.metapack
( 24%) Producing: schemas/self/v1/schemas/mcp/response/%/dependencies.metapack
( 24%) Producing: schemas/self/v1/schemas/mcp/response/%/locations.metapack
( 24%) Producing: schemas/self/v1/schemas/mcp/response/%/positions.metapack
( 24%) Producing: schemas/self/v1/schemas/mcp/response/%/stats.metapack
( 24%) Producing: schemas/self/v1/schemas/mcp/tools/call/evaluate-schema/request/%/dependencies.metapack
( 24%) Producing: schemas/self/v1/schemas/mcp/tools/call/evaluate-schema/request/%/locations.metapack
( 24%) Producing: schemas/self/v1/schemas/mcp/tools/call/evaluate-schema/request/%/positions.metapack
( 24%) Producing: schemas/self/v1/schemas/mcp/tools/call/evaluate-schema/request/%/stats.metapack
( 24%) Producing: schemas/self/v1/schemas/mcp/tools/call/evaluate-schema/response/%/dependencies.metapack
( 25%) Producing: schemas/self/v1/schemas/mcp/tools/call/evaluate-schema/response/%/locations.metapack
( 25%) Producing: schemas/self/v1/schemas/mcp/tools/call/evaluate-schema/response/%/positions.metapack
( 25%) Producing: schemas/self/v1/schemas/mcp/tools/call/evaluate-schema/response/%/stats.metapack
( 25%) Producing: schemas/self/v1/schemas/mcp/tools/call/get-schema-dependencies/request/%/dependencies.metapack
( 25%) Producing: schemas/self/v1/schemas/mcp/tools/call/get-schema-dependencies/request/%/locations.metapack
( 25%) Producing: schemas/self/v1/schemas/mcp/tools/call/get-schema-dependencies/request/%/positions.metapack
( 25%) Producing: schemas/self/v1/schemas/mcp/tools/call/get-schema-dependencies/request/%/stats.metapack
( 25%) Producing: schemas/self/v1/schemas/mcp/tools/call/get-schema-dependencies/response/%/dependencies.metapack
( 26%) Producing: schemas/self/v1/schemas/mcp/tools/call/get-schema-dependencies/response/%/locations.metapack
( 26%) Producing: schemas/self/v1/schemas/mcp/tools/call/get-schema-dependencies/response/%/positions.metapack
( 26%) Producing: schemas/self/v1/schemas/mcp/tools/call/get-schema-dependencies/response/%/stats.metapack
( 26%) Producing: schemas/self/v1/schemas/mcp/tools/call/get-schema-dependents/request/%/dependencies.metapack
( 26%) Producing: schemas/self/v1/schemas/mcp/tools/call/get-schema-dependents/request/%/locations.metapack
( 26%) Producing: schemas/self/v1/schemas/mcp/tools/call/get-schema-dependents/request/%/positions.metapack
( 26%) Producing: schemas/self/v1/schemas/mcp/tools/call/get-schema-dependents/request/%/stats.metapack
( 26%) Producing: schemas/self/v1/schemas/mcp/tools/call/get-schema-dependents/response/%/dependencies.metapack
( 26%) Producing: schemas/self/v1/schemas/mcp/tools/call/get-schema-dependents/response/%/locations.metapack
( 27%) Producing: schemas/self/v1/schemas/mcp/tools/call/get-schema-dependents/response/%/positions.metapack
( 27%) Producing: schemas/self/v1/schemas/mcp/tools/call/get-schema-dependents/response/%/stats.metapack
( 27%) Producing: schemas/self/v1/schemas/mcp/tools/call/get-schema-health/request/%/dependencies.metapack
( 27%) Producing: schemas/self/v1/schemas/mcp/tools/call/get-schema-health/request/%/locations.metapack
( 27%) Producing: schemas/self/v1/schemas/mcp/tools/call/get-schema-health/request/%/positions.metapack
( 27%) Producing: schemas/self/v1/schemas/mcp/tools/call/get-schema-health/request/%/stats.metapack
( 27%) Producing: schemas/self/v1/schemas/mcp/tools/call/get-schema-health/response/%/dependencies.metapack
( 27%) Producing: schemas/self/v1/schemas/mcp/tools/call/get-schema-health/response/%/locations.metapack
( 28%) Producing: schemas/self/v1/schemas/mcp/tools/call/get-schema-health/response/%/positions.metapack
( 28%) Producing: schemas/self/v1/schemas/mcp/tools/call/get-schema-health/response/%/stats.metapack
( 28%) Producing: schemas/self/v1/schemas/mcp/tools/call/get-schema-locations/request/%/dependencies.metapack
( 28%) Producing: schemas/self/v1/schemas/mcp/tools/call/get-schema-locations/request/%/locations.metapack
( 28%) Producing: schemas/self/v1/schemas/mcp/tools/call/get-schema-locations/request/%/positions.metapack
( 28%) Producing: schemas/self/v1/schemas/mcp/tools/call/get-schema-locations/request/%/stats.metapack
( 28%) Producing: schemas/self/v1/schemas/mcp/tools/call/get-schema-locations/response/%/dependencies.metapack
( 28%) Producing: schemas/self/v1/schemas/mcp/tools/call/get-schema-locations/response/%/locations.metapack
( 29%) Producing: schemas/self/v1/schemas/mcp/tools/call/get-schema-locations/response/%/positions.metapack
( 29%) Producing: schemas/self/v1/schemas/mcp/tools/call/get-schema-locations/response/%/stats.metapack
( 29%) Producing: schemas/self/v1/schemas/mcp/tools/call/get-schema-metadata/request/%/dependencies.metapack
( 29%) Producing: schemas/self/v1/schemas/mcp/tools/call/get-schema-metadata/request/%/locations.metapack
( 29%) Producing: schemas/self/v1/schemas/mcp/tools/call/get-schema-metadata/request/%/positions.metapack
( 29%) Producing: schemas/self/v1/schemas/mcp/tools/call/get-schema-metadata/request/%/stats.metapack
( 29%) Producing: schemas/self/v1/schemas/mcp/tools/call/get-schema-metadata/response/%/dependencies.metapack
( 29%) Producing: schemas/self/v1/schemas/mcp/tools/call/get-schema-metadata/response/%/locations.metapack
( 29%) Producing: schemas/self/v1/schemas/mcp/tools/call/get-schema-metadata/response/%/positions.metapack
( 30%) Producing: schemas/self/v1/schemas/mcp/tools/call/get-schema-metadata/response/%/stats.metapack
( 30%) Producing: schemas/self/v1/schemas/mcp/tools/call/get-schema-positions/request/%/dependencies.metapack
( 30%) Producing: schemas/self/v1/schemas/mcp/tools/call/get-schema-positions/request/%/locations.metapack
( 30%) Producing: schemas/self/v1/schemas/mcp/tools/call/get-schema-positions/request/%/positions.metapack
( 30%) Producing: schemas/self/v1/schemas/mcp/tools/call/get-schema-positions/request/%/stats.metapack
( 30%) Producing: schemas/self/v1/schemas/mcp/tools/call/get-schema-positions/response/%/dependencies.metapack
( 30%) Producing: schemas/self/v1/schemas/mcp/tools/call/get-schema-positions/response/%/locations.metapack
( 30%) Producing: schemas/self/v1/schemas/mcp/tools/call/get-schema-positions/response/%/positions.metapack
( 31%) Producing: schemas/self/v1/schemas/mcp/tools/call/get-schema-positions/response/%/stats.metapack
( 31%) Producing: schemas/self/v1/schemas/mcp/tools/call/get-schema-stats/request/%/dependencies.metapack
( 31%) Producing: schemas/self/v1/schemas/mcp/tools/call/get-schema-stats/request/%/locations.metapack
( 31%) Producing: schemas/self/v1/schemas/mcp/tools/call/get-schema-stats/request/%/positions.metapack
( 31%) Producing: schemas/self/v1/schemas/mcp/tools/call/get-schema-stats/request/%/stats.metapack
( 31%) Producing: schemas/self/v1/schemas/mcp/tools/call/get-schema-stats/response/%/dependencies.metapack
( 31%) Producing: schemas/self/v1/schemas/mcp/tools/call/get-schema-stats/response/%/locations.metapack
( 31%) Producing: schemas/self/v1/schemas/mcp/tools/call/get-schema-stats/response/%/positions.metapack
( 31%) Producing: schemas/self/v1/schemas/mcp/tools/call/get-schema-stats/response/%/stats.metapack
( 32%) Producing: schemas/self/v1/schemas/mcp/tools/call/instance-to-rdf/request/%/dependencies.metapack
( 32%) Producing: schemas/self/v1/schemas/mcp/tools/call/instance-to-rdf/request/%/locations.metapack
( 32%) Producing: schemas/self/v1/schemas/mcp/tools/call/instance-to-rdf/request/%/positions.metapack
( 32%) Producing: schemas/self/v1/schemas/mcp/tools/call/instance-to-rdf/request/%/stats.metapack
( 32%) Producing: schemas/self/v1/schemas/mcp/tools/call/instance-to-rdf/response/%/dependencies.metapack
( 32%) Producing: schemas/self/v1/schemas/mcp/tools/call/instance-to-rdf/response/%/locations.metapack
( 32%) Producing: schemas/self/v1/schemas/mcp/tools/call/instance-to-rdf/response/%/positions.metapack
( 32%) Producing: schemas/self/v1/schemas/mcp/tools/call/instance-to-rdf/response/%/stats.metapack
( 33%) Producing: schemas/self/v1/schemas/mcp/tools/call/list-directory/request/%/dependencies.metapack
( 33%) Producing: schemas/self/v1/schemas/mcp/tools/call/list-directory/request/%/locations.metapack
( 33%) Producing: schemas/self/v1/schemas/mcp/tools/call/list-directory/request/%/positions.metapack
( 33%) Producing: schemas/self/v1/schemas/mcp/tools/call/list-directory/request/%/stats.metapack
( 33%) Producing: schemas/self/v1/schemas/mcp/tools/call/list-directory/response/%/dependencies.metapack
( 33%) Producing: schemas/self/v1/schemas/mcp/tools/call/list-directory/response/%/locations.metapack
( 33%) Producing: schemas/self/v1/schemas/mcp/tools/call/list-directory/response/%/positions.metapack
( 33%) Producing: schemas/self/v1/schemas/mcp/tools/call/list-directory/response/%/stats.metapack
( 34%) Producing: schemas/self/v1/schemas/mcp/tools/call/request/%/dependencies.metapack
( 34%) Producing: schemas/self/v1/schemas/mcp/tools/call/request/%/locations.metapack
( 34%) Producing: schemas/self/v1/schemas/mcp/tools/call/request/%/positions.metapack
( 34%) Producing: schemas/self/v1/schemas/mcp/tools/call/request/%/stats.metapack
( 34%) Producing: schemas/self/v1/schemas/mcp/tools/call/response/%/dependencies.metapack
( 34%) Producing: schemas/self/v1/schemas/mcp/tools/call/response/%/locations.metapack
( 34%) Producing: schemas/self/v1/schemas/mcp/tools/call/response/%/positions.metapack
( 34%) Producing: schemas/self/v1/schemas/mcp/tools/call/response/%/stats.metapack
( 34%) Producing: schemas/self/v1/schemas/mcp/tools/call/search-schemas/request/%/dependencies.metapack
( 35%) Producing: schemas/self/v1/schemas/mcp/tools/call/search-schemas/request/%/locations.metapack
( 35%) Producing: schemas/self/v1/schemas/mcp/tools/call/search-schemas/request/%/positions.metapack
( 35%) Producing: schemas/self/v1/schemas/mcp/tools/call/search-schemas/request/%/stats.metapack
( 35%) Producing: schemas/self/v1/schemas/mcp/tools/call/search-schemas/response/%/dependencies.metapack
( 35%) Producing: schemas/self/v1/schemas/mcp/tools/call/search-schemas/response/%/locations.metapack
( 35%) Producing: schemas/self/v1/schemas/mcp/tools/call/search-schemas/response/%/positions.metapack
( 35%) Producing: schemas/self/v1/schemas/mcp/tools/call/search-schemas/response/%/stats.metapack
( 35%) Producing: schemas/self/v1/schemas/mcp/tools/call/trace-schema-evaluation/request/%/dependencies.metapack
( 36%) Producing: schemas/self/v1/schemas/mcp/tools/call/trace-schema-evaluation/request/%/locations.metapack
( 36%) Producing: schemas/self/v1/schemas/mcp/tools/call/trace-schema-evaluation/request/%/positions.metapack
( 36%) Producing: schemas/self/v1/schemas/mcp/tools/call/trace-schema-evaluation/request/%/stats.metapack
( 36%) Producing: schemas/self/v1/schemas/mcp/tools/call/trace-schema-evaluation/response/%/dependencies.metapack
( 36%) Producing: schemas/self/v1/schemas/mcp/tools/call/trace-schema-evaluation/response/%/locations.metapack
( 36%) Producing: schemas/self/v1/schemas/mcp/tools/call/trace-schema-evaluation/response/%/positions.metapack
( 36%) Producing: schemas/self/v1/schemas/mcp/tools/call/trace-schema-evaluation/response/%/stats.metapack
( 36%) Producing: schemas/self/v1/schemas/mcp/tools/list/request/%/dependencies.metapack
( 36%) Producing: schemas/self/v1/schemas/mcp/tools/list/request/%/locations.metapack
( 37%) Producing: schemas/self/v1/schemas/mcp/tools/list/request/%/positions.metapack
( 37%) Producing: schemas/self/v1/schemas/mcp/tools/list/request/%/stats.metapack
( 37%) Producing: schemas/self/v1/schemas/mcp/tools/list/response/%/dependencies.metapack
( 37%) Producing: schemas/self/v1/schemas/mcp/tools/list/response/%/locations.metapack
( 37%) Producing: schemas/self/v1/schemas/mcp/tools/list/response/%/positions.metapack
( 37%) Producing: schemas/self/v1/schemas/mcp/tools/list/response/%/stats.metapack
( 37%) Producing: schemas/example/schemas/old/%/bundle.metapack
( 37%) Producing: schemas/example/schemas/old/%/health.metapack
( 38%) Producing: schemas/self/v1/schemas/api/error/%/bundle.metapack
( 38%) Producing: schemas/self/v1/schemas/api/error/%/health.metapack
( 38%) Producing: schemas/self/v1/schemas/api/list/response/%/bundle.metapack
( 38%) Producing: schemas/self/v1/schemas/api/list/response/%/health.metapack
( 38%) Producing: schemas/self/v1/schemas/api/schemas/dependencies/response/%/bundle.metapack
( 38%) Producing: schemas/self/v1/schemas/api/schemas/dependencies/response/%/health.metapack
( 38%) Producing: schemas/self/v1/schemas/api/schemas/dependents/response/%/bundle.metapack
( 38%) Producing: schemas/self/v1/schemas/api/schemas/dependents/response/%/health.metapack
( 39%) Producing: schemas/self/v1/schemas/api/schemas/evaluate/request/%/bundle.metapack
( 39%) Producing: schemas/self/v1/schemas/api/schemas/evaluate/request/%/health.metapack
( 39%) Producing: schemas/self/v1/schemas/api/schemas/evaluate/response/%/bundle.metapack
( 39%) Producing: schemas/self/v1/schemas/api/schemas/evaluate/response/%/health.metapack
( 39%) Producing: schemas/self/v1/schemas/api/schemas/health/response/%/bundle.metapack
( 39%) Producing: schemas/self/v1/schemas/api/schemas/health/response/%/health.metapack
( 39%) Producing: schemas/self/v1/schemas/api/schemas/locations/response/%/bundle.metapack
( 39%) Producing: schemas/self/v1/schemas/api/schemas/locations/response/%/health.metapack
( 39%) Producing: schemas/self/v1/schemas/api/schemas/metadata/response/%/bundle.metapack
( 40%) Producing: schemas/self/v1/schemas/api/schemas/metadata/response/%/health.metapack
( 40%) Producing: schemas/self/v1/schemas/api/schemas/output-error/%/bundle.metapack
( 40%) Producing: schemas/self/v1/schemas/api/schemas/output-error/%/health.metapack
( 40%) Producing: schemas/self/v1/schemas/api/schemas/position/%/bundle.metapack
( 40%) Producing: schemas/self/v1/schemas/api/schemas/position/%/health.metapack
( 40%) Producing: schemas/self/v1/schemas/api/schemas/positions/response/%/bundle.metapack
( 40%) Producing: schemas/self/v1/schemas/api/schemas/positions/response/%/health.metapack
( 40%) Producing: schemas/self/v1/schemas/api/schemas/rdf/request/%/bundle.metapack
( 41%) Producing: schemas/self/v1/schemas/api/schemas/rdf/request/%/health.metapack
( 41%) Producing: schemas/self/v1/schemas/api/schemas/rdf/response/%/bundle.metapack
( 41%) Producing: schemas/self/v1/schemas/api/schemas/rdf/response/%/health.metapack
( 41%) Producing: schemas/self/v1/schemas/api/schemas/search/response/%/bundle.metapack
( 41%) Producing: schemas/self/v1/schemas/api/schemas/search/response/%/health.metapack
( 41%) Producing: schemas/self/v1/schemas/api/schemas/stats/response/%/bundle.metapack
( 41%) Producing: schemas/self/v1/schemas/api/schemas/stats/response/%/health.metapack
( 41%) Producing: schemas/self/v1/schemas/api/schemas/trace/request/%/bundle.metapack
( 41%) Producing: schemas/self/v1/schemas/api/schemas/trace/request/%/health.metapack
( 42%) Producing: schemas/self/v1/schemas/api/schemas/trace/response/%/bundle.metapack
( 42%) Producing: schemas/self/v1/schemas/api/schemas/trace/response/%/health.metapack
( 42%) Producing: schemas/self/v1/schemas/mcp/error/%/bundle.metapack
( 42%) Producing: schemas/self/v1/schemas/mcp/error/%/health.metapack
( 42%) Producing: schemas/self/v1/schemas/mcp/initialize/request/%/bundle.metapack
( 42%) Producing: schemas/self/v1/schemas/mcp/initialize/request/%/health.metapack
( 42%) Producing: schemas/self/v1/schemas/mcp/initialize/response/%/bundle.metapack
( 42%) Producing: schemas/self/v1/schemas/mcp/initialize/response/%/health.metapack
( 43%) Producing: schemas/self/v1/schemas/mcp/notifications/cancelled/%/bundle.metapack
( 43%) Producing: schemas/self/v1/schemas/mcp/notifications/cancelled/%/health.metapack
( 43%) Producing: schemas/self/v1/schemas/mcp/notifications/initialized/%/bundle.metapack
( 43%) Producing: schemas/self/v1/schemas/mcp/notifications/initialized/%/health.metapack
( 43%) Producing: schemas/self/v1/schemas/mcp/ping/request/%/bundle.metapack
( 43%) Producing: schemas/self/v1/schemas/mcp/ping/request/%/health.metapack
( 43%) Producing: schemas/self/v1/schemas/mcp/ping/response/%/bundle.metapack
( 43%) Producing: schemas/self/v1/schemas/mcp/ping/response/%/health.metapack
( 43%) Producing: schemas/self/v1/schemas/mcp/request/%/bundle.metapack
( 44%) Producing: schemas/self/v1/schemas/mcp/request/%/health.metapack
( 44%) Producing: schemas/self/v1/schemas/mcp/resources/list/request/%/bundle.metapack
( 44%) Producing: schemas/self/v1/schemas/mcp/resources/list/request/%/health.metapack
( 44%) Producing: schemas/self/v1/schemas/mcp/resources/list/response/%/bundle.metapack
( 44%) Producing: schemas/self/v1/schemas/mcp/resources/list/response/%/health.metapack
( 44%) Producing: schemas/self/v1/schemas/mcp/resources/read/request/%/bundle.metapack
( 44%) Producing: schemas/self/v1/schemas/mcp/resources/read/request/%/health.metapack
( 44%) Producing: schemas/self/v1/schemas/mcp/resources/read/response/%/bundle.metapack
( 45%) Producing: schemas/self/v1/schemas/mcp/resources/read/response/%/health.metapack
( 45%) Producing: schemas/self/v1/schemas/mcp/resources/templates/list/request/%/bundle.metapack
( 45%) Producing: schemas/self/v1/schemas/mcp/resources/templates/list/request/%/health.metapack
( 45%) Producing: schemas/self/v1/schemas/mcp/resources/templates/list/response/%/bundle.metapack
( 45%) Producing: schemas/self/v1/schemas/mcp/resources/templates/list/response/%/health.metapack
( 45%) Producing: schemas/self/v1/schemas/mcp/response/%/bundle.metapack
( 45%) Producing: schemas/self/v1/schemas/mcp/response/%/health.metapack
( 45%) Producing: schemas/self/v1/schemas/mcp/tools/call/evaluate-schema/request/%/bundle.metapack
( 46%) Producing: schemas/self/v1/schemas/mcp/tools/call/evaluate-schema/request/%/health.metapack
( 46%) Producing: schemas/self/v1/schemas/mcp/tools/call/evaluate-schema/response/%/bundle.metapack
( 46%) Producing: schemas/self/v1/schemas/mcp/tools/call/evaluate-schema/response/%/health.metapack
( 46%) Producing: schemas/self/v1/schemas/mcp/tools/call/get-schema-dependencies/request/%/bundle.metapack
( 46%) Producing: schemas/self/v1/schemas/mcp/tools/call/get-schema-dependencies/request/%/health.metapack
( 46%) Producing: schemas/self/v1/schemas/mcp/tools/call/get-schema-dependencies/response/%/bundle.metapack
( 46%) Producing: schemas/self/v1/schemas/mcp/tools/call/get-schema-dependencies/response/%/health.metapack
( 46%) Producing: schemas/self/v1/schemas/mcp/tools/call/get-schema-dependents/request/%/bundle.metapack
( 46%) Producing: schemas/self/v1/schemas/mcp/tools/call/get-schema-dependents/request/%/health.metapack
( 47%) Producing: schemas/self/v1/schemas/mcp/tools/call/get-schema-dependents/response/%/bundle.metapack
( 47%) Producing: schemas/self/v1/schemas/mcp/tools/call/get-schema-dependents/response/%/health.metapack
( 47%) Producing: schemas/self/v1/schemas/mcp/tools/call/get-schema-health/request/%/bundle.metapack
( 47%) Producing: schemas/self/v1/schemas/mcp/tools/call/get-schema-health/request/%/health.metapack
( 47%) Producing: schemas/self/v1/schemas/mcp/tools/call/get-schema-health/response/%/bundle.metapack
( 47%) Producing: schemas/self/v1/schemas/mcp/tools/call/get-schema-health/response/%/health.metapack
( 47%) Producing: schemas/self/v1/schemas/mcp/tools/call/get-schema-locations/request/%/bundle.metapack
( 47%) Producing: schemas/self/v1/schemas/mcp/tools/call/get-schema-locations/request/%/health.metapack
( 48%) Producing: schemas/self/v1/schemas/mcp/tools/call/get-schema-locations/response/%/bundle.metapack
( 48%) Producing: schemas/self/v1/schemas/mcp/tools/call/get-schema-locations/response/%/health.metapack
( 48%) Producing: schemas/self/v1/schemas/mcp/tools/call/get-schema-metadata/request/%/bundle.metapack
( 48%) Producing: schemas/self/v1/schemas/mcp/tools/call/get-schema-metadata/request/%/health.metapack
( 48%) Producing: schemas/self/v1/schemas/mcp/tools/call/get-schema-metadata/response/%/bundle.metapack
( 48%) Producing: schemas/self/v1/schemas/mcp/tools/call/get-schema-metadata/response/%/health.metapack
( 48%) Producing: schemas/self/v1/schemas/mcp/tools/call/get-schema-positions/request/%/bundle.metapack
( 48%) Producing: schemas/self/v1/schemas/mcp/tools/call/get-schema-positions/request/%/health.metapack
( 48%) Producing: schemas/self/v1/schemas/mcp/tools/call/get-schema-positions/response/%/bundle.metapack
( 49%) Producing: schemas/self/v1/schemas/mcp/tools/call/get-schema-positions/response/%/health.metapack
( 49%) Producing: schemas/self/v1/schemas/mcp/tools/call/get-schema-stats/request/%/bundle.metapack
( 49%) Producing: schemas/self/v1/schemas/mcp/tools/call/get-schema-stats/request/%/health.metapack
( 49%) Producing: schemas/self/v1/schemas/mcp/tools/call/get-schema-stats/response/%/bundle.metapack
( 49%) Producing: schemas/self/v1/schemas/mcp/tools/call/get-schema-stats/response/%/health.metapack
( 49%) Producing: schemas/self/v1/schemas/mcp/tools/call/instance-to-rdf/request/%/bundle.metapack
( 49%) Producing: schemas/self/v1/schemas/mcp/tools/call/instance-to-rdf/request/%/health.metapack
( 49%) Producing: schemas/self/v1/schemas/mcp/tools/call/instance-to-rdf/response/%/bundle.metapack
( 50%) Producing: schemas/self/v1/schemas/mcp/tools/call/instance-to-rdf/response/%/health.metapack
( 50%) Producing: schemas/self/v1/schemas/mcp/tools/call/list-directory/request/%/bundle.metapack
( 50%) Producing: schemas/self/v1/schemas/mcp/tools/call/list-directory/request/%/health.metapack
( 50%) Producing: schemas/self/v1/schemas/mcp/tools/call/list-directory/response/%/bundle.metapack
( 50%) Producing: schemas/self/v1/schemas/mcp/tools/call/list-directory/response/%/health.metapack
( 50%) Producing: schemas/self/v1/schemas/mcp/tools/call/request/%/bundle.metapack
( 50%) Producing: schemas/self/v1/schemas/mcp/tools/call/request/%/health.metapack
( 50%) Producing: schemas/self/v1/schemas/mcp/tools/call/response/%/bundle.metapack
( 51%) Producing: schemas/self/v1/schemas/mcp/tools/call/response/%/health.metapack
( 51%) Producing: schemas/self/v1/schemas/mcp/tools/call/search-schemas/request/%/bundle.metapack
( 51%) Producing: schemas/self/v1/schemas/mcp/tools/call/search-schemas/request/%/health.metapack
( 51%) Producing: schemas/self/v1/schemas/mcp/tools/call/search-schemas/response/%/bundle.metapack
( 51%) Producing: schemas/self/v1/schemas/mcp/tools/call/search-schemas/response/%/health.metapack
( 51%) Producing: schemas/self/v1/schemas/mcp/tools/call/trace-schema-evaluation/request/%/bundle.metapack
( 51%) Producing: schemas/self/v1/schemas/mcp/tools/call/trace-schema-evaluation/request/%/health.metapack
( 51%) Producing: schemas/self/v1/schemas/mcp/tools/call/trace-schema-evaluation/response/%/bundle.metapack
( 51%) Producing: schemas/self/v1/schemas/mcp/tools/call/trace-schema-evaluation/response/%/health.metapack
( 52%) Producing: schemas/self/v1/schemas/mcp/tools/list/request/%/bundle.metapack
( 52%) Producing: schemas/self/v1/schemas/mcp/tools/list/request/%/health.metapack
( 52%) Producing: schemas/self/v1/schemas/mcp/tools/list/response/%/bundle.metapack
( 52%) Producing: schemas/self/v1/schemas/mcp/tools/list/response/%/health.metapack
( 52%) Producing: explorer/example/schemas/old/%/schema.metapack
( 52%) Producing: explorer/self/v1/schemas/api/error/%/schema.metapack
( 52%) Producing: explorer/self/v1/schemas/api/list/response/%/schema.metapack
( 52%) Producing: explorer/self/v1/schemas/api/schemas/dependencies/response/%/schema.metapack
( 53%) Producing: explorer/self/v1/schemas/api/schemas/dependents/response/%/schema.metapack
( 53%) Producing: explorer/self/v1/schemas/api/schemas/evaluate/request/%/schema.metapack
( 53%) Producing: explorer/self/v1/schemas/api/schemas/evaluate/response/%/schema.metapack
( 53%) Producing: explorer/self/v1/schemas/api/schemas/health/response/%/schema.metapack
( 53%) Producing: explorer/self/v1/schemas/api/schemas/locations/response/%/schema.metapack
( 53%) Producing: explorer/self/v1/schemas/api/schemas/metadata/response/%/schema.metapack
( 53%) Producing: explorer/self/v1/schemas/api/schemas/output-error/%/schema.metapack
( 53%) Producing: explorer/self/v1/schemas/api/schemas/position/%/schema.metapack
( 53%) Producing: explorer/self/v1/schemas/api/schemas/positions/response/%/schema.metapack
( 54%) Producing: explorer/self/v1/schemas/api/schemas/rdf/request/%/schema.metapack
( 54%) Producing: explorer/self/v1/schemas/api/schemas/rdf/response/%/schema.metapack
( 54%) Producing: explorer/self/v1/schemas/api/schemas/search/response/%/schema.metapack
( 54%) Producing: explorer/self/v1/schemas/api/schemas/stats/response/%/schema.metapack
( 54%) Producing: explorer/self/v1/schemas/api/schemas/trace/request/%/schema.metapack
( 54%) Producing: explorer/self/v1/schemas/api/schemas/trace/response/%/schema.metapack
( 54%) Producing: explorer/self/v1/schemas/mcp/error/%/schema.metapack
( 54%) Producing: explorer/self/v1/schemas/mcp/initialize/request/%/schema.metapack
( 55%) Producing: explorer/self/v1/schemas/mcp/initialize/response/%/schema.metapack
( 55%) Producing: explorer/self/v1/schemas/mcp/notifications/cancelled/%/schema.metapack
( 55%) Producing: explorer/self/v1/schemas/mcp/notifications/initialized/%/schema.metapack
( 55%) Producing: explorer/self/v1/schemas/mcp/ping/request/%/schema.metapack
( 55%) Producing: explorer/self/v1/schemas/mcp/ping/response/%/schema.metapack
( 55%) Producing: explorer/self/v1/schemas/mcp/request/%/schema.metapack
( 55%) Producing: explorer/self/v1/schemas/mcp/resources/list/request/%/schema.metapack
( 55%) Producing: explorer/self/v1/schemas/mcp/resources/list/response/%/schema.metapack
( 56%) Producing: explorer/self/v1/schemas/mcp/resources/read/request/%/schema.metapack
( 56%) Producing: explorer/self/v1/schemas/mcp/resources/read/response/%/schema.metapack
( 56%) Producing: explorer/self/v1/schemas/mcp/resources/templates/list/request/%/schema.metapack
( 56%) Producing: explorer/self/v1/schemas/mcp/resources/templates/list/response/%/schema.metapack
( 56%) Producing: explorer/self/v1/schemas/mcp/response/%/schema.metapack
( 56%) Producing: explorer/self/v1/schemas/mcp/tools/call/evaluate-schema/request/%/schema.metapack
( 56%) Producing: explorer/self/v1/schemas/mcp/tools/call/evaluate-schema/response/%/schema.metapack
( 56%) Producing: explorer/self/v1/schemas/mcp/tools/call/get-schema-dependencies/request/%/schema.metapack
( 56%) Producing: explorer/self/v1/schemas/mcp/tools/call/get-schema-dependencies/response/%/schema.metapack
( 57%) Producing: explorer/self/v1/schemas/mcp/tools/call/get-schema-dependents/request/%/schema.metapack
( 57%) Producing: explorer/self/v1/schemas/mcp/tools/call/get-schema-dependents/response/%/schema.metapack
( 57%) Producing: explorer/self/v1/schemas/mcp/tools/call/get-schema-health/request/%/schema.metapack
( 57%) Producing: explorer/self/v1/schemas/mcp/tools/call/get-schema-health/response/%/schema.metapack
( 57%) Producing: explorer/self/v1/schemas/mcp/tools/call/get-schema-locations/request/%/schema.metapack
( 57%) Producing: explorer/self/v1/schemas/mcp/tools/call/get-schema-locations/response/%/schema.metapack
( 57%) Producing: explorer/self/v1/schemas/mcp/tools/call/get-schema-metadata/request/%/schema.metapack
( 57%) Producing: explorer/self/v1/schemas/mcp/tools/call/get-schema-metadata/response/%/schema.metapack
( 58%) Producing: explorer/self/v1/schemas/mcp/tools/call/get-schema-positions/request/%/schema.metapack
( 58%) Producing: explorer/self/v1/schemas/mcp/tools/call/get-schema-positions/response/%/schema.metapack
( 58%) Producing: explorer/self/v1/schemas/mcp/tools/call/get-schema-stats/request/%/schema.metapack
( 58%) Producing: explorer/self/v1/schemas/mcp/tools/call/get-schema-stats/response/%/schema.metapack
( 58%) Producing: explorer/self/v1/schemas/mcp/tools/call/instance-to-rdf/request/%/schema.metapack
( 58%) Producing: explorer/self/v1/schemas/mcp/tools/call/instance-to-rdf/response/%/schema.metapack
( 58%) Producing: explorer/self/v1/schemas/mcp/tools/call/list-directory/request/%/schema.metapack
( 58%) Producing: explorer/self/v1/schemas/mcp/tools/call/list-directory/response/%/schema.metapack
( 58%) Producing: explorer/self/v1/schemas/mcp/tools/call/request/%/schema.metapack
( 59%) Producing: explorer/self/v1/schemas/mcp/tools/call/response/%/schema.metapack
( 59%) Producing: explorer/self/v1/schemas/mcp/tools/call/search-schemas/request/%/schema.metapack
( 59%) Producing: explorer/self/v1/schemas/mcp/tools/call/search-schemas/response/%/schema.metapack
( 59%) Producing: explorer/self/v1/schemas/mcp/tools/call/trace-schema-evaluation/request/%/schema.metapack
( 59%) Producing: explorer/self/v1/schemas/mcp/tools/call/trace-schema-evaluation/response/%/schema.metapack
( 59%) Producing: explorer/self/v1/schemas/mcp/tools/list/request/%/schema.metapack
( 59%) Producing: explorer/self/v1/schemas/mcp/tools/list/response/%/schema.metapack
( 59%) Producing: schemas/example/schemas/old/%/blaze-exhaustive.metapack
( 60%) Producing: schemas/example/schemas/old/%/blaze-fast.metapack
( 60%) Producing: schemas/example/schemas/old/%/editor.metapack
( 60%) Producing: schemas/self/v1/schemas/api/error/%/blaze-exhaustive.metapack
( 60%) Producing: schemas/self/v1/schemas/api/error/%/blaze-fast.metapack
( 60%) Producing: schemas/self/v1/schemas/api/error/%/editor.metapack
( 60%) Producing: schemas/self/v1/schemas/api/list/response/%/blaze-exhaustive.metapack
( 60%) Producing: schemas/self/v1/schemas/api/list/response/%/blaze-fast.metapack
( 60%) Producing: schemas/self/v1/schemas/api/list/response/%/editor.metapack
( 60%) Producing: schemas/self/v1/schemas/api/schemas/dependencies/response/%/blaze-exhaustive.metapack
( 61%) Producing: schemas/self/v1/schemas/api/schemas/dependencies/response/%/blaze-fast.metapack
( 61%) Producing: schemas/self/v1/schemas/api/schemas/dependencies/response/%/editor.metapack
( 61%) Producing: schemas/self/v1/schemas/api/schemas/dependents/response/%/blaze-exhaustive.metapack
( 61%) Producing: schemas/self/v1/schemas/api/schemas/dependents/response/%/blaze-fast.metapack
( 61%) Producing: schemas/self/v1/schemas/api/schemas/dependents/response/%/editor.metapack
( 61%) Producing: schemas/self/v1/schemas/api/schemas/evaluate/request/%/blaze-exhaustive.metapack
( 61%) Producing: schemas/self/v1/schemas/api/schemas/evaluate/request/%/blaze-fast.metapack
( 61%) Producing: schemas/self/v1/schemas/api/schemas/evaluate/request/%/editor.metapack
( 62%) Producing: schemas/self/v1/schemas/api/schemas/evaluate/response/%/blaze-exhaustive.metapack
( 62%) Producing: schemas/self/v1/schemas/api/schemas/evaluate/response/%/blaze-fast.metapack
( 62%) Producing: schemas/self/v1/schemas/api/schemas/evaluate/response/%/editor.metapack
( 62%) Producing: schemas/self/v1/schemas/api/schemas/health/response/%/blaze-exhaustive.metapack
( 62%) Producing: schemas/self/v1/schemas/api/schemas/health/response/%/blaze-fast.metapack
( 62%) Producing: schemas/self/v1/schemas/api/schemas/health/response/%/editor.metapack
( 62%) Producing: schemas/self/v1/schemas/api/schemas/locations/response/%/blaze-exhaustive.metapack
( 62%) Producing: schemas/self/v1/schemas/api/schemas/locations/response/%/blaze-fast.metapack
( 63%) Producing: schemas/self/v1/schemas/api/schemas/locations/response/%/editor.metapack
( 63%) Producing: schemas/self/v1/schemas/api/schemas/metadata/response/%/blaze-exhaustive.metapack
( 63%) Producing: schemas/self/v1/schemas/api/schemas/metadata/response/%/blaze-fast.metapack
( 63%) Producing: schemas/self/v1/schemas/api/schemas/metadata/response/%/editor.metapack
( 63%) Producing: schemas/self/v1/schemas/api/schemas/output-error/%/blaze-exhaustive.metapack
( 63%) Producing: schemas/self/v1/schemas/api/schemas/output-error/%/blaze-fast.metapack
( 63%) Producing: schemas/self/v1/schemas/api/schemas/output-error/%/editor.metapack
( 63%) Producing: schemas/self/v1/schemas/api/schemas/position/%/blaze-exhaustive.metapack
( 63%) Producing: schemas/self/v1/schemas/api/schemas/position/%/blaze-fast.metapack
( 64%) Producing: schemas/self/v1/schemas/api/schemas/position/%/editor.metapack
( 64%) Producing: schemas/self/v1/schemas/api/schemas/positions/response/%/blaze-exhaustive.metapack
( 64%) Producing: schemas/self/v1/schemas/api/schemas/positions/response/%/blaze-fast.metapack
( 64%) Producing: schemas/self/v1/schemas/api/schemas/positions/response/%/editor.metapack
( 64%) Producing: schemas/self/v1/schemas/api/schemas/rdf/request/%/blaze-exhaustive.metapack
( 64%) Producing: schemas/self/v1/schemas/api/schemas/rdf/request/%/blaze-fast.metapack
( 64%) Producing: schemas/self/v1/schemas/api/schemas/rdf/request/%/editor.metapack
( 64%) Producing: schemas/self/v1/schemas/api/schemas/rdf/response/%/blaze-exhaustive.metapack
( 65%) Producing: schemas/self/v1/schemas/api/schemas/rdf/response/%/blaze-fast.metapack
( 65%) Producing: schemas/self/v1/schemas/api/schemas/rdf/response/%/editor.metapack
( 65%) Producing: schemas/self/v1/schemas/api/schemas/search/response/%/blaze-exhaustive.metapack
( 65%) Producing: schemas/self/v1/schemas/api/schemas/search/response/%/blaze-fast.metapack
( 65%) Producing: schemas/self/v1/schemas/api/schemas/search/response/%/editor.metapack
( 65%) Producing: schemas/self/v1/schemas/api/schemas/stats/response/%/blaze-exhaustive.metapack
( 65%) Producing: schemas/self/v1/schemas/api/schemas/stats/response/%/blaze-fast.metapack
( 65%) Producing: schemas/self/v1/schemas/api/schemas/stats/response/%/editor.metapack
( 65%) Producing: schemas/self/v1/schemas/api/schemas/trace/request/%/blaze-exhaustive.metapack
( 66%) Producing: schemas/self/v1/schemas/api/schemas/trace/request/%/blaze-fast.metapack
( 66%) Producing: schemas/self/v1/schemas/api/schemas/trace/request/%/editor.metapack
( 66%) Producing: schemas/self/v1/schemas/api/schemas/trace/response/%/blaze-exhaustive.metapack
( 66%) Producing: schemas/self/v1/schemas/api/schemas/trace/response/%/blaze-fast.metapack
( 66%) Producing: schemas/self/v1/schemas/api/schemas/trace/response/%/editor.metapack
( 66%) Producing: schemas/self/v1/schemas/mcp/error/%/blaze-exhaustive.metapack
( 66%) Producing: schemas/self/v1/schemas/mcp/error/%/blaze-fast.metapack
( 66%) Producing: schemas/self/v1/schemas/mcp/error/%/editor.metapack
( 67%) Producing: schemas/self/v1/schemas/mcp/initialize/request/%/blaze-exhaustive.metapack
( 67%) Producing: schemas/self/v1/schemas/mcp/initialize/request/%/blaze-fast.metapack
( 67%) Producing: schemas/self/v1/schemas/mcp/initialize/request/%/editor.metapack
( 67%) Producing: schemas/self/v1/schemas/mcp/initialize/response/%/blaze-exhaustive.metapack
( 67%) Producing: schemas/self/v1/schemas/mcp/initialize/response/%/blaze-fast.metapack
( 67%) Producing: schemas/self/v1/schemas/mcp/initialize/response/%/editor.metapack
( 67%) Producing: schemas/self/v1/schemas/mcp/notifications/cancelled/%/blaze-exhaustive.metapack
( 67%) Producing: schemas/self/v1/schemas/mcp/notifications/cancelled/%/blaze-fast.metapack
( 68%) Producing: schemas/self/v1/schemas/mcp/notifications/cancelled/%/editor.metapack
( 68%) Producing: schemas/self/v1/schemas/mcp/notifications/initialized/%/blaze-exhaustive.metapack
( 68%) Producing: schemas/self/v1/schemas/mcp/notifications/initialized/%/blaze-fast.metapack
( 68%) Producing: schemas/self/v1/schemas/mcp/notifications/initialized/%/editor.metapack
( 68%) Producing: schemas/self/v1/schemas/mcp/ping/request/%/blaze-exhaustive.metapack
( 68%) Producing: schemas/self/v1/schemas/mcp/ping/request/%/blaze-fast.metapack
( 68%) Producing: schemas/self/v1/schemas/mcp/ping/request/%/editor.metapack
( 68%) Producing: schemas/self/v1/schemas/mcp/ping/response/%/blaze-exhaustive.metapack
( 68%) Producing: schemas/self/v1/schemas/mcp/ping/response/%/blaze-fast.metapack
( 69%) Producing: schemas/self/v1/schemas/mcp/ping/response/%/editor.metapack
( 69%) Producing: schemas/self/v1/schemas/mcp/request/%/blaze-exhaustive.metapack
( 69%) Producing: schemas/self/v1/schemas/mcp/request/%/blaze-fast.metapack
( 69%) Producing: schemas/self/v1/schemas/mcp/request/%/editor.metapack
( 69%) Producing: schemas/self/v1/schemas/mcp/resources/list/request/%/blaze-exhaustive.metapack
( 69%) Producing: schemas/self/v1/schemas/mcp/resources/list/request/%/blaze-fast.metapack
( 69%) Producing: schemas/self/v1/schemas/mcp/resources/list/request/%/editor.metapack
( 69%) Producing: schemas/self/v1/schemas/mcp/resources/list/response/%/blaze-exhaustive.metapack
( 70%) Producing: schemas/self/v1/schemas/mcp/resources/list/response/%/blaze-fast.metapack
( 70%) Producing: schemas/self/v1/schemas/mcp/resources/list/response/%/editor.metapack
( 70%) Producing: schemas/self/v1/schemas/mcp/resources/read/request/%/blaze-exhaustive.metapack
( 70%) Producing: schemas/self/v1/schemas/mcp/resources/read/request/%/blaze-fast.metapack
( 70%) Producing: schemas/self/v1/schemas/mcp/resources/read/request/%/editor.metapack
( 70%) Producing: schemas/self/v1/schemas/mcp/resources/read/response/%/blaze-exhaustive.metapack
( 70%) Producing: schemas/self/v1/schemas/mcp/resources/read/response/%/blaze-fast.metapack
( 70%) Producing: schemas/self/v1/schemas/mcp/resources/read/response/%/editor.metapack
( 70%) Producing: schemas/self/v1/schemas/mcp/resources/templates/list/request/%/blaze-exhaustive.metapack
( 71%) Producing: schemas/self/v1/schemas/mcp/resources/templates/list/request/%/blaze-fast.metapack
( 71%) Producing: schemas/self/v1/schemas/mcp/resources/templates/list/request/%/editor.metapack
( 71%) Producing: schemas/self/v1/schemas/mcp/resources/templates/list/response/%/blaze-exhaustive.metapack
( 71%) Producing: schemas/self/v1/schemas/mcp/resources/templates/list/response/%/blaze-fast.metapack
( 71%) Producing: schemas/self/v1/schemas/mcp/resources/templates/list/response/%/editor.metapack
( 71%) Producing: schemas/self/v1/schemas/mcp/response/%/blaze-exhaustive.metapack
( 71%) Producing: schemas/self/v1/schemas/mcp/response/%/blaze-fast.metapack
( 71%) Producing: schemas/self/v1/schemas/mcp/response/%/editor.metapack
( 72%) Producing: schemas/self/v1/schemas/mcp/tools/call/evaluate-schema/request/%/blaze-exhaustive.metapack
( 72%) Producing: schemas/self/v1/schemas/mcp/tools/call/evaluate-schema/request/%/blaze-fast.metapack
( 72%) Producing: schemas/self/v1/schemas/mcp/tools/call/evaluate-schema/request/%/editor.metapack
( 72%) Producing: schemas/self/v1/schemas/mcp/tools/call/evaluate-schema/response/%/blaze-exhaustive.metapack
( 72%) Producing: schemas/self/v1/schemas/mcp/tools/call/evaluate-schema/response/%/blaze-fast.metapack
( 72%) Producing: schemas/self/v1/schemas/mcp/tools/call/evaluate-schema/response/%/editor.metapack
( 72%) Producing: schemas/self/v1/schemas/mcp/tools/call/get-schema-dependencies/request/%/blaze-exhaustive.metapack
( 72%) Producing: schemas/self/v1/schemas/mcp/tools/call/get-schema-dependencies/request/%/blaze-fast.metapack
( 73%) Producing: schemas/self/v1/schemas/mcp/tools/call/get-schema-dependencies/request/%/editor.metapack
( 73%) Producing: schemas/self/v1/schemas/mcp/tools/call/get-schema-dependencies/response/%/blaze-exhaustive.metapack
( 73%) Producing: schemas/self/v1/schemas/mcp/tools/call/get-schema-dependencies/response/%/blaze-fast.metapack
( 73%) Producing: schemas/self/v1/schemas/mcp/tools/call/get-schema-dependencies/response/%/editor.metapack
( 73%) Producing: schemas/self/v1/schemas/mcp/tools/call/get-schema-dependents/request/%/blaze-exhaustive.metapack
( 73%) Producing: schemas/self/v1/schemas/mcp/tools/call/get-schema-dependents/request/%/blaze-fast.metapack
( 73%) Producing: schemas/self/v1/schemas/mcp/tools/call/get-schema-dependents/request/%/editor.metapack
( 73%) Producing: schemas/self/v1/schemas/mcp/tools/call/get-schema-dependents/response/%/blaze-exhaustive.metapack
( 73%) Producing: schemas/self/v1/schemas/mcp/tools/call/get-schema-dependents/response/%/blaze-fast.metapack
( 74%) Producing: schemas/self/v1/schemas/mcp/tools/call/get-schema-dependents/response/%/editor.metapack
( 74%) Producing: schemas/self/v1/schemas/mcp/tools/call/get-schema-health/request/%/blaze-exhaustive.metapack
( 74%) Producing: schemas/self/v1/schemas/mcp/tools/call/get-schema-health/request/%/blaze-fast.metapack
( 74%) Producing: schemas/self/v1/schemas/mcp/tools/call/get-schema-health/request/%/editor.metapack
( 74%) Producing: schemas/self/v1/schemas/mcp/tools/call/get-schema-health/response/%/blaze-exhaustive.metapack
( 74%) Producing: schemas/self/v1/schemas/mcp/tools/call/get-schema-health/response/%/blaze-fast.metapack
( 74%) Producing: schemas/self/v1/schemas/mcp/tools/call/get-schema-health/response/%/editor.metapack
( 74%) Producing: schemas/self/v1/schemas/mcp/tools/call/get-schema-locations/request/%/blaze-exhaustive.metapack
( 75%) Producing: schemas/self/v1/schemas/mcp/tools/call/get-schema-locations/request/%/blaze-fast.metapack
( 75%) Producing: schemas/self/v1/schemas/mcp/tools/call/get-schema-locations/request/%/editor.metapack
( 75%) Producing: schemas/self/v1/schemas/mcp/tools/call/get-schema-locations/response/%/blaze-exhaustive.metapack
( 75%) Producing: schemas/self/v1/schemas/mcp/tools/call/get-schema-locations/response/%/blaze-fast.metapack
( 75%) Producing: schemas/self/v1/schemas/mcp/tools/call/get-schema-locations/response/%/editor.metapack
( 75%) Producing: schemas/self/v1/schemas/mcp/tools/call/get-schema-metadata/request/%/blaze-exhaustive.metapack
( 75%) Producing: schemas/self/v1/schemas/mcp/tools/call/get-schema-metadata/request/%/blaze-fast.metapack
( 75%) Producing: schemas/self/v1/schemas/mcp/tools/call/get-schema-metadata/request/%/editor.metapack
( 75%) Producing: schemas/self/v1/schemas/mcp/tools/call/get-schema-metadata/response/%/blaze-exhaustive.metapack
( 76%) Producing: schemas/self/v1/schemas/mcp/tools/call/get-schema-metadata/response/%/blaze-fast.metapack
( 76%) Producing: schemas/self/v1/schemas/mcp/tools/call/get-schema-metadata/response/%/editor.metapack
( 76%) Producing: schemas/self/v1/schemas/mcp/tools/call/get-schema-positions/request/%/blaze-exhaustive.metapack
( 76%) Producing: schemas/self/v1/schemas/mcp/tools/call/get-schema-positions/request/%/blaze-fast.metapack
( 76%) Producing: schemas/self/v1/schemas/mcp/tools/call/get-schema-positions/request/%/editor.metapack
( 76%) Producing: schemas/self/v1/schemas/mcp/tools/call/get-schema-positions/response/%/blaze-exhaustive.metapack
( 76%) Producing: schemas/self/v1/schemas/mcp/tools/call/get-schema-positions/response/%/blaze-fast.metapack
( 76%) Producing: schemas/self/v1/schemas/mcp/tools/call/get-schema-positions/response/%/editor.metapack
( 77%) Producing: schemas/self/v1/schemas/mcp/tools/call/get-schema-stats/request/%/blaze-exhaustive.metapack
( 77%) Producing: schemas/self/v1/schemas/mcp/tools/call/get-schema-stats/request/%/blaze-fast.metapack
( 77%) Producing: schemas/self/v1/schemas/mcp/tools/call/get-schema-stats/request/%/editor.metapack
( 77%) Producing: schemas/self/v1/schemas/mcp/tools/call/get-schema-stats/response/%/blaze-exhaustive.metapack
( 77%) Producing: schemas/self/v1/schemas/mcp/tools/call/get-schema-stats/response/%/blaze-fast.metapack
( 77%) Producing: schemas/self/v1/schemas/mcp/tools/call/get-schema-stats/response/%/editor.metapack
( 77%) Producing: schemas/self/v1/schemas/mcp/tools/call/instance-to-rdf/request/%/blaze-exhaustive.metapack
( 77%) Producing: schemas/self/v1/schemas/mcp/tools/call/instance-to-rdf/request/%/blaze-fast.metapack
( 78%) Producing: schemas/self/v1/schemas/mcp/tools/call/instance-to-rdf/request/%/editor.metapack
( 78%) Producing: schemas/self/v1/schemas/mcp/tools/call/instance-to-rdf/response/%/blaze-exhaustive.metapack
( 78%) Producing: schemas/self/v1/schemas/mcp/tools/call/instance-to-rdf/response/%/blaze-fast.metapack
( 78%) Producing: schemas/self/v1/schemas/mcp/tools/call/instance-to-rdf/response/%/editor.metapack
( 78%) Producing: schemas/self/v1/schemas/mcp/tools/call/list-directory/request/%/blaze-exhaustive.metapack
( 78%) Producing: schemas/self/v1/schemas/mcp/tools/call/list-directory/request/%/blaze-fast.metapack
( 78%) Producing: schemas/self/v1/schemas/mcp/tools/call/list-directory/request/%/editor.metapack
( 78%) Producing: schemas/self/v1/schemas/mcp/tools/call/list-directory/response/%/blaze-exhaustive.metapack
( 78%) Producing: schemas/self/v1/schemas/mcp/tools/call/list-directory/response/%/blaze-fast.metapack
( 79%) Producing: schemas/self/v1/schemas/mcp/tools/call/list-directory/response/%/editor.metapack
( 79%) Producing: schemas/self/v1/schemas/mcp/tools/call/request/%/blaze-exhaustive.metapack
( 79%) Producing: schemas/self/v1/schemas/mcp/tools/call/request/%/blaze-fast.metapack
( 79%) Producing: schemas/self/v1/schemas/mcp/tools/call/request/%/editor.metapack
( 79%) Producing: schemas/self/v1/schemas/mcp/tools/call/response/%/blaze-exhaustive.metapack
( 79%) Producing: schemas/self/v1/schemas/mcp/tools/call/response/%/blaze-fast.metapack
( 79%) Producing: schemas/self/v1/schemas/mcp/tools/call/response/%/editor.metapack
( 79%) Producing: schemas/self/v1/schemas/mcp/tools/call/search-schemas/request/%/blaze-exhaustive.metapack
( 80%) Producing: schemas/self/v1/schemas/mcp/tools/call/search-schemas/request/%/blaze-fast.metapack
( 80%) Producing: schemas/self/v1/schemas/mcp/tools/call/search-schemas/request/%/editor.metapack
( 80%) Producing: schemas/self/v1/schemas/mcp/tools/call/search-schemas/response/%/blaze-exhaustive.metapack
( 80%) Producing: schemas/self/v1/schemas/mcp/tools/call/search-schemas/response/%/blaze-fast.metapack
( 80%) Producing: schemas/self/v1/schemas/mcp/tools/call/search-schemas/response/%/editor.metapack
( 80%) Producing: schemas/self/v1/schemas/mcp/tools/call/trace-schema-evaluation/request/%/blaze-exhaustive.metapack
( 80%) Producing: schemas/self/v1/schemas/mcp/tools/call/trace-schema-evaluation/request/%/blaze-fast.metapack
( 80%) Producing: schemas/self/v1/schemas/mcp/tools/call/trace-schema-evaluation/request/%/editor.metapack
( 80%) Producing: schemas/self/v1/schemas/mcp/tools/call/trace-schema-evaluation/response/%/blaze-exhaustive.metapack
( 81%) Producing: schemas/self/v1/schemas/mcp/tools/call/trace-schema-evaluation/response/%/blaze-fast.metapack
( 81%) Producing: schemas/self/v1/schemas/mcp/tools/call/trace-schema-evaluation/response/%/editor.metapack
( 81%) Producing: schemas/self/v1/schemas/mcp/tools/list/request/%/blaze-exhaustive.metapack
( 81%) Producing: schemas/self/v1/schemas/mcp/tools/list/request/%/blaze-fast.metapack
( 81%) Producing: schemas/self/v1/schemas/mcp/tools/list/request/%/editor.metapack
( 81%) Producing: schemas/self/v1/schemas/mcp/tools/list/response/%/blaze-exhaustive.metapack
( 81%) Producing: schemas/self/v1/schemas/mcp/tools/list/response/%/blaze-fast.metapack
( 81%) Producing: schemas/self/v1/schemas/mcp/tools/list/response/%/editor.metapack
( 82%) Producing: explorer/example/schemas/%/directory.metapack
( 82%) Producing: explorer/example/schemas/old/%/schema-html.metapack
( 82%) Producing: explorer/self/v1/schemas/api/error/%/schema-html.metapack
( 82%) Producing: explorer/self/v1/schemas/api/list/%/directory.metapack
( 82%) Producing: explorer/self/v1/schemas/api/list/response/%/schema-html.metapack
( 82%) Producing: explorer/self/v1/schemas/api/schemas/dependencies/%/directory.metapack
( 82%) Producing: explorer/self/v1/schemas/api/schemas/dependencies/response/%/schema-html.metapack
( 82%) Producing: explorer/self/v1/schemas/api/schemas/dependents/%/directory.metapack
( 82%) Producing: explorer/self/v1/schemas/api/schemas/dependents/response/%/schema-html.metapack
( 83%) Producing: explorer/self/v1/schemas/api/schemas/evaluate/%/directory.metapack
( 83%) Producing: explorer/self/v1/schemas/api/schemas/evaluate/request/%/schema-html.metapack
( 83%) Producing: explorer/self/v1/schemas/api/schemas/evaluate/response/%/schema-html.metapack
( 83%) Producing: explorer/self/v1/schemas/api/schemas/health/%/directory.metapack
( 83%) Producing: explorer/self/v1/schemas/api/schemas/health/response/%/schema-html.metapack
( 83%) Producing: explorer/self/v1/schemas/api/schemas/locations/%/directory.metapack
( 83%) Producing: explorer/self/v1/schemas/api/schemas/locations/response/%/schema-html.metapack
( 83%) Producing: explorer/self/v1/schemas/api/schemas/metadata/%/directory.metapack
( 84%) Producing: explorer/self/v1/schemas/api/schemas/metadata/response/%/schema-html.metapack
( 84%) Producing: explorer/self/v1/schemas/api/schemas/output-error/%/schema-html.metapack
( 84%) Producing: explorer/self/v1/schemas/api/schemas/position/%/schema-html.metapack
( 84%) Producing: explorer/self/v1/schemas/api/schemas/positions/%/directory.metapack
( 84%) Producing: explorer/self/v1/schemas/api/schemas/positions/response/%/schema-html.metapack
( 84%) Producing: explorer/self/v1/schemas/api/schemas/rdf/%/directory.metapack
( 84%) Producing: explorer/self/v1/schemas/api/schemas/rdf/request/%/schema-html.metapack
( 84%) Producing: explorer/self/v1/schemas/api/schemas/rdf/response/%/schema-html.metapack
( 85%) Producing: explorer/self/v1/schemas/api/schemas/search/%/directory.metapack
( 85%) Producing: explorer/self/v1/schemas/api/schemas/search/response/%/schema-html.metapack
( 85%) Producing: explorer/self/v1/schemas/api/schemas/stats/%/directory.metapack
( 85%) Producing: explorer/self/v1/schemas/api/schemas/stats/response/%/schema-html.metapack
( 85%) Producing: explorer/self/v1/schemas/api/schemas/trace/%/directory.metapack
( 85%) Producing: explorer/self/v1/schemas/api/schemas/trace/request/%/schema-html.metapack
( 85%) Producing: explorer/self/v1/schemas/api/schemas/trace/response/%/schema-html.metapack
( 85%) Producing: explorer/self/v1/schemas/mcp/error/%/schema-html.metapack
( 85%) Producing: explorer/self/v1/schemas/mcp/initialize/%/directory.metapack
( 86%) Producing: explorer/self/v1/schemas/mcp/initialize/request/%/schema-html.metapack
( 86%) Producing: explorer/self/v1/schemas/mcp/initialize/response/%/schema-html.metapack
( 86%) Producing: explorer/self/v1/schemas/mcp/notifications/%/directory.metapack
( 86%) Producing: explorer/self/v1/schemas/mcp/notifications/cancelled/%/schema-html.metapack
( 86%) Producing: explorer/self/v1/schemas/mcp/notifications/initialized/%/schema-html.metapack
( 86%) Producing: explorer/self/v1/schemas/mcp/ping/%/directory.metapack
( 86%) Producing: explorer/self/v1/schemas/mcp/ping/request/%/schema-html.metapack
( 86%) Producing: explorer/self/v1/schemas/mcp/ping/response/%/schema-html.metapack
( 87%) Producing: explorer/self/v1/schemas/mcp/request/%/schema-html.metapack
( 87%) Producing: explorer/self/v1/schemas/mcp/resources/list/%/directory.metapack
( 87%) Producing: explorer/self/v1/schemas/mcp/resources/list/request/%/schema-html.metapack
( 87%) Producing: explorer/self/v1/schemas/mcp/resources/list/response/%/schema-html.metapack
( 87%) Producing: explorer/self/v1/schemas/mcp/resources/read/%/directory.metapack
( 87%) Producing: explorer/self/v1/schemas/mcp/resources/read/request/%/schema-html.metapack
( 87%) Producing: explorer/self/v1/schemas/mcp/resources/read/response/%/schema-html.metapack
( 87%) Producing: explorer/self/v1/schemas/mcp/resources/templates/list/%/directory.metapack
( 87%) Producing: explorer/self/v1/schemas/mcp/resources/templates/list/request/%/schema-html.metapack
( 88%) Producing: explorer/self/v1/schemas/mcp/resources/templates/list/response/%/schema-html.metapack
( 88%) Producing: explorer/self/v1/schemas/mcp/response/%/schema-html.metapack
( 88%) Producing: explorer/self/v1/schemas/mcp/tools/call/evaluate-schema/%/directory.metapack
( 88%) Producing: explorer/self/v1/schemas/mcp/tools/call/evaluate-schema/request/%/schema-html.metapack
( 88%) Producing: explorer/self/v1/schemas/mcp/tools/call/evaluate-schema/response/%/schema-html.metapack
( 88%) Producing: explorer/self/v1/schemas/mcp/tools/call/get-schema-dependencies/%/directory.metapack
( 88%) Producing: explorer/self/v1/schemas/mcp/tools/call/get-schema-dependencies/request/%/schema-html.metapack
( 88%) Producing: explorer/self/v1/schemas/mcp/tools/call/get-schema-dependencies/response/%/schema-html.metapack
( 89%) Producing: explorer/self/v1/schemas/mcp/tools/call/get-schema-dependents/%/directory.metapack
( 89%) Producing: explorer/self/v1/schemas/mcp/tools/call/get-schema-dependents/request/%/schema-html.metapack
( 89%) Producing: explorer/self/v1/schemas/mcp/tools/call/get-schema-dependents/response/%/schema-html.metapack
( 89%) Producing: explorer/self/v1/schemas/mcp/tools/call/get-schema-health/%/directory.metapack
( 89%) Producing: explorer/self/v1/schemas/mcp/tools/call/get-schema-health/request/%/schema-html.metapack
( 89%) Producing: explorer/self/v1/schemas/mcp/tools/call/get-schema-health/response/%/schema-html.metapack
( 89%) Producing: explorer/self/v1/schemas/mcp/tools/call/get-schema-locations/%/directory.metapack
( 89%) Producing: explorer/self/v1/schemas/mcp/tools/call/get-schema-locations/request/%/schema-html.metapack
( 90%) Producing: explorer/self/v1/schemas/mcp/tools/call/get-schema-locations/response/%/schema-html.metapack
( 90%) Producing: explorer/self/v1/schemas/mcp/tools/call/get-schema-metadata/%/directory.metapack
( 90%) Producing: explorer/self/v1/schemas/mcp/tools/call/get-schema-metadata/request/%/schema-html.metapack
( 90%) Producing: explorer/self/v1/schemas/mcp/tools/call/get-schema-metadata/response/%/schema-html.metapack
( 90%) Producing: explorer/self/v1/schemas/mcp/tools/call/get-schema-positions/%/directory.metapack
( 90%) Producing: explorer/self/v1/schemas/mcp/tools/call/get-schema-positions/request/%/schema-html.metapack
( 90%) Producing: explorer/self/v1/schemas/mcp/tools/call/get-schema-positions/response/%/schema-html.metapack
( 90%) Producing: explorer/self/v1/schemas/mcp/tools/call/get-schema-stats/%/directory.metapack
( 90%) Producing: explorer/self/v1/schemas/mcp/tools/call/get-schema-stats/request/%/schema-html.metapack
( 91%) Producing: explorer/self/v1/schemas/mcp/tools/call/get-schema-stats/response/%/schema-html.metapack
( 91%) Producing: explorer/self/v1/schemas/mcp/tools/call/instance-to-rdf/%/directory.metapack
( 91%) Producing: explorer/self/v1/schemas/mcp/tools/call/instance-to-rdf/request/%/schema-html.metapack
( 91%) Producing: explorer/self/v1/schemas/mcp/tools/call/instance-to-rdf/response/%/schema-html.metapack
( 91%) Producing: explorer/self/v1/schemas/mcp/tools/call/list-directory/%/directory.metapack
( 91%) Producing: explorer/self/v1/schemas/mcp/tools/call/list-directory/request/%/schema-html.metapack
( 91%) Producing: explorer/self/v1/schemas/mcp/tools/call/list-directory/response/%/schema-html.metapack
( 91%) Producing: explorer/self/v1/schemas/mcp/tools/call/request/%/schema-html.metapack
( 92%) Producing: explorer/self/v1/schemas/mcp/tools/call/response/%/schema-html.metapack
( 92%) Producing: explorer/self/v1/schemas/mcp/tools/call/search-schemas/%/directory.metapack
( 92%) Producing: explorer/self/v1/schemas/mcp/tools/call/search-schemas/request/%/schema-html.metapack
( 92%) Producing: explorer/self/v1/schemas/mcp/tools/call/search-schemas/response/%/schema-html.metapack
( 92%) Producing: explorer/self/v1/schemas/mcp/tools/call/trace-schema-evaluation/%/directory.metapack
( 92%) Producing: explorer/self/v1/schemas/mcp/tools/call/trace-schema-evaluation/request/%/schema-html.metapack
( 92%) Producing: explorer/self/v1/schemas/mcp/tools/call/trace-schema-evaluation/response/%/schema-html.metapack
( 92%) Producing: explorer/self/v1/schemas/mcp/tools/list/%/directory.metapack
( 92%) Producing: explorer/self/v1/schemas/mcp/tools/list/request/%/schema-html.metapack
( 93%) Producing: explorer/self/v1/schemas/mcp/tools/list/response/%/schema-html.metapack
( 93%) Producing: explorer/example/%/directory.metapack
( 93%) Producing: explorer/example/schemas/%/directory-html.metapack
( 93%) Producing: explorer/self/v1/schemas/api/list/%/directory-html.metapack
( 93%) Producing: explorer/self/v1/schemas/api/schemas/%/directory.metapack
( 93%) Producing: explorer/self/v1/schemas/api/schemas/dependencies/%/directory-html.metapack
( 93%) Producing: explorer/self/v1/schemas/api/schemas/dependents/%/directory-html.metapack
( 93%) Producing: explorer/self/v1/schemas/api/schemas/evaluate/%/directory-html.metapack
( 94%) Producing: explorer/self/v1/schemas/api/schemas/health/%/directory-html.metapack
( 94%) Producing: explorer/self/v1/schemas/api/schemas/locations/%/directory-html.metapack
( 94%) Producing: explorer/self/v1/schemas/api/schemas/metadata/%/directory-html.metapack
( 94%) Producing: explorer/self/v1/schemas/api/schemas/positions/%/directory-html.metapack
( 94%) Producing: explorer/self/v1/schemas/api/schemas/rdf/%/directory-html.metapack
( 94%) Producing: explorer/self/v1/schemas/api/schemas/search/%/directory-html.metapack
( 94%) Producing: explorer/self/v1/schemas/api/schemas/stats/%/directory-html.metapack
( 94%) Producing: explorer/self/v1/schemas/api/schemas/trace/%/directory-html.metapack
( 95%) Producing: explorer/self/v1/schemas/mcp/initialize/%/directory-html.metapack
( 95%) Producing: explorer/self/v1/schemas/mcp/notifications/%/directory-html.metapack
( 95%) Producing: explorer/self/v1/schemas/mcp/ping/%/directory-html.metapack
( 95%) Producing: explorer/self/v1/schemas/mcp/resources/list/%/directory-html.metapack
( 95%) Producing: explorer/self/v1/schemas/mcp/resources/read/%/directory-html.metapack
( 95%) Producing: explorer/self/v1/schemas/mcp/resources/templates/%/directory.metapack
( 95%) Producing: explorer/self/v1/schemas/mcp/resources/templates/list/%/directory-html.metapack
( 95%) Producing: explorer/self/v1/schemas/mcp/tools/call/%/directory.metapack
( 95%) Producing: explorer/self/v1/schemas/mcp/tools/call/evaluate-schema/%/directory-html.metapack
( 96%) Producing: explorer/self/v1/schemas/mcp/tools/call/get-schema-dependencies/%/directory-html.metapack
( 96%) Producing: explorer/self/v1/schemas/mcp/tools/call/get-schema-dependents/%/directory-html.metapack
( 96%) Producing: explorer/self/v1/schemas/mcp/tools/call/get-schema-health/%/directory-html.metapack
( 96%) Producing: explorer/self/v1/schemas/mcp/tools/call/get-schema-locations/%/directory-html.metapack
( 96%) Producing: explorer/self/v1/schemas/mcp/tools/call/get-schema-metadata/%/directory-html.metapack
( 96%) Producing: explorer/self/v1/schemas/mcp/tools/call/get-schema-positions/%/directory-html.metapack
( 96%) Producing: explorer/self/v1/schemas/mcp/tools/call/get-schema-stats/%/directory-html.metapack
( 96%) Producing: explorer/self/v1/schemas/mcp/tools/call/instance-to-rdf/%/directory-html.metapack
( 97%) Producing: explorer/self/v1/schemas/mcp/tools/call/list-directory/%/directory-html.metapack
( 97%) Producing: explorer/self/v1/schemas/mcp/tools/call/search-schemas/%/directory-html.metapack
( 97%) Producing: explorer/self/v1/schemas/mcp/tools/call/trace-schema-evaluation/%/directory-html.metapack
( 97%) Producing: explorer/self/v1/schemas/mcp/tools/list/%/directory-html.metapack
( 97%) Producing: explorer/example/%/directory-html.metapack
( 97%) Producing: explorer/self/v1/schemas/api/%/directory.metapack
( 97%) Producing: explorer/self/v1/schemas/api/schemas/%/directory-html.metapack
( 97%) Producing: explorer/self/v1/schemas/mcp/resources/%/directory.metapack
( 97%) Producing: explorer/self/v1/schemas/mcp/resources/templates/%/directory-html.metapack
( 98%) Producing: explorer/self/v1/schemas/mcp/tools/%/directory.metapack
( 98%) Producing: explorer/self/v1/schemas/mcp/tools/call/%/directory-html.metapack
( 98%) Producing: explorer/self/v1/schemas/api/%/directory-html.metapack
( 98%) Producing: explorer/self/v1/schemas/mcp/%/directory.metapack
( 98%) Producing: explorer/self/v1/schemas/mcp/resources/%/directory-html.metapack
( 98%) Producing: explorer/self/v1/schemas/mcp/tools/%/directory-html.metapack
( 98%) Producing: explorer/self/v1/schemas/%/directory.metapack
( 98%) Producing: explorer/self/v1/schemas/mcp/%/directory-html.metapack
( 99%) Producing: explorer/self/v1/%/directory.metapack
( 99%) Producing: explorer/self/v1/schemas/%/directory-html.metapack
( 99%) Producing: explorer/self/%/directory.metapack
( 99%) Producing: explorer/self/v1/%/directory-html.metapack
( 99%) Producing: explorer/%/directory.metapack
( 99%) Producing: explorer/self/%/directory-html.metapack
( 99%) Producing: explorer/%/directory-html.metapack
( 99%) Producing: explorer/%/search.metapack
(100%) Producing: explorer/%/mcp.metapack
(  1%) Combining: schemas/example/schemas/old/%/dependents.metapack
(  3%) Combining: schemas/self/v1/schemas/api/error/%/dependents.metapack
(  4%) Combining: schemas/self/v1/schemas/api/list/response/%/dependents.metapack
(  6%) Combining: schemas/self/v1/schemas/api/schemas/dependencies/response/%/dependents.metapack
(  8%) Combining: schemas/self/v1/schemas/api/schemas/dependents/response/%/dependents.metapack
(  9%) Combining: schemas/self/v1/schemas/api/schemas/evaluate/request/%/dependents.metapack
( 11%) Combining: schemas/self/v1/schemas/api/schemas/evaluate/response/%/dependents.metapack
( 12%) Combining: schemas/self/v1/schemas/api/schemas/health/response/%/dependents.metapack
( 14%) Combining: schemas/self/v1/schemas/api/schemas/locations/response/%/dependents.metapack
( 16%) Combining: schemas/self/v1/schemas/api/schemas/metadata/response/%/dependents.metapack
( 17%) Combining: schemas/self/v1/schemas/api/schemas/output-error/%/dependents.metapack
( 19%) Combining: schemas/self/v1/schemas/api/schemas/position/%/dependents.metapack
( 20%) Combining: schemas/self/v1/schemas/api/schemas/positions/response/%/dependents.metapack
( 22%) Combining: schemas/self/v1/schemas/api/schemas/rdf/request/%/dependents.metapack
( 24%) Combining: schemas/self/v1/schemas/api/schemas/rdf/response/%/dependents.metapack
( 25%) Combining: schemas/self/v1/schemas/api/schemas/search/response/%/dependents.metapack
( 27%) Combining: schemas/self/v1/schemas/api/schemas/stats/response/%/dependents.metapack
( 29%) Combining: schemas/self/v1/schemas/api/schemas/trace/request/%/dependents.metapack
( 30%) Combining: schemas/self/v1/schemas/api/schemas/trace/response/%/dependents.metapack
( 32%) Combining: schemas/self/v1/schemas/mcp/error/%/dependents.metapack
( 33%) Combining: schemas/self/v1/schemas/mcp/initialize/request/%/dependents.metapack
( 35%) Combining: schemas/self/v1/schemas/mcp/initialize/response/%/dependents.metapack
( 37%) Combining: schemas/self/v1/schemas/mcp/notifications/cancelled/%/dependents.metapack
( 38%) Combining: schemas/self/v1/schemas/mcp/notifications/initialized/%/dependents.metapack
( 40%) Combining: schemas/self/v1/schemas/mcp/ping/request/%/dependents.metapack
( 41%) Combining: schemas/self/v1/schemas/mcp/ping/response/%/dependents.metapack
( 43%) Combining: schemas/self/v1/schemas/mcp/request/%/dependents.metapack
( 45%) Combining: schemas/self/v1/schemas/mcp/resources/list/request/%/dependents.metapack
( 46%) Combining: schemas/self/v1/schemas/mcp/resources/list/response/%/dependents.metapack
( 48%) Combining: schemas/self/v1/schemas/mcp/resources/read/request/%/dependents.metapack
( 50%) Combining: schemas/self/v1/schemas/mcp/resources/read/response/%/dependents.metapack
( 51%) Combining: schemas/self/v1/schemas/mcp/resources/templates/list/request/%/dependents.metapack
( 53%) Combining: schemas/self/v1/schemas/mcp/resources/templates/list/response/%/dependents.metapack
( 54%) Combining: schemas/self/v1/schemas/mcp/response/%/dependents.metapack
( 56%) Combining: schemas/self/v1/schemas/mcp/tools/call/evaluate-schema/request/%/dependents.metapack
( 58%) Combining: schemas/self/v1/schemas/mcp/tools/call/evaluate-schema/response/%/dependents.metapack
( 59%) Combining: schemas/self/v1/schemas/mcp/tools/call/get-schema-dependencies/request/%/dependents.metapack
( 61%) Combining: schemas/self/v1/schemas/mcp/tools/call/get-schema-dependencies/response/%/dependents.metapack
( 62%) Combining: schemas/self/v1/schemas/mcp/tools/call/get-schema-dependents/request/%/dependents.metapack
( 64%) Combining: schemas/self/v1/schemas/mcp/tools/call/get-schema-dependents/response/%/dependents.metapack
( 66%) Combining: schemas/self/v1/schemas/mcp/tools/call/get-schema-health/request/%/dependents.metapack
( 67%) Combining: schemas/self/v1/schemas/mcp/tools/call/get-schema-health/response/%/dependents.metapack
( 69%) Combining: schemas/self/v1/schemas/mcp/tools/call/get-schema-locations/request/%/dependents.metapack
( 70%) Combining: schemas/self/v1/schemas/mcp/tools/call/get-schema-locations/response/%/dependents.metapack
( 72%) Combining: schemas/self/v1/schemas/mcp/tools/call/get-schema-metadata/request/%/dependents.metapack
( 74%) Combining: schemas/self/v1/schemas/mcp/tools/call/get-schema-metadata/response/%/dependents.metapack
( 75%) Combining: schemas/self/v1/schemas/mcp/tools/call/get-schema-positions/request/%/dependents.metapack
( 77%) Combining: schemas/self/v1/schemas/mcp/tools/call/get-schema-positions/response/%/dependents.metapack
( 79%) Combining: schemas/self/v1/schemas/mcp/tools/call/get-schema-stats/request/%/dependents.metapack
( 80%) Combining: schemas/self/v1/schemas/mcp/tools/call/get-schema-stats/response/%/dependents.metapack
( 82%) Combining: schemas/self/v1/schemas/mcp/tools/call/instance-to-rdf/request/%/dependents.metapack
( 83%) Combining: schemas/self/v1/schemas/mcp/tools/call/instance-to-rdf/response/%/dependents.metapack
( 85%) Combining: schemas/self/v1/schemas/mcp/tools/call/list-directory/request/%/dependents.metapack
( 87%) Combining: schemas/self/v1/schemas/mcp/tools/call/list-directory/response/%/dependents.metapack
( 88%) Combining: schemas/self/v1/schemas/mcp/tools/call/request/%/dependents.metapack
( 90%) Combining: schemas/self/v1/schemas/mcp/tools/call/response/%/dependents.metapack
( 91%) Combining: schemas/self/v1/schemas/mcp/tools/call/search-schemas/request/%/dependents.metapack
( 93%) Combining: schemas/self/v1/schemas/mcp/tools/call/search-schemas/response/%/dependents.metapack
( 95%) Combining: schemas/self/v1/schemas/mcp/tools/call/trace-schema-evaluation/request/%/dependents.metapack
( 96%) Combining: schemas/self/v1/schemas/mcp/tools/call/trace-schema-evaluation/response/%/dependents.metapack
( 98%) Combining: schemas/self/v1/schemas/mcp/tools/list/request/%/dependents.metapack
(100%) Combining: schemas/self/v1/schemas/mcp/tools/list/response/%/dependents.metapack
EOF
diff "$TMP/output.txt" "$TMP/expected.txt"

exists() {
  [ -f "$1" ] || (echo "File MUST exist: $1" 1>&2 && exit 1)
}

not_exists() {
  [ ! -f "$1" ] || (echo "File MUST NOT exists: $1" 1>&2 && exit 1)
}

exists "$TMP/output/explorer/example/schemas/old/%/schema.metapack"
exists "$TMP/output/schemas/example/schemas/old/%/bundle.metapack"
exists "$TMP/output/schemas/example/schemas/old/%/editor.metapack"
exists "$TMP/output/schemas/example/schemas/old/%/blaze-exhaustive.metapack"
exists "$TMP/output/schemas/example/schemas/old/%/schema.metapack"

cat << 'EOF' > "$TMP/schemas/test.json"
{
  "$schema": "http://json-schema.org/draft-07/schema#",
  "$id": "https://example.com/new"
}
EOF

"$1" --skip-banner --deterministic "$TMP/one.json" "$TMP/output" --concurrency 1 2> "$TMP/output.txt"
remove_threads_information "$TMP/output.txt"

cat << EOF > "$TMP/expected.txt"
Writing output to: $(realpath "$TMP")/output
Using configuration: $(realpath "$TMP")/one.json
Detecting: $(realpath "$TMP")/schemas/test.json (#1)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/api/error.json (#2)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/api/list/response.json (#3)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/api/schemas/dependencies/response.json (#4)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/api/schemas/dependents/response.json (#5)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/api/schemas/evaluate/request.json (#6)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/api/schemas/evaluate/response.json (#7)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/api/schemas/health/response.json (#8)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/api/schemas/locations/response.json (#9)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/api/schemas/metadata/response.json (#10)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/api/schemas/output-error.json (#11)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/api/schemas/position.json (#12)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/api/schemas/positions/response.json (#13)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/api/schemas/rdf/request.json (#14)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/api/schemas/rdf/response.json (#15)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/api/schemas/search/response.json (#16)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/api/schemas/stats/response.json (#17)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/api/schemas/trace/request.json (#18)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/api/schemas/trace/response.json (#19)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/error.json (#20)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/initialize/request.json (#21)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/initialize/response.json (#22)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/notifications/cancelled.json (#23)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/notifications/initialized.json (#24)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/ping/request.json (#25)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/ping/response.json (#26)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/request.json (#27)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/resources/list/request.json (#28)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/resources/list/response.json (#29)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/resources/read/request.json (#30)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/resources/read/response.json (#31)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/resources/templates/list/request.json (#32)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/resources/templates/list/response.json (#33)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/response.json (#34)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/tools/call/evaluate-schema/request.json (#35)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/tools/call/evaluate-schema/response.json (#36)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/tools/call/get-schema-dependencies/request.json (#37)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/tools/call/get-schema-dependencies/response.json (#38)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/tools/call/get-schema-dependents/request.json (#39)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/tools/call/get-schema-dependents/response.json (#40)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/tools/call/get-schema-health/request.json (#41)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/tools/call/get-schema-health/response.json (#42)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/tools/call/get-schema-locations/request.json (#43)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/tools/call/get-schema-locations/response.json (#44)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/tools/call/get-schema-metadata/request.json (#45)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/tools/call/get-schema-metadata/response.json (#46)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/tools/call/get-schema-positions/request.json (#47)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/tools/call/get-schema-positions/response.json (#48)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/tools/call/get-schema-stats/request.json (#49)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/tools/call/get-schema-stats/response.json (#50)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/tools/call/instance-to-rdf/request.json (#51)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/tools/call/instance-to-rdf/response.json (#52)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/tools/call/list-directory/request.json (#53)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/tools/call/list-directory/response.json (#54)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/tools/call/request.json (#55)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/tools/call/response.json (#56)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/tools/call/search-schemas/request.json (#57)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/tools/call/search-schemas/response.json (#58)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/tools/call/trace-schema-evaluation/request.json (#59)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/tools/call/trace-schema-evaluation/response.json (#60)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/tools/list/request.json (#61)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/tools/list/response.json (#62)
(100%) Resolving: example/schemas/test.json
(  4%) Producing: schemas/example/schemas/new/%/schema.metapack
(  9%) Producing: schemas/example/schemas/new/%/dependencies.metapack
( 13%) Producing: schemas/example/schemas/new/%/locations.metapack
( 18%) Producing: schemas/example/schemas/new/%/positions.metapack
( 22%) Producing: schemas/example/schemas/new/%/stats.metapack
( 27%) Producing: schemas/example/schemas/new/%/bundle.metapack
( 31%) Producing: schemas/example/schemas/new/%/health.metapack
( 36%) Producing: explorer/example/schemas/new/%/schema.metapack
( 40%) Producing: schemas/example/schemas/new/%/blaze-exhaustive.metapack
( 45%) Producing: schemas/example/schemas/new/%/blaze-fast.metapack
( 50%) Producing: schemas/example/schemas/new/%/editor.metapack
( 54%) Producing: explorer/example/schemas/%/directory.metapack
( 59%) Producing: explorer/example/schemas/new/%/schema-html.metapack
( 63%) Producing: explorer/example/%/directory.metapack
( 68%) Producing: explorer/example/schemas/%/directory-html.metapack
( 72%) Producing: explorer/%/directory.metapack
( 77%) Producing: explorer/example/%/directory-html.metapack
( 81%) Producing: explorer/%/directory-html.metapack
( 86%) Producing: explorer/%/search.metapack
( 90%) Producing: explorer/%/mcp.metapack
( 95%) Disposing: explorer/example/schemas/old
(100%) Disposing: schemas/example/schemas/old
(100%) Combining: schemas/example/schemas/new/%/dependents.metapack
EOF
diff "$TMP/output.txt" "$TMP/expected.txt"

exists "$TMP/output/explorer/example/schemas/new/%/schema.metapack"
exists "$TMP/output/schemas/example/schemas/new/%/bundle.metapack"
exists "$TMP/output/schemas/example/schemas/new/%/editor.metapack"
exists "$TMP/output/schemas/example/schemas/new/%/blaze-exhaustive.metapack"
exists "$TMP/output/schemas/example/schemas/new/%/schema.metapack"

not_exists "$TMP/output/explorer/example/schemas/old/%/schema.metapack"
not_exists "$TMP/output/schemas/example/schemas/old/%/bundle.metapack"
not_exists "$TMP/output/schemas/example/schemas/old/%/editor.metapack"
not_exists "$TMP/output/schemas/example/schemas/old/%/blaze-exhaustive.metapack"
not_exists "$TMP/output/schemas/example/schemas/old/%/schema.metapack"
