#!/bin/sh

set -o errexit
set -o nounset

TMP="$(mktemp -d)"
clean() { rm -rf "$TMP"; }
trap clean EXIT

mkdir "$TMP/schemas"

cat << 'EOF' > "$TMP/schemas/test.json"
{ "$schema": "https://json-schema.org/draft/2020-12/schema" }
EOF

cat << EOF > "$TMP/one.json"
{
  "url": "https://example.com",
  "contents": {
    "test": {
      "path": "./schemas",
      "lint": { "rules": [ "./rules/does-not-exist.json" ] }
    }
  }
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
Detecting: $(realpath "$TMP")/schemas/test.json (#29)
(  3%) Resolving: self/v1/schemas/api/error.json
(  6%) Resolving: self/v1/schemas/api/list/response.json
( 10%) Resolving: self/v1/schemas/api/schemas/dependencies/response.json
( 13%) Resolving: self/v1/schemas/api/schemas/dependents/response.json
( 17%) Resolving: self/v1/schemas/api/schemas/evaluate/response.json
( 20%) Resolving: self/v1/schemas/api/schemas/health/response.json
( 24%) Resolving: self/v1/schemas/api/schemas/locations/response.json
( 27%) Resolving: self/v1/schemas/api/schemas/metadata/response.json
( 31%) Resolving: self/v1/schemas/api/schemas/position.json
( 34%) Resolving: self/v1/schemas/api/schemas/positions/response.json
( 37%) Resolving: self/v1/schemas/api/schemas/search/response.json
( 41%) Resolving: self/v1/schemas/api/schemas/stats/response.json
( 44%) Resolving: self/v1/schemas/api/schemas/trace/response.json
( 48%) Resolving: self/v1/schemas/mcp/error.json
( 51%) Resolving: self/v1/schemas/mcp/initialize/request.json
( 55%) Resolving: self/v1/schemas/mcp/initialize/response.json
( 58%) Resolving: self/v1/schemas/mcp/notifications/cancelled.json
( 62%) Resolving: self/v1/schemas/mcp/notifications/initialized.json
( 65%) Resolving: self/v1/schemas/mcp/ping/request.json
( 68%) Resolving: self/v1/schemas/mcp/ping/response.json
( 72%) Resolving: self/v1/schemas/mcp/request.json
( 75%) Resolving: self/v1/schemas/mcp/resources/list/request.json
( 79%) Resolving: self/v1/schemas/mcp/resources/list/response.json
( 82%) Resolving: self/v1/schemas/mcp/resources/read/request.json
( 86%) Resolving: self/v1/schemas/mcp/resources/read/response.json
( 89%) Resolving: self/v1/schemas/mcp/resources/templates/list/request.json
( 93%) Resolving: self/v1/schemas/mcp/resources/templates/list/response.json
( 96%) Resolving: self/v1/schemas/mcp/response.json
(100%) Resolving: test/test.json
(  0%) Producing: configuration.json
(  0%) Producing: metadata.json
(  0%) Producing: version.json
(  0%) Producing: explorer/%/404.metapack
(  1%) Producing: schemas/self/v1/schemas/api/error/%/schema.metapack
(  1%) Producing: schemas/self/v1/schemas/api/list/response/%/schema.metapack
(  1%) Producing: schemas/self/v1/schemas/api/schemas/dependencies/response/%/schema.metapack
(  1%) Producing: schemas/self/v1/schemas/api/schemas/dependents/response/%/schema.metapack
(  2%) Producing: schemas/self/v1/schemas/api/schemas/evaluate/response/%/schema.metapack
(  2%) Producing: schemas/self/v1/schemas/api/schemas/health/response/%/schema.metapack
(  2%) Producing: schemas/self/v1/schemas/api/schemas/locations/response/%/schema.metapack
(  2%) Producing: schemas/self/v1/schemas/api/schemas/metadata/response/%/schema.metapack
(  3%) Producing: schemas/self/v1/schemas/api/schemas/position/%/schema.metapack
(  3%) Producing: schemas/self/v1/schemas/api/schemas/positions/response/%/schema.metapack
(  3%) Producing: schemas/self/v1/schemas/api/schemas/search/response/%/schema.metapack
(  3%) Producing: schemas/self/v1/schemas/api/schemas/stats/response/%/schema.metapack
(  4%) Producing: schemas/self/v1/schemas/api/schemas/trace/response/%/schema.metapack
(  4%) Producing: schemas/self/v1/schemas/mcp/error/%/schema.metapack
(  4%) Producing: schemas/self/v1/schemas/mcp/initialize/request/%/schema.metapack
(  4%) Producing: schemas/self/v1/schemas/mcp/initialize/response/%/schema.metapack
(  5%) Producing: schemas/self/v1/schemas/mcp/notifications/cancelled/%/schema.metapack
(  5%) Producing: schemas/self/v1/schemas/mcp/notifications/initialized/%/schema.metapack
(  5%) Producing: schemas/self/v1/schemas/mcp/ping/request/%/schema.metapack
(  5%) Producing: schemas/self/v1/schemas/mcp/ping/response/%/schema.metapack
(  6%) Producing: schemas/self/v1/schemas/mcp/request/%/schema.metapack
(  6%) Producing: schemas/self/v1/schemas/mcp/resources/list/request/%/schema.metapack
(  6%) Producing: schemas/self/v1/schemas/mcp/resources/list/response/%/schema.metapack
(  6%) Producing: schemas/self/v1/schemas/mcp/resources/read/request/%/schema.metapack
(  7%) Producing: schemas/self/v1/schemas/mcp/resources/read/response/%/schema.metapack
(  7%) Producing: schemas/self/v1/schemas/mcp/resources/templates/list/request/%/schema.metapack
(  7%) Producing: schemas/self/v1/schemas/mcp/resources/templates/list/response/%/schema.metapack
(  7%) Producing: schemas/self/v1/schemas/mcp/response/%/schema.metapack
(  8%) Producing: schemas/test/test/%/schema.metapack
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
( 12%) Producing: schemas/self/v1/schemas/api/schemas/evaluate/response/%/stats.metapack
( 13%) Producing: schemas/self/v1/schemas/api/schemas/health/response/%/dependencies.metapack
( 13%) Producing: schemas/self/v1/schemas/api/schemas/health/response/%/locations.metapack
( 13%) Producing: schemas/self/v1/schemas/api/schemas/health/response/%/positions.metapack
( 13%) Producing: schemas/self/v1/schemas/api/schemas/health/response/%/stats.metapack
( 14%) Producing: schemas/self/v1/schemas/api/schemas/locations/response/%/dependencies.metapack
( 14%) Producing: schemas/self/v1/schemas/api/schemas/locations/response/%/locations.metapack
( 14%) Producing: schemas/self/v1/schemas/api/schemas/locations/response/%/positions.metapack
( 14%) Producing: schemas/self/v1/schemas/api/schemas/locations/response/%/stats.metapack
( 15%) Producing: schemas/self/v1/schemas/api/schemas/metadata/response/%/dependencies.metapack
( 15%) Producing: schemas/self/v1/schemas/api/schemas/metadata/response/%/locations.metapack
( 15%) Producing: schemas/self/v1/schemas/api/schemas/metadata/response/%/positions.metapack
( 15%) Producing: schemas/self/v1/schemas/api/schemas/metadata/response/%/stats.metapack
( 16%) Producing: schemas/self/v1/schemas/api/schemas/position/%/dependencies.metapack
( 16%) Producing: schemas/self/v1/schemas/api/schemas/position/%/locations.metapack
( 16%) Producing: schemas/self/v1/schemas/api/schemas/position/%/positions.metapack
( 16%) Producing: schemas/self/v1/schemas/api/schemas/position/%/stats.metapack
( 17%) Producing: schemas/self/v1/schemas/api/schemas/positions/response/%/dependencies.metapack
( 17%) Producing: schemas/self/v1/schemas/api/schemas/positions/response/%/locations.metapack
( 17%) Producing: schemas/self/v1/schemas/api/schemas/positions/response/%/positions.metapack
( 17%) Producing: schemas/self/v1/schemas/api/schemas/positions/response/%/stats.metapack
( 18%) Producing: schemas/self/v1/schemas/api/schemas/search/response/%/dependencies.metapack
( 18%) Producing: schemas/self/v1/schemas/api/schemas/search/response/%/locations.metapack
( 18%) Producing: schemas/self/v1/schemas/api/schemas/search/response/%/positions.metapack
( 18%) Producing: schemas/self/v1/schemas/api/schemas/search/response/%/stats.metapack
( 19%) Producing: schemas/self/v1/schemas/api/schemas/stats/response/%/dependencies.metapack
( 19%) Producing: schemas/self/v1/schemas/api/schemas/stats/response/%/locations.metapack
( 19%) Producing: schemas/self/v1/schemas/api/schemas/stats/response/%/positions.metapack
( 19%) Producing: schemas/self/v1/schemas/api/schemas/stats/response/%/stats.metapack
( 20%) Producing: schemas/self/v1/schemas/api/schemas/trace/response/%/dependencies.metapack
( 20%) Producing: schemas/self/v1/schemas/api/schemas/trace/response/%/locations.metapack
( 20%) Producing: schemas/self/v1/schemas/api/schemas/trace/response/%/positions.metapack
( 20%) Producing: schemas/self/v1/schemas/api/schemas/trace/response/%/stats.metapack
( 21%) Producing: schemas/self/v1/schemas/mcp/error/%/dependencies.metapack
( 21%) Producing: schemas/self/v1/schemas/mcp/error/%/locations.metapack
( 21%) Producing: schemas/self/v1/schemas/mcp/error/%/positions.metapack
( 21%) Producing: schemas/self/v1/schemas/mcp/error/%/stats.metapack
( 22%) Producing: schemas/self/v1/schemas/mcp/initialize/request/%/dependencies.metapack
( 22%) Producing: schemas/self/v1/schemas/mcp/initialize/request/%/locations.metapack
( 22%) Producing: schemas/self/v1/schemas/mcp/initialize/request/%/positions.metapack
( 22%) Producing: schemas/self/v1/schemas/mcp/initialize/request/%/stats.metapack
( 23%) Producing: schemas/self/v1/schemas/mcp/initialize/response/%/dependencies.metapack
( 23%) Producing: schemas/self/v1/schemas/mcp/initialize/response/%/locations.metapack
( 23%) Producing: schemas/self/v1/schemas/mcp/initialize/response/%/positions.metapack
( 23%) Producing: schemas/self/v1/schemas/mcp/initialize/response/%/stats.metapack
( 24%) Producing: schemas/self/v1/schemas/mcp/notifications/cancelled/%/dependencies.metapack
( 24%) Producing: schemas/self/v1/schemas/mcp/notifications/cancelled/%/locations.metapack
( 24%) Producing: schemas/self/v1/schemas/mcp/notifications/cancelled/%/positions.metapack
( 24%) Producing: schemas/self/v1/schemas/mcp/notifications/cancelled/%/stats.metapack
( 25%) Producing: schemas/self/v1/schemas/mcp/notifications/initialized/%/dependencies.metapack
( 25%) Producing: schemas/self/v1/schemas/mcp/notifications/initialized/%/locations.metapack
( 25%) Producing: schemas/self/v1/schemas/mcp/notifications/initialized/%/positions.metapack
( 25%) Producing: schemas/self/v1/schemas/mcp/notifications/initialized/%/stats.metapack
( 25%) Producing: schemas/self/v1/schemas/mcp/ping/request/%/dependencies.metapack
( 26%) Producing: schemas/self/v1/schemas/mcp/ping/request/%/locations.metapack
( 26%) Producing: schemas/self/v1/schemas/mcp/ping/request/%/positions.metapack
( 26%) Producing: schemas/self/v1/schemas/mcp/ping/request/%/stats.metapack
( 26%) Producing: schemas/self/v1/schemas/mcp/ping/response/%/dependencies.metapack
( 27%) Producing: schemas/self/v1/schemas/mcp/ping/response/%/locations.metapack
( 27%) Producing: schemas/self/v1/schemas/mcp/ping/response/%/positions.metapack
( 27%) Producing: schemas/self/v1/schemas/mcp/ping/response/%/stats.metapack
( 27%) Producing: schemas/self/v1/schemas/mcp/request/%/dependencies.metapack
( 28%) Producing: schemas/self/v1/schemas/mcp/request/%/locations.metapack
( 28%) Producing: schemas/self/v1/schemas/mcp/request/%/positions.metapack
( 28%) Producing: schemas/self/v1/schemas/mcp/request/%/stats.metapack
( 28%) Producing: schemas/self/v1/schemas/mcp/resources/list/request/%/dependencies.metapack
( 29%) Producing: schemas/self/v1/schemas/mcp/resources/list/request/%/locations.metapack
( 29%) Producing: schemas/self/v1/schemas/mcp/resources/list/request/%/positions.metapack
( 29%) Producing: schemas/self/v1/schemas/mcp/resources/list/request/%/stats.metapack
( 29%) Producing: schemas/self/v1/schemas/mcp/resources/list/response/%/dependencies.metapack
( 30%) Producing: schemas/self/v1/schemas/mcp/resources/list/response/%/locations.metapack
( 30%) Producing: schemas/self/v1/schemas/mcp/resources/list/response/%/positions.metapack
( 30%) Producing: schemas/self/v1/schemas/mcp/resources/list/response/%/stats.metapack
( 30%) Producing: schemas/self/v1/schemas/mcp/resources/read/request/%/dependencies.metapack
( 31%) Producing: schemas/self/v1/schemas/mcp/resources/read/request/%/locations.metapack
( 31%) Producing: schemas/self/v1/schemas/mcp/resources/read/request/%/positions.metapack
( 31%) Producing: schemas/self/v1/schemas/mcp/resources/read/request/%/stats.metapack
( 31%) Producing: schemas/self/v1/schemas/mcp/resources/read/response/%/dependencies.metapack
( 32%) Producing: schemas/self/v1/schemas/mcp/resources/read/response/%/locations.metapack
( 32%) Producing: schemas/self/v1/schemas/mcp/resources/read/response/%/positions.metapack
( 32%) Producing: schemas/self/v1/schemas/mcp/resources/read/response/%/stats.metapack
( 32%) Producing: schemas/self/v1/schemas/mcp/resources/templates/list/request/%/dependencies.metapack
( 33%) Producing: schemas/self/v1/schemas/mcp/resources/templates/list/request/%/locations.metapack
( 33%) Producing: schemas/self/v1/schemas/mcp/resources/templates/list/request/%/positions.metapack
( 33%) Producing: schemas/self/v1/schemas/mcp/resources/templates/list/request/%/stats.metapack
( 33%) Producing: schemas/self/v1/schemas/mcp/resources/templates/list/response/%/dependencies.metapack
( 34%) Producing: schemas/self/v1/schemas/mcp/resources/templates/list/response/%/locations.metapack
( 34%) Producing: schemas/self/v1/schemas/mcp/resources/templates/list/response/%/positions.metapack
( 34%) Producing: schemas/self/v1/schemas/mcp/resources/templates/list/response/%/stats.metapack
( 34%) Producing: schemas/self/v1/schemas/mcp/response/%/dependencies.metapack
( 35%) Producing: schemas/self/v1/schemas/mcp/response/%/locations.metapack
( 35%) Producing: schemas/self/v1/schemas/mcp/response/%/positions.metapack
( 35%) Producing: schemas/self/v1/schemas/mcp/response/%/stats.metapack
( 35%) Producing: schemas/test/test/%/dependencies.metapack
( 36%) Producing: schemas/test/test/%/locations.metapack
( 36%) Producing: schemas/test/test/%/positions.metapack
( 36%) Producing: schemas/test/test/%/stats.metapack
( 36%) Producing: schemas/self/v1/schemas/api/error/%/bundle.metapack
( 37%) Producing: schemas/self/v1/schemas/api/error/%/health.metapack
( 37%) Producing: schemas/self/v1/schemas/api/list/response/%/bundle.metapack
( 37%) Producing: schemas/self/v1/schemas/api/list/response/%/health.metapack
( 37%) Producing: schemas/self/v1/schemas/api/schemas/dependencies/response/%/bundle.metapack
( 37%) Producing: schemas/self/v1/schemas/api/schemas/dependencies/response/%/health.metapack
( 38%) Producing: schemas/self/v1/schemas/api/schemas/dependents/response/%/bundle.metapack
( 38%) Producing: schemas/self/v1/schemas/api/schemas/dependents/response/%/health.metapack
( 38%) Producing: schemas/self/v1/schemas/api/schemas/evaluate/response/%/bundle.metapack
( 38%) Producing: schemas/self/v1/schemas/api/schemas/evaluate/response/%/health.metapack
( 39%) Producing: schemas/self/v1/schemas/api/schemas/health/response/%/bundle.metapack
( 39%) Producing: schemas/self/v1/schemas/api/schemas/health/response/%/health.metapack
( 39%) Producing: schemas/self/v1/schemas/api/schemas/locations/response/%/bundle.metapack
( 39%) Producing: schemas/self/v1/schemas/api/schemas/locations/response/%/health.metapack
( 40%) Producing: schemas/self/v1/schemas/api/schemas/metadata/response/%/bundle.metapack
( 40%) Producing: schemas/self/v1/schemas/api/schemas/metadata/response/%/health.metapack
( 40%) Producing: schemas/self/v1/schemas/api/schemas/position/%/bundle.metapack
( 40%) Producing: schemas/self/v1/schemas/api/schemas/position/%/health.metapack
( 41%) Producing: schemas/self/v1/schemas/api/schemas/positions/response/%/bundle.metapack
( 41%) Producing: schemas/self/v1/schemas/api/schemas/positions/response/%/health.metapack
( 41%) Producing: schemas/self/v1/schemas/api/schemas/search/response/%/bundle.metapack
( 41%) Producing: schemas/self/v1/schemas/api/schemas/search/response/%/health.metapack
( 42%) Producing: schemas/self/v1/schemas/api/schemas/stats/response/%/bundle.metapack
( 42%) Producing: schemas/self/v1/schemas/api/schemas/stats/response/%/health.metapack
( 42%) Producing: schemas/self/v1/schemas/api/schemas/trace/response/%/bundle.metapack
( 42%) Producing: schemas/self/v1/schemas/api/schemas/trace/response/%/health.metapack
( 43%) Producing: schemas/self/v1/schemas/mcp/error/%/bundle.metapack
( 43%) Producing: schemas/self/v1/schemas/mcp/error/%/health.metapack
( 43%) Producing: schemas/self/v1/schemas/mcp/initialize/request/%/bundle.metapack
( 43%) Producing: schemas/self/v1/schemas/mcp/initialize/request/%/health.metapack
( 44%) Producing: schemas/self/v1/schemas/mcp/initialize/response/%/bundle.metapack
( 44%) Producing: schemas/self/v1/schemas/mcp/initialize/response/%/health.metapack
( 44%) Producing: schemas/self/v1/schemas/mcp/notifications/cancelled/%/bundle.metapack
( 44%) Producing: schemas/self/v1/schemas/mcp/notifications/cancelled/%/health.metapack
( 45%) Producing: schemas/self/v1/schemas/mcp/notifications/initialized/%/bundle.metapack
( 45%) Producing: schemas/self/v1/schemas/mcp/notifications/initialized/%/health.metapack
( 45%) Producing: schemas/self/v1/schemas/mcp/ping/request/%/bundle.metapack
( 45%) Producing: schemas/self/v1/schemas/mcp/ping/request/%/health.metapack
( 46%) Producing: schemas/self/v1/schemas/mcp/ping/response/%/bundle.metapack
( 46%) Producing: schemas/self/v1/schemas/mcp/ping/response/%/health.metapack
( 46%) Producing: schemas/self/v1/schemas/mcp/request/%/bundle.metapack
( 46%) Producing: schemas/self/v1/schemas/mcp/request/%/health.metapack
( 47%) Producing: schemas/self/v1/schemas/mcp/resources/list/request/%/bundle.metapack
( 47%) Producing: schemas/self/v1/schemas/mcp/resources/list/request/%/health.metapack
( 47%) Producing: schemas/self/v1/schemas/mcp/resources/list/response/%/bundle.metapack
( 47%) Producing: schemas/self/v1/schemas/mcp/resources/list/response/%/health.metapack
( 48%) Producing: schemas/self/v1/schemas/mcp/resources/read/request/%/bundle.metapack
( 48%) Producing: schemas/self/v1/schemas/mcp/resources/read/request/%/health.metapack
( 48%) Producing: schemas/self/v1/schemas/mcp/resources/read/response/%/bundle.metapack
( 48%) Producing: schemas/self/v1/schemas/mcp/resources/read/response/%/health.metapack
( 49%) Producing: schemas/self/v1/schemas/mcp/resources/templates/list/request/%/bundle.metapack
( 49%) Producing: schemas/self/v1/schemas/mcp/resources/templates/list/request/%/health.metapack
( 49%) Producing: schemas/self/v1/schemas/mcp/resources/templates/list/response/%/bundle.metapack
( 49%) Producing: schemas/self/v1/schemas/mcp/resources/templates/list/response/%/health.metapack
( 50%) Producing: schemas/self/v1/schemas/mcp/response/%/bundle.metapack
( 50%) Producing: schemas/self/v1/schemas/mcp/response/%/health.metapack
( 50%) Producing: schemas/test/test/%/bundle.metapack
( 50%) Producing: schemas/test/test/%/health.metapack
error: Could not locate the requested file
  at path $(realpath "$TMP")/rules/does-not-exist.json
EOF

diff "$TMP/output.txt" "$TMP/expected.txt"
