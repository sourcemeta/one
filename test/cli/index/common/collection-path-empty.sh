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
          "path": ""
        }
      }
    }
  }
}
EOF

"$1" --skip-banner --deterministic "$TMP/one.json" "$TMP/output" --concurrency 1 \
  2> "$TMP/output.txt" && CODE="$?" || CODE="$?"
test "$CODE" = "0" || exit 1

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
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/resources/list/request.json (#22)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/resources/list/response.json (#23)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/resources/read/request.json (#24)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/resources/read/response.json (#25)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/resources/templates/list/request.json (#26)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/resources/templates/list/response.json (#27)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/response.json (#28)
(  3%) Resolving: self/v1/schemas/api/error.json
(  7%) Resolving: self/v1/schemas/api/list/response.json
( 10%) Resolving: self/v1/schemas/api/schemas/dependencies/response.json
( 14%) Resolving: self/v1/schemas/api/schemas/dependents/response.json
( 17%) Resolving: self/v1/schemas/api/schemas/evaluate/response.json
( 21%) Resolving: self/v1/schemas/api/schemas/health/response.json
( 25%) Resolving: self/v1/schemas/api/schemas/locations/response.json
( 28%) Resolving: self/v1/schemas/api/schemas/metadata/response.json
( 32%) Resolving: self/v1/schemas/api/schemas/position.json
( 35%) Resolving: self/v1/schemas/api/schemas/positions/response.json
( 39%) Resolving: self/v1/schemas/api/schemas/search/response.json
( 42%) Resolving: self/v1/schemas/api/schemas/stats/response.json
( 46%) Resolving: self/v1/schemas/api/schemas/trace/response.json
( 50%) Resolving: self/v1/schemas/mcp/error.json
( 53%) Resolving: self/v1/schemas/mcp/initialize/request.json
( 57%) Resolving: self/v1/schemas/mcp/initialize/response.json
( 60%) Resolving: self/v1/schemas/mcp/notifications/cancelled.json
( 64%) Resolving: self/v1/schemas/mcp/notifications/initialized.json
( 67%) Resolving: self/v1/schemas/mcp/ping/request.json
( 71%) Resolving: self/v1/schemas/mcp/ping/response.json
( 75%) Resolving: self/v1/schemas/mcp/request.json
( 78%) Resolving: self/v1/schemas/mcp/resources/list/request.json
( 82%) Resolving: self/v1/schemas/mcp/resources/list/response.json
( 85%) Resolving: self/v1/schemas/mcp/resources/read/request.json
( 89%) Resolving: self/v1/schemas/mcp/resources/read/response.json
( 92%) Resolving: self/v1/schemas/mcp/resources/templates/list/request.json
( 96%) Resolving: self/v1/schemas/mcp/resources/templates/list/response.json
(100%) Resolving: self/v1/schemas/mcp/response.json
(  0%) Producing: configuration.json
(  0%) Producing: metadata.json
(  0%) Producing: version.json
(  1%) Producing: explorer/%/404.metapack
(  1%) Producing: schemas/self/v1/schemas/api/error/%/schema.metapack
(  1%) Producing: schemas/self/v1/schemas/api/list/response/%/schema.metapack
(  1%) Producing: schemas/self/v1/schemas/api/schemas/dependencies/response/%/schema.metapack
(  2%) Producing: schemas/self/v1/schemas/api/schemas/dependents/response/%/schema.metapack
(  2%) Producing: schemas/self/v1/schemas/api/schemas/evaluate/response/%/schema.metapack
(  2%) Producing: schemas/self/v1/schemas/api/schemas/health/response/%/schema.metapack
(  2%) Producing: schemas/self/v1/schemas/api/schemas/locations/response/%/schema.metapack
(  3%) Producing: schemas/self/v1/schemas/api/schemas/metadata/response/%/schema.metapack
(  3%) Producing: schemas/self/v1/schemas/api/schemas/position/%/schema.metapack
(  3%) Producing: schemas/self/v1/schemas/api/schemas/positions/response/%/schema.metapack
(  3%) Producing: schemas/self/v1/schemas/api/schemas/search/response/%/schema.metapack
(  4%) Producing: schemas/self/v1/schemas/api/schemas/stats/response/%/schema.metapack
(  4%) Producing: schemas/self/v1/schemas/api/schemas/trace/response/%/schema.metapack
(  4%) Producing: schemas/self/v1/schemas/mcp/error/%/schema.metapack
(  4%) Producing: schemas/self/v1/schemas/mcp/initialize/request/%/schema.metapack
(  5%) Producing: schemas/self/v1/schemas/mcp/initialize/response/%/schema.metapack
(  5%) Producing: schemas/self/v1/schemas/mcp/notifications/cancelled/%/schema.metapack
(  5%) Producing: schemas/self/v1/schemas/mcp/notifications/initialized/%/schema.metapack
(  5%) Producing: schemas/self/v1/schemas/mcp/ping/request/%/schema.metapack
(  6%) Producing: schemas/self/v1/schemas/mcp/ping/response/%/schema.metapack
(  6%) Producing: schemas/self/v1/schemas/mcp/request/%/schema.metapack
(  6%) Producing: schemas/self/v1/schemas/mcp/resources/list/request/%/schema.metapack
(  6%) Producing: schemas/self/v1/schemas/mcp/resources/list/response/%/schema.metapack
(  7%) Producing: schemas/self/v1/schemas/mcp/resources/read/request/%/schema.metapack
(  7%) Producing: schemas/self/v1/schemas/mcp/resources/read/response/%/schema.metapack
(  7%) Producing: schemas/self/v1/schemas/mcp/resources/templates/list/request/%/schema.metapack
(  7%) Producing: schemas/self/v1/schemas/mcp/resources/templates/list/response/%/schema.metapack
(  8%) Producing: schemas/self/v1/schemas/mcp/response/%/schema.metapack
(  8%) Producing: schemas/self/v1/schemas/api/error/%/dependencies.metapack
(  8%) Producing: schemas/self/v1/schemas/api/error/%/locations.metapack
(  8%) Producing: schemas/self/v1/schemas/api/error/%/positions.metapack
(  9%) Producing: schemas/self/v1/schemas/api/error/%/stats.metapack
(  9%) Producing: schemas/self/v1/schemas/api/list/response/%/dependencies.metapack
(  9%) Producing: schemas/self/v1/schemas/api/list/response/%/locations.metapack
(  9%) Producing: schemas/self/v1/schemas/api/list/response/%/positions.metapack
( 10%) Producing: schemas/self/v1/schemas/api/list/response/%/stats.metapack
( 10%) Producing: schemas/self/v1/schemas/api/schemas/dependencies/response/%/dependencies.metapack
( 10%) Producing: schemas/self/v1/schemas/api/schemas/dependencies/response/%/locations.metapack
( 10%) Producing: schemas/self/v1/schemas/api/schemas/dependencies/response/%/positions.metapack
( 11%) Producing: schemas/self/v1/schemas/api/schemas/dependencies/response/%/stats.metapack
( 11%) Producing: schemas/self/v1/schemas/api/schemas/dependents/response/%/dependencies.metapack
( 11%) Producing: schemas/self/v1/schemas/api/schemas/dependents/response/%/locations.metapack
( 11%) Producing: schemas/self/v1/schemas/api/schemas/dependents/response/%/positions.metapack
( 12%) Producing: schemas/self/v1/schemas/api/schemas/dependents/response/%/stats.metapack
( 12%) Producing: schemas/self/v1/schemas/api/schemas/evaluate/response/%/dependencies.metapack
( 12%) Producing: schemas/self/v1/schemas/api/schemas/evaluate/response/%/locations.metapack
( 12%) Producing: schemas/self/v1/schemas/api/schemas/evaluate/response/%/positions.metapack
( 13%) Producing: schemas/self/v1/schemas/api/schemas/evaluate/response/%/stats.metapack
( 13%) Producing: schemas/self/v1/schemas/api/schemas/health/response/%/dependencies.metapack
( 13%) Producing: schemas/self/v1/schemas/api/schemas/health/response/%/locations.metapack
( 13%) Producing: schemas/self/v1/schemas/api/schemas/health/response/%/positions.metapack
( 14%) Producing: schemas/self/v1/schemas/api/schemas/health/response/%/stats.metapack
( 14%) Producing: schemas/self/v1/schemas/api/schemas/locations/response/%/dependencies.metapack
( 14%) Producing: schemas/self/v1/schemas/api/schemas/locations/response/%/locations.metapack
( 14%) Producing: schemas/self/v1/schemas/api/schemas/locations/response/%/positions.metapack
( 15%) Producing: schemas/self/v1/schemas/api/schemas/locations/response/%/stats.metapack
( 15%) Producing: schemas/self/v1/schemas/api/schemas/metadata/response/%/dependencies.metapack
( 15%) Producing: schemas/self/v1/schemas/api/schemas/metadata/response/%/locations.metapack
( 15%) Producing: schemas/self/v1/schemas/api/schemas/metadata/response/%/positions.metapack
( 16%) Producing: schemas/self/v1/schemas/api/schemas/metadata/response/%/stats.metapack
( 16%) Producing: schemas/self/v1/schemas/api/schemas/position/%/dependencies.metapack
( 16%) Producing: schemas/self/v1/schemas/api/schemas/position/%/locations.metapack
( 17%) Producing: schemas/self/v1/schemas/api/schemas/position/%/positions.metapack
( 17%) Producing: schemas/self/v1/schemas/api/schemas/position/%/stats.metapack
( 17%) Producing: schemas/self/v1/schemas/api/schemas/positions/response/%/dependencies.metapack
( 17%) Producing: schemas/self/v1/schemas/api/schemas/positions/response/%/locations.metapack
( 18%) Producing: schemas/self/v1/schemas/api/schemas/positions/response/%/positions.metapack
( 18%) Producing: schemas/self/v1/schemas/api/schemas/positions/response/%/stats.metapack
( 18%) Producing: schemas/self/v1/schemas/api/schemas/search/response/%/dependencies.metapack
( 18%) Producing: schemas/self/v1/schemas/api/schemas/search/response/%/locations.metapack
( 19%) Producing: schemas/self/v1/schemas/api/schemas/search/response/%/positions.metapack
( 19%) Producing: schemas/self/v1/schemas/api/schemas/search/response/%/stats.metapack
( 19%) Producing: schemas/self/v1/schemas/api/schemas/stats/response/%/dependencies.metapack
( 19%) Producing: schemas/self/v1/schemas/api/schemas/stats/response/%/locations.metapack
( 20%) Producing: schemas/self/v1/schemas/api/schemas/stats/response/%/positions.metapack
( 20%) Producing: schemas/self/v1/schemas/api/schemas/stats/response/%/stats.metapack
( 20%) Producing: schemas/self/v1/schemas/api/schemas/trace/response/%/dependencies.metapack
( 20%) Producing: schemas/self/v1/schemas/api/schemas/trace/response/%/locations.metapack
( 21%) Producing: schemas/self/v1/schemas/api/schemas/trace/response/%/positions.metapack
( 21%) Producing: schemas/self/v1/schemas/api/schemas/trace/response/%/stats.metapack
( 21%) Producing: schemas/self/v1/schemas/mcp/error/%/dependencies.metapack
( 21%) Producing: schemas/self/v1/schemas/mcp/error/%/locations.metapack
( 22%) Producing: schemas/self/v1/schemas/mcp/error/%/positions.metapack
( 22%) Producing: schemas/self/v1/schemas/mcp/error/%/stats.metapack
( 22%) Producing: schemas/self/v1/schemas/mcp/initialize/request/%/dependencies.metapack
( 22%) Producing: schemas/self/v1/schemas/mcp/initialize/request/%/locations.metapack
( 23%) Producing: schemas/self/v1/schemas/mcp/initialize/request/%/positions.metapack
( 23%) Producing: schemas/self/v1/schemas/mcp/initialize/request/%/stats.metapack
( 23%) Producing: schemas/self/v1/schemas/mcp/initialize/response/%/dependencies.metapack
( 23%) Producing: schemas/self/v1/schemas/mcp/initialize/response/%/locations.metapack
( 24%) Producing: schemas/self/v1/schemas/mcp/initialize/response/%/positions.metapack
( 24%) Producing: schemas/self/v1/schemas/mcp/initialize/response/%/stats.metapack
( 24%) Producing: schemas/self/v1/schemas/mcp/notifications/cancelled/%/dependencies.metapack
( 24%) Producing: schemas/self/v1/schemas/mcp/notifications/cancelled/%/locations.metapack
( 25%) Producing: schemas/self/v1/schemas/mcp/notifications/cancelled/%/positions.metapack
( 25%) Producing: schemas/self/v1/schemas/mcp/notifications/cancelled/%/stats.metapack
( 25%) Producing: schemas/self/v1/schemas/mcp/notifications/initialized/%/dependencies.metapack
( 25%) Producing: schemas/self/v1/schemas/mcp/notifications/initialized/%/locations.metapack
( 26%) Producing: schemas/self/v1/schemas/mcp/notifications/initialized/%/positions.metapack
( 26%) Producing: schemas/self/v1/schemas/mcp/notifications/initialized/%/stats.metapack
( 26%) Producing: schemas/self/v1/schemas/mcp/ping/request/%/dependencies.metapack
( 26%) Producing: schemas/self/v1/schemas/mcp/ping/request/%/locations.metapack
( 27%) Producing: schemas/self/v1/schemas/mcp/ping/request/%/positions.metapack
( 27%) Producing: schemas/self/v1/schemas/mcp/ping/request/%/stats.metapack
( 27%) Producing: schemas/self/v1/schemas/mcp/ping/response/%/dependencies.metapack
( 27%) Producing: schemas/self/v1/schemas/mcp/ping/response/%/locations.metapack
( 28%) Producing: schemas/self/v1/schemas/mcp/ping/response/%/positions.metapack
( 28%) Producing: schemas/self/v1/schemas/mcp/ping/response/%/stats.metapack
( 28%) Producing: schemas/self/v1/schemas/mcp/request/%/dependencies.metapack
( 28%) Producing: schemas/self/v1/schemas/mcp/request/%/locations.metapack
( 29%) Producing: schemas/self/v1/schemas/mcp/request/%/positions.metapack
( 29%) Producing: schemas/self/v1/schemas/mcp/request/%/stats.metapack
( 29%) Producing: schemas/self/v1/schemas/mcp/resources/list/request/%/dependencies.metapack
( 29%) Producing: schemas/self/v1/schemas/mcp/resources/list/request/%/locations.metapack
( 30%) Producing: schemas/self/v1/schemas/mcp/resources/list/request/%/positions.metapack
( 30%) Producing: schemas/self/v1/schemas/mcp/resources/list/request/%/stats.metapack
( 30%) Producing: schemas/self/v1/schemas/mcp/resources/list/response/%/dependencies.metapack
( 30%) Producing: schemas/self/v1/schemas/mcp/resources/list/response/%/locations.metapack
( 31%) Producing: schemas/self/v1/schemas/mcp/resources/list/response/%/positions.metapack
( 31%) Producing: schemas/self/v1/schemas/mcp/resources/list/response/%/stats.metapack
( 31%) Producing: schemas/self/v1/schemas/mcp/resources/read/request/%/dependencies.metapack
( 31%) Producing: schemas/self/v1/schemas/mcp/resources/read/request/%/locations.metapack
( 32%) Producing: schemas/self/v1/schemas/mcp/resources/read/request/%/positions.metapack
( 32%) Producing: schemas/self/v1/schemas/mcp/resources/read/request/%/stats.metapack
( 32%) Producing: schemas/self/v1/schemas/mcp/resources/read/response/%/dependencies.metapack
( 32%) Producing: schemas/self/v1/schemas/mcp/resources/read/response/%/locations.metapack
( 33%) Producing: schemas/self/v1/schemas/mcp/resources/read/response/%/positions.metapack
( 33%) Producing: schemas/self/v1/schemas/mcp/resources/read/response/%/stats.metapack
( 33%) Producing: schemas/self/v1/schemas/mcp/resources/templates/list/request/%/dependencies.metapack
( 34%) Producing: schemas/self/v1/schemas/mcp/resources/templates/list/request/%/locations.metapack
( 34%) Producing: schemas/self/v1/schemas/mcp/resources/templates/list/request/%/positions.metapack
( 34%) Producing: schemas/self/v1/schemas/mcp/resources/templates/list/request/%/stats.metapack
( 34%) Producing: schemas/self/v1/schemas/mcp/resources/templates/list/response/%/dependencies.metapack
( 35%) Producing: schemas/self/v1/schemas/mcp/resources/templates/list/response/%/locations.metapack
( 35%) Producing: schemas/self/v1/schemas/mcp/resources/templates/list/response/%/positions.metapack
( 35%) Producing: schemas/self/v1/schemas/mcp/resources/templates/list/response/%/stats.metapack
( 35%) Producing: schemas/self/v1/schemas/mcp/response/%/dependencies.metapack
( 36%) Producing: schemas/self/v1/schemas/mcp/response/%/locations.metapack
( 36%) Producing: schemas/self/v1/schemas/mcp/response/%/positions.metapack
( 36%) Producing: schemas/self/v1/schemas/mcp/response/%/stats.metapack
( 36%) Producing: schemas/self/v1/schemas/api/error/%/bundle.metapack
( 37%) Producing: schemas/self/v1/schemas/api/error/%/health.metapack
( 37%) Producing: schemas/self/v1/schemas/api/list/response/%/bundle.metapack
( 37%) Producing: schemas/self/v1/schemas/api/list/response/%/health.metapack
( 37%) Producing: schemas/self/v1/schemas/api/schemas/dependencies/response/%/bundle.metapack
( 38%) Producing: schemas/self/v1/schemas/api/schemas/dependencies/response/%/health.metapack
( 38%) Producing: schemas/self/v1/schemas/api/schemas/dependents/response/%/bundle.metapack
( 38%) Producing: schemas/self/v1/schemas/api/schemas/dependents/response/%/health.metapack
( 38%) Producing: schemas/self/v1/schemas/api/schemas/evaluate/response/%/bundle.metapack
( 39%) Producing: schemas/self/v1/schemas/api/schemas/evaluate/response/%/health.metapack
( 39%) Producing: schemas/self/v1/schemas/api/schemas/health/response/%/bundle.metapack
( 39%) Producing: schemas/self/v1/schemas/api/schemas/health/response/%/health.metapack
( 39%) Producing: schemas/self/v1/schemas/api/schemas/locations/response/%/bundle.metapack
( 40%) Producing: schemas/self/v1/schemas/api/schemas/locations/response/%/health.metapack
( 40%) Producing: schemas/self/v1/schemas/api/schemas/metadata/response/%/bundle.metapack
( 40%) Producing: schemas/self/v1/schemas/api/schemas/metadata/response/%/health.metapack
( 40%) Producing: schemas/self/v1/schemas/api/schemas/position/%/bundle.metapack
( 41%) Producing: schemas/self/v1/schemas/api/schemas/position/%/health.metapack
( 41%) Producing: schemas/self/v1/schemas/api/schemas/positions/response/%/bundle.metapack
( 41%) Producing: schemas/self/v1/schemas/api/schemas/positions/response/%/health.metapack
( 41%) Producing: schemas/self/v1/schemas/api/schemas/search/response/%/bundle.metapack
( 42%) Producing: schemas/self/v1/schemas/api/schemas/search/response/%/health.metapack
( 42%) Producing: schemas/self/v1/schemas/api/schemas/stats/response/%/bundle.metapack
( 42%) Producing: schemas/self/v1/schemas/api/schemas/stats/response/%/health.metapack
( 42%) Producing: schemas/self/v1/schemas/api/schemas/trace/response/%/bundle.metapack
( 43%) Producing: schemas/self/v1/schemas/api/schemas/trace/response/%/health.metapack
( 43%) Producing: schemas/self/v1/schemas/mcp/error/%/bundle.metapack
( 43%) Producing: schemas/self/v1/schemas/mcp/error/%/health.metapack
( 43%) Producing: schemas/self/v1/schemas/mcp/initialize/request/%/bundle.metapack
( 44%) Producing: schemas/self/v1/schemas/mcp/initialize/request/%/health.metapack
( 44%) Producing: schemas/self/v1/schemas/mcp/initialize/response/%/bundle.metapack
( 44%) Producing: schemas/self/v1/schemas/mcp/initialize/response/%/health.metapack
( 44%) Producing: schemas/self/v1/schemas/mcp/notifications/cancelled/%/bundle.metapack
( 45%) Producing: schemas/self/v1/schemas/mcp/notifications/cancelled/%/health.metapack
( 45%) Producing: schemas/self/v1/schemas/mcp/notifications/initialized/%/bundle.metapack
( 45%) Producing: schemas/self/v1/schemas/mcp/notifications/initialized/%/health.metapack
( 45%) Producing: schemas/self/v1/schemas/mcp/ping/request/%/bundle.metapack
( 46%) Producing: schemas/self/v1/schemas/mcp/ping/request/%/health.metapack
( 46%) Producing: schemas/self/v1/schemas/mcp/ping/response/%/bundle.metapack
( 46%) Producing: schemas/self/v1/schemas/mcp/ping/response/%/health.metapack
( 46%) Producing: schemas/self/v1/schemas/mcp/request/%/bundle.metapack
( 47%) Producing: schemas/self/v1/schemas/mcp/request/%/health.metapack
( 47%) Producing: schemas/self/v1/schemas/mcp/resources/list/request/%/bundle.metapack
( 47%) Producing: schemas/self/v1/schemas/mcp/resources/list/request/%/health.metapack
( 47%) Producing: schemas/self/v1/schemas/mcp/resources/list/response/%/bundle.metapack
( 48%) Producing: schemas/self/v1/schemas/mcp/resources/list/response/%/health.metapack
( 48%) Producing: schemas/self/v1/schemas/mcp/resources/read/request/%/bundle.metapack
( 48%) Producing: schemas/self/v1/schemas/mcp/resources/read/request/%/health.metapack
( 48%) Producing: schemas/self/v1/schemas/mcp/resources/read/response/%/bundle.metapack
( 49%) Producing: schemas/self/v1/schemas/mcp/resources/read/response/%/health.metapack
( 49%) Producing: schemas/self/v1/schemas/mcp/resources/templates/list/request/%/bundle.metapack
( 49%) Producing: schemas/self/v1/schemas/mcp/resources/templates/list/request/%/health.metapack
( 50%) Producing: schemas/self/v1/schemas/mcp/resources/templates/list/response/%/bundle.metapack
( 50%) Producing: schemas/self/v1/schemas/mcp/resources/templates/list/response/%/health.metapack
( 50%) Producing: schemas/self/v1/schemas/mcp/response/%/bundle.metapack
( 50%) Producing: schemas/self/v1/schemas/mcp/response/%/health.metapack
( 51%) Producing: explorer/self/v1/schemas/api/error/%/schema.metapack
( 51%) Producing: explorer/self/v1/schemas/api/list/response/%/schema.metapack
( 51%) Producing: explorer/self/v1/schemas/api/schemas/dependencies/response/%/schema.metapack
( 51%) Producing: explorer/self/v1/schemas/api/schemas/dependents/response/%/schema.metapack
( 52%) Producing: explorer/self/v1/schemas/api/schemas/evaluate/response/%/schema.metapack
( 52%) Producing: explorer/self/v1/schemas/api/schemas/health/response/%/schema.metapack
( 52%) Producing: explorer/self/v1/schemas/api/schemas/locations/response/%/schema.metapack
( 52%) Producing: explorer/self/v1/schemas/api/schemas/metadata/response/%/schema.metapack
( 53%) Producing: explorer/self/v1/schemas/api/schemas/position/%/schema.metapack
( 53%) Producing: explorer/self/v1/schemas/api/schemas/positions/response/%/schema.metapack
( 53%) Producing: explorer/self/v1/schemas/api/schemas/search/response/%/schema.metapack
( 53%) Producing: explorer/self/v1/schemas/api/schemas/stats/response/%/schema.metapack
( 54%) Producing: explorer/self/v1/schemas/api/schemas/trace/response/%/schema.metapack
( 54%) Producing: explorer/self/v1/schemas/mcp/error/%/schema.metapack
( 54%) Producing: explorer/self/v1/schemas/mcp/initialize/request/%/schema.metapack
( 54%) Producing: explorer/self/v1/schemas/mcp/initialize/response/%/schema.metapack
( 55%) Producing: explorer/self/v1/schemas/mcp/notifications/cancelled/%/schema.metapack
( 55%) Producing: explorer/self/v1/schemas/mcp/notifications/initialized/%/schema.metapack
( 55%) Producing: explorer/self/v1/schemas/mcp/ping/request/%/schema.metapack
( 55%) Producing: explorer/self/v1/schemas/mcp/ping/response/%/schema.metapack
( 56%) Producing: explorer/self/v1/schemas/mcp/request/%/schema.metapack
( 56%) Producing: explorer/self/v1/schemas/mcp/resources/list/request/%/schema.metapack
( 56%) Producing: explorer/self/v1/schemas/mcp/resources/list/response/%/schema.metapack
( 56%) Producing: explorer/self/v1/schemas/mcp/resources/read/request/%/schema.metapack
( 57%) Producing: explorer/self/v1/schemas/mcp/resources/read/response/%/schema.metapack
( 57%) Producing: explorer/self/v1/schemas/mcp/resources/templates/list/request/%/schema.metapack
( 57%) Producing: explorer/self/v1/schemas/mcp/resources/templates/list/response/%/schema.metapack
( 57%) Producing: explorer/self/v1/schemas/mcp/response/%/schema.metapack
( 58%) Producing: schemas/self/v1/schemas/api/error/%/blaze-exhaustive.metapack
( 58%) Producing: schemas/self/v1/schemas/api/error/%/blaze-fast.metapack
( 58%) Producing: schemas/self/v1/schemas/api/error/%/editor.metapack
( 58%) Producing: schemas/self/v1/schemas/api/list/response/%/blaze-exhaustive.metapack
( 59%) Producing: schemas/self/v1/schemas/api/list/response/%/blaze-fast.metapack
( 59%) Producing: schemas/self/v1/schemas/api/list/response/%/editor.metapack
( 59%) Producing: schemas/self/v1/schemas/api/schemas/dependencies/response/%/blaze-exhaustive.metapack
( 59%) Producing: schemas/self/v1/schemas/api/schemas/dependencies/response/%/blaze-fast.metapack
( 60%) Producing: schemas/self/v1/schemas/api/schemas/dependencies/response/%/editor.metapack
( 60%) Producing: schemas/self/v1/schemas/api/schemas/dependents/response/%/blaze-exhaustive.metapack
( 60%) Producing: schemas/self/v1/schemas/api/schemas/dependents/response/%/blaze-fast.metapack
( 60%) Producing: schemas/self/v1/schemas/api/schemas/dependents/response/%/editor.metapack
( 61%) Producing: schemas/self/v1/schemas/api/schemas/evaluate/response/%/blaze-exhaustive.metapack
( 61%) Producing: schemas/self/v1/schemas/api/schemas/evaluate/response/%/blaze-fast.metapack
( 61%) Producing: schemas/self/v1/schemas/api/schemas/evaluate/response/%/editor.metapack
( 61%) Producing: schemas/self/v1/schemas/api/schemas/health/response/%/blaze-exhaustive.metapack
( 62%) Producing: schemas/self/v1/schemas/api/schemas/health/response/%/blaze-fast.metapack
( 62%) Producing: schemas/self/v1/schemas/api/schemas/health/response/%/editor.metapack
( 62%) Producing: schemas/self/v1/schemas/api/schemas/locations/response/%/blaze-exhaustive.metapack
( 62%) Producing: schemas/self/v1/schemas/api/schemas/locations/response/%/blaze-fast.metapack
( 63%) Producing: schemas/self/v1/schemas/api/schemas/locations/response/%/editor.metapack
( 63%) Producing: schemas/self/v1/schemas/api/schemas/metadata/response/%/blaze-exhaustive.metapack
( 63%) Producing: schemas/self/v1/schemas/api/schemas/metadata/response/%/blaze-fast.metapack
( 63%) Producing: schemas/self/v1/schemas/api/schemas/metadata/response/%/editor.metapack
( 64%) Producing: schemas/self/v1/schemas/api/schemas/position/%/blaze-exhaustive.metapack
( 64%) Producing: schemas/self/v1/schemas/api/schemas/position/%/blaze-fast.metapack
( 64%) Producing: schemas/self/v1/schemas/api/schemas/position/%/editor.metapack
( 64%) Producing: schemas/self/v1/schemas/api/schemas/positions/response/%/blaze-exhaustive.metapack
( 65%) Producing: schemas/self/v1/schemas/api/schemas/positions/response/%/blaze-fast.metapack
( 65%) Producing: schemas/self/v1/schemas/api/schemas/positions/response/%/editor.metapack
( 65%) Producing: schemas/self/v1/schemas/api/schemas/search/response/%/blaze-exhaustive.metapack
( 65%) Producing: schemas/self/v1/schemas/api/schemas/search/response/%/blaze-fast.metapack
( 66%) Producing: schemas/self/v1/schemas/api/schemas/search/response/%/editor.metapack
( 66%) Producing: schemas/self/v1/schemas/api/schemas/stats/response/%/blaze-exhaustive.metapack
( 66%) Producing: schemas/self/v1/schemas/api/schemas/stats/response/%/blaze-fast.metapack
( 67%) Producing: schemas/self/v1/schemas/api/schemas/stats/response/%/editor.metapack
( 67%) Producing: schemas/self/v1/schemas/api/schemas/trace/response/%/blaze-exhaustive.metapack
( 67%) Producing: schemas/self/v1/schemas/api/schemas/trace/response/%/blaze-fast.metapack
( 67%) Producing: schemas/self/v1/schemas/api/schemas/trace/response/%/editor.metapack
( 68%) Producing: schemas/self/v1/schemas/mcp/error/%/blaze-exhaustive.metapack
( 68%) Producing: schemas/self/v1/schemas/mcp/error/%/blaze-fast.metapack
( 68%) Producing: schemas/self/v1/schemas/mcp/error/%/editor.metapack
( 68%) Producing: schemas/self/v1/schemas/mcp/initialize/request/%/blaze-exhaustive.metapack
( 69%) Producing: schemas/self/v1/schemas/mcp/initialize/request/%/blaze-fast.metapack
( 69%) Producing: schemas/self/v1/schemas/mcp/initialize/request/%/editor.metapack
( 69%) Producing: schemas/self/v1/schemas/mcp/initialize/response/%/blaze-exhaustive.metapack
( 69%) Producing: schemas/self/v1/schemas/mcp/initialize/response/%/blaze-fast.metapack
( 70%) Producing: schemas/self/v1/schemas/mcp/initialize/response/%/editor.metapack
( 70%) Producing: schemas/self/v1/schemas/mcp/notifications/cancelled/%/blaze-exhaustive.metapack
( 70%) Producing: schemas/self/v1/schemas/mcp/notifications/cancelled/%/blaze-fast.metapack
( 70%) Producing: schemas/self/v1/schemas/mcp/notifications/cancelled/%/editor.metapack
( 71%) Producing: schemas/self/v1/schemas/mcp/notifications/initialized/%/blaze-exhaustive.metapack
( 71%) Producing: schemas/self/v1/schemas/mcp/notifications/initialized/%/blaze-fast.metapack
( 71%) Producing: schemas/self/v1/schemas/mcp/notifications/initialized/%/editor.metapack
( 71%) Producing: schemas/self/v1/schemas/mcp/ping/request/%/blaze-exhaustive.metapack
( 72%) Producing: schemas/self/v1/schemas/mcp/ping/request/%/blaze-fast.metapack
( 72%) Producing: schemas/self/v1/schemas/mcp/ping/request/%/editor.metapack
( 72%) Producing: schemas/self/v1/schemas/mcp/ping/response/%/blaze-exhaustive.metapack
( 72%) Producing: schemas/self/v1/schemas/mcp/ping/response/%/blaze-fast.metapack
( 73%) Producing: schemas/self/v1/schemas/mcp/ping/response/%/editor.metapack
( 73%) Producing: schemas/self/v1/schemas/mcp/request/%/blaze-exhaustive.metapack
( 73%) Producing: schemas/self/v1/schemas/mcp/request/%/blaze-fast.metapack
( 73%) Producing: schemas/self/v1/schemas/mcp/request/%/editor.metapack
( 74%) Producing: schemas/self/v1/schemas/mcp/resources/list/request/%/blaze-exhaustive.metapack
( 74%) Producing: schemas/self/v1/schemas/mcp/resources/list/request/%/blaze-fast.metapack
( 74%) Producing: schemas/self/v1/schemas/mcp/resources/list/request/%/editor.metapack
( 74%) Producing: schemas/self/v1/schemas/mcp/resources/list/response/%/blaze-exhaustive.metapack
( 75%) Producing: schemas/self/v1/schemas/mcp/resources/list/response/%/blaze-fast.metapack
( 75%) Producing: schemas/self/v1/schemas/mcp/resources/list/response/%/editor.metapack
( 75%) Producing: schemas/self/v1/schemas/mcp/resources/read/request/%/blaze-exhaustive.metapack
( 75%) Producing: schemas/self/v1/schemas/mcp/resources/read/request/%/blaze-fast.metapack
( 76%) Producing: schemas/self/v1/schemas/mcp/resources/read/request/%/editor.metapack
( 76%) Producing: schemas/self/v1/schemas/mcp/resources/read/response/%/blaze-exhaustive.metapack
( 76%) Producing: schemas/self/v1/schemas/mcp/resources/read/response/%/blaze-fast.metapack
( 76%) Producing: schemas/self/v1/schemas/mcp/resources/read/response/%/editor.metapack
( 77%) Producing: schemas/self/v1/schemas/mcp/resources/templates/list/request/%/blaze-exhaustive.metapack
( 77%) Producing: schemas/self/v1/schemas/mcp/resources/templates/list/request/%/blaze-fast.metapack
( 77%) Producing: schemas/self/v1/schemas/mcp/resources/templates/list/request/%/editor.metapack
( 77%) Producing: schemas/self/v1/schemas/mcp/resources/templates/list/response/%/blaze-exhaustive.metapack
( 78%) Producing: schemas/self/v1/schemas/mcp/resources/templates/list/response/%/blaze-fast.metapack
( 78%) Producing: schemas/self/v1/schemas/mcp/resources/templates/list/response/%/editor.metapack
( 78%) Producing: schemas/self/v1/schemas/mcp/response/%/blaze-exhaustive.metapack
( 78%) Producing: schemas/self/v1/schemas/mcp/response/%/blaze-fast.metapack
( 79%) Producing: schemas/self/v1/schemas/mcp/response/%/editor.metapack
( 79%) Producing: explorer/self/v1/schemas/api/error/%/schema-html.metapack
( 79%) Producing: explorer/self/v1/schemas/api/list/%/directory.metapack
( 79%) Producing: explorer/self/v1/schemas/api/list/response/%/schema-html.metapack
( 80%) Producing: explorer/self/v1/schemas/api/schemas/dependencies/%/directory.metapack
( 80%) Producing: explorer/self/v1/schemas/api/schemas/dependencies/response/%/schema-html.metapack
( 80%) Producing: explorer/self/v1/schemas/api/schemas/dependents/%/directory.metapack
( 80%) Producing: explorer/self/v1/schemas/api/schemas/dependents/response/%/schema-html.metapack
( 81%) Producing: explorer/self/v1/schemas/api/schemas/evaluate/%/directory.metapack
( 81%) Producing: explorer/self/v1/schemas/api/schemas/evaluate/response/%/schema-html.metapack
( 81%) Producing: explorer/self/v1/schemas/api/schemas/health/%/directory.metapack
( 81%) Producing: explorer/self/v1/schemas/api/schemas/health/response/%/schema-html.metapack
( 82%) Producing: explorer/self/v1/schemas/api/schemas/locations/%/directory.metapack
( 82%) Producing: explorer/self/v1/schemas/api/schemas/locations/response/%/schema-html.metapack
( 82%) Producing: explorer/self/v1/schemas/api/schemas/metadata/%/directory.metapack
( 82%) Producing: explorer/self/v1/schemas/api/schemas/metadata/response/%/schema-html.metapack
( 83%) Producing: explorer/self/v1/schemas/api/schemas/position/%/schema-html.metapack
( 83%) Producing: explorer/self/v1/schemas/api/schemas/positions/%/directory.metapack
( 83%) Producing: explorer/self/v1/schemas/api/schemas/positions/response/%/schema-html.metapack
( 84%) Producing: explorer/self/v1/schemas/api/schemas/search/%/directory.metapack
( 84%) Producing: explorer/self/v1/schemas/api/schemas/search/response/%/schema-html.metapack
( 84%) Producing: explorer/self/v1/schemas/api/schemas/stats/%/directory.metapack
( 84%) Producing: explorer/self/v1/schemas/api/schemas/stats/response/%/schema-html.metapack
( 85%) Producing: explorer/self/v1/schemas/api/schemas/trace/%/directory.metapack
( 85%) Producing: explorer/self/v1/schemas/api/schemas/trace/response/%/schema-html.metapack
( 85%) Producing: explorer/self/v1/schemas/mcp/error/%/schema-html.metapack
( 85%) Producing: explorer/self/v1/schemas/mcp/initialize/%/directory.metapack
( 86%) Producing: explorer/self/v1/schemas/mcp/initialize/request/%/schema-html.metapack
( 86%) Producing: explorer/self/v1/schemas/mcp/initialize/response/%/schema-html.metapack
( 86%) Producing: explorer/self/v1/schemas/mcp/notifications/%/directory.metapack
( 86%) Producing: explorer/self/v1/schemas/mcp/notifications/cancelled/%/schema-html.metapack
( 87%) Producing: explorer/self/v1/schemas/mcp/notifications/initialized/%/schema-html.metapack
( 87%) Producing: explorer/self/v1/schemas/mcp/ping/%/directory.metapack
( 87%) Producing: explorer/self/v1/schemas/mcp/ping/request/%/schema-html.metapack
( 87%) Producing: explorer/self/v1/schemas/mcp/ping/response/%/schema-html.metapack
( 88%) Producing: explorer/self/v1/schemas/mcp/request/%/schema-html.metapack
( 88%) Producing: explorer/self/v1/schemas/mcp/resources/list/%/directory.metapack
( 88%) Producing: explorer/self/v1/schemas/mcp/resources/list/request/%/schema-html.metapack
( 88%) Producing: explorer/self/v1/schemas/mcp/resources/list/response/%/schema-html.metapack
( 89%) Producing: explorer/self/v1/schemas/mcp/resources/read/%/directory.metapack
( 89%) Producing: explorer/self/v1/schemas/mcp/resources/read/request/%/schema-html.metapack
( 89%) Producing: explorer/self/v1/schemas/mcp/resources/read/response/%/schema-html.metapack
( 89%) Producing: explorer/self/v1/schemas/mcp/resources/templates/list/%/directory.metapack
( 90%) Producing: explorer/self/v1/schemas/mcp/resources/templates/list/request/%/schema-html.metapack
( 90%) Producing: explorer/self/v1/schemas/mcp/resources/templates/list/response/%/schema-html.metapack
( 90%) Producing: explorer/self/v1/schemas/mcp/response/%/schema-html.metapack
( 90%) Producing: explorer/self/v1/schemas/api/list/%/directory-html.metapack
( 91%) Producing: explorer/self/v1/schemas/api/schemas/%/directory.metapack
( 91%) Producing: explorer/self/v1/schemas/api/schemas/dependencies/%/directory-html.metapack
( 91%) Producing: explorer/self/v1/schemas/api/schemas/dependents/%/directory-html.metapack
( 91%) Producing: explorer/self/v1/schemas/api/schemas/evaluate/%/directory-html.metapack
( 92%) Producing: explorer/self/v1/schemas/api/schemas/health/%/directory-html.metapack
( 92%) Producing: explorer/self/v1/schemas/api/schemas/locations/%/directory-html.metapack
( 92%) Producing: explorer/self/v1/schemas/api/schemas/metadata/%/directory-html.metapack
( 92%) Producing: explorer/self/v1/schemas/api/schemas/positions/%/directory-html.metapack
( 93%) Producing: explorer/self/v1/schemas/api/schemas/search/%/directory-html.metapack
( 93%) Producing: explorer/self/v1/schemas/api/schemas/stats/%/directory-html.metapack
( 93%) Producing: explorer/self/v1/schemas/api/schemas/trace/%/directory-html.metapack
( 93%) Producing: explorer/self/v1/schemas/mcp/initialize/%/directory-html.metapack
( 94%) Producing: explorer/self/v1/schemas/mcp/notifications/%/directory-html.metapack
( 94%) Producing: explorer/self/v1/schemas/mcp/ping/%/directory-html.metapack
( 94%) Producing: explorer/self/v1/schemas/mcp/resources/list/%/directory-html.metapack
( 94%) Producing: explorer/self/v1/schemas/mcp/resources/read/%/directory-html.metapack
( 95%) Producing: explorer/self/v1/schemas/mcp/resources/templates/%/directory.metapack
( 95%) Producing: explorer/self/v1/schemas/mcp/resources/templates/list/%/directory-html.metapack
( 95%) Producing: explorer/self/v1/schemas/api/%/directory.metapack
( 95%) Producing: explorer/self/v1/schemas/api/schemas/%/directory-html.metapack
( 96%) Producing: explorer/self/v1/schemas/mcp/resources/%/directory.metapack
( 96%) Producing: explorer/self/v1/schemas/mcp/resources/templates/%/directory-html.metapack
( 96%) Producing: explorer/self/v1/schemas/api/%/directory-html.metapack
( 96%) Producing: explorer/self/v1/schemas/mcp/%/directory.metapack
( 97%) Producing: explorer/self/v1/schemas/mcp/resources/%/directory-html.metapack
( 97%) Producing: explorer/self/v1/schemas/%/directory.metapack
( 97%) Producing: explorer/self/v1/schemas/mcp/%/directory-html.metapack
( 97%) Producing: explorer/self/v1/%/directory.metapack
( 98%) Producing: explorer/self/v1/schemas/%/directory-html.metapack
( 98%) Producing: explorer/self/%/directory.metapack
( 98%) Producing: explorer/self/v1/%/directory-html.metapack
( 98%) Producing: explorer/%/directory.metapack
( 99%) Producing: explorer/self/%/directory-html.metapack
( 99%) Producing: explorer/%/directory-html.metapack
( 99%) Producing: explorer/%/search.metapack
(100%) Producing: routes.bin
(  3%) Combining: schemas/self/v1/schemas/api/error/%/dependents.metapack
(  7%) Combining: schemas/self/v1/schemas/api/list/response/%/dependents.metapack
( 10%) Combining: schemas/self/v1/schemas/api/schemas/dependencies/response/%/dependents.metapack
( 14%) Combining: schemas/self/v1/schemas/api/schemas/dependents/response/%/dependents.metapack
( 17%) Combining: schemas/self/v1/schemas/api/schemas/evaluate/response/%/dependents.metapack
( 21%) Combining: schemas/self/v1/schemas/api/schemas/health/response/%/dependents.metapack
( 25%) Combining: schemas/self/v1/schemas/api/schemas/locations/response/%/dependents.metapack
( 28%) Combining: schemas/self/v1/schemas/api/schemas/metadata/response/%/dependents.metapack
( 32%) Combining: schemas/self/v1/schemas/api/schemas/position/%/dependents.metapack
( 35%) Combining: schemas/self/v1/schemas/api/schemas/positions/response/%/dependents.metapack
( 39%) Combining: schemas/self/v1/schemas/api/schemas/search/response/%/dependents.metapack
( 42%) Combining: schemas/self/v1/schemas/api/schemas/stats/response/%/dependents.metapack
( 46%) Combining: schemas/self/v1/schemas/api/schemas/trace/response/%/dependents.metapack
( 50%) Combining: schemas/self/v1/schemas/mcp/error/%/dependents.metapack
( 53%) Combining: schemas/self/v1/schemas/mcp/initialize/request/%/dependents.metapack
( 57%) Combining: schemas/self/v1/schemas/mcp/initialize/response/%/dependents.metapack
( 60%) Combining: schemas/self/v1/schemas/mcp/notifications/cancelled/%/dependents.metapack
( 64%) Combining: schemas/self/v1/schemas/mcp/notifications/initialized/%/dependents.metapack
( 67%) Combining: schemas/self/v1/schemas/mcp/ping/request/%/dependents.metapack
( 71%) Combining: schemas/self/v1/schemas/mcp/ping/response/%/dependents.metapack
( 75%) Combining: schemas/self/v1/schemas/mcp/request/%/dependents.metapack
( 78%) Combining: schemas/self/v1/schemas/mcp/resources/list/request/%/dependents.metapack
( 82%) Combining: schemas/self/v1/schemas/mcp/resources/list/response/%/dependents.metapack
( 85%) Combining: schemas/self/v1/schemas/mcp/resources/read/request/%/dependents.metapack
( 89%) Combining: schemas/self/v1/schemas/mcp/resources/read/response/%/dependents.metapack
( 92%) Combining: schemas/self/v1/schemas/mcp/resources/templates/list/request/%/dependents.metapack
( 96%) Combining: schemas/self/v1/schemas/mcp/resources/templates/list/response/%/dependents.metapack
(100%) Combining: schemas/self/v1/schemas/mcp/response/%/dependents.metapack
EOF

diff "$TMP/output.txt" "$TMP/expected.txt"
