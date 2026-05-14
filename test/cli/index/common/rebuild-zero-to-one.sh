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
    "schemas": {
      "baseUri": "https://example.com/",
      "path": "./schemas"
    }
  }
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

# First run: the schemas directory does not exist at all
test ! -d "$TMP/schemas"
"$1" --skip-banner --deterministic "$TMP/one.json" "$TMP/output" --concurrency 1 2> "$TMP/output.txt"
remove_threads_information "$TMP/output.txt"

cat << EOF > "$TMP/expected.txt"
Writing output to: $(realpath "$TMP")/output
Using configuration: $(realpath "$TMP")/one.json
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/api/error.json (#1)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/api/list/response.json (#2)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/api/list/rpc.json (#3)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/api/schemas/dependencies/response.json (#4)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/api/schemas/dependencies/rpc.json (#5)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/api/schemas/dependents/response.json (#6)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/api/schemas/dependents/rpc.json (#7)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/api/schemas/evaluate/request.json (#8)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/api/schemas/evaluate/response.json (#9)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/api/schemas/evaluate/rpc.json (#10)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/api/schemas/health/response.json (#11)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/api/schemas/health/rpc.json (#12)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/api/schemas/locations/response.json (#13)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/api/schemas/locations/rpc.json (#14)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/api/schemas/metadata/response.json (#15)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/api/schemas/metadata/rpc.json (#16)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/api/schemas/position.json (#17)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/api/schemas/positions/response.json (#18)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/api/schemas/positions/rpc.json (#19)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/api/schemas/search/response.json (#20)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/api/schemas/search/rpc.json (#21)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/api/schemas/stats/response.json (#22)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/api/schemas/stats/rpc.json (#23)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/api/schemas/trace/request.json (#24)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/api/schemas/trace/response.json (#25)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/api/schemas/trace/rpc.json (#26)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/error.json (#27)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/initialize/request.json (#28)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/initialize/response.json (#29)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/notifications/cancelled.json (#30)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/notifications/initialized.json (#31)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/ping/request.json (#32)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/ping/response.json (#33)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/request.json (#34)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/resources/list/request.json (#35)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/resources/list/response.json (#36)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/resources/read/request.json (#37)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/resources/read/response.json (#38)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/resources/templates/list/request.json (#39)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/resources/templates/list/response.json (#40)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/response.json (#41)
(  2%) Resolving: self/v1/schemas/api/error.json
(  4%) Resolving: self/v1/schemas/api/list/response.json
(  7%) Resolving: self/v1/schemas/api/list/rpc.json
(  9%) Resolving: self/v1/schemas/api/schemas/dependencies/response.json
( 12%) Resolving: self/v1/schemas/api/schemas/dependencies/rpc.json
( 14%) Resolving: self/v1/schemas/api/schemas/dependents/response.json
( 17%) Resolving: self/v1/schemas/api/schemas/dependents/rpc.json
( 19%) Resolving: self/v1/schemas/api/schemas/evaluate/request.json
( 21%) Resolving: self/v1/schemas/api/schemas/evaluate/response.json
( 24%) Resolving: self/v1/schemas/api/schemas/evaluate/rpc.json
( 26%) Resolving: self/v1/schemas/api/schemas/health/response.json
( 29%) Resolving: self/v1/schemas/api/schemas/health/rpc.json
( 31%) Resolving: self/v1/schemas/api/schemas/locations/response.json
( 34%) Resolving: self/v1/schemas/api/schemas/locations/rpc.json
( 36%) Resolving: self/v1/schemas/api/schemas/metadata/response.json
( 39%) Resolving: self/v1/schemas/api/schemas/metadata/rpc.json
( 41%) Resolving: self/v1/schemas/api/schemas/position.json
( 43%) Resolving: self/v1/schemas/api/schemas/positions/response.json
( 46%) Resolving: self/v1/schemas/api/schemas/positions/rpc.json
( 48%) Resolving: self/v1/schemas/api/schemas/search/response.json
( 51%) Resolving: self/v1/schemas/api/schemas/search/rpc.json
( 53%) Resolving: self/v1/schemas/api/schemas/stats/response.json
( 56%) Resolving: self/v1/schemas/api/schemas/stats/rpc.json
( 58%) Resolving: self/v1/schemas/api/schemas/trace/request.json
( 60%) Resolving: self/v1/schemas/api/schemas/trace/response.json
( 63%) Resolving: self/v1/schemas/api/schemas/trace/rpc.json
( 65%) Resolving: self/v1/schemas/mcp/error.json
( 68%) Resolving: self/v1/schemas/mcp/initialize/request.json
( 70%) Resolving: self/v1/schemas/mcp/initialize/response.json
( 73%) Resolving: self/v1/schemas/mcp/notifications/cancelled.json
( 75%) Resolving: self/v1/schemas/mcp/notifications/initialized.json
( 78%) Resolving: self/v1/schemas/mcp/ping/request.json
( 80%) Resolving: self/v1/schemas/mcp/ping/response.json
( 82%) Resolving: self/v1/schemas/mcp/request.json
( 85%) Resolving: self/v1/schemas/mcp/resources/list/request.json
( 87%) Resolving: self/v1/schemas/mcp/resources/list/response.json
( 90%) Resolving: self/v1/schemas/mcp/resources/read/request.json
( 92%) Resolving: self/v1/schemas/mcp/resources/read/response.json
( 95%) Resolving: self/v1/schemas/mcp/resources/templates/list/request.json
( 97%) Resolving: self/v1/schemas/mcp/resources/templates/list/response.json
(100%) Resolving: self/v1/schemas/mcp/response.json
(  0%) Producing: configuration.json
(  0%) Producing: version.json
(  0%) Producing: explorer/%/404.metapack
(  0%) Producing: schemas/self/v1/schemas/api/error/%/schema.metapack
(  0%) Producing: schemas/self/v1/schemas/api/list/response/%/schema.metapack
(  1%) Producing: schemas/self/v1/schemas/api/list/rpc/%/schema.metapack
(  1%) Producing: schemas/self/v1/schemas/api/schemas/dependencies/response/%/schema.metapack
(  1%) Producing: schemas/self/v1/schemas/api/schemas/dependencies/rpc/%/schema.metapack
(  1%) Producing: schemas/self/v1/schemas/api/schemas/dependents/response/%/schema.metapack
(  1%) Producing: schemas/self/v1/schemas/api/schemas/dependents/rpc/%/schema.metapack
(  2%) Producing: schemas/self/v1/schemas/api/schemas/evaluate/request/%/schema.metapack
(  2%) Producing: schemas/self/v1/schemas/api/schemas/evaluate/response/%/schema.metapack
(  2%) Producing: schemas/self/v1/schemas/api/schemas/evaluate/rpc/%/schema.metapack
(  2%) Producing: schemas/self/v1/schemas/api/schemas/health/response/%/schema.metapack
(  2%) Producing: schemas/self/v1/schemas/api/schemas/health/rpc/%/schema.metapack
(  2%) Producing: schemas/self/v1/schemas/api/schemas/locations/response/%/schema.metapack
(  3%) Producing: schemas/self/v1/schemas/api/schemas/locations/rpc/%/schema.metapack
(  3%) Producing: schemas/self/v1/schemas/api/schemas/metadata/response/%/schema.metapack
(  3%) Producing: schemas/self/v1/schemas/api/schemas/metadata/rpc/%/schema.metapack
(  3%) Producing: schemas/self/v1/schemas/api/schemas/position/%/schema.metapack
(  3%) Producing: schemas/self/v1/schemas/api/schemas/positions/response/%/schema.metapack
(  4%) Producing: schemas/self/v1/schemas/api/schemas/positions/rpc/%/schema.metapack
(  4%) Producing: schemas/self/v1/schemas/api/schemas/search/response/%/schema.metapack
(  4%) Producing: schemas/self/v1/schemas/api/schemas/search/rpc/%/schema.metapack
(  4%) Producing: schemas/self/v1/schemas/api/schemas/stats/response/%/schema.metapack
(  4%) Producing: schemas/self/v1/schemas/api/schemas/stats/rpc/%/schema.metapack
(  4%) Producing: schemas/self/v1/schemas/api/schemas/trace/request/%/schema.metapack
(  5%) Producing: schemas/self/v1/schemas/api/schemas/trace/response/%/schema.metapack
(  5%) Producing: schemas/self/v1/schemas/api/schemas/trace/rpc/%/schema.metapack
(  5%) Producing: schemas/self/v1/schemas/mcp/error/%/schema.metapack
(  5%) Producing: schemas/self/v1/schemas/mcp/initialize/request/%/schema.metapack
(  5%) Producing: schemas/self/v1/schemas/mcp/initialize/response/%/schema.metapack
(  6%) Producing: schemas/self/v1/schemas/mcp/notifications/cancelled/%/schema.metapack
(  6%) Producing: schemas/self/v1/schemas/mcp/notifications/initialized/%/schema.metapack
(  6%) Producing: schemas/self/v1/schemas/mcp/ping/request/%/schema.metapack
(  6%) Producing: schemas/self/v1/schemas/mcp/ping/response/%/schema.metapack
(  6%) Producing: schemas/self/v1/schemas/mcp/request/%/schema.metapack
(  6%) Producing: schemas/self/v1/schemas/mcp/resources/list/request/%/schema.metapack
(  7%) Producing: schemas/self/v1/schemas/mcp/resources/list/response/%/schema.metapack
(  7%) Producing: schemas/self/v1/schemas/mcp/resources/read/request/%/schema.metapack
(  7%) Producing: schemas/self/v1/schemas/mcp/resources/read/response/%/schema.metapack
(  7%) Producing: schemas/self/v1/schemas/mcp/resources/templates/list/request/%/schema.metapack
(  7%) Producing: schemas/self/v1/schemas/mcp/resources/templates/list/response/%/schema.metapack
(  8%) Producing: schemas/self/v1/schemas/mcp/response/%/schema.metapack
(  8%) Producing: schemas/self/v1/schemas/api/error/%/dependencies.metapack
(  8%) Producing: schemas/self/v1/schemas/api/error/%/locations.metapack
(  8%) Producing: schemas/self/v1/schemas/api/error/%/positions.metapack
(  8%) Producing: schemas/self/v1/schemas/api/error/%/stats.metapack
(  8%) Producing: schemas/self/v1/schemas/api/list/response/%/dependencies.metapack
(  9%) Producing: schemas/self/v1/schemas/api/list/response/%/locations.metapack
(  9%) Producing: schemas/self/v1/schemas/api/list/response/%/positions.metapack
(  9%) Producing: schemas/self/v1/schemas/api/list/response/%/stats.metapack
(  9%) Producing: schemas/self/v1/schemas/api/list/rpc/%/dependencies.metapack
(  9%) Producing: schemas/self/v1/schemas/api/list/rpc/%/locations.metapack
( 10%) Producing: schemas/self/v1/schemas/api/list/rpc/%/positions.metapack
( 10%) Producing: schemas/self/v1/schemas/api/list/rpc/%/stats.metapack
( 10%) Producing: schemas/self/v1/schemas/api/schemas/dependencies/response/%/dependencies.metapack
( 10%) Producing: schemas/self/v1/schemas/api/schemas/dependencies/response/%/locations.metapack
( 10%) Producing: schemas/self/v1/schemas/api/schemas/dependencies/response/%/positions.metapack
( 10%) Producing: schemas/self/v1/schemas/api/schemas/dependencies/response/%/stats.metapack
( 11%) Producing: schemas/self/v1/schemas/api/schemas/dependencies/rpc/%/dependencies.metapack
( 11%) Producing: schemas/self/v1/schemas/api/schemas/dependencies/rpc/%/locations.metapack
( 11%) Producing: schemas/self/v1/schemas/api/schemas/dependencies/rpc/%/positions.metapack
( 11%) Producing: schemas/self/v1/schemas/api/schemas/dependencies/rpc/%/stats.metapack
( 11%) Producing: schemas/self/v1/schemas/api/schemas/dependents/response/%/dependencies.metapack
( 12%) Producing: schemas/self/v1/schemas/api/schemas/dependents/response/%/locations.metapack
( 12%) Producing: schemas/self/v1/schemas/api/schemas/dependents/response/%/positions.metapack
( 12%) Producing: schemas/self/v1/schemas/api/schemas/dependents/response/%/stats.metapack
( 12%) Producing: schemas/self/v1/schemas/api/schemas/dependents/rpc/%/dependencies.metapack
( 12%) Producing: schemas/self/v1/schemas/api/schemas/dependents/rpc/%/locations.metapack
( 12%) Producing: schemas/self/v1/schemas/api/schemas/dependents/rpc/%/positions.metapack
( 13%) Producing: schemas/self/v1/schemas/api/schemas/dependents/rpc/%/stats.metapack
( 13%) Producing: schemas/self/v1/schemas/api/schemas/evaluate/request/%/dependencies.metapack
( 13%) Producing: schemas/self/v1/schemas/api/schemas/evaluate/request/%/locations.metapack
( 13%) Producing: schemas/self/v1/schemas/api/schemas/evaluate/request/%/positions.metapack
( 13%) Producing: schemas/self/v1/schemas/api/schemas/evaluate/request/%/stats.metapack
( 14%) Producing: schemas/self/v1/schemas/api/schemas/evaluate/response/%/dependencies.metapack
( 14%) Producing: schemas/self/v1/schemas/api/schemas/evaluate/response/%/locations.metapack
( 14%) Producing: schemas/self/v1/schemas/api/schemas/evaluate/response/%/positions.metapack
( 14%) Producing: schemas/self/v1/schemas/api/schemas/evaluate/response/%/stats.metapack
( 14%) Producing: schemas/self/v1/schemas/api/schemas/evaluate/rpc/%/dependencies.metapack
( 14%) Producing: schemas/self/v1/schemas/api/schemas/evaluate/rpc/%/locations.metapack
( 15%) Producing: schemas/self/v1/schemas/api/schemas/evaluate/rpc/%/positions.metapack
( 15%) Producing: schemas/self/v1/schemas/api/schemas/evaluate/rpc/%/stats.metapack
( 15%) Producing: schemas/self/v1/schemas/api/schemas/health/response/%/dependencies.metapack
( 15%) Producing: schemas/self/v1/schemas/api/schemas/health/response/%/locations.metapack
( 15%) Producing: schemas/self/v1/schemas/api/schemas/health/response/%/positions.metapack
( 16%) Producing: schemas/self/v1/schemas/api/schemas/health/response/%/stats.metapack
( 16%) Producing: schemas/self/v1/schemas/api/schemas/health/rpc/%/dependencies.metapack
( 16%) Producing: schemas/self/v1/schemas/api/schemas/health/rpc/%/locations.metapack
( 16%) Producing: schemas/self/v1/schemas/api/schemas/health/rpc/%/positions.metapack
( 16%) Producing: schemas/self/v1/schemas/api/schemas/health/rpc/%/stats.metapack
( 16%) Producing: schemas/self/v1/schemas/api/schemas/locations/response/%/dependencies.metapack
( 17%) Producing: schemas/self/v1/schemas/api/schemas/locations/response/%/locations.metapack
( 17%) Producing: schemas/self/v1/schemas/api/schemas/locations/response/%/positions.metapack
( 17%) Producing: schemas/self/v1/schemas/api/schemas/locations/response/%/stats.metapack
( 17%) Producing: schemas/self/v1/schemas/api/schemas/locations/rpc/%/dependencies.metapack
( 17%) Producing: schemas/self/v1/schemas/api/schemas/locations/rpc/%/locations.metapack
( 18%) Producing: schemas/self/v1/schemas/api/schemas/locations/rpc/%/positions.metapack
( 18%) Producing: schemas/self/v1/schemas/api/schemas/locations/rpc/%/stats.metapack
( 18%) Producing: schemas/self/v1/schemas/api/schemas/metadata/response/%/dependencies.metapack
( 18%) Producing: schemas/self/v1/schemas/api/schemas/metadata/response/%/locations.metapack
( 18%) Producing: schemas/self/v1/schemas/api/schemas/metadata/response/%/positions.metapack
( 18%) Producing: schemas/self/v1/schemas/api/schemas/metadata/response/%/stats.metapack
( 19%) Producing: schemas/self/v1/schemas/api/schemas/metadata/rpc/%/dependencies.metapack
( 19%) Producing: schemas/self/v1/schemas/api/schemas/metadata/rpc/%/locations.metapack
( 19%) Producing: schemas/self/v1/schemas/api/schemas/metadata/rpc/%/positions.metapack
( 19%) Producing: schemas/self/v1/schemas/api/schemas/metadata/rpc/%/stats.metapack
( 19%) Producing: schemas/self/v1/schemas/api/schemas/position/%/dependencies.metapack
( 20%) Producing: schemas/self/v1/schemas/api/schemas/position/%/locations.metapack
( 20%) Producing: schemas/self/v1/schemas/api/schemas/position/%/positions.metapack
( 20%) Producing: schemas/self/v1/schemas/api/schemas/position/%/stats.metapack
( 20%) Producing: schemas/self/v1/schemas/api/schemas/positions/response/%/dependencies.metapack
( 20%) Producing: schemas/self/v1/schemas/api/schemas/positions/response/%/locations.metapack
( 20%) Producing: schemas/self/v1/schemas/api/schemas/positions/response/%/positions.metapack
( 21%) Producing: schemas/self/v1/schemas/api/schemas/positions/response/%/stats.metapack
( 21%) Producing: schemas/self/v1/schemas/api/schemas/positions/rpc/%/dependencies.metapack
( 21%) Producing: schemas/self/v1/schemas/api/schemas/positions/rpc/%/locations.metapack
( 21%) Producing: schemas/self/v1/schemas/api/schemas/positions/rpc/%/positions.metapack
( 21%) Producing: schemas/self/v1/schemas/api/schemas/positions/rpc/%/stats.metapack
( 22%) Producing: schemas/self/v1/schemas/api/schemas/search/response/%/dependencies.metapack
( 22%) Producing: schemas/self/v1/schemas/api/schemas/search/response/%/locations.metapack
( 22%) Producing: schemas/self/v1/schemas/api/schemas/search/response/%/positions.metapack
( 22%) Producing: schemas/self/v1/schemas/api/schemas/search/response/%/stats.metapack
( 22%) Producing: schemas/self/v1/schemas/api/schemas/search/rpc/%/dependencies.metapack
( 22%) Producing: schemas/self/v1/schemas/api/schemas/search/rpc/%/locations.metapack
( 23%) Producing: schemas/self/v1/schemas/api/schemas/search/rpc/%/positions.metapack
( 23%) Producing: schemas/self/v1/schemas/api/schemas/search/rpc/%/stats.metapack
( 23%) Producing: schemas/self/v1/schemas/api/schemas/stats/response/%/dependencies.metapack
( 23%) Producing: schemas/self/v1/schemas/api/schemas/stats/response/%/locations.metapack
( 23%) Producing: schemas/self/v1/schemas/api/schemas/stats/response/%/positions.metapack
( 24%) Producing: schemas/self/v1/schemas/api/schemas/stats/response/%/stats.metapack
( 24%) Producing: schemas/self/v1/schemas/api/schemas/stats/rpc/%/dependencies.metapack
( 24%) Producing: schemas/self/v1/schemas/api/schemas/stats/rpc/%/locations.metapack
( 24%) Producing: schemas/self/v1/schemas/api/schemas/stats/rpc/%/positions.metapack
( 24%) Producing: schemas/self/v1/schemas/api/schemas/stats/rpc/%/stats.metapack
( 24%) Producing: schemas/self/v1/schemas/api/schemas/trace/request/%/dependencies.metapack
( 25%) Producing: schemas/self/v1/schemas/api/schemas/trace/request/%/locations.metapack
( 25%) Producing: schemas/self/v1/schemas/api/schemas/trace/request/%/positions.metapack
( 25%) Producing: schemas/self/v1/schemas/api/schemas/trace/request/%/stats.metapack
( 25%) Producing: schemas/self/v1/schemas/api/schemas/trace/response/%/dependencies.metapack
( 25%) Producing: schemas/self/v1/schemas/api/schemas/trace/response/%/locations.metapack
( 26%) Producing: schemas/self/v1/schemas/api/schemas/trace/response/%/positions.metapack
( 26%) Producing: schemas/self/v1/schemas/api/schemas/trace/response/%/stats.metapack
( 26%) Producing: schemas/self/v1/schemas/api/schemas/trace/rpc/%/dependencies.metapack
( 26%) Producing: schemas/self/v1/schemas/api/schemas/trace/rpc/%/locations.metapack
( 26%) Producing: schemas/self/v1/schemas/api/schemas/trace/rpc/%/positions.metapack
( 26%) Producing: schemas/self/v1/schemas/api/schemas/trace/rpc/%/stats.metapack
( 27%) Producing: schemas/self/v1/schemas/mcp/error/%/dependencies.metapack
( 27%) Producing: schemas/self/v1/schemas/mcp/error/%/locations.metapack
( 27%) Producing: schemas/self/v1/schemas/mcp/error/%/positions.metapack
( 27%) Producing: schemas/self/v1/schemas/mcp/error/%/stats.metapack
( 27%) Producing: schemas/self/v1/schemas/mcp/initialize/request/%/dependencies.metapack
( 28%) Producing: schemas/self/v1/schemas/mcp/initialize/request/%/locations.metapack
( 28%) Producing: schemas/self/v1/schemas/mcp/initialize/request/%/positions.metapack
( 28%) Producing: schemas/self/v1/schemas/mcp/initialize/request/%/stats.metapack
( 28%) Producing: schemas/self/v1/schemas/mcp/initialize/response/%/dependencies.metapack
( 28%) Producing: schemas/self/v1/schemas/mcp/initialize/response/%/locations.metapack
( 28%) Producing: schemas/self/v1/schemas/mcp/initialize/response/%/positions.metapack
( 29%) Producing: schemas/self/v1/schemas/mcp/initialize/response/%/stats.metapack
( 29%) Producing: schemas/self/v1/schemas/mcp/notifications/cancelled/%/dependencies.metapack
( 29%) Producing: schemas/self/v1/schemas/mcp/notifications/cancelled/%/locations.metapack
( 29%) Producing: schemas/self/v1/schemas/mcp/notifications/cancelled/%/positions.metapack
( 29%) Producing: schemas/self/v1/schemas/mcp/notifications/cancelled/%/stats.metapack
( 30%) Producing: schemas/self/v1/schemas/mcp/notifications/initialized/%/dependencies.metapack
( 30%) Producing: schemas/self/v1/schemas/mcp/notifications/initialized/%/locations.metapack
( 30%) Producing: schemas/self/v1/schemas/mcp/notifications/initialized/%/positions.metapack
( 30%) Producing: schemas/self/v1/schemas/mcp/notifications/initialized/%/stats.metapack
( 30%) Producing: schemas/self/v1/schemas/mcp/ping/request/%/dependencies.metapack
( 30%) Producing: schemas/self/v1/schemas/mcp/ping/request/%/locations.metapack
( 31%) Producing: schemas/self/v1/schemas/mcp/ping/request/%/positions.metapack
( 31%) Producing: schemas/self/v1/schemas/mcp/ping/request/%/stats.metapack
( 31%) Producing: schemas/self/v1/schemas/mcp/ping/response/%/dependencies.metapack
( 31%) Producing: schemas/self/v1/schemas/mcp/ping/response/%/locations.metapack
( 31%) Producing: schemas/self/v1/schemas/mcp/ping/response/%/positions.metapack
( 32%) Producing: schemas/self/v1/schemas/mcp/ping/response/%/stats.metapack
( 32%) Producing: schemas/self/v1/schemas/mcp/request/%/dependencies.metapack
( 32%) Producing: schemas/self/v1/schemas/mcp/request/%/locations.metapack
( 32%) Producing: schemas/self/v1/schemas/mcp/request/%/positions.metapack
( 32%) Producing: schemas/self/v1/schemas/mcp/request/%/stats.metapack
( 32%) Producing: schemas/self/v1/schemas/mcp/resources/list/request/%/dependencies.metapack
( 33%) Producing: schemas/self/v1/schemas/mcp/resources/list/request/%/locations.metapack
( 33%) Producing: schemas/self/v1/schemas/mcp/resources/list/request/%/positions.metapack
( 33%) Producing: schemas/self/v1/schemas/mcp/resources/list/request/%/stats.metapack
( 33%) Producing: schemas/self/v1/schemas/mcp/resources/list/response/%/dependencies.metapack
( 33%) Producing: schemas/self/v1/schemas/mcp/resources/list/response/%/locations.metapack
( 34%) Producing: schemas/self/v1/schemas/mcp/resources/list/response/%/positions.metapack
( 34%) Producing: schemas/self/v1/schemas/mcp/resources/list/response/%/stats.metapack
( 34%) Producing: schemas/self/v1/schemas/mcp/resources/read/request/%/dependencies.metapack
( 34%) Producing: schemas/self/v1/schemas/mcp/resources/read/request/%/locations.metapack
( 34%) Producing: schemas/self/v1/schemas/mcp/resources/read/request/%/positions.metapack
( 34%) Producing: schemas/self/v1/schemas/mcp/resources/read/request/%/stats.metapack
( 35%) Producing: schemas/self/v1/schemas/mcp/resources/read/response/%/dependencies.metapack
( 35%) Producing: schemas/self/v1/schemas/mcp/resources/read/response/%/locations.metapack
( 35%) Producing: schemas/self/v1/schemas/mcp/resources/read/response/%/positions.metapack
( 35%) Producing: schemas/self/v1/schemas/mcp/resources/read/response/%/stats.metapack
( 35%) Producing: schemas/self/v1/schemas/mcp/resources/templates/list/request/%/dependencies.metapack
( 36%) Producing: schemas/self/v1/schemas/mcp/resources/templates/list/request/%/locations.metapack
( 36%) Producing: schemas/self/v1/schemas/mcp/resources/templates/list/request/%/positions.metapack
( 36%) Producing: schemas/self/v1/schemas/mcp/resources/templates/list/request/%/stats.metapack
( 36%) Producing: schemas/self/v1/schemas/mcp/resources/templates/list/response/%/dependencies.metapack
( 36%) Producing: schemas/self/v1/schemas/mcp/resources/templates/list/response/%/locations.metapack
( 36%) Producing: schemas/self/v1/schemas/mcp/resources/templates/list/response/%/positions.metapack
( 37%) Producing: schemas/self/v1/schemas/mcp/resources/templates/list/response/%/stats.metapack
( 37%) Producing: schemas/self/v1/schemas/mcp/response/%/dependencies.metapack
( 37%) Producing: schemas/self/v1/schemas/mcp/response/%/locations.metapack
( 37%) Producing: schemas/self/v1/schemas/mcp/response/%/positions.metapack
( 37%) Producing: schemas/self/v1/schemas/mcp/response/%/stats.metapack
( 38%) Producing: schemas/self/v1/schemas/api/error/%/bundle.metapack
( 38%) Producing: schemas/self/v1/schemas/api/error/%/health.metapack
( 38%) Producing: schemas/self/v1/schemas/api/list/response/%/bundle.metapack
( 38%) Producing: schemas/self/v1/schemas/api/list/response/%/health.metapack
( 38%) Producing: schemas/self/v1/schemas/api/list/rpc/%/bundle.metapack
( 38%) Producing: schemas/self/v1/schemas/api/list/rpc/%/health.metapack
( 39%) Producing: schemas/self/v1/schemas/api/schemas/dependencies/response/%/bundle.metapack
( 39%) Producing: schemas/self/v1/schemas/api/schemas/dependencies/response/%/health.metapack
( 39%) Producing: schemas/self/v1/schemas/api/schemas/dependencies/rpc/%/bundle.metapack
( 39%) Producing: schemas/self/v1/schemas/api/schemas/dependencies/rpc/%/health.metapack
( 39%) Producing: schemas/self/v1/schemas/api/schemas/dependents/response/%/bundle.metapack
( 40%) Producing: schemas/self/v1/schemas/api/schemas/dependents/response/%/health.metapack
( 40%) Producing: schemas/self/v1/schemas/api/schemas/dependents/rpc/%/bundle.metapack
( 40%) Producing: schemas/self/v1/schemas/api/schemas/dependents/rpc/%/health.metapack
( 40%) Producing: schemas/self/v1/schemas/api/schemas/evaluate/request/%/bundle.metapack
( 40%) Producing: schemas/self/v1/schemas/api/schemas/evaluate/request/%/health.metapack
( 40%) Producing: schemas/self/v1/schemas/api/schemas/evaluate/response/%/bundle.metapack
( 41%) Producing: schemas/self/v1/schemas/api/schemas/evaluate/response/%/health.metapack
( 41%) Producing: schemas/self/v1/schemas/api/schemas/evaluate/rpc/%/bundle.metapack
( 41%) Producing: schemas/self/v1/schemas/api/schemas/evaluate/rpc/%/health.metapack
( 41%) Producing: schemas/self/v1/schemas/api/schemas/health/response/%/bundle.metapack
( 41%) Producing: schemas/self/v1/schemas/api/schemas/health/response/%/health.metapack
( 42%) Producing: schemas/self/v1/schemas/api/schemas/health/rpc/%/bundle.metapack
( 42%) Producing: schemas/self/v1/schemas/api/schemas/health/rpc/%/health.metapack
( 42%) Producing: schemas/self/v1/schemas/api/schemas/locations/response/%/bundle.metapack
( 42%) Producing: schemas/self/v1/schemas/api/schemas/locations/response/%/health.metapack
( 42%) Producing: schemas/self/v1/schemas/api/schemas/locations/rpc/%/bundle.metapack
( 42%) Producing: schemas/self/v1/schemas/api/schemas/locations/rpc/%/health.metapack
( 43%) Producing: schemas/self/v1/schemas/api/schemas/metadata/response/%/bundle.metapack
( 43%) Producing: schemas/self/v1/schemas/api/schemas/metadata/response/%/health.metapack
( 43%) Producing: schemas/self/v1/schemas/api/schemas/metadata/rpc/%/bundle.metapack
( 43%) Producing: schemas/self/v1/schemas/api/schemas/metadata/rpc/%/health.metapack
( 43%) Producing: schemas/self/v1/schemas/api/schemas/position/%/bundle.metapack
( 44%) Producing: schemas/self/v1/schemas/api/schemas/position/%/health.metapack
( 44%) Producing: schemas/self/v1/schemas/api/schemas/positions/response/%/bundle.metapack
( 44%) Producing: schemas/self/v1/schemas/api/schemas/positions/response/%/health.metapack
( 44%) Producing: schemas/self/v1/schemas/api/schemas/positions/rpc/%/bundle.metapack
( 44%) Producing: schemas/self/v1/schemas/api/schemas/positions/rpc/%/health.metapack
( 44%) Producing: schemas/self/v1/schemas/api/schemas/search/response/%/bundle.metapack
( 45%) Producing: schemas/self/v1/schemas/api/schemas/search/response/%/health.metapack
( 45%) Producing: schemas/self/v1/schemas/api/schemas/search/rpc/%/bundle.metapack
( 45%) Producing: schemas/self/v1/schemas/api/schemas/search/rpc/%/health.metapack
( 45%) Producing: schemas/self/v1/schemas/api/schemas/stats/response/%/bundle.metapack
( 45%) Producing: schemas/self/v1/schemas/api/schemas/stats/response/%/health.metapack
( 46%) Producing: schemas/self/v1/schemas/api/schemas/stats/rpc/%/bundle.metapack
( 46%) Producing: schemas/self/v1/schemas/api/schemas/stats/rpc/%/health.metapack
( 46%) Producing: schemas/self/v1/schemas/api/schemas/trace/request/%/bundle.metapack
( 46%) Producing: schemas/self/v1/schemas/api/schemas/trace/request/%/health.metapack
( 46%) Producing: schemas/self/v1/schemas/api/schemas/trace/response/%/bundle.metapack
( 46%) Producing: schemas/self/v1/schemas/api/schemas/trace/response/%/health.metapack
( 47%) Producing: schemas/self/v1/schemas/api/schemas/trace/rpc/%/bundle.metapack
( 47%) Producing: schemas/self/v1/schemas/api/schemas/trace/rpc/%/health.metapack
( 47%) Producing: schemas/self/v1/schemas/mcp/error/%/bundle.metapack
( 47%) Producing: schemas/self/v1/schemas/mcp/error/%/health.metapack
( 47%) Producing: schemas/self/v1/schemas/mcp/initialize/request/%/bundle.metapack
( 48%) Producing: schemas/self/v1/schemas/mcp/initialize/request/%/health.metapack
( 48%) Producing: schemas/self/v1/schemas/mcp/initialize/response/%/bundle.metapack
( 48%) Producing: schemas/self/v1/schemas/mcp/initialize/response/%/health.metapack
( 48%) Producing: schemas/self/v1/schemas/mcp/notifications/cancelled/%/bundle.metapack
( 48%) Producing: schemas/self/v1/schemas/mcp/notifications/cancelled/%/health.metapack
( 48%) Producing: schemas/self/v1/schemas/mcp/notifications/initialized/%/bundle.metapack
( 49%) Producing: schemas/self/v1/schemas/mcp/notifications/initialized/%/health.metapack
( 49%) Producing: schemas/self/v1/schemas/mcp/ping/request/%/bundle.metapack
( 49%) Producing: schemas/self/v1/schemas/mcp/ping/request/%/health.metapack
( 49%) Producing: schemas/self/v1/schemas/mcp/ping/response/%/bundle.metapack
( 49%) Producing: schemas/self/v1/schemas/mcp/ping/response/%/health.metapack
( 50%) Producing: schemas/self/v1/schemas/mcp/request/%/bundle.metapack
( 50%) Producing: schemas/self/v1/schemas/mcp/request/%/health.metapack
( 50%) Producing: schemas/self/v1/schemas/mcp/resources/list/request/%/bundle.metapack
( 50%) Producing: schemas/self/v1/schemas/mcp/resources/list/request/%/health.metapack
( 50%) Producing: schemas/self/v1/schemas/mcp/resources/list/response/%/bundle.metapack
( 50%) Producing: schemas/self/v1/schemas/mcp/resources/list/response/%/health.metapack
( 51%) Producing: schemas/self/v1/schemas/mcp/resources/read/request/%/bundle.metapack
( 51%) Producing: schemas/self/v1/schemas/mcp/resources/read/request/%/health.metapack
( 51%) Producing: schemas/self/v1/schemas/mcp/resources/read/response/%/bundle.metapack
( 51%) Producing: schemas/self/v1/schemas/mcp/resources/read/response/%/health.metapack
( 51%) Producing: schemas/self/v1/schemas/mcp/resources/templates/list/request/%/bundle.metapack
( 52%) Producing: schemas/self/v1/schemas/mcp/resources/templates/list/request/%/health.metapack
( 52%) Producing: schemas/self/v1/schemas/mcp/resources/templates/list/response/%/bundle.metapack
( 52%) Producing: schemas/self/v1/schemas/mcp/resources/templates/list/response/%/health.metapack
( 52%) Producing: schemas/self/v1/schemas/mcp/response/%/bundle.metapack
( 52%) Producing: schemas/self/v1/schemas/mcp/response/%/health.metapack
( 52%) Producing: explorer/self/v1/schemas/api/error/%/schema.metapack
( 53%) Producing: explorer/self/v1/schemas/api/list/response/%/schema.metapack
( 53%) Producing: explorer/self/v1/schemas/api/list/rpc/%/schema.metapack
( 53%) Producing: explorer/self/v1/schemas/api/schemas/dependencies/response/%/schema.metapack
( 53%) Producing: explorer/self/v1/schemas/api/schemas/dependencies/rpc/%/schema.metapack
( 53%) Producing: explorer/self/v1/schemas/api/schemas/dependents/response/%/schema.metapack
( 54%) Producing: explorer/self/v1/schemas/api/schemas/dependents/rpc/%/schema.metapack
( 54%) Producing: explorer/self/v1/schemas/api/schemas/evaluate/request/%/schema.metapack
( 54%) Producing: explorer/self/v1/schemas/api/schemas/evaluate/response/%/schema.metapack
( 54%) Producing: explorer/self/v1/schemas/api/schemas/evaluate/rpc/%/schema.metapack
( 54%) Producing: explorer/self/v1/schemas/api/schemas/health/response/%/schema.metapack
( 54%) Producing: explorer/self/v1/schemas/api/schemas/health/rpc/%/schema.metapack
( 55%) Producing: explorer/self/v1/schemas/api/schemas/locations/response/%/schema.metapack
( 55%) Producing: explorer/self/v1/schemas/api/schemas/locations/rpc/%/schema.metapack
( 55%) Producing: explorer/self/v1/schemas/api/schemas/metadata/response/%/schema.metapack
( 55%) Producing: explorer/self/v1/schemas/api/schemas/metadata/rpc/%/schema.metapack
( 55%) Producing: explorer/self/v1/schemas/api/schemas/position/%/schema.metapack
( 56%) Producing: explorer/self/v1/schemas/api/schemas/positions/response/%/schema.metapack
( 56%) Producing: explorer/self/v1/schemas/api/schemas/positions/rpc/%/schema.metapack
( 56%) Producing: explorer/self/v1/schemas/api/schemas/search/response/%/schema.metapack
( 56%) Producing: explorer/self/v1/schemas/api/schemas/search/rpc/%/schema.metapack
( 56%) Producing: explorer/self/v1/schemas/api/schemas/stats/response/%/schema.metapack
( 56%) Producing: explorer/self/v1/schemas/api/schemas/stats/rpc/%/schema.metapack
( 57%) Producing: explorer/self/v1/schemas/api/schemas/trace/request/%/schema.metapack
( 57%) Producing: explorer/self/v1/schemas/api/schemas/trace/response/%/schema.metapack
( 57%) Producing: explorer/self/v1/schemas/api/schemas/trace/rpc/%/schema.metapack
( 57%) Producing: explorer/self/v1/schemas/mcp/error/%/schema.metapack
( 57%) Producing: explorer/self/v1/schemas/mcp/initialize/request/%/schema.metapack
( 58%) Producing: explorer/self/v1/schemas/mcp/initialize/response/%/schema.metapack
( 58%) Producing: explorer/self/v1/schemas/mcp/notifications/cancelled/%/schema.metapack
( 58%) Producing: explorer/self/v1/schemas/mcp/notifications/initialized/%/schema.metapack
( 58%) Producing: explorer/self/v1/schemas/mcp/ping/request/%/schema.metapack
( 58%) Producing: explorer/self/v1/schemas/mcp/ping/response/%/schema.metapack
( 58%) Producing: explorer/self/v1/schemas/mcp/request/%/schema.metapack
( 59%) Producing: explorer/self/v1/schemas/mcp/resources/list/request/%/schema.metapack
( 59%) Producing: explorer/self/v1/schemas/mcp/resources/list/response/%/schema.metapack
( 59%) Producing: explorer/self/v1/schemas/mcp/resources/read/request/%/schema.metapack
( 59%) Producing: explorer/self/v1/schemas/mcp/resources/read/response/%/schema.metapack
( 59%) Producing: explorer/self/v1/schemas/mcp/resources/templates/list/request/%/schema.metapack
( 60%) Producing: explorer/self/v1/schemas/mcp/resources/templates/list/response/%/schema.metapack
( 60%) Producing: explorer/self/v1/schemas/mcp/response/%/schema.metapack
( 60%) Producing: schemas/self/v1/schemas/api/error/%/blaze-exhaustive.metapack
( 60%) Producing: schemas/self/v1/schemas/api/error/%/blaze-fast.metapack
( 60%) Producing: schemas/self/v1/schemas/api/error/%/editor.metapack
( 60%) Producing: schemas/self/v1/schemas/api/list/response/%/blaze-exhaustive.metapack
( 61%) Producing: schemas/self/v1/schemas/api/list/response/%/blaze-fast.metapack
( 61%) Producing: schemas/self/v1/schemas/api/list/response/%/editor.metapack
( 61%) Producing: schemas/self/v1/schemas/api/list/rpc/%/blaze-exhaustive.metapack
( 61%) Producing: schemas/self/v1/schemas/api/list/rpc/%/blaze-fast.metapack
( 61%) Producing: schemas/self/v1/schemas/api/list/rpc/%/editor.metapack
( 62%) Producing: schemas/self/v1/schemas/api/schemas/dependencies/response/%/blaze-exhaustive.metapack
( 62%) Producing: schemas/self/v1/schemas/api/schemas/dependencies/response/%/blaze-fast.metapack
( 62%) Producing: schemas/self/v1/schemas/api/schemas/dependencies/response/%/editor.metapack
( 62%) Producing: schemas/self/v1/schemas/api/schemas/dependencies/rpc/%/blaze-exhaustive.metapack
( 62%) Producing: schemas/self/v1/schemas/api/schemas/dependencies/rpc/%/blaze-fast.metapack
( 62%) Producing: schemas/self/v1/schemas/api/schemas/dependencies/rpc/%/editor.metapack
( 63%) Producing: schemas/self/v1/schemas/api/schemas/dependents/response/%/blaze-exhaustive.metapack
( 63%) Producing: schemas/self/v1/schemas/api/schemas/dependents/response/%/blaze-fast.metapack
( 63%) Producing: schemas/self/v1/schemas/api/schemas/dependents/response/%/editor.metapack
( 63%) Producing: schemas/self/v1/schemas/api/schemas/dependents/rpc/%/blaze-exhaustive.metapack
( 63%) Producing: schemas/self/v1/schemas/api/schemas/dependents/rpc/%/blaze-fast.metapack
( 64%) Producing: schemas/self/v1/schemas/api/schemas/dependents/rpc/%/editor.metapack
( 64%) Producing: schemas/self/v1/schemas/api/schemas/evaluate/request/%/blaze-exhaustive.metapack
( 64%) Producing: schemas/self/v1/schemas/api/schemas/evaluate/request/%/blaze-fast.metapack
( 64%) Producing: schemas/self/v1/schemas/api/schemas/evaluate/request/%/editor.metapack
( 64%) Producing: schemas/self/v1/schemas/api/schemas/evaluate/response/%/blaze-exhaustive.metapack
( 64%) Producing: schemas/self/v1/schemas/api/schemas/evaluate/response/%/blaze-fast.metapack
( 65%) Producing: schemas/self/v1/schemas/api/schemas/evaluate/response/%/editor.metapack
( 65%) Producing: schemas/self/v1/schemas/api/schemas/evaluate/rpc/%/blaze-exhaustive.metapack
( 65%) Producing: schemas/self/v1/schemas/api/schemas/evaluate/rpc/%/blaze-fast.metapack
( 65%) Producing: schemas/self/v1/schemas/api/schemas/evaluate/rpc/%/editor.metapack
( 65%) Producing: schemas/self/v1/schemas/api/schemas/health/response/%/blaze-exhaustive.metapack
( 66%) Producing: schemas/self/v1/schemas/api/schemas/health/response/%/blaze-fast.metapack
( 66%) Producing: schemas/self/v1/schemas/api/schemas/health/response/%/editor.metapack
( 66%) Producing: schemas/self/v1/schemas/api/schemas/health/rpc/%/blaze-exhaustive.metapack
( 66%) Producing: schemas/self/v1/schemas/api/schemas/health/rpc/%/blaze-fast.metapack
( 66%) Producing: schemas/self/v1/schemas/api/schemas/health/rpc/%/editor.metapack
( 66%) Producing: schemas/self/v1/schemas/api/schemas/locations/response/%/blaze-exhaustive.metapack
( 67%) Producing: schemas/self/v1/schemas/api/schemas/locations/response/%/blaze-fast.metapack
( 67%) Producing: schemas/self/v1/schemas/api/schemas/locations/response/%/editor.metapack
( 67%) Producing: schemas/self/v1/schemas/api/schemas/locations/rpc/%/blaze-exhaustive.metapack
( 67%) Producing: schemas/self/v1/schemas/api/schemas/locations/rpc/%/blaze-fast.metapack
( 67%) Producing: schemas/self/v1/schemas/api/schemas/locations/rpc/%/editor.metapack
( 68%) Producing: schemas/self/v1/schemas/api/schemas/metadata/response/%/blaze-exhaustive.metapack
( 68%) Producing: schemas/self/v1/schemas/api/schemas/metadata/response/%/blaze-fast.metapack
( 68%) Producing: schemas/self/v1/schemas/api/schemas/metadata/response/%/editor.metapack
( 68%) Producing: schemas/self/v1/schemas/api/schemas/metadata/rpc/%/blaze-exhaustive.metapack
( 68%) Producing: schemas/self/v1/schemas/api/schemas/metadata/rpc/%/blaze-fast.metapack
( 68%) Producing: schemas/self/v1/schemas/api/schemas/metadata/rpc/%/editor.metapack
( 69%) Producing: schemas/self/v1/schemas/api/schemas/position/%/blaze-exhaustive.metapack
( 69%) Producing: schemas/self/v1/schemas/api/schemas/position/%/blaze-fast.metapack
( 69%) Producing: schemas/self/v1/schemas/api/schemas/position/%/editor.metapack
( 69%) Producing: schemas/self/v1/schemas/api/schemas/positions/response/%/blaze-exhaustive.metapack
( 69%) Producing: schemas/self/v1/schemas/api/schemas/positions/response/%/blaze-fast.metapack
( 70%) Producing: schemas/self/v1/schemas/api/schemas/positions/response/%/editor.metapack
( 70%) Producing: schemas/self/v1/schemas/api/schemas/positions/rpc/%/blaze-exhaustive.metapack
( 70%) Producing: schemas/self/v1/schemas/api/schemas/positions/rpc/%/blaze-fast.metapack
( 70%) Producing: schemas/self/v1/schemas/api/schemas/positions/rpc/%/editor.metapack
( 70%) Producing: schemas/self/v1/schemas/api/schemas/search/response/%/blaze-exhaustive.metapack
( 70%) Producing: schemas/self/v1/schemas/api/schemas/search/response/%/blaze-fast.metapack
( 71%) Producing: schemas/self/v1/schemas/api/schemas/search/response/%/editor.metapack
( 71%) Producing: schemas/self/v1/schemas/api/schemas/search/rpc/%/blaze-exhaustive.metapack
( 71%) Producing: schemas/self/v1/schemas/api/schemas/search/rpc/%/blaze-fast.metapack
( 71%) Producing: schemas/self/v1/schemas/api/schemas/search/rpc/%/editor.metapack
( 71%) Producing: schemas/self/v1/schemas/api/schemas/stats/response/%/blaze-exhaustive.metapack
( 72%) Producing: schemas/self/v1/schemas/api/schemas/stats/response/%/blaze-fast.metapack
( 72%) Producing: schemas/self/v1/schemas/api/schemas/stats/response/%/editor.metapack
( 72%) Producing: schemas/self/v1/schemas/api/schemas/stats/rpc/%/blaze-exhaustive.metapack
( 72%) Producing: schemas/self/v1/schemas/api/schemas/stats/rpc/%/blaze-fast.metapack
( 72%) Producing: schemas/self/v1/schemas/api/schemas/stats/rpc/%/editor.metapack
( 72%) Producing: schemas/self/v1/schemas/api/schemas/trace/request/%/blaze-exhaustive.metapack
( 73%) Producing: schemas/self/v1/schemas/api/schemas/trace/request/%/blaze-fast.metapack
( 73%) Producing: schemas/self/v1/schemas/api/schemas/trace/request/%/editor.metapack
( 73%) Producing: schemas/self/v1/schemas/api/schemas/trace/response/%/blaze-exhaustive.metapack
( 73%) Producing: schemas/self/v1/schemas/api/schemas/trace/response/%/blaze-fast.metapack
( 73%) Producing: schemas/self/v1/schemas/api/schemas/trace/response/%/editor.metapack
( 74%) Producing: schemas/self/v1/schemas/api/schemas/trace/rpc/%/blaze-exhaustive.metapack
( 74%) Producing: schemas/self/v1/schemas/api/schemas/trace/rpc/%/blaze-fast.metapack
( 74%) Producing: schemas/self/v1/schemas/api/schemas/trace/rpc/%/editor.metapack
( 74%) Producing: schemas/self/v1/schemas/mcp/error/%/blaze-exhaustive.metapack
( 74%) Producing: schemas/self/v1/schemas/mcp/error/%/blaze-fast.metapack
( 74%) Producing: schemas/self/v1/schemas/mcp/error/%/editor.metapack
( 75%) Producing: schemas/self/v1/schemas/mcp/initialize/request/%/blaze-exhaustive.metapack
( 75%) Producing: schemas/self/v1/schemas/mcp/initialize/request/%/blaze-fast.metapack
( 75%) Producing: schemas/self/v1/schemas/mcp/initialize/request/%/editor.metapack
( 75%) Producing: schemas/self/v1/schemas/mcp/initialize/response/%/blaze-exhaustive.metapack
( 75%) Producing: schemas/self/v1/schemas/mcp/initialize/response/%/blaze-fast.metapack
( 76%) Producing: schemas/self/v1/schemas/mcp/initialize/response/%/editor.metapack
( 76%) Producing: schemas/self/v1/schemas/mcp/notifications/cancelled/%/blaze-exhaustive.metapack
( 76%) Producing: schemas/self/v1/schemas/mcp/notifications/cancelled/%/blaze-fast.metapack
( 76%) Producing: schemas/self/v1/schemas/mcp/notifications/cancelled/%/editor.metapack
( 76%) Producing: schemas/self/v1/schemas/mcp/notifications/initialized/%/blaze-exhaustive.metapack
( 76%) Producing: schemas/self/v1/schemas/mcp/notifications/initialized/%/blaze-fast.metapack
( 77%) Producing: schemas/self/v1/schemas/mcp/notifications/initialized/%/editor.metapack
( 77%) Producing: schemas/self/v1/schemas/mcp/ping/request/%/blaze-exhaustive.metapack
( 77%) Producing: schemas/self/v1/schemas/mcp/ping/request/%/blaze-fast.metapack
( 77%) Producing: schemas/self/v1/schemas/mcp/ping/request/%/editor.metapack
( 77%) Producing: schemas/self/v1/schemas/mcp/ping/response/%/blaze-exhaustive.metapack
( 78%) Producing: schemas/self/v1/schemas/mcp/ping/response/%/blaze-fast.metapack
( 78%) Producing: schemas/self/v1/schemas/mcp/ping/response/%/editor.metapack
( 78%) Producing: schemas/self/v1/schemas/mcp/request/%/blaze-exhaustive.metapack
( 78%) Producing: schemas/self/v1/schemas/mcp/request/%/blaze-fast.metapack
( 78%) Producing: schemas/self/v1/schemas/mcp/request/%/editor.metapack
( 78%) Producing: schemas/self/v1/schemas/mcp/resources/list/request/%/blaze-exhaustive.metapack
( 79%) Producing: schemas/self/v1/schemas/mcp/resources/list/request/%/blaze-fast.metapack
( 79%) Producing: schemas/self/v1/schemas/mcp/resources/list/request/%/editor.metapack
( 79%) Producing: schemas/self/v1/schemas/mcp/resources/list/response/%/blaze-exhaustive.metapack
( 79%) Producing: schemas/self/v1/schemas/mcp/resources/list/response/%/blaze-fast.metapack
( 79%) Producing: schemas/self/v1/schemas/mcp/resources/list/response/%/editor.metapack
( 80%) Producing: schemas/self/v1/schemas/mcp/resources/read/request/%/blaze-exhaustive.metapack
( 80%) Producing: schemas/self/v1/schemas/mcp/resources/read/request/%/blaze-fast.metapack
( 80%) Producing: schemas/self/v1/schemas/mcp/resources/read/request/%/editor.metapack
( 80%) Producing: schemas/self/v1/schemas/mcp/resources/read/response/%/blaze-exhaustive.metapack
( 80%) Producing: schemas/self/v1/schemas/mcp/resources/read/response/%/blaze-fast.metapack
( 80%) Producing: schemas/self/v1/schemas/mcp/resources/read/response/%/editor.metapack
( 81%) Producing: schemas/self/v1/schemas/mcp/resources/templates/list/request/%/blaze-exhaustive.metapack
( 81%) Producing: schemas/self/v1/schemas/mcp/resources/templates/list/request/%/blaze-fast.metapack
( 81%) Producing: schemas/self/v1/schemas/mcp/resources/templates/list/request/%/editor.metapack
( 81%) Producing: schemas/self/v1/schemas/mcp/resources/templates/list/response/%/blaze-exhaustive.metapack
( 81%) Producing: schemas/self/v1/schemas/mcp/resources/templates/list/response/%/blaze-fast.metapack
( 82%) Producing: schemas/self/v1/schemas/mcp/resources/templates/list/response/%/editor.metapack
( 82%) Producing: schemas/self/v1/schemas/mcp/response/%/blaze-exhaustive.metapack
( 82%) Producing: schemas/self/v1/schemas/mcp/response/%/blaze-fast.metapack
( 82%) Producing: schemas/self/v1/schemas/mcp/response/%/editor.metapack
( 82%) Producing: explorer/self/v1/schemas/api/error/%/schema-html.metapack
( 82%) Producing: explorer/self/v1/schemas/api/list/%/directory.metapack
( 83%) Producing: explorer/self/v1/schemas/api/list/response/%/schema-html.metapack
( 83%) Producing: explorer/self/v1/schemas/api/list/rpc/%/schema-html.metapack
( 83%) Producing: explorer/self/v1/schemas/api/schemas/dependencies/%/directory.metapack
( 83%) Producing: explorer/self/v1/schemas/api/schemas/dependencies/response/%/schema-html.metapack
( 83%) Producing: explorer/self/v1/schemas/api/schemas/dependencies/rpc/%/schema-html.metapack
( 84%) Producing: explorer/self/v1/schemas/api/schemas/dependents/%/directory.metapack
( 84%) Producing: explorer/self/v1/schemas/api/schemas/dependents/response/%/schema-html.metapack
( 84%) Producing: explorer/self/v1/schemas/api/schemas/dependents/rpc/%/schema-html.metapack
( 84%) Producing: explorer/self/v1/schemas/api/schemas/evaluate/%/directory.metapack
( 84%) Producing: explorer/self/v1/schemas/api/schemas/evaluate/request/%/schema-html.metapack
( 84%) Producing: explorer/self/v1/schemas/api/schemas/evaluate/response/%/schema-html.metapack
( 85%) Producing: explorer/self/v1/schemas/api/schemas/evaluate/rpc/%/schema-html.metapack
( 85%) Producing: explorer/self/v1/schemas/api/schemas/health/%/directory.metapack
( 85%) Producing: explorer/self/v1/schemas/api/schemas/health/response/%/schema-html.metapack
( 85%) Producing: explorer/self/v1/schemas/api/schemas/health/rpc/%/schema-html.metapack
( 85%) Producing: explorer/self/v1/schemas/api/schemas/locations/%/directory.metapack
( 86%) Producing: explorer/self/v1/schemas/api/schemas/locations/response/%/schema-html.metapack
( 86%) Producing: explorer/self/v1/schemas/api/schemas/locations/rpc/%/schema-html.metapack
( 86%) Producing: explorer/self/v1/schemas/api/schemas/metadata/%/directory.metapack
( 86%) Producing: explorer/self/v1/schemas/api/schemas/metadata/response/%/schema-html.metapack
( 86%) Producing: explorer/self/v1/schemas/api/schemas/metadata/rpc/%/schema-html.metapack
( 86%) Producing: explorer/self/v1/schemas/api/schemas/position/%/schema-html.metapack
( 87%) Producing: explorer/self/v1/schemas/api/schemas/positions/%/directory.metapack
( 87%) Producing: explorer/self/v1/schemas/api/schemas/positions/response/%/schema-html.metapack
( 87%) Producing: explorer/self/v1/schemas/api/schemas/positions/rpc/%/schema-html.metapack
( 87%) Producing: explorer/self/v1/schemas/api/schemas/search/%/directory.metapack
( 87%) Producing: explorer/self/v1/schemas/api/schemas/search/response/%/schema-html.metapack
( 88%) Producing: explorer/self/v1/schemas/api/schemas/search/rpc/%/schema-html.metapack
( 88%) Producing: explorer/self/v1/schemas/api/schemas/stats/%/directory.metapack
( 88%) Producing: explorer/self/v1/schemas/api/schemas/stats/response/%/schema-html.metapack
( 88%) Producing: explorer/self/v1/schemas/api/schemas/stats/rpc/%/schema-html.metapack
( 88%) Producing: explorer/self/v1/schemas/api/schemas/trace/%/directory.metapack
( 88%) Producing: explorer/self/v1/schemas/api/schemas/trace/request/%/schema-html.metapack
( 89%) Producing: explorer/self/v1/schemas/api/schemas/trace/response/%/schema-html.metapack
( 89%) Producing: explorer/self/v1/schemas/api/schemas/trace/rpc/%/schema-html.metapack
( 89%) Producing: explorer/self/v1/schemas/mcp/error/%/schema-html.metapack
( 89%) Producing: explorer/self/v1/schemas/mcp/initialize/%/directory.metapack
( 89%) Producing: explorer/self/v1/schemas/mcp/initialize/request/%/schema-html.metapack
( 90%) Producing: explorer/self/v1/schemas/mcp/initialize/response/%/schema-html.metapack
( 90%) Producing: explorer/self/v1/schemas/mcp/notifications/%/directory.metapack
( 90%) Producing: explorer/self/v1/schemas/mcp/notifications/cancelled/%/schema-html.metapack
( 90%) Producing: explorer/self/v1/schemas/mcp/notifications/initialized/%/schema-html.metapack
( 90%) Producing: explorer/self/v1/schemas/mcp/ping/%/directory.metapack
( 90%) Producing: explorer/self/v1/schemas/mcp/ping/request/%/schema-html.metapack
( 91%) Producing: explorer/self/v1/schemas/mcp/ping/response/%/schema-html.metapack
( 91%) Producing: explorer/self/v1/schemas/mcp/request/%/schema-html.metapack
( 91%) Producing: explorer/self/v1/schemas/mcp/resources/list/%/directory.metapack
( 91%) Producing: explorer/self/v1/schemas/mcp/resources/list/request/%/schema-html.metapack
( 91%) Producing: explorer/self/v1/schemas/mcp/resources/list/response/%/schema-html.metapack
( 92%) Producing: explorer/self/v1/schemas/mcp/resources/read/%/directory.metapack
( 92%) Producing: explorer/self/v1/schemas/mcp/resources/read/request/%/schema-html.metapack
( 92%) Producing: explorer/self/v1/schemas/mcp/resources/read/response/%/schema-html.metapack
( 92%) Producing: explorer/self/v1/schemas/mcp/resources/templates/list/%/directory.metapack
( 92%) Producing: explorer/self/v1/schemas/mcp/resources/templates/list/request/%/schema-html.metapack
( 92%) Producing: explorer/self/v1/schemas/mcp/resources/templates/list/response/%/schema-html.metapack
( 93%) Producing: explorer/self/v1/schemas/mcp/response/%/schema-html.metapack
( 93%) Producing: explorer/self/v1/schemas/api/list/%/directory-html.metapack
( 93%) Producing: explorer/self/v1/schemas/api/schemas/%/directory.metapack
( 93%) Producing: explorer/self/v1/schemas/api/schemas/dependencies/%/directory-html.metapack
( 93%) Producing: explorer/self/v1/schemas/api/schemas/dependents/%/directory-html.metapack
( 94%) Producing: explorer/self/v1/schemas/api/schemas/evaluate/%/directory-html.metapack
( 94%) Producing: explorer/self/v1/schemas/api/schemas/health/%/directory-html.metapack
( 94%) Producing: explorer/self/v1/schemas/api/schemas/locations/%/directory-html.metapack
( 94%) Producing: explorer/self/v1/schemas/api/schemas/metadata/%/directory-html.metapack
( 94%) Producing: explorer/self/v1/schemas/api/schemas/positions/%/directory-html.metapack
( 94%) Producing: explorer/self/v1/schemas/api/schemas/search/%/directory-html.metapack
( 95%) Producing: explorer/self/v1/schemas/api/schemas/stats/%/directory-html.metapack
( 95%) Producing: explorer/self/v1/schemas/api/schemas/trace/%/directory-html.metapack
( 95%) Producing: explorer/self/v1/schemas/mcp/initialize/%/directory-html.metapack
( 95%) Producing: explorer/self/v1/schemas/mcp/notifications/%/directory-html.metapack
( 95%) Producing: explorer/self/v1/schemas/mcp/ping/%/directory-html.metapack
( 96%) Producing: explorer/self/v1/schemas/mcp/resources/list/%/directory-html.metapack
( 96%) Producing: explorer/self/v1/schemas/mcp/resources/read/%/directory-html.metapack
( 96%) Producing: explorer/self/v1/schemas/mcp/resources/templates/%/directory.metapack
( 96%) Producing: explorer/self/v1/schemas/mcp/resources/templates/list/%/directory-html.metapack
( 96%) Producing: explorer/self/v1/schemas/api/%/directory.metapack
( 96%) Producing: explorer/self/v1/schemas/api/schemas/%/directory-html.metapack
( 97%) Producing: explorer/self/v1/schemas/mcp/resources/%/directory.metapack
( 97%) Producing: explorer/self/v1/schemas/mcp/resources/templates/%/directory-html.metapack
( 97%) Producing: explorer/self/v1/schemas/api/%/directory-html.metapack
( 97%) Producing: explorer/self/v1/schemas/mcp/%/directory.metapack
( 97%) Producing: explorer/self/v1/schemas/mcp/resources/%/directory-html.metapack
( 98%) Producing: explorer/self/v1/schemas/%/directory.metapack
( 98%) Producing: explorer/self/v1/schemas/mcp/%/directory-html.metapack
( 98%) Producing: explorer/self/v1/%/directory.metapack
( 98%) Producing: explorer/self/v1/schemas/%/directory-html.metapack
( 98%) Producing: explorer/self/%/directory.metapack
( 98%) Producing: explorer/self/v1/%/directory-html.metapack
( 99%) Producing: explorer/%/directory.metapack
( 99%) Producing: explorer/self/%/directory-html.metapack
( 99%) Producing: explorer/%/directory-html.metapack
( 99%) Producing: explorer/%/search.metapack
( 99%) Producing: explorer/%/mcp.metapack
(100%) Producing: routes.bin
(  2%) Combining: schemas/self/v1/schemas/api/error/%/dependents.metapack
(  4%) Combining: schemas/self/v1/schemas/api/list/response/%/dependents.metapack
(  7%) Combining: schemas/self/v1/schemas/api/list/rpc/%/dependents.metapack
(  9%) Combining: schemas/self/v1/schemas/api/schemas/dependencies/response/%/dependents.metapack
( 12%) Combining: schemas/self/v1/schemas/api/schemas/dependencies/rpc/%/dependents.metapack
( 14%) Combining: schemas/self/v1/schemas/api/schemas/dependents/response/%/dependents.metapack
( 17%) Combining: schemas/self/v1/schemas/api/schemas/dependents/rpc/%/dependents.metapack
( 19%) Combining: schemas/self/v1/schemas/api/schemas/evaluate/request/%/dependents.metapack
( 21%) Combining: schemas/self/v1/schemas/api/schemas/evaluate/response/%/dependents.metapack
( 24%) Combining: schemas/self/v1/schemas/api/schemas/evaluate/rpc/%/dependents.metapack
( 26%) Combining: schemas/self/v1/schemas/api/schemas/health/response/%/dependents.metapack
( 29%) Combining: schemas/self/v1/schemas/api/schemas/health/rpc/%/dependents.metapack
( 31%) Combining: schemas/self/v1/schemas/api/schemas/locations/response/%/dependents.metapack
( 34%) Combining: schemas/self/v1/schemas/api/schemas/locations/rpc/%/dependents.metapack
( 36%) Combining: schemas/self/v1/schemas/api/schemas/metadata/response/%/dependents.metapack
( 39%) Combining: schemas/self/v1/schemas/api/schemas/metadata/rpc/%/dependents.metapack
( 41%) Combining: schemas/self/v1/schemas/api/schemas/position/%/dependents.metapack
( 43%) Combining: schemas/self/v1/schemas/api/schemas/positions/response/%/dependents.metapack
( 46%) Combining: schemas/self/v1/schemas/api/schemas/positions/rpc/%/dependents.metapack
( 48%) Combining: schemas/self/v1/schemas/api/schemas/search/response/%/dependents.metapack
( 51%) Combining: schemas/self/v1/schemas/api/schemas/search/rpc/%/dependents.metapack
( 53%) Combining: schemas/self/v1/schemas/api/schemas/stats/response/%/dependents.metapack
( 56%) Combining: schemas/self/v1/schemas/api/schemas/stats/rpc/%/dependents.metapack
( 58%) Combining: schemas/self/v1/schemas/api/schemas/trace/request/%/dependents.metapack
( 60%) Combining: schemas/self/v1/schemas/api/schemas/trace/response/%/dependents.metapack
( 63%) Combining: schemas/self/v1/schemas/api/schemas/trace/rpc/%/dependents.metapack
( 65%) Combining: schemas/self/v1/schemas/mcp/error/%/dependents.metapack
( 68%) Combining: schemas/self/v1/schemas/mcp/initialize/request/%/dependents.metapack
( 70%) Combining: schemas/self/v1/schemas/mcp/initialize/response/%/dependents.metapack
( 73%) Combining: schemas/self/v1/schemas/mcp/notifications/cancelled/%/dependents.metapack
( 75%) Combining: schemas/self/v1/schemas/mcp/notifications/initialized/%/dependents.metapack
( 78%) Combining: schemas/self/v1/schemas/mcp/ping/request/%/dependents.metapack
( 80%) Combining: schemas/self/v1/schemas/mcp/ping/response/%/dependents.metapack
( 82%) Combining: schemas/self/v1/schemas/mcp/request/%/dependents.metapack
( 85%) Combining: schemas/self/v1/schemas/mcp/resources/list/request/%/dependents.metapack
( 87%) Combining: schemas/self/v1/schemas/mcp/resources/list/response/%/dependents.metapack
( 90%) Combining: schemas/self/v1/schemas/mcp/resources/read/request/%/dependents.metapack
( 92%) Combining: schemas/self/v1/schemas/mcp/resources/read/response/%/dependents.metapack
( 95%) Combining: schemas/self/v1/schemas/mcp/resources/templates/list/request/%/dependents.metapack
( 97%) Combining: schemas/self/v1/schemas/mcp/resources/templates/list/response/%/dependents.metapack
(100%) Combining: schemas/self/v1/schemas/mcp/response/%/dependents.metapack
EOF
diff "$TMP/output.txt" "$TMP/expected.txt"

cd "$TMP/output"
find . -mindepth 1 | LC_ALL=C sort > "$TMP/manifest.txt"
cd - > /dev/null

cat << 'EOF' > "$TMP/expected_manifest.txt"
./configuration.json
./explorer
./explorer/%
./explorer/%/404.metapack
./explorer/%/directory-html.metapack
./explorer/%/directory.metapack
./explorer/%/mcp.metapack
./explorer/%/search.metapack
./explorer/self
./explorer/self/%
./explorer/self/%/directory-html.metapack
./explorer/self/%/directory.metapack
./explorer/self/v1
./explorer/self/v1/%
./explorer/self/v1/%/directory-html.metapack
./explorer/self/v1/%/directory.metapack
./explorer/self/v1/schemas
./explorer/self/v1/schemas/%
./explorer/self/v1/schemas/%/directory-html.metapack
./explorer/self/v1/schemas/%/directory.metapack
./explorer/self/v1/schemas/api
./explorer/self/v1/schemas/api/%
./explorer/self/v1/schemas/api/%/directory-html.metapack
./explorer/self/v1/schemas/api/%/directory.metapack
./explorer/self/v1/schemas/api/error
./explorer/self/v1/schemas/api/error/%
./explorer/self/v1/schemas/api/error/%/schema-html.metapack
./explorer/self/v1/schemas/api/error/%/schema.metapack
./explorer/self/v1/schemas/api/list
./explorer/self/v1/schemas/api/list/%
./explorer/self/v1/schemas/api/list/%/directory-html.metapack
./explorer/self/v1/schemas/api/list/%/directory.metapack
./explorer/self/v1/schemas/api/list/response
./explorer/self/v1/schemas/api/list/response/%
./explorer/self/v1/schemas/api/list/response/%/schema-html.metapack
./explorer/self/v1/schemas/api/list/response/%/schema.metapack
./explorer/self/v1/schemas/api/list/rpc
./explorer/self/v1/schemas/api/list/rpc/%
./explorer/self/v1/schemas/api/list/rpc/%/schema-html.metapack
./explorer/self/v1/schemas/api/list/rpc/%/schema.metapack
./explorer/self/v1/schemas/api/schemas
./explorer/self/v1/schemas/api/schemas/%
./explorer/self/v1/schemas/api/schemas/%/directory-html.metapack
./explorer/self/v1/schemas/api/schemas/%/directory.metapack
./explorer/self/v1/schemas/api/schemas/dependencies
./explorer/self/v1/schemas/api/schemas/dependencies/%
./explorer/self/v1/schemas/api/schemas/dependencies/%/directory-html.metapack
./explorer/self/v1/schemas/api/schemas/dependencies/%/directory.metapack
./explorer/self/v1/schemas/api/schemas/dependencies/response
./explorer/self/v1/schemas/api/schemas/dependencies/response/%
./explorer/self/v1/schemas/api/schemas/dependencies/response/%/schema-html.metapack
./explorer/self/v1/schemas/api/schemas/dependencies/response/%/schema.metapack
./explorer/self/v1/schemas/api/schemas/dependencies/rpc
./explorer/self/v1/schemas/api/schemas/dependencies/rpc/%
./explorer/self/v1/schemas/api/schemas/dependencies/rpc/%/schema-html.metapack
./explorer/self/v1/schemas/api/schemas/dependencies/rpc/%/schema.metapack
./explorer/self/v1/schemas/api/schemas/dependents
./explorer/self/v1/schemas/api/schemas/dependents/%
./explorer/self/v1/schemas/api/schemas/dependents/%/directory-html.metapack
./explorer/self/v1/schemas/api/schemas/dependents/%/directory.metapack
./explorer/self/v1/schemas/api/schemas/dependents/response
./explorer/self/v1/schemas/api/schemas/dependents/response/%
./explorer/self/v1/schemas/api/schemas/dependents/response/%/schema-html.metapack
./explorer/self/v1/schemas/api/schemas/dependents/response/%/schema.metapack
./explorer/self/v1/schemas/api/schemas/dependents/rpc
./explorer/self/v1/schemas/api/schemas/dependents/rpc/%
./explorer/self/v1/schemas/api/schemas/dependents/rpc/%/schema-html.metapack
./explorer/self/v1/schemas/api/schemas/dependents/rpc/%/schema.metapack
./explorer/self/v1/schemas/api/schemas/evaluate
./explorer/self/v1/schemas/api/schemas/evaluate/%
./explorer/self/v1/schemas/api/schemas/evaluate/%/directory-html.metapack
./explorer/self/v1/schemas/api/schemas/evaluate/%/directory.metapack
./explorer/self/v1/schemas/api/schemas/evaluate/request
./explorer/self/v1/schemas/api/schemas/evaluate/request/%
./explorer/self/v1/schemas/api/schemas/evaluate/request/%/schema-html.metapack
./explorer/self/v1/schemas/api/schemas/evaluate/request/%/schema.metapack
./explorer/self/v1/schemas/api/schemas/evaluate/response
./explorer/self/v1/schemas/api/schemas/evaluate/response/%
./explorer/self/v1/schemas/api/schemas/evaluate/response/%/schema-html.metapack
./explorer/self/v1/schemas/api/schemas/evaluate/response/%/schema.metapack
./explorer/self/v1/schemas/api/schemas/evaluate/rpc
./explorer/self/v1/schemas/api/schemas/evaluate/rpc/%
./explorer/self/v1/schemas/api/schemas/evaluate/rpc/%/schema-html.metapack
./explorer/self/v1/schemas/api/schemas/evaluate/rpc/%/schema.metapack
./explorer/self/v1/schemas/api/schemas/health
./explorer/self/v1/schemas/api/schemas/health/%
./explorer/self/v1/schemas/api/schemas/health/%/directory-html.metapack
./explorer/self/v1/schemas/api/schemas/health/%/directory.metapack
./explorer/self/v1/schemas/api/schemas/health/response
./explorer/self/v1/schemas/api/schemas/health/response/%
./explorer/self/v1/schemas/api/schemas/health/response/%/schema-html.metapack
./explorer/self/v1/schemas/api/schemas/health/response/%/schema.metapack
./explorer/self/v1/schemas/api/schemas/health/rpc
./explorer/self/v1/schemas/api/schemas/health/rpc/%
./explorer/self/v1/schemas/api/schemas/health/rpc/%/schema-html.metapack
./explorer/self/v1/schemas/api/schemas/health/rpc/%/schema.metapack
./explorer/self/v1/schemas/api/schemas/locations
./explorer/self/v1/schemas/api/schemas/locations/%
./explorer/self/v1/schemas/api/schemas/locations/%/directory-html.metapack
./explorer/self/v1/schemas/api/schemas/locations/%/directory.metapack
./explorer/self/v1/schemas/api/schemas/locations/response
./explorer/self/v1/schemas/api/schemas/locations/response/%
./explorer/self/v1/schemas/api/schemas/locations/response/%/schema-html.metapack
./explorer/self/v1/schemas/api/schemas/locations/response/%/schema.metapack
./explorer/self/v1/schemas/api/schemas/locations/rpc
./explorer/self/v1/schemas/api/schemas/locations/rpc/%
./explorer/self/v1/schemas/api/schemas/locations/rpc/%/schema-html.metapack
./explorer/self/v1/schemas/api/schemas/locations/rpc/%/schema.metapack
./explorer/self/v1/schemas/api/schemas/metadata
./explorer/self/v1/schemas/api/schemas/metadata/%
./explorer/self/v1/schemas/api/schemas/metadata/%/directory-html.metapack
./explorer/self/v1/schemas/api/schemas/metadata/%/directory.metapack
./explorer/self/v1/schemas/api/schemas/metadata/response
./explorer/self/v1/schemas/api/schemas/metadata/response/%
./explorer/self/v1/schemas/api/schemas/metadata/response/%/schema-html.metapack
./explorer/self/v1/schemas/api/schemas/metadata/response/%/schema.metapack
./explorer/self/v1/schemas/api/schemas/metadata/rpc
./explorer/self/v1/schemas/api/schemas/metadata/rpc/%
./explorer/self/v1/schemas/api/schemas/metadata/rpc/%/schema-html.metapack
./explorer/self/v1/schemas/api/schemas/metadata/rpc/%/schema.metapack
./explorer/self/v1/schemas/api/schemas/position
./explorer/self/v1/schemas/api/schemas/position/%
./explorer/self/v1/schemas/api/schemas/position/%/schema-html.metapack
./explorer/self/v1/schemas/api/schemas/position/%/schema.metapack
./explorer/self/v1/schemas/api/schemas/positions
./explorer/self/v1/schemas/api/schemas/positions/%
./explorer/self/v1/schemas/api/schemas/positions/%/directory-html.metapack
./explorer/self/v1/schemas/api/schemas/positions/%/directory.metapack
./explorer/self/v1/schemas/api/schemas/positions/response
./explorer/self/v1/schemas/api/schemas/positions/response/%
./explorer/self/v1/schemas/api/schemas/positions/response/%/schema-html.metapack
./explorer/self/v1/schemas/api/schemas/positions/response/%/schema.metapack
./explorer/self/v1/schemas/api/schemas/positions/rpc
./explorer/self/v1/schemas/api/schemas/positions/rpc/%
./explorer/self/v1/schemas/api/schemas/positions/rpc/%/schema-html.metapack
./explorer/self/v1/schemas/api/schemas/positions/rpc/%/schema.metapack
./explorer/self/v1/schemas/api/schemas/search
./explorer/self/v1/schemas/api/schemas/search/%
./explorer/self/v1/schemas/api/schemas/search/%/directory-html.metapack
./explorer/self/v1/schemas/api/schemas/search/%/directory.metapack
./explorer/self/v1/schemas/api/schemas/search/response
./explorer/self/v1/schemas/api/schemas/search/response/%
./explorer/self/v1/schemas/api/schemas/search/response/%/schema-html.metapack
./explorer/self/v1/schemas/api/schemas/search/response/%/schema.metapack
./explorer/self/v1/schemas/api/schemas/search/rpc
./explorer/self/v1/schemas/api/schemas/search/rpc/%
./explorer/self/v1/schemas/api/schemas/search/rpc/%/schema-html.metapack
./explorer/self/v1/schemas/api/schemas/search/rpc/%/schema.metapack
./explorer/self/v1/schemas/api/schemas/stats
./explorer/self/v1/schemas/api/schemas/stats/%
./explorer/self/v1/schemas/api/schemas/stats/%/directory-html.metapack
./explorer/self/v1/schemas/api/schemas/stats/%/directory.metapack
./explorer/self/v1/schemas/api/schemas/stats/response
./explorer/self/v1/schemas/api/schemas/stats/response/%
./explorer/self/v1/schemas/api/schemas/stats/response/%/schema-html.metapack
./explorer/self/v1/schemas/api/schemas/stats/response/%/schema.metapack
./explorer/self/v1/schemas/api/schemas/stats/rpc
./explorer/self/v1/schemas/api/schemas/stats/rpc/%
./explorer/self/v1/schemas/api/schemas/stats/rpc/%/schema-html.metapack
./explorer/self/v1/schemas/api/schemas/stats/rpc/%/schema.metapack
./explorer/self/v1/schemas/api/schemas/trace
./explorer/self/v1/schemas/api/schemas/trace/%
./explorer/self/v1/schemas/api/schemas/trace/%/directory-html.metapack
./explorer/self/v1/schemas/api/schemas/trace/%/directory.metapack
./explorer/self/v1/schemas/api/schemas/trace/request
./explorer/self/v1/schemas/api/schemas/trace/request/%
./explorer/self/v1/schemas/api/schemas/trace/request/%/schema-html.metapack
./explorer/self/v1/schemas/api/schemas/trace/request/%/schema.metapack
./explorer/self/v1/schemas/api/schemas/trace/response
./explorer/self/v1/schemas/api/schemas/trace/response/%
./explorer/self/v1/schemas/api/schemas/trace/response/%/schema-html.metapack
./explorer/self/v1/schemas/api/schemas/trace/response/%/schema.metapack
./explorer/self/v1/schemas/api/schemas/trace/rpc
./explorer/self/v1/schemas/api/schemas/trace/rpc/%
./explorer/self/v1/schemas/api/schemas/trace/rpc/%/schema-html.metapack
./explorer/self/v1/schemas/api/schemas/trace/rpc/%/schema.metapack
./explorer/self/v1/schemas/mcp
./explorer/self/v1/schemas/mcp/%
./explorer/self/v1/schemas/mcp/%/directory-html.metapack
./explorer/self/v1/schemas/mcp/%/directory.metapack
./explorer/self/v1/schemas/mcp/error
./explorer/self/v1/schemas/mcp/error/%
./explorer/self/v1/schemas/mcp/error/%/schema-html.metapack
./explorer/self/v1/schemas/mcp/error/%/schema.metapack
./explorer/self/v1/schemas/mcp/initialize
./explorer/self/v1/schemas/mcp/initialize/%
./explorer/self/v1/schemas/mcp/initialize/%/directory-html.metapack
./explorer/self/v1/schemas/mcp/initialize/%/directory.metapack
./explorer/self/v1/schemas/mcp/initialize/request
./explorer/self/v1/schemas/mcp/initialize/request/%
./explorer/self/v1/schemas/mcp/initialize/request/%/schema-html.metapack
./explorer/self/v1/schemas/mcp/initialize/request/%/schema.metapack
./explorer/self/v1/schemas/mcp/initialize/response
./explorer/self/v1/schemas/mcp/initialize/response/%
./explorer/self/v1/schemas/mcp/initialize/response/%/schema-html.metapack
./explorer/self/v1/schemas/mcp/initialize/response/%/schema.metapack
./explorer/self/v1/schemas/mcp/notifications
./explorer/self/v1/schemas/mcp/notifications/%
./explorer/self/v1/schemas/mcp/notifications/%/directory-html.metapack
./explorer/self/v1/schemas/mcp/notifications/%/directory.metapack
./explorer/self/v1/schemas/mcp/notifications/cancelled
./explorer/self/v1/schemas/mcp/notifications/cancelled/%
./explorer/self/v1/schemas/mcp/notifications/cancelled/%/schema-html.metapack
./explorer/self/v1/schemas/mcp/notifications/cancelled/%/schema.metapack
./explorer/self/v1/schemas/mcp/notifications/initialized
./explorer/self/v1/schemas/mcp/notifications/initialized/%
./explorer/self/v1/schemas/mcp/notifications/initialized/%/schema-html.metapack
./explorer/self/v1/schemas/mcp/notifications/initialized/%/schema.metapack
./explorer/self/v1/schemas/mcp/ping
./explorer/self/v1/schemas/mcp/ping/%
./explorer/self/v1/schemas/mcp/ping/%/directory-html.metapack
./explorer/self/v1/schemas/mcp/ping/%/directory.metapack
./explorer/self/v1/schemas/mcp/ping/request
./explorer/self/v1/schemas/mcp/ping/request/%
./explorer/self/v1/schemas/mcp/ping/request/%/schema-html.metapack
./explorer/self/v1/schemas/mcp/ping/request/%/schema.metapack
./explorer/self/v1/schemas/mcp/ping/response
./explorer/self/v1/schemas/mcp/ping/response/%
./explorer/self/v1/schemas/mcp/ping/response/%/schema-html.metapack
./explorer/self/v1/schemas/mcp/ping/response/%/schema.metapack
./explorer/self/v1/schemas/mcp/request
./explorer/self/v1/schemas/mcp/request/%
./explorer/self/v1/schemas/mcp/request/%/schema-html.metapack
./explorer/self/v1/schemas/mcp/request/%/schema.metapack
./explorer/self/v1/schemas/mcp/resources
./explorer/self/v1/schemas/mcp/resources/%
./explorer/self/v1/schemas/mcp/resources/%/directory-html.metapack
./explorer/self/v1/schemas/mcp/resources/%/directory.metapack
./explorer/self/v1/schemas/mcp/resources/list
./explorer/self/v1/schemas/mcp/resources/list/%
./explorer/self/v1/schemas/mcp/resources/list/%/directory-html.metapack
./explorer/self/v1/schemas/mcp/resources/list/%/directory.metapack
./explorer/self/v1/schemas/mcp/resources/list/request
./explorer/self/v1/schemas/mcp/resources/list/request/%
./explorer/self/v1/schemas/mcp/resources/list/request/%/schema-html.metapack
./explorer/self/v1/schemas/mcp/resources/list/request/%/schema.metapack
./explorer/self/v1/schemas/mcp/resources/list/response
./explorer/self/v1/schemas/mcp/resources/list/response/%
./explorer/self/v1/schemas/mcp/resources/list/response/%/schema-html.metapack
./explorer/self/v1/schemas/mcp/resources/list/response/%/schema.metapack
./explorer/self/v1/schemas/mcp/resources/read
./explorer/self/v1/schemas/mcp/resources/read/%
./explorer/self/v1/schemas/mcp/resources/read/%/directory-html.metapack
./explorer/self/v1/schemas/mcp/resources/read/%/directory.metapack
./explorer/self/v1/schemas/mcp/resources/read/request
./explorer/self/v1/schemas/mcp/resources/read/request/%
./explorer/self/v1/schemas/mcp/resources/read/request/%/schema-html.metapack
./explorer/self/v1/schemas/mcp/resources/read/request/%/schema.metapack
./explorer/self/v1/schemas/mcp/resources/read/response
./explorer/self/v1/schemas/mcp/resources/read/response/%
./explorer/self/v1/schemas/mcp/resources/read/response/%/schema-html.metapack
./explorer/self/v1/schemas/mcp/resources/read/response/%/schema.metapack
./explorer/self/v1/schemas/mcp/resources/templates
./explorer/self/v1/schemas/mcp/resources/templates/%
./explorer/self/v1/schemas/mcp/resources/templates/%/directory-html.metapack
./explorer/self/v1/schemas/mcp/resources/templates/%/directory.metapack
./explorer/self/v1/schemas/mcp/resources/templates/list
./explorer/self/v1/schemas/mcp/resources/templates/list/%
./explorer/self/v1/schemas/mcp/resources/templates/list/%/directory-html.metapack
./explorer/self/v1/schemas/mcp/resources/templates/list/%/directory.metapack
./explorer/self/v1/schemas/mcp/resources/templates/list/request
./explorer/self/v1/schemas/mcp/resources/templates/list/request/%
./explorer/self/v1/schemas/mcp/resources/templates/list/request/%/schema-html.metapack
./explorer/self/v1/schemas/mcp/resources/templates/list/request/%/schema.metapack
./explorer/self/v1/schemas/mcp/resources/templates/list/response
./explorer/self/v1/schemas/mcp/resources/templates/list/response/%
./explorer/self/v1/schemas/mcp/resources/templates/list/response/%/schema-html.metapack
./explorer/self/v1/schemas/mcp/resources/templates/list/response/%/schema.metapack
./explorer/self/v1/schemas/mcp/response
./explorer/self/v1/schemas/mcp/response/%
./explorer/self/v1/schemas/mcp/response/%/schema-html.metapack
./explorer/self/v1/schemas/mcp/response/%/schema.metapack
./routes.bin
./schemas
./schemas/self
./schemas/self/v1
./schemas/self/v1/schemas
./schemas/self/v1/schemas/api
./schemas/self/v1/schemas/api/error
./schemas/self/v1/schemas/api/error/%
./schemas/self/v1/schemas/api/error/%/blaze-exhaustive.metapack
./schemas/self/v1/schemas/api/error/%/blaze-fast.metapack
./schemas/self/v1/schemas/api/error/%/bundle.metapack
./schemas/self/v1/schemas/api/error/%/dependencies.metapack
./schemas/self/v1/schemas/api/error/%/dependents.metapack
./schemas/self/v1/schemas/api/error/%/editor.metapack
./schemas/self/v1/schemas/api/error/%/health.metapack
./schemas/self/v1/schemas/api/error/%/locations.metapack
./schemas/self/v1/schemas/api/error/%/positions.metapack
./schemas/self/v1/schemas/api/error/%/schema.metapack
./schemas/self/v1/schemas/api/error/%/stats.metapack
./schemas/self/v1/schemas/api/list
./schemas/self/v1/schemas/api/list/response
./schemas/self/v1/schemas/api/list/response/%
./schemas/self/v1/schemas/api/list/response/%/blaze-exhaustive.metapack
./schemas/self/v1/schemas/api/list/response/%/blaze-fast.metapack
./schemas/self/v1/schemas/api/list/response/%/bundle.metapack
./schemas/self/v1/schemas/api/list/response/%/dependencies.metapack
./schemas/self/v1/schemas/api/list/response/%/dependents.metapack
./schemas/self/v1/schemas/api/list/response/%/editor.metapack
./schemas/self/v1/schemas/api/list/response/%/health.metapack
./schemas/self/v1/schemas/api/list/response/%/locations.metapack
./schemas/self/v1/schemas/api/list/response/%/positions.metapack
./schemas/self/v1/schemas/api/list/response/%/schema.metapack
./schemas/self/v1/schemas/api/list/response/%/stats.metapack
./schemas/self/v1/schemas/api/list/rpc
./schemas/self/v1/schemas/api/list/rpc/%
./schemas/self/v1/schemas/api/list/rpc/%/blaze-exhaustive.metapack
./schemas/self/v1/schemas/api/list/rpc/%/blaze-fast.metapack
./schemas/self/v1/schemas/api/list/rpc/%/bundle.metapack
./schemas/self/v1/schemas/api/list/rpc/%/dependencies.metapack
./schemas/self/v1/schemas/api/list/rpc/%/dependents.metapack
./schemas/self/v1/schemas/api/list/rpc/%/editor.metapack
./schemas/self/v1/schemas/api/list/rpc/%/health.metapack
./schemas/self/v1/schemas/api/list/rpc/%/locations.metapack
./schemas/self/v1/schemas/api/list/rpc/%/positions.metapack
./schemas/self/v1/schemas/api/list/rpc/%/schema.metapack
./schemas/self/v1/schemas/api/list/rpc/%/stats.metapack
./schemas/self/v1/schemas/api/schemas
./schemas/self/v1/schemas/api/schemas/dependencies
./schemas/self/v1/schemas/api/schemas/dependencies/response
./schemas/self/v1/schemas/api/schemas/dependencies/response/%
./schemas/self/v1/schemas/api/schemas/dependencies/response/%/blaze-exhaustive.metapack
./schemas/self/v1/schemas/api/schemas/dependencies/response/%/blaze-fast.metapack
./schemas/self/v1/schemas/api/schemas/dependencies/response/%/bundle.metapack
./schemas/self/v1/schemas/api/schemas/dependencies/response/%/dependencies.metapack
./schemas/self/v1/schemas/api/schemas/dependencies/response/%/dependents.metapack
./schemas/self/v1/schemas/api/schemas/dependencies/response/%/editor.metapack
./schemas/self/v1/schemas/api/schemas/dependencies/response/%/health.metapack
./schemas/self/v1/schemas/api/schemas/dependencies/response/%/locations.metapack
./schemas/self/v1/schemas/api/schemas/dependencies/response/%/positions.metapack
./schemas/self/v1/schemas/api/schemas/dependencies/response/%/schema.metapack
./schemas/self/v1/schemas/api/schemas/dependencies/response/%/stats.metapack
./schemas/self/v1/schemas/api/schemas/dependencies/rpc
./schemas/self/v1/schemas/api/schemas/dependencies/rpc/%
./schemas/self/v1/schemas/api/schemas/dependencies/rpc/%/blaze-exhaustive.metapack
./schemas/self/v1/schemas/api/schemas/dependencies/rpc/%/blaze-fast.metapack
./schemas/self/v1/schemas/api/schemas/dependencies/rpc/%/bundle.metapack
./schemas/self/v1/schemas/api/schemas/dependencies/rpc/%/dependencies.metapack
./schemas/self/v1/schemas/api/schemas/dependencies/rpc/%/dependents.metapack
./schemas/self/v1/schemas/api/schemas/dependencies/rpc/%/editor.metapack
./schemas/self/v1/schemas/api/schemas/dependencies/rpc/%/health.metapack
./schemas/self/v1/schemas/api/schemas/dependencies/rpc/%/locations.metapack
./schemas/self/v1/schemas/api/schemas/dependencies/rpc/%/positions.metapack
./schemas/self/v1/schemas/api/schemas/dependencies/rpc/%/schema.metapack
./schemas/self/v1/schemas/api/schemas/dependencies/rpc/%/stats.metapack
./schemas/self/v1/schemas/api/schemas/dependents
./schemas/self/v1/schemas/api/schemas/dependents/response
./schemas/self/v1/schemas/api/schemas/dependents/response/%
./schemas/self/v1/schemas/api/schemas/dependents/response/%/blaze-exhaustive.metapack
./schemas/self/v1/schemas/api/schemas/dependents/response/%/blaze-fast.metapack
./schemas/self/v1/schemas/api/schemas/dependents/response/%/bundle.metapack
./schemas/self/v1/schemas/api/schemas/dependents/response/%/dependencies.metapack
./schemas/self/v1/schemas/api/schemas/dependents/response/%/dependents.metapack
./schemas/self/v1/schemas/api/schemas/dependents/response/%/editor.metapack
./schemas/self/v1/schemas/api/schemas/dependents/response/%/health.metapack
./schemas/self/v1/schemas/api/schemas/dependents/response/%/locations.metapack
./schemas/self/v1/schemas/api/schemas/dependents/response/%/positions.metapack
./schemas/self/v1/schemas/api/schemas/dependents/response/%/schema.metapack
./schemas/self/v1/schemas/api/schemas/dependents/response/%/stats.metapack
./schemas/self/v1/schemas/api/schemas/dependents/rpc
./schemas/self/v1/schemas/api/schemas/dependents/rpc/%
./schemas/self/v1/schemas/api/schemas/dependents/rpc/%/blaze-exhaustive.metapack
./schemas/self/v1/schemas/api/schemas/dependents/rpc/%/blaze-fast.metapack
./schemas/self/v1/schemas/api/schemas/dependents/rpc/%/bundle.metapack
./schemas/self/v1/schemas/api/schemas/dependents/rpc/%/dependencies.metapack
./schemas/self/v1/schemas/api/schemas/dependents/rpc/%/dependents.metapack
./schemas/self/v1/schemas/api/schemas/dependents/rpc/%/editor.metapack
./schemas/self/v1/schemas/api/schemas/dependents/rpc/%/health.metapack
./schemas/self/v1/schemas/api/schemas/dependents/rpc/%/locations.metapack
./schemas/self/v1/schemas/api/schemas/dependents/rpc/%/positions.metapack
./schemas/self/v1/schemas/api/schemas/dependents/rpc/%/schema.metapack
./schemas/self/v1/schemas/api/schemas/dependents/rpc/%/stats.metapack
./schemas/self/v1/schemas/api/schemas/evaluate
./schemas/self/v1/schemas/api/schemas/evaluate/request
./schemas/self/v1/schemas/api/schemas/evaluate/request/%
./schemas/self/v1/schemas/api/schemas/evaluate/request/%/blaze-exhaustive.metapack
./schemas/self/v1/schemas/api/schemas/evaluate/request/%/blaze-fast.metapack
./schemas/self/v1/schemas/api/schemas/evaluate/request/%/bundle.metapack
./schemas/self/v1/schemas/api/schemas/evaluate/request/%/dependencies.metapack
./schemas/self/v1/schemas/api/schemas/evaluate/request/%/dependents.metapack
./schemas/self/v1/schemas/api/schemas/evaluate/request/%/editor.metapack
./schemas/self/v1/schemas/api/schemas/evaluate/request/%/health.metapack
./schemas/self/v1/schemas/api/schemas/evaluate/request/%/locations.metapack
./schemas/self/v1/schemas/api/schemas/evaluate/request/%/positions.metapack
./schemas/self/v1/schemas/api/schemas/evaluate/request/%/schema.metapack
./schemas/self/v1/schemas/api/schemas/evaluate/request/%/stats.metapack
./schemas/self/v1/schemas/api/schemas/evaluate/response
./schemas/self/v1/schemas/api/schemas/evaluate/response/%
./schemas/self/v1/schemas/api/schemas/evaluate/response/%/blaze-exhaustive.metapack
./schemas/self/v1/schemas/api/schemas/evaluate/response/%/blaze-fast.metapack
./schemas/self/v1/schemas/api/schemas/evaluate/response/%/bundle.metapack
./schemas/self/v1/schemas/api/schemas/evaluate/response/%/dependencies.metapack
./schemas/self/v1/schemas/api/schemas/evaluate/response/%/dependents.metapack
./schemas/self/v1/schemas/api/schemas/evaluate/response/%/editor.metapack
./schemas/self/v1/schemas/api/schemas/evaluate/response/%/health.metapack
./schemas/self/v1/schemas/api/schemas/evaluate/response/%/locations.metapack
./schemas/self/v1/schemas/api/schemas/evaluate/response/%/positions.metapack
./schemas/self/v1/schemas/api/schemas/evaluate/response/%/schema.metapack
./schemas/self/v1/schemas/api/schemas/evaluate/response/%/stats.metapack
./schemas/self/v1/schemas/api/schemas/evaluate/rpc
./schemas/self/v1/schemas/api/schemas/evaluate/rpc/%
./schemas/self/v1/schemas/api/schemas/evaluate/rpc/%/blaze-exhaustive.metapack
./schemas/self/v1/schemas/api/schemas/evaluate/rpc/%/blaze-fast.metapack
./schemas/self/v1/schemas/api/schemas/evaluate/rpc/%/bundle.metapack
./schemas/self/v1/schemas/api/schemas/evaluate/rpc/%/dependencies.metapack
./schemas/self/v1/schemas/api/schemas/evaluate/rpc/%/dependents.metapack
./schemas/self/v1/schemas/api/schemas/evaluate/rpc/%/editor.metapack
./schemas/self/v1/schemas/api/schemas/evaluate/rpc/%/health.metapack
./schemas/self/v1/schemas/api/schemas/evaluate/rpc/%/locations.metapack
./schemas/self/v1/schemas/api/schemas/evaluate/rpc/%/positions.metapack
./schemas/self/v1/schemas/api/schemas/evaluate/rpc/%/schema.metapack
./schemas/self/v1/schemas/api/schemas/evaluate/rpc/%/stats.metapack
./schemas/self/v1/schemas/api/schemas/health
./schemas/self/v1/schemas/api/schemas/health/response
./schemas/self/v1/schemas/api/schemas/health/response/%
./schemas/self/v1/schemas/api/schemas/health/response/%/blaze-exhaustive.metapack
./schemas/self/v1/schemas/api/schemas/health/response/%/blaze-fast.metapack
./schemas/self/v1/schemas/api/schemas/health/response/%/bundle.metapack
./schemas/self/v1/schemas/api/schemas/health/response/%/dependencies.metapack
./schemas/self/v1/schemas/api/schemas/health/response/%/dependents.metapack
./schemas/self/v1/schemas/api/schemas/health/response/%/editor.metapack
./schemas/self/v1/schemas/api/schemas/health/response/%/health.metapack
./schemas/self/v1/schemas/api/schemas/health/response/%/locations.metapack
./schemas/self/v1/schemas/api/schemas/health/response/%/positions.metapack
./schemas/self/v1/schemas/api/schemas/health/response/%/schema.metapack
./schemas/self/v1/schemas/api/schemas/health/response/%/stats.metapack
./schemas/self/v1/schemas/api/schemas/health/rpc
./schemas/self/v1/schemas/api/schemas/health/rpc/%
./schemas/self/v1/schemas/api/schemas/health/rpc/%/blaze-exhaustive.metapack
./schemas/self/v1/schemas/api/schemas/health/rpc/%/blaze-fast.metapack
./schemas/self/v1/schemas/api/schemas/health/rpc/%/bundle.metapack
./schemas/self/v1/schemas/api/schemas/health/rpc/%/dependencies.metapack
./schemas/self/v1/schemas/api/schemas/health/rpc/%/dependents.metapack
./schemas/self/v1/schemas/api/schemas/health/rpc/%/editor.metapack
./schemas/self/v1/schemas/api/schemas/health/rpc/%/health.metapack
./schemas/self/v1/schemas/api/schemas/health/rpc/%/locations.metapack
./schemas/self/v1/schemas/api/schemas/health/rpc/%/positions.metapack
./schemas/self/v1/schemas/api/schemas/health/rpc/%/schema.metapack
./schemas/self/v1/schemas/api/schemas/health/rpc/%/stats.metapack
./schemas/self/v1/schemas/api/schemas/locations
./schemas/self/v1/schemas/api/schemas/locations/response
./schemas/self/v1/schemas/api/schemas/locations/response/%
./schemas/self/v1/schemas/api/schemas/locations/response/%/blaze-exhaustive.metapack
./schemas/self/v1/schemas/api/schemas/locations/response/%/blaze-fast.metapack
./schemas/self/v1/schemas/api/schemas/locations/response/%/bundle.metapack
./schemas/self/v1/schemas/api/schemas/locations/response/%/dependencies.metapack
./schemas/self/v1/schemas/api/schemas/locations/response/%/dependents.metapack
./schemas/self/v1/schemas/api/schemas/locations/response/%/editor.metapack
./schemas/self/v1/schemas/api/schemas/locations/response/%/health.metapack
./schemas/self/v1/schemas/api/schemas/locations/response/%/locations.metapack
./schemas/self/v1/schemas/api/schemas/locations/response/%/positions.metapack
./schemas/self/v1/schemas/api/schemas/locations/response/%/schema.metapack
./schemas/self/v1/schemas/api/schemas/locations/response/%/stats.metapack
./schemas/self/v1/schemas/api/schemas/locations/rpc
./schemas/self/v1/schemas/api/schemas/locations/rpc/%
./schemas/self/v1/schemas/api/schemas/locations/rpc/%/blaze-exhaustive.metapack
./schemas/self/v1/schemas/api/schemas/locations/rpc/%/blaze-fast.metapack
./schemas/self/v1/schemas/api/schemas/locations/rpc/%/bundle.metapack
./schemas/self/v1/schemas/api/schemas/locations/rpc/%/dependencies.metapack
./schemas/self/v1/schemas/api/schemas/locations/rpc/%/dependents.metapack
./schemas/self/v1/schemas/api/schemas/locations/rpc/%/editor.metapack
./schemas/self/v1/schemas/api/schemas/locations/rpc/%/health.metapack
./schemas/self/v1/schemas/api/schemas/locations/rpc/%/locations.metapack
./schemas/self/v1/schemas/api/schemas/locations/rpc/%/positions.metapack
./schemas/self/v1/schemas/api/schemas/locations/rpc/%/schema.metapack
./schemas/self/v1/schemas/api/schemas/locations/rpc/%/stats.metapack
./schemas/self/v1/schemas/api/schemas/metadata
./schemas/self/v1/schemas/api/schemas/metadata/response
./schemas/self/v1/schemas/api/schemas/metadata/response/%
./schemas/self/v1/schemas/api/schemas/metadata/response/%/blaze-exhaustive.metapack
./schemas/self/v1/schemas/api/schemas/metadata/response/%/blaze-fast.metapack
./schemas/self/v1/schemas/api/schemas/metadata/response/%/bundle.metapack
./schemas/self/v1/schemas/api/schemas/metadata/response/%/dependencies.metapack
./schemas/self/v1/schemas/api/schemas/metadata/response/%/dependents.metapack
./schemas/self/v1/schemas/api/schemas/metadata/response/%/editor.metapack
./schemas/self/v1/schemas/api/schemas/metadata/response/%/health.metapack
./schemas/self/v1/schemas/api/schemas/metadata/response/%/locations.metapack
./schemas/self/v1/schemas/api/schemas/metadata/response/%/positions.metapack
./schemas/self/v1/schemas/api/schemas/metadata/response/%/schema.metapack
./schemas/self/v1/schemas/api/schemas/metadata/response/%/stats.metapack
./schemas/self/v1/schemas/api/schemas/metadata/rpc
./schemas/self/v1/schemas/api/schemas/metadata/rpc/%
./schemas/self/v1/schemas/api/schemas/metadata/rpc/%/blaze-exhaustive.metapack
./schemas/self/v1/schemas/api/schemas/metadata/rpc/%/blaze-fast.metapack
./schemas/self/v1/schemas/api/schemas/metadata/rpc/%/bundle.metapack
./schemas/self/v1/schemas/api/schemas/metadata/rpc/%/dependencies.metapack
./schemas/self/v1/schemas/api/schemas/metadata/rpc/%/dependents.metapack
./schemas/self/v1/schemas/api/schemas/metadata/rpc/%/editor.metapack
./schemas/self/v1/schemas/api/schemas/metadata/rpc/%/health.metapack
./schemas/self/v1/schemas/api/schemas/metadata/rpc/%/locations.metapack
./schemas/self/v1/schemas/api/schemas/metadata/rpc/%/positions.metapack
./schemas/self/v1/schemas/api/schemas/metadata/rpc/%/schema.metapack
./schemas/self/v1/schemas/api/schemas/metadata/rpc/%/stats.metapack
./schemas/self/v1/schemas/api/schemas/position
./schemas/self/v1/schemas/api/schemas/position/%
./schemas/self/v1/schemas/api/schemas/position/%/blaze-exhaustive.metapack
./schemas/self/v1/schemas/api/schemas/position/%/blaze-fast.metapack
./schemas/self/v1/schemas/api/schemas/position/%/bundle.metapack
./schemas/self/v1/schemas/api/schemas/position/%/dependencies.metapack
./schemas/self/v1/schemas/api/schemas/position/%/dependents.metapack
./schemas/self/v1/schemas/api/schemas/position/%/editor.metapack
./schemas/self/v1/schemas/api/schemas/position/%/health.metapack
./schemas/self/v1/schemas/api/schemas/position/%/locations.metapack
./schemas/self/v1/schemas/api/schemas/position/%/positions.metapack
./schemas/self/v1/schemas/api/schemas/position/%/schema.metapack
./schemas/self/v1/schemas/api/schemas/position/%/stats.metapack
./schemas/self/v1/schemas/api/schemas/positions
./schemas/self/v1/schemas/api/schemas/positions/response
./schemas/self/v1/schemas/api/schemas/positions/response/%
./schemas/self/v1/schemas/api/schemas/positions/response/%/blaze-exhaustive.metapack
./schemas/self/v1/schemas/api/schemas/positions/response/%/blaze-fast.metapack
./schemas/self/v1/schemas/api/schemas/positions/response/%/bundle.metapack
./schemas/self/v1/schemas/api/schemas/positions/response/%/dependencies.metapack
./schemas/self/v1/schemas/api/schemas/positions/response/%/dependents.metapack
./schemas/self/v1/schemas/api/schemas/positions/response/%/editor.metapack
./schemas/self/v1/schemas/api/schemas/positions/response/%/health.metapack
./schemas/self/v1/schemas/api/schemas/positions/response/%/locations.metapack
./schemas/self/v1/schemas/api/schemas/positions/response/%/positions.metapack
./schemas/self/v1/schemas/api/schemas/positions/response/%/schema.metapack
./schemas/self/v1/schemas/api/schemas/positions/response/%/stats.metapack
./schemas/self/v1/schemas/api/schemas/positions/rpc
./schemas/self/v1/schemas/api/schemas/positions/rpc/%
./schemas/self/v1/schemas/api/schemas/positions/rpc/%/blaze-exhaustive.metapack
./schemas/self/v1/schemas/api/schemas/positions/rpc/%/blaze-fast.metapack
./schemas/self/v1/schemas/api/schemas/positions/rpc/%/bundle.metapack
./schemas/self/v1/schemas/api/schemas/positions/rpc/%/dependencies.metapack
./schemas/self/v1/schemas/api/schemas/positions/rpc/%/dependents.metapack
./schemas/self/v1/schemas/api/schemas/positions/rpc/%/editor.metapack
./schemas/self/v1/schemas/api/schemas/positions/rpc/%/health.metapack
./schemas/self/v1/schemas/api/schemas/positions/rpc/%/locations.metapack
./schemas/self/v1/schemas/api/schemas/positions/rpc/%/positions.metapack
./schemas/self/v1/schemas/api/schemas/positions/rpc/%/schema.metapack
./schemas/self/v1/schemas/api/schemas/positions/rpc/%/stats.metapack
./schemas/self/v1/schemas/api/schemas/search
./schemas/self/v1/schemas/api/schemas/search/response
./schemas/self/v1/schemas/api/schemas/search/response/%
./schemas/self/v1/schemas/api/schemas/search/response/%/blaze-exhaustive.metapack
./schemas/self/v1/schemas/api/schemas/search/response/%/blaze-fast.metapack
./schemas/self/v1/schemas/api/schemas/search/response/%/bundle.metapack
./schemas/self/v1/schemas/api/schemas/search/response/%/dependencies.metapack
./schemas/self/v1/schemas/api/schemas/search/response/%/dependents.metapack
./schemas/self/v1/schemas/api/schemas/search/response/%/editor.metapack
./schemas/self/v1/schemas/api/schemas/search/response/%/health.metapack
./schemas/self/v1/schemas/api/schemas/search/response/%/locations.metapack
./schemas/self/v1/schemas/api/schemas/search/response/%/positions.metapack
./schemas/self/v1/schemas/api/schemas/search/response/%/schema.metapack
./schemas/self/v1/schemas/api/schemas/search/response/%/stats.metapack
./schemas/self/v1/schemas/api/schemas/search/rpc
./schemas/self/v1/schemas/api/schemas/search/rpc/%
./schemas/self/v1/schemas/api/schemas/search/rpc/%/blaze-exhaustive.metapack
./schemas/self/v1/schemas/api/schemas/search/rpc/%/blaze-fast.metapack
./schemas/self/v1/schemas/api/schemas/search/rpc/%/bundle.metapack
./schemas/self/v1/schemas/api/schemas/search/rpc/%/dependencies.metapack
./schemas/self/v1/schemas/api/schemas/search/rpc/%/dependents.metapack
./schemas/self/v1/schemas/api/schemas/search/rpc/%/editor.metapack
./schemas/self/v1/schemas/api/schemas/search/rpc/%/health.metapack
./schemas/self/v1/schemas/api/schemas/search/rpc/%/locations.metapack
./schemas/self/v1/schemas/api/schemas/search/rpc/%/positions.metapack
./schemas/self/v1/schemas/api/schemas/search/rpc/%/schema.metapack
./schemas/self/v1/schemas/api/schemas/search/rpc/%/stats.metapack
./schemas/self/v1/schemas/api/schemas/stats
./schemas/self/v1/schemas/api/schemas/stats/response
./schemas/self/v1/schemas/api/schemas/stats/response/%
./schemas/self/v1/schemas/api/schemas/stats/response/%/blaze-exhaustive.metapack
./schemas/self/v1/schemas/api/schemas/stats/response/%/blaze-fast.metapack
./schemas/self/v1/schemas/api/schemas/stats/response/%/bundle.metapack
./schemas/self/v1/schemas/api/schemas/stats/response/%/dependencies.metapack
./schemas/self/v1/schemas/api/schemas/stats/response/%/dependents.metapack
./schemas/self/v1/schemas/api/schemas/stats/response/%/editor.metapack
./schemas/self/v1/schemas/api/schemas/stats/response/%/health.metapack
./schemas/self/v1/schemas/api/schemas/stats/response/%/locations.metapack
./schemas/self/v1/schemas/api/schemas/stats/response/%/positions.metapack
./schemas/self/v1/schemas/api/schemas/stats/response/%/schema.metapack
./schemas/self/v1/schemas/api/schemas/stats/response/%/stats.metapack
./schemas/self/v1/schemas/api/schemas/stats/rpc
./schemas/self/v1/schemas/api/schemas/stats/rpc/%
./schemas/self/v1/schemas/api/schemas/stats/rpc/%/blaze-exhaustive.metapack
./schemas/self/v1/schemas/api/schemas/stats/rpc/%/blaze-fast.metapack
./schemas/self/v1/schemas/api/schemas/stats/rpc/%/bundle.metapack
./schemas/self/v1/schemas/api/schemas/stats/rpc/%/dependencies.metapack
./schemas/self/v1/schemas/api/schemas/stats/rpc/%/dependents.metapack
./schemas/self/v1/schemas/api/schemas/stats/rpc/%/editor.metapack
./schemas/self/v1/schemas/api/schemas/stats/rpc/%/health.metapack
./schemas/self/v1/schemas/api/schemas/stats/rpc/%/locations.metapack
./schemas/self/v1/schemas/api/schemas/stats/rpc/%/positions.metapack
./schemas/self/v1/schemas/api/schemas/stats/rpc/%/schema.metapack
./schemas/self/v1/schemas/api/schemas/stats/rpc/%/stats.metapack
./schemas/self/v1/schemas/api/schemas/trace
./schemas/self/v1/schemas/api/schemas/trace/request
./schemas/self/v1/schemas/api/schemas/trace/request/%
./schemas/self/v1/schemas/api/schemas/trace/request/%/blaze-exhaustive.metapack
./schemas/self/v1/schemas/api/schemas/trace/request/%/blaze-fast.metapack
./schemas/self/v1/schemas/api/schemas/trace/request/%/bundle.metapack
./schemas/self/v1/schemas/api/schemas/trace/request/%/dependencies.metapack
./schemas/self/v1/schemas/api/schemas/trace/request/%/dependents.metapack
./schemas/self/v1/schemas/api/schemas/trace/request/%/editor.metapack
./schemas/self/v1/schemas/api/schemas/trace/request/%/health.metapack
./schemas/self/v1/schemas/api/schemas/trace/request/%/locations.metapack
./schemas/self/v1/schemas/api/schemas/trace/request/%/positions.metapack
./schemas/self/v1/schemas/api/schemas/trace/request/%/schema.metapack
./schemas/self/v1/schemas/api/schemas/trace/request/%/stats.metapack
./schemas/self/v1/schemas/api/schemas/trace/response
./schemas/self/v1/schemas/api/schemas/trace/response/%
./schemas/self/v1/schemas/api/schemas/trace/response/%/blaze-exhaustive.metapack
./schemas/self/v1/schemas/api/schemas/trace/response/%/blaze-fast.metapack
./schemas/self/v1/schemas/api/schemas/trace/response/%/bundle.metapack
./schemas/self/v1/schemas/api/schemas/trace/response/%/dependencies.metapack
./schemas/self/v1/schemas/api/schemas/trace/response/%/dependents.metapack
./schemas/self/v1/schemas/api/schemas/trace/response/%/editor.metapack
./schemas/self/v1/schemas/api/schemas/trace/response/%/health.metapack
./schemas/self/v1/schemas/api/schemas/trace/response/%/locations.metapack
./schemas/self/v1/schemas/api/schemas/trace/response/%/positions.metapack
./schemas/self/v1/schemas/api/schemas/trace/response/%/schema.metapack
./schemas/self/v1/schemas/api/schemas/trace/response/%/stats.metapack
./schemas/self/v1/schemas/api/schemas/trace/rpc
./schemas/self/v1/schemas/api/schemas/trace/rpc/%
./schemas/self/v1/schemas/api/schemas/trace/rpc/%/blaze-exhaustive.metapack
./schemas/self/v1/schemas/api/schemas/trace/rpc/%/blaze-fast.metapack
./schemas/self/v1/schemas/api/schemas/trace/rpc/%/bundle.metapack
./schemas/self/v1/schemas/api/schemas/trace/rpc/%/dependencies.metapack
./schemas/self/v1/schemas/api/schemas/trace/rpc/%/dependents.metapack
./schemas/self/v1/schemas/api/schemas/trace/rpc/%/editor.metapack
./schemas/self/v1/schemas/api/schemas/trace/rpc/%/health.metapack
./schemas/self/v1/schemas/api/schemas/trace/rpc/%/locations.metapack
./schemas/self/v1/schemas/api/schemas/trace/rpc/%/positions.metapack
./schemas/self/v1/schemas/api/schemas/trace/rpc/%/schema.metapack
./schemas/self/v1/schemas/api/schemas/trace/rpc/%/stats.metapack
./schemas/self/v1/schemas/mcp
./schemas/self/v1/schemas/mcp/error
./schemas/self/v1/schemas/mcp/error/%
./schemas/self/v1/schemas/mcp/error/%/blaze-exhaustive.metapack
./schemas/self/v1/schemas/mcp/error/%/blaze-fast.metapack
./schemas/self/v1/schemas/mcp/error/%/bundle.metapack
./schemas/self/v1/schemas/mcp/error/%/dependencies.metapack
./schemas/self/v1/schemas/mcp/error/%/dependents.metapack
./schemas/self/v1/schemas/mcp/error/%/editor.metapack
./schemas/self/v1/schemas/mcp/error/%/health.metapack
./schemas/self/v1/schemas/mcp/error/%/locations.metapack
./schemas/self/v1/schemas/mcp/error/%/positions.metapack
./schemas/self/v1/schemas/mcp/error/%/schema.metapack
./schemas/self/v1/schemas/mcp/error/%/stats.metapack
./schemas/self/v1/schemas/mcp/initialize
./schemas/self/v1/schemas/mcp/initialize/request
./schemas/self/v1/schemas/mcp/initialize/request/%
./schemas/self/v1/schemas/mcp/initialize/request/%/blaze-exhaustive.metapack
./schemas/self/v1/schemas/mcp/initialize/request/%/blaze-fast.metapack
./schemas/self/v1/schemas/mcp/initialize/request/%/bundle.metapack
./schemas/self/v1/schemas/mcp/initialize/request/%/dependencies.metapack
./schemas/self/v1/schemas/mcp/initialize/request/%/dependents.metapack
./schemas/self/v1/schemas/mcp/initialize/request/%/editor.metapack
./schemas/self/v1/schemas/mcp/initialize/request/%/health.metapack
./schemas/self/v1/schemas/mcp/initialize/request/%/locations.metapack
./schemas/self/v1/schemas/mcp/initialize/request/%/positions.metapack
./schemas/self/v1/schemas/mcp/initialize/request/%/schema.metapack
./schemas/self/v1/schemas/mcp/initialize/request/%/stats.metapack
./schemas/self/v1/schemas/mcp/initialize/response
./schemas/self/v1/schemas/mcp/initialize/response/%
./schemas/self/v1/schemas/mcp/initialize/response/%/blaze-exhaustive.metapack
./schemas/self/v1/schemas/mcp/initialize/response/%/blaze-fast.metapack
./schemas/self/v1/schemas/mcp/initialize/response/%/bundle.metapack
./schemas/self/v1/schemas/mcp/initialize/response/%/dependencies.metapack
./schemas/self/v1/schemas/mcp/initialize/response/%/dependents.metapack
./schemas/self/v1/schemas/mcp/initialize/response/%/editor.metapack
./schemas/self/v1/schemas/mcp/initialize/response/%/health.metapack
./schemas/self/v1/schemas/mcp/initialize/response/%/locations.metapack
./schemas/self/v1/schemas/mcp/initialize/response/%/positions.metapack
./schemas/self/v1/schemas/mcp/initialize/response/%/schema.metapack
./schemas/self/v1/schemas/mcp/initialize/response/%/stats.metapack
./schemas/self/v1/schemas/mcp/notifications
./schemas/self/v1/schemas/mcp/notifications/cancelled
./schemas/self/v1/schemas/mcp/notifications/cancelled/%
./schemas/self/v1/schemas/mcp/notifications/cancelled/%/blaze-exhaustive.metapack
./schemas/self/v1/schemas/mcp/notifications/cancelled/%/blaze-fast.metapack
./schemas/self/v1/schemas/mcp/notifications/cancelled/%/bundle.metapack
./schemas/self/v1/schemas/mcp/notifications/cancelled/%/dependencies.metapack
./schemas/self/v1/schemas/mcp/notifications/cancelled/%/dependents.metapack
./schemas/self/v1/schemas/mcp/notifications/cancelled/%/editor.metapack
./schemas/self/v1/schemas/mcp/notifications/cancelled/%/health.metapack
./schemas/self/v1/schemas/mcp/notifications/cancelled/%/locations.metapack
./schemas/self/v1/schemas/mcp/notifications/cancelled/%/positions.metapack
./schemas/self/v1/schemas/mcp/notifications/cancelled/%/schema.metapack
./schemas/self/v1/schemas/mcp/notifications/cancelled/%/stats.metapack
./schemas/self/v1/schemas/mcp/notifications/initialized
./schemas/self/v1/schemas/mcp/notifications/initialized/%
./schemas/self/v1/schemas/mcp/notifications/initialized/%/blaze-exhaustive.metapack
./schemas/self/v1/schemas/mcp/notifications/initialized/%/blaze-fast.metapack
./schemas/self/v1/schemas/mcp/notifications/initialized/%/bundle.metapack
./schemas/self/v1/schemas/mcp/notifications/initialized/%/dependencies.metapack
./schemas/self/v1/schemas/mcp/notifications/initialized/%/dependents.metapack
./schemas/self/v1/schemas/mcp/notifications/initialized/%/editor.metapack
./schemas/self/v1/schemas/mcp/notifications/initialized/%/health.metapack
./schemas/self/v1/schemas/mcp/notifications/initialized/%/locations.metapack
./schemas/self/v1/schemas/mcp/notifications/initialized/%/positions.metapack
./schemas/self/v1/schemas/mcp/notifications/initialized/%/schema.metapack
./schemas/self/v1/schemas/mcp/notifications/initialized/%/stats.metapack
./schemas/self/v1/schemas/mcp/ping
./schemas/self/v1/schemas/mcp/ping/request
./schemas/self/v1/schemas/mcp/ping/request/%
./schemas/self/v1/schemas/mcp/ping/request/%/blaze-exhaustive.metapack
./schemas/self/v1/schemas/mcp/ping/request/%/blaze-fast.metapack
./schemas/self/v1/schemas/mcp/ping/request/%/bundle.metapack
./schemas/self/v1/schemas/mcp/ping/request/%/dependencies.metapack
./schemas/self/v1/schemas/mcp/ping/request/%/dependents.metapack
./schemas/self/v1/schemas/mcp/ping/request/%/editor.metapack
./schemas/self/v1/schemas/mcp/ping/request/%/health.metapack
./schemas/self/v1/schemas/mcp/ping/request/%/locations.metapack
./schemas/self/v1/schemas/mcp/ping/request/%/positions.metapack
./schemas/self/v1/schemas/mcp/ping/request/%/schema.metapack
./schemas/self/v1/schemas/mcp/ping/request/%/stats.metapack
./schemas/self/v1/schemas/mcp/ping/response
./schemas/self/v1/schemas/mcp/ping/response/%
./schemas/self/v1/schemas/mcp/ping/response/%/blaze-exhaustive.metapack
./schemas/self/v1/schemas/mcp/ping/response/%/blaze-fast.metapack
./schemas/self/v1/schemas/mcp/ping/response/%/bundle.metapack
./schemas/self/v1/schemas/mcp/ping/response/%/dependencies.metapack
./schemas/self/v1/schemas/mcp/ping/response/%/dependents.metapack
./schemas/self/v1/schemas/mcp/ping/response/%/editor.metapack
./schemas/self/v1/schemas/mcp/ping/response/%/health.metapack
./schemas/self/v1/schemas/mcp/ping/response/%/locations.metapack
./schemas/self/v1/schemas/mcp/ping/response/%/positions.metapack
./schemas/self/v1/schemas/mcp/ping/response/%/schema.metapack
./schemas/self/v1/schemas/mcp/ping/response/%/stats.metapack
./schemas/self/v1/schemas/mcp/request
./schemas/self/v1/schemas/mcp/request/%
./schemas/self/v1/schemas/mcp/request/%/blaze-exhaustive.metapack
./schemas/self/v1/schemas/mcp/request/%/blaze-fast.metapack
./schemas/self/v1/schemas/mcp/request/%/bundle.metapack
./schemas/self/v1/schemas/mcp/request/%/dependencies.metapack
./schemas/self/v1/schemas/mcp/request/%/dependents.metapack
./schemas/self/v1/schemas/mcp/request/%/editor.metapack
./schemas/self/v1/schemas/mcp/request/%/health.metapack
./schemas/self/v1/schemas/mcp/request/%/locations.metapack
./schemas/self/v1/schemas/mcp/request/%/positions.metapack
./schemas/self/v1/schemas/mcp/request/%/schema.metapack
./schemas/self/v1/schemas/mcp/request/%/stats.metapack
./schemas/self/v1/schemas/mcp/resources
./schemas/self/v1/schemas/mcp/resources/list
./schemas/self/v1/schemas/mcp/resources/list/request
./schemas/self/v1/schemas/mcp/resources/list/request/%
./schemas/self/v1/schemas/mcp/resources/list/request/%/blaze-exhaustive.metapack
./schemas/self/v1/schemas/mcp/resources/list/request/%/blaze-fast.metapack
./schemas/self/v1/schemas/mcp/resources/list/request/%/bundle.metapack
./schemas/self/v1/schemas/mcp/resources/list/request/%/dependencies.metapack
./schemas/self/v1/schemas/mcp/resources/list/request/%/dependents.metapack
./schemas/self/v1/schemas/mcp/resources/list/request/%/editor.metapack
./schemas/self/v1/schemas/mcp/resources/list/request/%/health.metapack
./schemas/self/v1/schemas/mcp/resources/list/request/%/locations.metapack
./schemas/self/v1/schemas/mcp/resources/list/request/%/positions.metapack
./schemas/self/v1/schemas/mcp/resources/list/request/%/schema.metapack
./schemas/self/v1/schemas/mcp/resources/list/request/%/stats.metapack
./schemas/self/v1/schemas/mcp/resources/list/response
./schemas/self/v1/schemas/mcp/resources/list/response/%
./schemas/self/v1/schemas/mcp/resources/list/response/%/blaze-exhaustive.metapack
./schemas/self/v1/schemas/mcp/resources/list/response/%/blaze-fast.metapack
./schemas/self/v1/schemas/mcp/resources/list/response/%/bundle.metapack
./schemas/self/v1/schemas/mcp/resources/list/response/%/dependencies.metapack
./schemas/self/v1/schemas/mcp/resources/list/response/%/dependents.metapack
./schemas/self/v1/schemas/mcp/resources/list/response/%/editor.metapack
./schemas/self/v1/schemas/mcp/resources/list/response/%/health.metapack
./schemas/self/v1/schemas/mcp/resources/list/response/%/locations.metapack
./schemas/self/v1/schemas/mcp/resources/list/response/%/positions.metapack
./schemas/self/v1/schemas/mcp/resources/list/response/%/schema.metapack
./schemas/self/v1/schemas/mcp/resources/list/response/%/stats.metapack
./schemas/self/v1/schemas/mcp/resources/read
./schemas/self/v1/schemas/mcp/resources/read/request
./schemas/self/v1/schemas/mcp/resources/read/request/%
./schemas/self/v1/schemas/mcp/resources/read/request/%/blaze-exhaustive.metapack
./schemas/self/v1/schemas/mcp/resources/read/request/%/blaze-fast.metapack
./schemas/self/v1/schemas/mcp/resources/read/request/%/bundle.metapack
./schemas/self/v1/schemas/mcp/resources/read/request/%/dependencies.metapack
./schemas/self/v1/schemas/mcp/resources/read/request/%/dependents.metapack
./schemas/self/v1/schemas/mcp/resources/read/request/%/editor.metapack
./schemas/self/v1/schemas/mcp/resources/read/request/%/health.metapack
./schemas/self/v1/schemas/mcp/resources/read/request/%/locations.metapack
./schemas/self/v1/schemas/mcp/resources/read/request/%/positions.metapack
./schemas/self/v1/schemas/mcp/resources/read/request/%/schema.metapack
./schemas/self/v1/schemas/mcp/resources/read/request/%/stats.metapack
./schemas/self/v1/schemas/mcp/resources/read/response
./schemas/self/v1/schemas/mcp/resources/read/response/%
./schemas/self/v1/schemas/mcp/resources/read/response/%/blaze-exhaustive.metapack
./schemas/self/v1/schemas/mcp/resources/read/response/%/blaze-fast.metapack
./schemas/self/v1/schemas/mcp/resources/read/response/%/bundle.metapack
./schemas/self/v1/schemas/mcp/resources/read/response/%/dependencies.metapack
./schemas/self/v1/schemas/mcp/resources/read/response/%/dependents.metapack
./schemas/self/v1/schemas/mcp/resources/read/response/%/editor.metapack
./schemas/self/v1/schemas/mcp/resources/read/response/%/health.metapack
./schemas/self/v1/schemas/mcp/resources/read/response/%/locations.metapack
./schemas/self/v1/schemas/mcp/resources/read/response/%/positions.metapack
./schemas/self/v1/schemas/mcp/resources/read/response/%/schema.metapack
./schemas/self/v1/schemas/mcp/resources/read/response/%/stats.metapack
./schemas/self/v1/schemas/mcp/resources/templates
./schemas/self/v1/schemas/mcp/resources/templates/list
./schemas/self/v1/schemas/mcp/resources/templates/list/request
./schemas/self/v1/schemas/mcp/resources/templates/list/request/%
./schemas/self/v1/schemas/mcp/resources/templates/list/request/%/blaze-exhaustive.metapack
./schemas/self/v1/schemas/mcp/resources/templates/list/request/%/blaze-fast.metapack
./schemas/self/v1/schemas/mcp/resources/templates/list/request/%/bundle.metapack
./schemas/self/v1/schemas/mcp/resources/templates/list/request/%/dependencies.metapack
./schemas/self/v1/schemas/mcp/resources/templates/list/request/%/dependents.metapack
./schemas/self/v1/schemas/mcp/resources/templates/list/request/%/editor.metapack
./schemas/self/v1/schemas/mcp/resources/templates/list/request/%/health.metapack
./schemas/self/v1/schemas/mcp/resources/templates/list/request/%/locations.metapack
./schemas/self/v1/schemas/mcp/resources/templates/list/request/%/positions.metapack
./schemas/self/v1/schemas/mcp/resources/templates/list/request/%/schema.metapack
./schemas/self/v1/schemas/mcp/resources/templates/list/request/%/stats.metapack
./schemas/self/v1/schemas/mcp/resources/templates/list/response
./schemas/self/v1/schemas/mcp/resources/templates/list/response/%
./schemas/self/v1/schemas/mcp/resources/templates/list/response/%/blaze-exhaustive.metapack
./schemas/self/v1/schemas/mcp/resources/templates/list/response/%/blaze-fast.metapack
./schemas/self/v1/schemas/mcp/resources/templates/list/response/%/bundle.metapack
./schemas/self/v1/schemas/mcp/resources/templates/list/response/%/dependencies.metapack
./schemas/self/v1/schemas/mcp/resources/templates/list/response/%/dependents.metapack
./schemas/self/v1/schemas/mcp/resources/templates/list/response/%/editor.metapack
./schemas/self/v1/schemas/mcp/resources/templates/list/response/%/health.metapack
./schemas/self/v1/schemas/mcp/resources/templates/list/response/%/locations.metapack
./schemas/self/v1/schemas/mcp/resources/templates/list/response/%/positions.metapack
./schemas/self/v1/schemas/mcp/resources/templates/list/response/%/schema.metapack
./schemas/self/v1/schemas/mcp/resources/templates/list/response/%/stats.metapack
./schemas/self/v1/schemas/mcp/response
./schemas/self/v1/schemas/mcp/response/%
./schemas/self/v1/schemas/mcp/response/%/blaze-exhaustive.metapack
./schemas/self/v1/schemas/mcp/response/%/blaze-fast.metapack
./schemas/self/v1/schemas/mcp/response/%/bundle.metapack
./schemas/self/v1/schemas/mcp/response/%/dependencies.metapack
./schemas/self/v1/schemas/mcp/response/%/dependents.metapack
./schemas/self/v1/schemas/mcp/response/%/editor.metapack
./schemas/self/v1/schemas/mcp/response/%/health.metapack
./schemas/self/v1/schemas/mcp/response/%/locations.metapack
./schemas/self/v1/schemas/mcp/response/%/positions.metapack
./schemas/self/v1/schemas/mcp/response/%/schema.metapack
./schemas/self/v1/schemas/mcp/response/%/stats.metapack
./state.bin
./version.json
EOF
diff "$TMP/manifest.txt" "$TMP/expected_manifest.txt"

# Second run: now create the schemas directory with a schema
mkdir "$TMP/schemas"

cat << 'EOF' > "$TMP/schemas/test.json"
{
  "$schema": "http://json-schema.org/draft-07/schema#",
  "$id": "https://example.com/test"
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
(100%) Resolving: schemas/test.json
(  5%) Producing: schemas/schemas/test/%/schema.metapack
( 11%) Producing: schemas/schemas/test/%/dependencies.metapack
( 16%) Producing: schemas/schemas/test/%/locations.metapack
( 22%) Producing: schemas/schemas/test/%/positions.metapack
( 27%) Producing: schemas/schemas/test/%/stats.metapack
( 33%) Producing: schemas/schemas/test/%/bundle.metapack
( 38%) Producing: schemas/schemas/test/%/health.metapack
( 44%) Producing: explorer/schemas/test/%/schema.metapack
( 50%) Producing: schemas/schemas/test/%/blaze-exhaustive.metapack
( 55%) Producing: schemas/schemas/test/%/blaze-fast.metapack
( 61%) Producing: schemas/schemas/test/%/editor.metapack
( 66%) Producing: explorer/schemas/%/directory.metapack
( 72%) Producing: explorer/schemas/test/%/schema-html.metapack
( 77%) Producing: explorer/%/directory.metapack
( 83%) Producing: explorer/schemas/%/directory-html.metapack
( 88%) Producing: explorer/%/directory-html.metapack
( 94%) Producing: explorer/%/search.metapack
(100%) Producing: explorer/%/mcp.metapack
(100%) Combining: schemas/schemas/test/%/dependents.metapack
EOF
diff "$TMP/output.txt" "$TMP/expected.txt"

cd "$TMP/output"
find . -mindepth 1 | LC_ALL=C sort > "$TMP/manifest.txt"
cd - > /dev/null

cat << 'EOF' > "$TMP/expected_manifest.txt"
./configuration.json
./explorer
./explorer/%
./explorer/%/404.metapack
./explorer/%/directory-html.metapack
./explorer/%/directory.metapack
./explorer/%/mcp.metapack
./explorer/%/search.metapack
./explorer/schemas
./explorer/schemas/%
./explorer/schemas/%/directory-html.metapack
./explorer/schemas/%/directory.metapack
./explorer/schemas/test
./explorer/schemas/test/%
./explorer/schemas/test/%/schema-html.metapack
./explorer/schemas/test/%/schema.metapack
./explorer/self
./explorer/self/%
./explorer/self/%/directory-html.metapack
./explorer/self/%/directory.metapack
./explorer/self/v1
./explorer/self/v1/%
./explorer/self/v1/%/directory-html.metapack
./explorer/self/v1/%/directory.metapack
./explorer/self/v1/schemas
./explorer/self/v1/schemas/%
./explorer/self/v1/schemas/%/directory-html.metapack
./explorer/self/v1/schemas/%/directory.metapack
./explorer/self/v1/schemas/api
./explorer/self/v1/schemas/api/%
./explorer/self/v1/schemas/api/%/directory-html.metapack
./explorer/self/v1/schemas/api/%/directory.metapack
./explorer/self/v1/schemas/api/error
./explorer/self/v1/schemas/api/error/%
./explorer/self/v1/schemas/api/error/%/schema-html.metapack
./explorer/self/v1/schemas/api/error/%/schema.metapack
./explorer/self/v1/schemas/api/list
./explorer/self/v1/schemas/api/list/%
./explorer/self/v1/schemas/api/list/%/directory-html.metapack
./explorer/self/v1/schemas/api/list/%/directory.metapack
./explorer/self/v1/schemas/api/list/response
./explorer/self/v1/schemas/api/list/response/%
./explorer/self/v1/schemas/api/list/response/%/schema-html.metapack
./explorer/self/v1/schemas/api/list/response/%/schema.metapack
./explorer/self/v1/schemas/api/list/rpc
./explorer/self/v1/schemas/api/list/rpc/%
./explorer/self/v1/schemas/api/list/rpc/%/schema-html.metapack
./explorer/self/v1/schemas/api/list/rpc/%/schema.metapack
./explorer/self/v1/schemas/api/schemas
./explorer/self/v1/schemas/api/schemas/%
./explorer/self/v1/schemas/api/schemas/%/directory-html.metapack
./explorer/self/v1/schemas/api/schemas/%/directory.metapack
./explorer/self/v1/schemas/api/schemas/dependencies
./explorer/self/v1/schemas/api/schemas/dependencies/%
./explorer/self/v1/schemas/api/schemas/dependencies/%/directory-html.metapack
./explorer/self/v1/schemas/api/schemas/dependencies/%/directory.metapack
./explorer/self/v1/schemas/api/schemas/dependencies/response
./explorer/self/v1/schemas/api/schemas/dependencies/response/%
./explorer/self/v1/schemas/api/schemas/dependencies/response/%/schema-html.metapack
./explorer/self/v1/schemas/api/schemas/dependencies/response/%/schema.metapack
./explorer/self/v1/schemas/api/schemas/dependencies/rpc
./explorer/self/v1/schemas/api/schemas/dependencies/rpc/%
./explorer/self/v1/schemas/api/schemas/dependencies/rpc/%/schema-html.metapack
./explorer/self/v1/schemas/api/schemas/dependencies/rpc/%/schema.metapack
./explorer/self/v1/schemas/api/schemas/dependents
./explorer/self/v1/schemas/api/schemas/dependents/%
./explorer/self/v1/schemas/api/schemas/dependents/%/directory-html.metapack
./explorer/self/v1/schemas/api/schemas/dependents/%/directory.metapack
./explorer/self/v1/schemas/api/schemas/dependents/response
./explorer/self/v1/schemas/api/schemas/dependents/response/%
./explorer/self/v1/schemas/api/schemas/dependents/response/%/schema-html.metapack
./explorer/self/v1/schemas/api/schemas/dependents/response/%/schema.metapack
./explorer/self/v1/schemas/api/schemas/dependents/rpc
./explorer/self/v1/schemas/api/schemas/dependents/rpc/%
./explorer/self/v1/schemas/api/schemas/dependents/rpc/%/schema-html.metapack
./explorer/self/v1/schemas/api/schemas/dependents/rpc/%/schema.metapack
./explorer/self/v1/schemas/api/schemas/evaluate
./explorer/self/v1/schemas/api/schemas/evaluate/%
./explorer/self/v1/schemas/api/schemas/evaluate/%/directory-html.metapack
./explorer/self/v1/schemas/api/schemas/evaluate/%/directory.metapack
./explorer/self/v1/schemas/api/schemas/evaluate/request
./explorer/self/v1/schemas/api/schemas/evaluate/request/%
./explorer/self/v1/schemas/api/schemas/evaluate/request/%/schema-html.metapack
./explorer/self/v1/schemas/api/schemas/evaluate/request/%/schema.metapack
./explorer/self/v1/schemas/api/schemas/evaluate/response
./explorer/self/v1/schemas/api/schemas/evaluate/response/%
./explorer/self/v1/schemas/api/schemas/evaluate/response/%/schema-html.metapack
./explorer/self/v1/schemas/api/schemas/evaluate/response/%/schema.metapack
./explorer/self/v1/schemas/api/schemas/evaluate/rpc
./explorer/self/v1/schemas/api/schemas/evaluate/rpc/%
./explorer/self/v1/schemas/api/schemas/evaluate/rpc/%/schema-html.metapack
./explorer/self/v1/schemas/api/schemas/evaluate/rpc/%/schema.metapack
./explorer/self/v1/schemas/api/schemas/health
./explorer/self/v1/schemas/api/schemas/health/%
./explorer/self/v1/schemas/api/schemas/health/%/directory-html.metapack
./explorer/self/v1/schemas/api/schemas/health/%/directory.metapack
./explorer/self/v1/schemas/api/schemas/health/response
./explorer/self/v1/schemas/api/schemas/health/response/%
./explorer/self/v1/schemas/api/schemas/health/response/%/schema-html.metapack
./explorer/self/v1/schemas/api/schemas/health/response/%/schema.metapack
./explorer/self/v1/schemas/api/schemas/health/rpc
./explorer/self/v1/schemas/api/schemas/health/rpc/%
./explorer/self/v1/schemas/api/schemas/health/rpc/%/schema-html.metapack
./explorer/self/v1/schemas/api/schemas/health/rpc/%/schema.metapack
./explorer/self/v1/schemas/api/schemas/locations
./explorer/self/v1/schemas/api/schemas/locations/%
./explorer/self/v1/schemas/api/schemas/locations/%/directory-html.metapack
./explorer/self/v1/schemas/api/schemas/locations/%/directory.metapack
./explorer/self/v1/schemas/api/schemas/locations/response
./explorer/self/v1/schemas/api/schemas/locations/response/%
./explorer/self/v1/schemas/api/schemas/locations/response/%/schema-html.metapack
./explorer/self/v1/schemas/api/schemas/locations/response/%/schema.metapack
./explorer/self/v1/schemas/api/schemas/locations/rpc
./explorer/self/v1/schemas/api/schemas/locations/rpc/%
./explorer/self/v1/schemas/api/schemas/locations/rpc/%/schema-html.metapack
./explorer/self/v1/schemas/api/schemas/locations/rpc/%/schema.metapack
./explorer/self/v1/schemas/api/schemas/metadata
./explorer/self/v1/schemas/api/schemas/metadata/%
./explorer/self/v1/schemas/api/schemas/metadata/%/directory-html.metapack
./explorer/self/v1/schemas/api/schemas/metadata/%/directory.metapack
./explorer/self/v1/schemas/api/schemas/metadata/response
./explorer/self/v1/schemas/api/schemas/metadata/response/%
./explorer/self/v1/schemas/api/schemas/metadata/response/%/schema-html.metapack
./explorer/self/v1/schemas/api/schemas/metadata/response/%/schema.metapack
./explorer/self/v1/schemas/api/schemas/metadata/rpc
./explorer/self/v1/schemas/api/schemas/metadata/rpc/%
./explorer/self/v1/schemas/api/schemas/metadata/rpc/%/schema-html.metapack
./explorer/self/v1/schemas/api/schemas/metadata/rpc/%/schema.metapack
./explorer/self/v1/schemas/api/schemas/position
./explorer/self/v1/schemas/api/schemas/position/%
./explorer/self/v1/schemas/api/schemas/position/%/schema-html.metapack
./explorer/self/v1/schemas/api/schemas/position/%/schema.metapack
./explorer/self/v1/schemas/api/schemas/positions
./explorer/self/v1/schemas/api/schemas/positions/%
./explorer/self/v1/schemas/api/schemas/positions/%/directory-html.metapack
./explorer/self/v1/schemas/api/schemas/positions/%/directory.metapack
./explorer/self/v1/schemas/api/schemas/positions/response
./explorer/self/v1/schemas/api/schemas/positions/response/%
./explorer/self/v1/schemas/api/schemas/positions/response/%/schema-html.metapack
./explorer/self/v1/schemas/api/schemas/positions/response/%/schema.metapack
./explorer/self/v1/schemas/api/schemas/positions/rpc
./explorer/self/v1/schemas/api/schemas/positions/rpc/%
./explorer/self/v1/schemas/api/schemas/positions/rpc/%/schema-html.metapack
./explorer/self/v1/schemas/api/schemas/positions/rpc/%/schema.metapack
./explorer/self/v1/schemas/api/schemas/search
./explorer/self/v1/schemas/api/schemas/search/%
./explorer/self/v1/schemas/api/schemas/search/%/directory-html.metapack
./explorer/self/v1/schemas/api/schemas/search/%/directory.metapack
./explorer/self/v1/schemas/api/schemas/search/response
./explorer/self/v1/schemas/api/schemas/search/response/%
./explorer/self/v1/schemas/api/schemas/search/response/%/schema-html.metapack
./explorer/self/v1/schemas/api/schemas/search/response/%/schema.metapack
./explorer/self/v1/schemas/api/schemas/search/rpc
./explorer/self/v1/schemas/api/schemas/search/rpc/%
./explorer/self/v1/schemas/api/schemas/search/rpc/%/schema-html.metapack
./explorer/self/v1/schemas/api/schemas/search/rpc/%/schema.metapack
./explorer/self/v1/schemas/api/schemas/stats
./explorer/self/v1/schemas/api/schemas/stats/%
./explorer/self/v1/schemas/api/schemas/stats/%/directory-html.metapack
./explorer/self/v1/schemas/api/schemas/stats/%/directory.metapack
./explorer/self/v1/schemas/api/schemas/stats/response
./explorer/self/v1/schemas/api/schemas/stats/response/%
./explorer/self/v1/schemas/api/schemas/stats/response/%/schema-html.metapack
./explorer/self/v1/schemas/api/schemas/stats/response/%/schema.metapack
./explorer/self/v1/schemas/api/schemas/stats/rpc
./explorer/self/v1/schemas/api/schemas/stats/rpc/%
./explorer/self/v1/schemas/api/schemas/stats/rpc/%/schema-html.metapack
./explorer/self/v1/schemas/api/schemas/stats/rpc/%/schema.metapack
./explorer/self/v1/schemas/api/schemas/trace
./explorer/self/v1/schemas/api/schemas/trace/%
./explorer/self/v1/schemas/api/schemas/trace/%/directory-html.metapack
./explorer/self/v1/schemas/api/schemas/trace/%/directory.metapack
./explorer/self/v1/schemas/api/schemas/trace/request
./explorer/self/v1/schemas/api/schemas/trace/request/%
./explorer/self/v1/schemas/api/schemas/trace/request/%/schema-html.metapack
./explorer/self/v1/schemas/api/schemas/trace/request/%/schema.metapack
./explorer/self/v1/schemas/api/schemas/trace/response
./explorer/self/v1/schemas/api/schemas/trace/response/%
./explorer/self/v1/schemas/api/schemas/trace/response/%/schema-html.metapack
./explorer/self/v1/schemas/api/schemas/trace/response/%/schema.metapack
./explorer/self/v1/schemas/api/schemas/trace/rpc
./explorer/self/v1/schemas/api/schemas/trace/rpc/%
./explorer/self/v1/schemas/api/schemas/trace/rpc/%/schema-html.metapack
./explorer/self/v1/schemas/api/schemas/trace/rpc/%/schema.metapack
./explorer/self/v1/schemas/mcp
./explorer/self/v1/schemas/mcp/%
./explorer/self/v1/schemas/mcp/%/directory-html.metapack
./explorer/self/v1/schemas/mcp/%/directory.metapack
./explorer/self/v1/schemas/mcp/error
./explorer/self/v1/schemas/mcp/error/%
./explorer/self/v1/schemas/mcp/error/%/schema-html.metapack
./explorer/self/v1/schemas/mcp/error/%/schema.metapack
./explorer/self/v1/schemas/mcp/initialize
./explorer/self/v1/schemas/mcp/initialize/%
./explorer/self/v1/schemas/mcp/initialize/%/directory-html.metapack
./explorer/self/v1/schemas/mcp/initialize/%/directory.metapack
./explorer/self/v1/schemas/mcp/initialize/request
./explorer/self/v1/schemas/mcp/initialize/request/%
./explorer/self/v1/schemas/mcp/initialize/request/%/schema-html.metapack
./explorer/self/v1/schemas/mcp/initialize/request/%/schema.metapack
./explorer/self/v1/schemas/mcp/initialize/response
./explorer/self/v1/schemas/mcp/initialize/response/%
./explorer/self/v1/schemas/mcp/initialize/response/%/schema-html.metapack
./explorer/self/v1/schemas/mcp/initialize/response/%/schema.metapack
./explorer/self/v1/schemas/mcp/notifications
./explorer/self/v1/schemas/mcp/notifications/%
./explorer/self/v1/schemas/mcp/notifications/%/directory-html.metapack
./explorer/self/v1/schemas/mcp/notifications/%/directory.metapack
./explorer/self/v1/schemas/mcp/notifications/cancelled
./explorer/self/v1/schemas/mcp/notifications/cancelled/%
./explorer/self/v1/schemas/mcp/notifications/cancelled/%/schema-html.metapack
./explorer/self/v1/schemas/mcp/notifications/cancelled/%/schema.metapack
./explorer/self/v1/schemas/mcp/notifications/initialized
./explorer/self/v1/schemas/mcp/notifications/initialized/%
./explorer/self/v1/schemas/mcp/notifications/initialized/%/schema-html.metapack
./explorer/self/v1/schemas/mcp/notifications/initialized/%/schema.metapack
./explorer/self/v1/schemas/mcp/ping
./explorer/self/v1/schemas/mcp/ping/%
./explorer/self/v1/schemas/mcp/ping/%/directory-html.metapack
./explorer/self/v1/schemas/mcp/ping/%/directory.metapack
./explorer/self/v1/schemas/mcp/ping/request
./explorer/self/v1/schemas/mcp/ping/request/%
./explorer/self/v1/schemas/mcp/ping/request/%/schema-html.metapack
./explorer/self/v1/schemas/mcp/ping/request/%/schema.metapack
./explorer/self/v1/schemas/mcp/ping/response
./explorer/self/v1/schemas/mcp/ping/response/%
./explorer/self/v1/schemas/mcp/ping/response/%/schema-html.metapack
./explorer/self/v1/schemas/mcp/ping/response/%/schema.metapack
./explorer/self/v1/schemas/mcp/request
./explorer/self/v1/schemas/mcp/request/%
./explorer/self/v1/schemas/mcp/request/%/schema-html.metapack
./explorer/self/v1/schemas/mcp/request/%/schema.metapack
./explorer/self/v1/schemas/mcp/resources
./explorer/self/v1/schemas/mcp/resources/%
./explorer/self/v1/schemas/mcp/resources/%/directory-html.metapack
./explorer/self/v1/schemas/mcp/resources/%/directory.metapack
./explorer/self/v1/schemas/mcp/resources/list
./explorer/self/v1/schemas/mcp/resources/list/%
./explorer/self/v1/schemas/mcp/resources/list/%/directory-html.metapack
./explorer/self/v1/schemas/mcp/resources/list/%/directory.metapack
./explorer/self/v1/schemas/mcp/resources/list/request
./explorer/self/v1/schemas/mcp/resources/list/request/%
./explorer/self/v1/schemas/mcp/resources/list/request/%/schema-html.metapack
./explorer/self/v1/schemas/mcp/resources/list/request/%/schema.metapack
./explorer/self/v1/schemas/mcp/resources/list/response
./explorer/self/v1/schemas/mcp/resources/list/response/%
./explorer/self/v1/schemas/mcp/resources/list/response/%/schema-html.metapack
./explorer/self/v1/schemas/mcp/resources/list/response/%/schema.metapack
./explorer/self/v1/schemas/mcp/resources/read
./explorer/self/v1/schemas/mcp/resources/read/%
./explorer/self/v1/schemas/mcp/resources/read/%/directory-html.metapack
./explorer/self/v1/schemas/mcp/resources/read/%/directory.metapack
./explorer/self/v1/schemas/mcp/resources/read/request
./explorer/self/v1/schemas/mcp/resources/read/request/%
./explorer/self/v1/schemas/mcp/resources/read/request/%/schema-html.metapack
./explorer/self/v1/schemas/mcp/resources/read/request/%/schema.metapack
./explorer/self/v1/schemas/mcp/resources/read/response
./explorer/self/v1/schemas/mcp/resources/read/response/%
./explorer/self/v1/schemas/mcp/resources/read/response/%/schema-html.metapack
./explorer/self/v1/schemas/mcp/resources/read/response/%/schema.metapack
./explorer/self/v1/schemas/mcp/resources/templates
./explorer/self/v1/schemas/mcp/resources/templates/%
./explorer/self/v1/schemas/mcp/resources/templates/%/directory-html.metapack
./explorer/self/v1/schemas/mcp/resources/templates/%/directory.metapack
./explorer/self/v1/schemas/mcp/resources/templates/list
./explorer/self/v1/schemas/mcp/resources/templates/list/%
./explorer/self/v1/schemas/mcp/resources/templates/list/%/directory-html.metapack
./explorer/self/v1/schemas/mcp/resources/templates/list/%/directory.metapack
./explorer/self/v1/schemas/mcp/resources/templates/list/request
./explorer/self/v1/schemas/mcp/resources/templates/list/request/%
./explorer/self/v1/schemas/mcp/resources/templates/list/request/%/schema-html.metapack
./explorer/self/v1/schemas/mcp/resources/templates/list/request/%/schema.metapack
./explorer/self/v1/schemas/mcp/resources/templates/list/response
./explorer/self/v1/schemas/mcp/resources/templates/list/response/%
./explorer/self/v1/schemas/mcp/resources/templates/list/response/%/schema-html.metapack
./explorer/self/v1/schemas/mcp/resources/templates/list/response/%/schema.metapack
./explorer/self/v1/schemas/mcp/response
./explorer/self/v1/schemas/mcp/response/%
./explorer/self/v1/schemas/mcp/response/%/schema-html.metapack
./explorer/self/v1/schemas/mcp/response/%/schema.metapack
./routes.bin
./schemas
./schemas/schemas
./schemas/schemas/test
./schemas/schemas/test/%
./schemas/schemas/test/%/blaze-exhaustive.metapack
./schemas/schemas/test/%/blaze-fast.metapack
./schemas/schemas/test/%/bundle.metapack
./schemas/schemas/test/%/dependencies.metapack
./schemas/schemas/test/%/dependents.metapack
./schemas/schemas/test/%/editor.metapack
./schemas/schemas/test/%/health.metapack
./schemas/schemas/test/%/locations.metapack
./schemas/schemas/test/%/positions.metapack
./schemas/schemas/test/%/schema.metapack
./schemas/schemas/test/%/stats.metapack
./schemas/self
./schemas/self/v1
./schemas/self/v1/schemas
./schemas/self/v1/schemas/api
./schemas/self/v1/schemas/api/error
./schemas/self/v1/schemas/api/error/%
./schemas/self/v1/schemas/api/error/%/blaze-exhaustive.metapack
./schemas/self/v1/schemas/api/error/%/blaze-fast.metapack
./schemas/self/v1/schemas/api/error/%/bundle.metapack
./schemas/self/v1/schemas/api/error/%/dependencies.metapack
./schemas/self/v1/schemas/api/error/%/dependents.metapack
./schemas/self/v1/schemas/api/error/%/editor.metapack
./schemas/self/v1/schemas/api/error/%/health.metapack
./schemas/self/v1/schemas/api/error/%/locations.metapack
./schemas/self/v1/schemas/api/error/%/positions.metapack
./schemas/self/v1/schemas/api/error/%/schema.metapack
./schemas/self/v1/schemas/api/error/%/stats.metapack
./schemas/self/v1/schemas/api/list
./schemas/self/v1/schemas/api/list/response
./schemas/self/v1/schemas/api/list/response/%
./schemas/self/v1/schemas/api/list/response/%/blaze-exhaustive.metapack
./schemas/self/v1/schemas/api/list/response/%/blaze-fast.metapack
./schemas/self/v1/schemas/api/list/response/%/bundle.metapack
./schemas/self/v1/schemas/api/list/response/%/dependencies.metapack
./schemas/self/v1/schemas/api/list/response/%/dependents.metapack
./schemas/self/v1/schemas/api/list/response/%/editor.metapack
./schemas/self/v1/schemas/api/list/response/%/health.metapack
./schemas/self/v1/schemas/api/list/response/%/locations.metapack
./schemas/self/v1/schemas/api/list/response/%/positions.metapack
./schemas/self/v1/schemas/api/list/response/%/schema.metapack
./schemas/self/v1/schemas/api/list/response/%/stats.metapack
./schemas/self/v1/schemas/api/list/rpc
./schemas/self/v1/schemas/api/list/rpc/%
./schemas/self/v1/schemas/api/list/rpc/%/blaze-exhaustive.metapack
./schemas/self/v1/schemas/api/list/rpc/%/blaze-fast.metapack
./schemas/self/v1/schemas/api/list/rpc/%/bundle.metapack
./schemas/self/v1/schemas/api/list/rpc/%/dependencies.metapack
./schemas/self/v1/schemas/api/list/rpc/%/dependents.metapack
./schemas/self/v1/schemas/api/list/rpc/%/editor.metapack
./schemas/self/v1/schemas/api/list/rpc/%/health.metapack
./schemas/self/v1/schemas/api/list/rpc/%/locations.metapack
./schemas/self/v1/schemas/api/list/rpc/%/positions.metapack
./schemas/self/v1/schemas/api/list/rpc/%/schema.metapack
./schemas/self/v1/schemas/api/list/rpc/%/stats.metapack
./schemas/self/v1/schemas/api/schemas
./schemas/self/v1/schemas/api/schemas/dependencies
./schemas/self/v1/schemas/api/schemas/dependencies/response
./schemas/self/v1/schemas/api/schemas/dependencies/response/%
./schemas/self/v1/schemas/api/schemas/dependencies/response/%/blaze-exhaustive.metapack
./schemas/self/v1/schemas/api/schemas/dependencies/response/%/blaze-fast.metapack
./schemas/self/v1/schemas/api/schemas/dependencies/response/%/bundle.metapack
./schemas/self/v1/schemas/api/schemas/dependencies/response/%/dependencies.metapack
./schemas/self/v1/schemas/api/schemas/dependencies/response/%/dependents.metapack
./schemas/self/v1/schemas/api/schemas/dependencies/response/%/editor.metapack
./schemas/self/v1/schemas/api/schemas/dependencies/response/%/health.metapack
./schemas/self/v1/schemas/api/schemas/dependencies/response/%/locations.metapack
./schemas/self/v1/schemas/api/schemas/dependencies/response/%/positions.metapack
./schemas/self/v1/schemas/api/schemas/dependencies/response/%/schema.metapack
./schemas/self/v1/schemas/api/schemas/dependencies/response/%/stats.metapack
./schemas/self/v1/schemas/api/schemas/dependencies/rpc
./schemas/self/v1/schemas/api/schemas/dependencies/rpc/%
./schemas/self/v1/schemas/api/schemas/dependencies/rpc/%/blaze-exhaustive.metapack
./schemas/self/v1/schemas/api/schemas/dependencies/rpc/%/blaze-fast.metapack
./schemas/self/v1/schemas/api/schemas/dependencies/rpc/%/bundle.metapack
./schemas/self/v1/schemas/api/schemas/dependencies/rpc/%/dependencies.metapack
./schemas/self/v1/schemas/api/schemas/dependencies/rpc/%/dependents.metapack
./schemas/self/v1/schemas/api/schemas/dependencies/rpc/%/editor.metapack
./schemas/self/v1/schemas/api/schemas/dependencies/rpc/%/health.metapack
./schemas/self/v1/schemas/api/schemas/dependencies/rpc/%/locations.metapack
./schemas/self/v1/schemas/api/schemas/dependencies/rpc/%/positions.metapack
./schemas/self/v1/schemas/api/schemas/dependencies/rpc/%/schema.metapack
./schemas/self/v1/schemas/api/schemas/dependencies/rpc/%/stats.metapack
./schemas/self/v1/schemas/api/schemas/dependents
./schemas/self/v1/schemas/api/schemas/dependents/response
./schemas/self/v1/schemas/api/schemas/dependents/response/%
./schemas/self/v1/schemas/api/schemas/dependents/response/%/blaze-exhaustive.metapack
./schemas/self/v1/schemas/api/schemas/dependents/response/%/blaze-fast.metapack
./schemas/self/v1/schemas/api/schemas/dependents/response/%/bundle.metapack
./schemas/self/v1/schemas/api/schemas/dependents/response/%/dependencies.metapack
./schemas/self/v1/schemas/api/schemas/dependents/response/%/dependents.metapack
./schemas/self/v1/schemas/api/schemas/dependents/response/%/editor.metapack
./schemas/self/v1/schemas/api/schemas/dependents/response/%/health.metapack
./schemas/self/v1/schemas/api/schemas/dependents/response/%/locations.metapack
./schemas/self/v1/schemas/api/schemas/dependents/response/%/positions.metapack
./schemas/self/v1/schemas/api/schemas/dependents/response/%/schema.metapack
./schemas/self/v1/schemas/api/schemas/dependents/response/%/stats.metapack
./schemas/self/v1/schemas/api/schemas/dependents/rpc
./schemas/self/v1/schemas/api/schemas/dependents/rpc/%
./schemas/self/v1/schemas/api/schemas/dependents/rpc/%/blaze-exhaustive.metapack
./schemas/self/v1/schemas/api/schemas/dependents/rpc/%/blaze-fast.metapack
./schemas/self/v1/schemas/api/schemas/dependents/rpc/%/bundle.metapack
./schemas/self/v1/schemas/api/schemas/dependents/rpc/%/dependencies.metapack
./schemas/self/v1/schemas/api/schemas/dependents/rpc/%/dependents.metapack
./schemas/self/v1/schemas/api/schemas/dependents/rpc/%/editor.metapack
./schemas/self/v1/schemas/api/schemas/dependents/rpc/%/health.metapack
./schemas/self/v1/schemas/api/schemas/dependents/rpc/%/locations.metapack
./schemas/self/v1/schemas/api/schemas/dependents/rpc/%/positions.metapack
./schemas/self/v1/schemas/api/schemas/dependents/rpc/%/schema.metapack
./schemas/self/v1/schemas/api/schemas/dependents/rpc/%/stats.metapack
./schemas/self/v1/schemas/api/schemas/evaluate
./schemas/self/v1/schemas/api/schemas/evaluate/request
./schemas/self/v1/schemas/api/schemas/evaluate/request/%
./schemas/self/v1/schemas/api/schemas/evaluate/request/%/blaze-exhaustive.metapack
./schemas/self/v1/schemas/api/schemas/evaluate/request/%/blaze-fast.metapack
./schemas/self/v1/schemas/api/schemas/evaluate/request/%/bundle.metapack
./schemas/self/v1/schemas/api/schemas/evaluate/request/%/dependencies.metapack
./schemas/self/v1/schemas/api/schemas/evaluate/request/%/dependents.metapack
./schemas/self/v1/schemas/api/schemas/evaluate/request/%/editor.metapack
./schemas/self/v1/schemas/api/schemas/evaluate/request/%/health.metapack
./schemas/self/v1/schemas/api/schemas/evaluate/request/%/locations.metapack
./schemas/self/v1/schemas/api/schemas/evaluate/request/%/positions.metapack
./schemas/self/v1/schemas/api/schemas/evaluate/request/%/schema.metapack
./schemas/self/v1/schemas/api/schemas/evaluate/request/%/stats.metapack
./schemas/self/v1/schemas/api/schemas/evaluate/response
./schemas/self/v1/schemas/api/schemas/evaluate/response/%
./schemas/self/v1/schemas/api/schemas/evaluate/response/%/blaze-exhaustive.metapack
./schemas/self/v1/schemas/api/schemas/evaluate/response/%/blaze-fast.metapack
./schemas/self/v1/schemas/api/schemas/evaluate/response/%/bundle.metapack
./schemas/self/v1/schemas/api/schemas/evaluate/response/%/dependencies.metapack
./schemas/self/v1/schemas/api/schemas/evaluate/response/%/dependents.metapack
./schemas/self/v1/schemas/api/schemas/evaluate/response/%/editor.metapack
./schemas/self/v1/schemas/api/schemas/evaluate/response/%/health.metapack
./schemas/self/v1/schemas/api/schemas/evaluate/response/%/locations.metapack
./schemas/self/v1/schemas/api/schemas/evaluate/response/%/positions.metapack
./schemas/self/v1/schemas/api/schemas/evaluate/response/%/schema.metapack
./schemas/self/v1/schemas/api/schemas/evaluate/response/%/stats.metapack
./schemas/self/v1/schemas/api/schemas/evaluate/rpc
./schemas/self/v1/schemas/api/schemas/evaluate/rpc/%
./schemas/self/v1/schemas/api/schemas/evaluate/rpc/%/blaze-exhaustive.metapack
./schemas/self/v1/schemas/api/schemas/evaluate/rpc/%/blaze-fast.metapack
./schemas/self/v1/schemas/api/schemas/evaluate/rpc/%/bundle.metapack
./schemas/self/v1/schemas/api/schemas/evaluate/rpc/%/dependencies.metapack
./schemas/self/v1/schemas/api/schemas/evaluate/rpc/%/dependents.metapack
./schemas/self/v1/schemas/api/schemas/evaluate/rpc/%/editor.metapack
./schemas/self/v1/schemas/api/schemas/evaluate/rpc/%/health.metapack
./schemas/self/v1/schemas/api/schemas/evaluate/rpc/%/locations.metapack
./schemas/self/v1/schemas/api/schemas/evaluate/rpc/%/positions.metapack
./schemas/self/v1/schemas/api/schemas/evaluate/rpc/%/schema.metapack
./schemas/self/v1/schemas/api/schemas/evaluate/rpc/%/stats.metapack
./schemas/self/v1/schemas/api/schemas/health
./schemas/self/v1/schemas/api/schemas/health/response
./schemas/self/v1/schemas/api/schemas/health/response/%
./schemas/self/v1/schemas/api/schemas/health/response/%/blaze-exhaustive.metapack
./schemas/self/v1/schemas/api/schemas/health/response/%/blaze-fast.metapack
./schemas/self/v1/schemas/api/schemas/health/response/%/bundle.metapack
./schemas/self/v1/schemas/api/schemas/health/response/%/dependencies.metapack
./schemas/self/v1/schemas/api/schemas/health/response/%/dependents.metapack
./schemas/self/v1/schemas/api/schemas/health/response/%/editor.metapack
./schemas/self/v1/schemas/api/schemas/health/response/%/health.metapack
./schemas/self/v1/schemas/api/schemas/health/response/%/locations.metapack
./schemas/self/v1/schemas/api/schemas/health/response/%/positions.metapack
./schemas/self/v1/schemas/api/schemas/health/response/%/schema.metapack
./schemas/self/v1/schemas/api/schemas/health/response/%/stats.metapack
./schemas/self/v1/schemas/api/schemas/health/rpc
./schemas/self/v1/schemas/api/schemas/health/rpc/%
./schemas/self/v1/schemas/api/schemas/health/rpc/%/blaze-exhaustive.metapack
./schemas/self/v1/schemas/api/schemas/health/rpc/%/blaze-fast.metapack
./schemas/self/v1/schemas/api/schemas/health/rpc/%/bundle.metapack
./schemas/self/v1/schemas/api/schemas/health/rpc/%/dependencies.metapack
./schemas/self/v1/schemas/api/schemas/health/rpc/%/dependents.metapack
./schemas/self/v1/schemas/api/schemas/health/rpc/%/editor.metapack
./schemas/self/v1/schemas/api/schemas/health/rpc/%/health.metapack
./schemas/self/v1/schemas/api/schemas/health/rpc/%/locations.metapack
./schemas/self/v1/schemas/api/schemas/health/rpc/%/positions.metapack
./schemas/self/v1/schemas/api/schemas/health/rpc/%/schema.metapack
./schemas/self/v1/schemas/api/schemas/health/rpc/%/stats.metapack
./schemas/self/v1/schemas/api/schemas/locations
./schemas/self/v1/schemas/api/schemas/locations/response
./schemas/self/v1/schemas/api/schemas/locations/response/%
./schemas/self/v1/schemas/api/schemas/locations/response/%/blaze-exhaustive.metapack
./schemas/self/v1/schemas/api/schemas/locations/response/%/blaze-fast.metapack
./schemas/self/v1/schemas/api/schemas/locations/response/%/bundle.metapack
./schemas/self/v1/schemas/api/schemas/locations/response/%/dependencies.metapack
./schemas/self/v1/schemas/api/schemas/locations/response/%/dependents.metapack
./schemas/self/v1/schemas/api/schemas/locations/response/%/editor.metapack
./schemas/self/v1/schemas/api/schemas/locations/response/%/health.metapack
./schemas/self/v1/schemas/api/schemas/locations/response/%/locations.metapack
./schemas/self/v1/schemas/api/schemas/locations/response/%/positions.metapack
./schemas/self/v1/schemas/api/schemas/locations/response/%/schema.metapack
./schemas/self/v1/schemas/api/schemas/locations/response/%/stats.metapack
./schemas/self/v1/schemas/api/schemas/locations/rpc
./schemas/self/v1/schemas/api/schemas/locations/rpc/%
./schemas/self/v1/schemas/api/schemas/locations/rpc/%/blaze-exhaustive.metapack
./schemas/self/v1/schemas/api/schemas/locations/rpc/%/blaze-fast.metapack
./schemas/self/v1/schemas/api/schemas/locations/rpc/%/bundle.metapack
./schemas/self/v1/schemas/api/schemas/locations/rpc/%/dependencies.metapack
./schemas/self/v1/schemas/api/schemas/locations/rpc/%/dependents.metapack
./schemas/self/v1/schemas/api/schemas/locations/rpc/%/editor.metapack
./schemas/self/v1/schemas/api/schemas/locations/rpc/%/health.metapack
./schemas/self/v1/schemas/api/schemas/locations/rpc/%/locations.metapack
./schemas/self/v1/schemas/api/schemas/locations/rpc/%/positions.metapack
./schemas/self/v1/schemas/api/schemas/locations/rpc/%/schema.metapack
./schemas/self/v1/schemas/api/schemas/locations/rpc/%/stats.metapack
./schemas/self/v1/schemas/api/schemas/metadata
./schemas/self/v1/schemas/api/schemas/metadata/response
./schemas/self/v1/schemas/api/schemas/metadata/response/%
./schemas/self/v1/schemas/api/schemas/metadata/response/%/blaze-exhaustive.metapack
./schemas/self/v1/schemas/api/schemas/metadata/response/%/blaze-fast.metapack
./schemas/self/v1/schemas/api/schemas/metadata/response/%/bundle.metapack
./schemas/self/v1/schemas/api/schemas/metadata/response/%/dependencies.metapack
./schemas/self/v1/schemas/api/schemas/metadata/response/%/dependents.metapack
./schemas/self/v1/schemas/api/schemas/metadata/response/%/editor.metapack
./schemas/self/v1/schemas/api/schemas/metadata/response/%/health.metapack
./schemas/self/v1/schemas/api/schemas/metadata/response/%/locations.metapack
./schemas/self/v1/schemas/api/schemas/metadata/response/%/positions.metapack
./schemas/self/v1/schemas/api/schemas/metadata/response/%/schema.metapack
./schemas/self/v1/schemas/api/schemas/metadata/response/%/stats.metapack
./schemas/self/v1/schemas/api/schemas/metadata/rpc
./schemas/self/v1/schemas/api/schemas/metadata/rpc/%
./schemas/self/v1/schemas/api/schemas/metadata/rpc/%/blaze-exhaustive.metapack
./schemas/self/v1/schemas/api/schemas/metadata/rpc/%/blaze-fast.metapack
./schemas/self/v1/schemas/api/schemas/metadata/rpc/%/bundle.metapack
./schemas/self/v1/schemas/api/schemas/metadata/rpc/%/dependencies.metapack
./schemas/self/v1/schemas/api/schemas/metadata/rpc/%/dependents.metapack
./schemas/self/v1/schemas/api/schemas/metadata/rpc/%/editor.metapack
./schemas/self/v1/schemas/api/schemas/metadata/rpc/%/health.metapack
./schemas/self/v1/schemas/api/schemas/metadata/rpc/%/locations.metapack
./schemas/self/v1/schemas/api/schemas/metadata/rpc/%/positions.metapack
./schemas/self/v1/schemas/api/schemas/metadata/rpc/%/schema.metapack
./schemas/self/v1/schemas/api/schemas/metadata/rpc/%/stats.metapack
./schemas/self/v1/schemas/api/schemas/position
./schemas/self/v1/schemas/api/schemas/position/%
./schemas/self/v1/schemas/api/schemas/position/%/blaze-exhaustive.metapack
./schemas/self/v1/schemas/api/schemas/position/%/blaze-fast.metapack
./schemas/self/v1/schemas/api/schemas/position/%/bundle.metapack
./schemas/self/v1/schemas/api/schemas/position/%/dependencies.metapack
./schemas/self/v1/schemas/api/schemas/position/%/dependents.metapack
./schemas/self/v1/schemas/api/schemas/position/%/editor.metapack
./schemas/self/v1/schemas/api/schemas/position/%/health.metapack
./schemas/self/v1/schemas/api/schemas/position/%/locations.metapack
./schemas/self/v1/schemas/api/schemas/position/%/positions.metapack
./schemas/self/v1/schemas/api/schemas/position/%/schema.metapack
./schemas/self/v1/schemas/api/schemas/position/%/stats.metapack
./schemas/self/v1/schemas/api/schemas/positions
./schemas/self/v1/schemas/api/schemas/positions/response
./schemas/self/v1/schemas/api/schemas/positions/response/%
./schemas/self/v1/schemas/api/schemas/positions/response/%/blaze-exhaustive.metapack
./schemas/self/v1/schemas/api/schemas/positions/response/%/blaze-fast.metapack
./schemas/self/v1/schemas/api/schemas/positions/response/%/bundle.metapack
./schemas/self/v1/schemas/api/schemas/positions/response/%/dependencies.metapack
./schemas/self/v1/schemas/api/schemas/positions/response/%/dependents.metapack
./schemas/self/v1/schemas/api/schemas/positions/response/%/editor.metapack
./schemas/self/v1/schemas/api/schemas/positions/response/%/health.metapack
./schemas/self/v1/schemas/api/schemas/positions/response/%/locations.metapack
./schemas/self/v1/schemas/api/schemas/positions/response/%/positions.metapack
./schemas/self/v1/schemas/api/schemas/positions/response/%/schema.metapack
./schemas/self/v1/schemas/api/schemas/positions/response/%/stats.metapack
./schemas/self/v1/schemas/api/schemas/positions/rpc
./schemas/self/v1/schemas/api/schemas/positions/rpc/%
./schemas/self/v1/schemas/api/schemas/positions/rpc/%/blaze-exhaustive.metapack
./schemas/self/v1/schemas/api/schemas/positions/rpc/%/blaze-fast.metapack
./schemas/self/v1/schemas/api/schemas/positions/rpc/%/bundle.metapack
./schemas/self/v1/schemas/api/schemas/positions/rpc/%/dependencies.metapack
./schemas/self/v1/schemas/api/schemas/positions/rpc/%/dependents.metapack
./schemas/self/v1/schemas/api/schemas/positions/rpc/%/editor.metapack
./schemas/self/v1/schemas/api/schemas/positions/rpc/%/health.metapack
./schemas/self/v1/schemas/api/schemas/positions/rpc/%/locations.metapack
./schemas/self/v1/schemas/api/schemas/positions/rpc/%/positions.metapack
./schemas/self/v1/schemas/api/schemas/positions/rpc/%/schema.metapack
./schemas/self/v1/schemas/api/schemas/positions/rpc/%/stats.metapack
./schemas/self/v1/schemas/api/schemas/search
./schemas/self/v1/schemas/api/schemas/search/response
./schemas/self/v1/schemas/api/schemas/search/response/%
./schemas/self/v1/schemas/api/schemas/search/response/%/blaze-exhaustive.metapack
./schemas/self/v1/schemas/api/schemas/search/response/%/blaze-fast.metapack
./schemas/self/v1/schemas/api/schemas/search/response/%/bundle.metapack
./schemas/self/v1/schemas/api/schemas/search/response/%/dependencies.metapack
./schemas/self/v1/schemas/api/schemas/search/response/%/dependents.metapack
./schemas/self/v1/schemas/api/schemas/search/response/%/editor.metapack
./schemas/self/v1/schemas/api/schemas/search/response/%/health.metapack
./schemas/self/v1/schemas/api/schemas/search/response/%/locations.metapack
./schemas/self/v1/schemas/api/schemas/search/response/%/positions.metapack
./schemas/self/v1/schemas/api/schemas/search/response/%/schema.metapack
./schemas/self/v1/schemas/api/schemas/search/response/%/stats.metapack
./schemas/self/v1/schemas/api/schemas/search/rpc
./schemas/self/v1/schemas/api/schemas/search/rpc/%
./schemas/self/v1/schemas/api/schemas/search/rpc/%/blaze-exhaustive.metapack
./schemas/self/v1/schemas/api/schemas/search/rpc/%/blaze-fast.metapack
./schemas/self/v1/schemas/api/schemas/search/rpc/%/bundle.metapack
./schemas/self/v1/schemas/api/schemas/search/rpc/%/dependencies.metapack
./schemas/self/v1/schemas/api/schemas/search/rpc/%/dependents.metapack
./schemas/self/v1/schemas/api/schemas/search/rpc/%/editor.metapack
./schemas/self/v1/schemas/api/schemas/search/rpc/%/health.metapack
./schemas/self/v1/schemas/api/schemas/search/rpc/%/locations.metapack
./schemas/self/v1/schemas/api/schemas/search/rpc/%/positions.metapack
./schemas/self/v1/schemas/api/schemas/search/rpc/%/schema.metapack
./schemas/self/v1/schemas/api/schemas/search/rpc/%/stats.metapack
./schemas/self/v1/schemas/api/schemas/stats
./schemas/self/v1/schemas/api/schemas/stats/response
./schemas/self/v1/schemas/api/schemas/stats/response/%
./schemas/self/v1/schemas/api/schemas/stats/response/%/blaze-exhaustive.metapack
./schemas/self/v1/schemas/api/schemas/stats/response/%/blaze-fast.metapack
./schemas/self/v1/schemas/api/schemas/stats/response/%/bundle.metapack
./schemas/self/v1/schemas/api/schemas/stats/response/%/dependencies.metapack
./schemas/self/v1/schemas/api/schemas/stats/response/%/dependents.metapack
./schemas/self/v1/schemas/api/schemas/stats/response/%/editor.metapack
./schemas/self/v1/schemas/api/schemas/stats/response/%/health.metapack
./schemas/self/v1/schemas/api/schemas/stats/response/%/locations.metapack
./schemas/self/v1/schemas/api/schemas/stats/response/%/positions.metapack
./schemas/self/v1/schemas/api/schemas/stats/response/%/schema.metapack
./schemas/self/v1/schemas/api/schemas/stats/response/%/stats.metapack
./schemas/self/v1/schemas/api/schemas/stats/rpc
./schemas/self/v1/schemas/api/schemas/stats/rpc/%
./schemas/self/v1/schemas/api/schemas/stats/rpc/%/blaze-exhaustive.metapack
./schemas/self/v1/schemas/api/schemas/stats/rpc/%/blaze-fast.metapack
./schemas/self/v1/schemas/api/schemas/stats/rpc/%/bundle.metapack
./schemas/self/v1/schemas/api/schemas/stats/rpc/%/dependencies.metapack
./schemas/self/v1/schemas/api/schemas/stats/rpc/%/dependents.metapack
./schemas/self/v1/schemas/api/schemas/stats/rpc/%/editor.metapack
./schemas/self/v1/schemas/api/schemas/stats/rpc/%/health.metapack
./schemas/self/v1/schemas/api/schemas/stats/rpc/%/locations.metapack
./schemas/self/v1/schemas/api/schemas/stats/rpc/%/positions.metapack
./schemas/self/v1/schemas/api/schemas/stats/rpc/%/schema.metapack
./schemas/self/v1/schemas/api/schemas/stats/rpc/%/stats.metapack
./schemas/self/v1/schemas/api/schemas/trace
./schemas/self/v1/schemas/api/schemas/trace/request
./schemas/self/v1/schemas/api/schemas/trace/request/%
./schemas/self/v1/schemas/api/schemas/trace/request/%/blaze-exhaustive.metapack
./schemas/self/v1/schemas/api/schemas/trace/request/%/blaze-fast.metapack
./schemas/self/v1/schemas/api/schemas/trace/request/%/bundle.metapack
./schemas/self/v1/schemas/api/schemas/trace/request/%/dependencies.metapack
./schemas/self/v1/schemas/api/schemas/trace/request/%/dependents.metapack
./schemas/self/v1/schemas/api/schemas/trace/request/%/editor.metapack
./schemas/self/v1/schemas/api/schemas/trace/request/%/health.metapack
./schemas/self/v1/schemas/api/schemas/trace/request/%/locations.metapack
./schemas/self/v1/schemas/api/schemas/trace/request/%/positions.metapack
./schemas/self/v1/schemas/api/schemas/trace/request/%/schema.metapack
./schemas/self/v1/schemas/api/schemas/trace/request/%/stats.metapack
./schemas/self/v1/schemas/api/schemas/trace/response
./schemas/self/v1/schemas/api/schemas/trace/response/%
./schemas/self/v1/schemas/api/schemas/trace/response/%/blaze-exhaustive.metapack
./schemas/self/v1/schemas/api/schemas/trace/response/%/blaze-fast.metapack
./schemas/self/v1/schemas/api/schemas/trace/response/%/bundle.metapack
./schemas/self/v1/schemas/api/schemas/trace/response/%/dependencies.metapack
./schemas/self/v1/schemas/api/schemas/trace/response/%/dependents.metapack
./schemas/self/v1/schemas/api/schemas/trace/response/%/editor.metapack
./schemas/self/v1/schemas/api/schemas/trace/response/%/health.metapack
./schemas/self/v1/schemas/api/schemas/trace/response/%/locations.metapack
./schemas/self/v1/schemas/api/schemas/trace/response/%/positions.metapack
./schemas/self/v1/schemas/api/schemas/trace/response/%/schema.metapack
./schemas/self/v1/schemas/api/schemas/trace/response/%/stats.metapack
./schemas/self/v1/schemas/api/schemas/trace/rpc
./schemas/self/v1/schemas/api/schemas/trace/rpc/%
./schemas/self/v1/schemas/api/schemas/trace/rpc/%/blaze-exhaustive.metapack
./schemas/self/v1/schemas/api/schemas/trace/rpc/%/blaze-fast.metapack
./schemas/self/v1/schemas/api/schemas/trace/rpc/%/bundle.metapack
./schemas/self/v1/schemas/api/schemas/trace/rpc/%/dependencies.metapack
./schemas/self/v1/schemas/api/schemas/trace/rpc/%/dependents.metapack
./schemas/self/v1/schemas/api/schemas/trace/rpc/%/editor.metapack
./schemas/self/v1/schemas/api/schemas/trace/rpc/%/health.metapack
./schemas/self/v1/schemas/api/schemas/trace/rpc/%/locations.metapack
./schemas/self/v1/schemas/api/schemas/trace/rpc/%/positions.metapack
./schemas/self/v1/schemas/api/schemas/trace/rpc/%/schema.metapack
./schemas/self/v1/schemas/api/schemas/trace/rpc/%/stats.metapack
./schemas/self/v1/schemas/mcp
./schemas/self/v1/schemas/mcp/error
./schemas/self/v1/schemas/mcp/error/%
./schemas/self/v1/schemas/mcp/error/%/blaze-exhaustive.metapack
./schemas/self/v1/schemas/mcp/error/%/blaze-fast.metapack
./schemas/self/v1/schemas/mcp/error/%/bundle.metapack
./schemas/self/v1/schemas/mcp/error/%/dependencies.metapack
./schemas/self/v1/schemas/mcp/error/%/dependents.metapack
./schemas/self/v1/schemas/mcp/error/%/editor.metapack
./schemas/self/v1/schemas/mcp/error/%/health.metapack
./schemas/self/v1/schemas/mcp/error/%/locations.metapack
./schemas/self/v1/schemas/mcp/error/%/positions.metapack
./schemas/self/v1/schemas/mcp/error/%/schema.metapack
./schemas/self/v1/schemas/mcp/error/%/stats.metapack
./schemas/self/v1/schemas/mcp/initialize
./schemas/self/v1/schemas/mcp/initialize/request
./schemas/self/v1/schemas/mcp/initialize/request/%
./schemas/self/v1/schemas/mcp/initialize/request/%/blaze-exhaustive.metapack
./schemas/self/v1/schemas/mcp/initialize/request/%/blaze-fast.metapack
./schemas/self/v1/schemas/mcp/initialize/request/%/bundle.metapack
./schemas/self/v1/schemas/mcp/initialize/request/%/dependencies.metapack
./schemas/self/v1/schemas/mcp/initialize/request/%/dependents.metapack
./schemas/self/v1/schemas/mcp/initialize/request/%/editor.metapack
./schemas/self/v1/schemas/mcp/initialize/request/%/health.metapack
./schemas/self/v1/schemas/mcp/initialize/request/%/locations.metapack
./schemas/self/v1/schemas/mcp/initialize/request/%/positions.metapack
./schemas/self/v1/schemas/mcp/initialize/request/%/schema.metapack
./schemas/self/v1/schemas/mcp/initialize/request/%/stats.metapack
./schemas/self/v1/schemas/mcp/initialize/response
./schemas/self/v1/schemas/mcp/initialize/response/%
./schemas/self/v1/schemas/mcp/initialize/response/%/blaze-exhaustive.metapack
./schemas/self/v1/schemas/mcp/initialize/response/%/blaze-fast.metapack
./schemas/self/v1/schemas/mcp/initialize/response/%/bundle.metapack
./schemas/self/v1/schemas/mcp/initialize/response/%/dependencies.metapack
./schemas/self/v1/schemas/mcp/initialize/response/%/dependents.metapack
./schemas/self/v1/schemas/mcp/initialize/response/%/editor.metapack
./schemas/self/v1/schemas/mcp/initialize/response/%/health.metapack
./schemas/self/v1/schemas/mcp/initialize/response/%/locations.metapack
./schemas/self/v1/schemas/mcp/initialize/response/%/positions.metapack
./schemas/self/v1/schemas/mcp/initialize/response/%/schema.metapack
./schemas/self/v1/schemas/mcp/initialize/response/%/stats.metapack
./schemas/self/v1/schemas/mcp/notifications
./schemas/self/v1/schemas/mcp/notifications/cancelled
./schemas/self/v1/schemas/mcp/notifications/cancelled/%
./schemas/self/v1/schemas/mcp/notifications/cancelled/%/blaze-exhaustive.metapack
./schemas/self/v1/schemas/mcp/notifications/cancelled/%/blaze-fast.metapack
./schemas/self/v1/schemas/mcp/notifications/cancelled/%/bundle.metapack
./schemas/self/v1/schemas/mcp/notifications/cancelled/%/dependencies.metapack
./schemas/self/v1/schemas/mcp/notifications/cancelled/%/dependents.metapack
./schemas/self/v1/schemas/mcp/notifications/cancelled/%/editor.metapack
./schemas/self/v1/schemas/mcp/notifications/cancelled/%/health.metapack
./schemas/self/v1/schemas/mcp/notifications/cancelled/%/locations.metapack
./schemas/self/v1/schemas/mcp/notifications/cancelled/%/positions.metapack
./schemas/self/v1/schemas/mcp/notifications/cancelled/%/schema.metapack
./schemas/self/v1/schemas/mcp/notifications/cancelled/%/stats.metapack
./schemas/self/v1/schemas/mcp/notifications/initialized
./schemas/self/v1/schemas/mcp/notifications/initialized/%
./schemas/self/v1/schemas/mcp/notifications/initialized/%/blaze-exhaustive.metapack
./schemas/self/v1/schemas/mcp/notifications/initialized/%/blaze-fast.metapack
./schemas/self/v1/schemas/mcp/notifications/initialized/%/bundle.metapack
./schemas/self/v1/schemas/mcp/notifications/initialized/%/dependencies.metapack
./schemas/self/v1/schemas/mcp/notifications/initialized/%/dependents.metapack
./schemas/self/v1/schemas/mcp/notifications/initialized/%/editor.metapack
./schemas/self/v1/schemas/mcp/notifications/initialized/%/health.metapack
./schemas/self/v1/schemas/mcp/notifications/initialized/%/locations.metapack
./schemas/self/v1/schemas/mcp/notifications/initialized/%/positions.metapack
./schemas/self/v1/schemas/mcp/notifications/initialized/%/schema.metapack
./schemas/self/v1/schemas/mcp/notifications/initialized/%/stats.metapack
./schemas/self/v1/schemas/mcp/ping
./schemas/self/v1/schemas/mcp/ping/request
./schemas/self/v1/schemas/mcp/ping/request/%
./schemas/self/v1/schemas/mcp/ping/request/%/blaze-exhaustive.metapack
./schemas/self/v1/schemas/mcp/ping/request/%/blaze-fast.metapack
./schemas/self/v1/schemas/mcp/ping/request/%/bundle.metapack
./schemas/self/v1/schemas/mcp/ping/request/%/dependencies.metapack
./schemas/self/v1/schemas/mcp/ping/request/%/dependents.metapack
./schemas/self/v1/schemas/mcp/ping/request/%/editor.metapack
./schemas/self/v1/schemas/mcp/ping/request/%/health.metapack
./schemas/self/v1/schemas/mcp/ping/request/%/locations.metapack
./schemas/self/v1/schemas/mcp/ping/request/%/positions.metapack
./schemas/self/v1/schemas/mcp/ping/request/%/schema.metapack
./schemas/self/v1/schemas/mcp/ping/request/%/stats.metapack
./schemas/self/v1/schemas/mcp/ping/response
./schemas/self/v1/schemas/mcp/ping/response/%
./schemas/self/v1/schemas/mcp/ping/response/%/blaze-exhaustive.metapack
./schemas/self/v1/schemas/mcp/ping/response/%/blaze-fast.metapack
./schemas/self/v1/schemas/mcp/ping/response/%/bundle.metapack
./schemas/self/v1/schemas/mcp/ping/response/%/dependencies.metapack
./schemas/self/v1/schemas/mcp/ping/response/%/dependents.metapack
./schemas/self/v1/schemas/mcp/ping/response/%/editor.metapack
./schemas/self/v1/schemas/mcp/ping/response/%/health.metapack
./schemas/self/v1/schemas/mcp/ping/response/%/locations.metapack
./schemas/self/v1/schemas/mcp/ping/response/%/positions.metapack
./schemas/self/v1/schemas/mcp/ping/response/%/schema.metapack
./schemas/self/v1/schemas/mcp/ping/response/%/stats.metapack
./schemas/self/v1/schemas/mcp/request
./schemas/self/v1/schemas/mcp/request/%
./schemas/self/v1/schemas/mcp/request/%/blaze-exhaustive.metapack
./schemas/self/v1/schemas/mcp/request/%/blaze-fast.metapack
./schemas/self/v1/schemas/mcp/request/%/bundle.metapack
./schemas/self/v1/schemas/mcp/request/%/dependencies.metapack
./schemas/self/v1/schemas/mcp/request/%/dependents.metapack
./schemas/self/v1/schemas/mcp/request/%/editor.metapack
./schemas/self/v1/schemas/mcp/request/%/health.metapack
./schemas/self/v1/schemas/mcp/request/%/locations.metapack
./schemas/self/v1/schemas/mcp/request/%/positions.metapack
./schemas/self/v1/schemas/mcp/request/%/schema.metapack
./schemas/self/v1/schemas/mcp/request/%/stats.metapack
./schemas/self/v1/schemas/mcp/resources
./schemas/self/v1/schemas/mcp/resources/list
./schemas/self/v1/schemas/mcp/resources/list/request
./schemas/self/v1/schemas/mcp/resources/list/request/%
./schemas/self/v1/schemas/mcp/resources/list/request/%/blaze-exhaustive.metapack
./schemas/self/v1/schemas/mcp/resources/list/request/%/blaze-fast.metapack
./schemas/self/v1/schemas/mcp/resources/list/request/%/bundle.metapack
./schemas/self/v1/schemas/mcp/resources/list/request/%/dependencies.metapack
./schemas/self/v1/schemas/mcp/resources/list/request/%/dependents.metapack
./schemas/self/v1/schemas/mcp/resources/list/request/%/editor.metapack
./schemas/self/v1/schemas/mcp/resources/list/request/%/health.metapack
./schemas/self/v1/schemas/mcp/resources/list/request/%/locations.metapack
./schemas/self/v1/schemas/mcp/resources/list/request/%/positions.metapack
./schemas/self/v1/schemas/mcp/resources/list/request/%/schema.metapack
./schemas/self/v1/schemas/mcp/resources/list/request/%/stats.metapack
./schemas/self/v1/schemas/mcp/resources/list/response
./schemas/self/v1/schemas/mcp/resources/list/response/%
./schemas/self/v1/schemas/mcp/resources/list/response/%/blaze-exhaustive.metapack
./schemas/self/v1/schemas/mcp/resources/list/response/%/blaze-fast.metapack
./schemas/self/v1/schemas/mcp/resources/list/response/%/bundle.metapack
./schemas/self/v1/schemas/mcp/resources/list/response/%/dependencies.metapack
./schemas/self/v1/schemas/mcp/resources/list/response/%/dependents.metapack
./schemas/self/v1/schemas/mcp/resources/list/response/%/editor.metapack
./schemas/self/v1/schemas/mcp/resources/list/response/%/health.metapack
./schemas/self/v1/schemas/mcp/resources/list/response/%/locations.metapack
./schemas/self/v1/schemas/mcp/resources/list/response/%/positions.metapack
./schemas/self/v1/schemas/mcp/resources/list/response/%/schema.metapack
./schemas/self/v1/schemas/mcp/resources/list/response/%/stats.metapack
./schemas/self/v1/schemas/mcp/resources/read
./schemas/self/v1/schemas/mcp/resources/read/request
./schemas/self/v1/schemas/mcp/resources/read/request/%
./schemas/self/v1/schemas/mcp/resources/read/request/%/blaze-exhaustive.metapack
./schemas/self/v1/schemas/mcp/resources/read/request/%/blaze-fast.metapack
./schemas/self/v1/schemas/mcp/resources/read/request/%/bundle.metapack
./schemas/self/v1/schemas/mcp/resources/read/request/%/dependencies.metapack
./schemas/self/v1/schemas/mcp/resources/read/request/%/dependents.metapack
./schemas/self/v1/schemas/mcp/resources/read/request/%/editor.metapack
./schemas/self/v1/schemas/mcp/resources/read/request/%/health.metapack
./schemas/self/v1/schemas/mcp/resources/read/request/%/locations.metapack
./schemas/self/v1/schemas/mcp/resources/read/request/%/positions.metapack
./schemas/self/v1/schemas/mcp/resources/read/request/%/schema.metapack
./schemas/self/v1/schemas/mcp/resources/read/request/%/stats.metapack
./schemas/self/v1/schemas/mcp/resources/read/response
./schemas/self/v1/schemas/mcp/resources/read/response/%
./schemas/self/v1/schemas/mcp/resources/read/response/%/blaze-exhaustive.metapack
./schemas/self/v1/schemas/mcp/resources/read/response/%/blaze-fast.metapack
./schemas/self/v1/schemas/mcp/resources/read/response/%/bundle.metapack
./schemas/self/v1/schemas/mcp/resources/read/response/%/dependencies.metapack
./schemas/self/v1/schemas/mcp/resources/read/response/%/dependents.metapack
./schemas/self/v1/schemas/mcp/resources/read/response/%/editor.metapack
./schemas/self/v1/schemas/mcp/resources/read/response/%/health.metapack
./schemas/self/v1/schemas/mcp/resources/read/response/%/locations.metapack
./schemas/self/v1/schemas/mcp/resources/read/response/%/positions.metapack
./schemas/self/v1/schemas/mcp/resources/read/response/%/schema.metapack
./schemas/self/v1/schemas/mcp/resources/read/response/%/stats.metapack
./schemas/self/v1/schemas/mcp/resources/templates
./schemas/self/v1/schemas/mcp/resources/templates/list
./schemas/self/v1/schemas/mcp/resources/templates/list/request
./schemas/self/v1/schemas/mcp/resources/templates/list/request/%
./schemas/self/v1/schemas/mcp/resources/templates/list/request/%/blaze-exhaustive.metapack
./schemas/self/v1/schemas/mcp/resources/templates/list/request/%/blaze-fast.metapack
./schemas/self/v1/schemas/mcp/resources/templates/list/request/%/bundle.metapack
./schemas/self/v1/schemas/mcp/resources/templates/list/request/%/dependencies.metapack
./schemas/self/v1/schemas/mcp/resources/templates/list/request/%/dependents.metapack
./schemas/self/v1/schemas/mcp/resources/templates/list/request/%/editor.metapack
./schemas/self/v1/schemas/mcp/resources/templates/list/request/%/health.metapack
./schemas/self/v1/schemas/mcp/resources/templates/list/request/%/locations.metapack
./schemas/self/v1/schemas/mcp/resources/templates/list/request/%/positions.metapack
./schemas/self/v1/schemas/mcp/resources/templates/list/request/%/schema.metapack
./schemas/self/v1/schemas/mcp/resources/templates/list/request/%/stats.metapack
./schemas/self/v1/schemas/mcp/resources/templates/list/response
./schemas/self/v1/schemas/mcp/resources/templates/list/response/%
./schemas/self/v1/schemas/mcp/resources/templates/list/response/%/blaze-exhaustive.metapack
./schemas/self/v1/schemas/mcp/resources/templates/list/response/%/blaze-fast.metapack
./schemas/self/v1/schemas/mcp/resources/templates/list/response/%/bundle.metapack
./schemas/self/v1/schemas/mcp/resources/templates/list/response/%/dependencies.metapack
./schemas/self/v1/schemas/mcp/resources/templates/list/response/%/dependents.metapack
./schemas/self/v1/schemas/mcp/resources/templates/list/response/%/editor.metapack
./schemas/self/v1/schemas/mcp/resources/templates/list/response/%/health.metapack
./schemas/self/v1/schemas/mcp/resources/templates/list/response/%/locations.metapack
./schemas/self/v1/schemas/mcp/resources/templates/list/response/%/positions.metapack
./schemas/self/v1/schemas/mcp/resources/templates/list/response/%/schema.metapack
./schemas/self/v1/schemas/mcp/resources/templates/list/response/%/stats.metapack
./schemas/self/v1/schemas/mcp/response
./schemas/self/v1/schemas/mcp/response/%
./schemas/self/v1/schemas/mcp/response/%/blaze-exhaustive.metapack
./schemas/self/v1/schemas/mcp/response/%/blaze-fast.metapack
./schemas/self/v1/schemas/mcp/response/%/bundle.metapack
./schemas/self/v1/schemas/mcp/response/%/dependencies.metapack
./schemas/self/v1/schemas/mcp/response/%/dependents.metapack
./schemas/self/v1/schemas/mcp/response/%/editor.metapack
./schemas/self/v1/schemas/mcp/response/%/health.metapack
./schemas/self/v1/schemas/mcp/response/%/locations.metapack
./schemas/self/v1/schemas/mcp/response/%/positions.metapack
./schemas/self/v1/schemas/mcp/response/%/schema.metapack
./schemas/self/v1/schemas/mcp/response/%/stats.metapack
./state.bin
./version.json
EOF
diff "$TMP/manifest.txt" "$TMP/expected_manifest.txt"
