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
          "path": "."
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
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/response.json (#22)
(  4%) Resolving: self/v1/schemas/api/error.json
(  9%) Resolving: self/v1/schemas/api/list/response.json
( 13%) Resolving: self/v1/schemas/api/schemas/dependencies/response.json
( 18%) Resolving: self/v1/schemas/api/schemas/dependents/response.json
( 22%) Resolving: self/v1/schemas/api/schemas/evaluate/response.json
( 27%) Resolving: self/v1/schemas/api/schemas/health/response.json
( 31%) Resolving: self/v1/schemas/api/schemas/locations/response.json
( 36%) Resolving: self/v1/schemas/api/schemas/metadata/response.json
( 40%) Resolving: self/v1/schemas/api/schemas/position.json
( 45%) Resolving: self/v1/schemas/api/schemas/positions/response.json
( 50%) Resolving: self/v1/schemas/api/schemas/search/response.json
( 54%) Resolving: self/v1/schemas/api/schemas/stats/response.json
( 59%) Resolving: self/v1/schemas/api/schemas/trace/response.json
( 63%) Resolving: self/v1/schemas/mcp/error.json
( 68%) Resolving: self/v1/schemas/mcp/initialize/request.json
( 72%) Resolving: self/v1/schemas/mcp/initialize/response.json
( 77%) Resolving: self/v1/schemas/mcp/notifications/cancelled.json
( 81%) Resolving: self/v1/schemas/mcp/notifications/initialized.json
( 86%) Resolving: self/v1/schemas/mcp/ping/request.json
( 90%) Resolving: self/v1/schemas/mcp/ping/response.json
( 95%) Resolving: self/v1/schemas/mcp/request.json
(100%) Resolving: self/v1/schemas/mcp/response.json
(  0%) Producing: configuration.json
(  0%) Producing: metadata.json
(  0%) Producing: version.json
(  1%) Producing: explorer/%/404.metapack
(  1%) Producing: schemas/self/v1/schemas/api/error/%/schema.metapack
(  1%) Producing: schemas/self/v1/schemas/api/list/response/%/schema.metapack
(  2%) Producing: schemas/self/v1/schemas/api/schemas/dependencies/response/%/schema.metapack
(  2%) Producing: schemas/self/v1/schemas/api/schemas/dependents/response/%/schema.metapack
(  2%) Producing: schemas/self/v1/schemas/api/schemas/evaluate/response/%/schema.metapack
(  3%) Producing: schemas/self/v1/schemas/api/schemas/health/response/%/schema.metapack
(  3%) Producing: schemas/self/v1/schemas/api/schemas/locations/response/%/schema.metapack
(  3%) Producing: schemas/self/v1/schemas/api/schemas/metadata/response/%/schema.metapack
(  4%) Producing: schemas/self/v1/schemas/api/schemas/position/%/schema.metapack
(  4%) Producing: schemas/self/v1/schemas/api/schemas/positions/response/%/schema.metapack
(  4%) Producing: schemas/self/v1/schemas/api/schemas/search/response/%/schema.metapack
(  5%) Producing: schemas/self/v1/schemas/api/schemas/stats/response/%/schema.metapack
(  5%) Producing: schemas/self/v1/schemas/api/schemas/trace/response/%/schema.metapack
(  5%) Producing: schemas/self/v1/schemas/mcp/error/%/schema.metapack
(  6%) Producing: schemas/self/v1/schemas/mcp/initialize/request/%/schema.metapack
(  6%) Producing: schemas/self/v1/schemas/mcp/initialize/response/%/schema.metapack
(  6%) Producing: schemas/self/v1/schemas/mcp/notifications/cancelled/%/schema.metapack
(  7%) Producing: schemas/self/v1/schemas/mcp/notifications/initialized/%/schema.metapack
(  7%) Producing: schemas/self/v1/schemas/mcp/ping/request/%/schema.metapack
(  7%) Producing: schemas/self/v1/schemas/mcp/ping/response/%/schema.metapack
(  8%) Producing: schemas/self/v1/schemas/mcp/request/%/schema.metapack
(  8%) Producing: schemas/self/v1/schemas/mcp/response/%/schema.metapack
(  8%) Producing: schemas/self/v1/schemas/api/error/%/dependencies.metapack
(  8%) Producing: schemas/self/v1/schemas/api/error/%/locations.metapack
(  9%) Producing: schemas/self/v1/schemas/api/error/%/positions.metapack
(  9%) Producing: schemas/self/v1/schemas/api/error/%/stats.metapack
(  9%) Producing: schemas/self/v1/schemas/api/list/response/%/dependencies.metapack
( 10%) Producing: schemas/self/v1/schemas/api/list/response/%/locations.metapack
( 10%) Producing: schemas/self/v1/schemas/api/list/response/%/positions.metapack
( 10%) Producing: schemas/self/v1/schemas/api/list/response/%/stats.metapack
( 11%) Producing: schemas/self/v1/schemas/api/schemas/dependencies/response/%/dependencies.metapack
( 11%) Producing: schemas/self/v1/schemas/api/schemas/dependencies/response/%/locations.metapack
( 11%) Producing: schemas/self/v1/schemas/api/schemas/dependencies/response/%/positions.metapack
( 12%) Producing: schemas/self/v1/schemas/api/schemas/dependencies/response/%/stats.metapack
( 12%) Producing: schemas/self/v1/schemas/api/schemas/dependents/response/%/dependencies.metapack
( 12%) Producing: schemas/self/v1/schemas/api/schemas/dependents/response/%/locations.metapack
( 13%) Producing: schemas/self/v1/schemas/api/schemas/dependents/response/%/positions.metapack
( 13%) Producing: schemas/self/v1/schemas/api/schemas/dependents/response/%/stats.metapack
( 13%) Producing: schemas/self/v1/schemas/api/schemas/evaluate/response/%/dependencies.metapack
( 14%) Producing: schemas/self/v1/schemas/api/schemas/evaluate/response/%/locations.metapack
( 14%) Producing: schemas/self/v1/schemas/api/schemas/evaluate/response/%/positions.metapack
( 14%) Producing: schemas/self/v1/schemas/api/schemas/evaluate/response/%/stats.metapack
( 15%) Producing: schemas/self/v1/schemas/api/schemas/health/response/%/dependencies.metapack
( 15%) Producing: schemas/self/v1/schemas/api/schemas/health/response/%/locations.metapack
( 15%) Producing: schemas/self/v1/schemas/api/schemas/health/response/%/positions.metapack
( 16%) Producing: schemas/self/v1/schemas/api/schemas/health/response/%/stats.metapack
( 16%) Producing: schemas/self/v1/schemas/api/schemas/locations/response/%/dependencies.metapack
( 16%) Producing: schemas/self/v1/schemas/api/schemas/locations/response/%/locations.metapack
( 16%) Producing: schemas/self/v1/schemas/api/schemas/locations/response/%/positions.metapack
( 17%) Producing: schemas/self/v1/schemas/api/schemas/locations/response/%/stats.metapack
( 17%) Producing: schemas/self/v1/schemas/api/schemas/metadata/response/%/dependencies.metapack
( 17%) Producing: schemas/self/v1/schemas/api/schemas/metadata/response/%/locations.metapack
( 18%) Producing: schemas/self/v1/schemas/api/schemas/metadata/response/%/positions.metapack
( 18%) Producing: schemas/self/v1/schemas/api/schemas/metadata/response/%/stats.metapack
( 18%) Producing: schemas/self/v1/schemas/api/schemas/position/%/dependencies.metapack
( 19%) Producing: schemas/self/v1/schemas/api/schemas/position/%/locations.metapack
( 19%) Producing: schemas/self/v1/schemas/api/schemas/position/%/positions.metapack
( 19%) Producing: schemas/self/v1/schemas/api/schemas/position/%/stats.metapack
( 20%) Producing: schemas/self/v1/schemas/api/schemas/positions/response/%/dependencies.metapack
( 20%) Producing: schemas/self/v1/schemas/api/schemas/positions/response/%/locations.metapack
( 20%) Producing: schemas/self/v1/schemas/api/schemas/positions/response/%/positions.metapack
( 21%) Producing: schemas/self/v1/schemas/api/schemas/positions/response/%/stats.metapack
( 21%) Producing: schemas/self/v1/schemas/api/schemas/search/response/%/dependencies.metapack
( 21%) Producing: schemas/self/v1/schemas/api/schemas/search/response/%/locations.metapack
( 22%) Producing: schemas/self/v1/schemas/api/schemas/search/response/%/positions.metapack
( 22%) Producing: schemas/self/v1/schemas/api/schemas/search/response/%/stats.metapack
( 22%) Producing: schemas/self/v1/schemas/api/schemas/stats/response/%/dependencies.metapack
( 23%) Producing: schemas/self/v1/schemas/api/schemas/stats/response/%/locations.metapack
( 23%) Producing: schemas/self/v1/schemas/api/schemas/stats/response/%/positions.metapack
( 23%) Producing: schemas/self/v1/schemas/api/schemas/stats/response/%/stats.metapack
( 24%) Producing: schemas/self/v1/schemas/api/schemas/trace/response/%/dependencies.metapack
( 24%) Producing: schemas/self/v1/schemas/api/schemas/trace/response/%/locations.metapack
( 24%) Producing: schemas/self/v1/schemas/api/schemas/trace/response/%/positions.metapack
( 25%) Producing: schemas/self/v1/schemas/api/schemas/trace/response/%/stats.metapack
( 25%) Producing: schemas/self/v1/schemas/mcp/error/%/dependencies.metapack
( 25%) Producing: schemas/self/v1/schemas/mcp/error/%/locations.metapack
( 25%) Producing: schemas/self/v1/schemas/mcp/error/%/positions.metapack
( 26%) Producing: schemas/self/v1/schemas/mcp/error/%/stats.metapack
( 26%) Producing: schemas/self/v1/schemas/mcp/initialize/request/%/dependencies.metapack
( 26%) Producing: schemas/self/v1/schemas/mcp/initialize/request/%/locations.metapack
( 27%) Producing: schemas/self/v1/schemas/mcp/initialize/request/%/positions.metapack
( 27%) Producing: schemas/self/v1/schemas/mcp/initialize/request/%/stats.metapack
( 27%) Producing: schemas/self/v1/schemas/mcp/initialize/response/%/dependencies.metapack
( 28%) Producing: schemas/self/v1/schemas/mcp/initialize/response/%/locations.metapack
( 28%) Producing: schemas/self/v1/schemas/mcp/initialize/response/%/positions.metapack
( 28%) Producing: schemas/self/v1/schemas/mcp/initialize/response/%/stats.metapack
( 29%) Producing: schemas/self/v1/schemas/mcp/notifications/cancelled/%/dependencies.metapack
( 29%) Producing: schemas/self/v1/schemas/mcp/notifications/cancelled/%/locations.metapack
( 29%) Producing: schemas/self/v1/schemas/mcp/notifications/cancelled/%/positions.metapack
( 30%) Producing: schemas/self/v1/schemas/mcp/notifications/cancelled/%/stats.metapack
( 30%) Producing: schemas/self/v1/schemas/mcp/notifications/initialized/%/dependencies.metapack
( 30%) Producing: schemas/self/v1/schemas/mcp/notifications/initialized/%/locations.metapack
( 31%) Producing: schemas/self/v1/schemas/mcp/notifications/initialized/%/positions.metapack
( 31%) Producing: schemas/self/v1/schemas/mcp/notifications/initialized/%/stats.metapack
( 31%) Producing: schemas/self/v1/schemas/mcp/ping/request/%/dependencies.metapack
( 32%) Producing: schemas/self/v1/schemas/mcp/ping/request/%/locations.metapack
( 32%) Producing: schemas/self/v1/schemas/mcp/ping/request/%/positions.metapack
( 32%) Producing: schemas/self/v1/schemas/mcp/ping/request/%/stats.metapack
( 33%) Producing: schemas/self/v1/schemas/mcp/ping/response/%/dependencies.metapack
( 33%) Producing: schemas/self/v1/schemas/mcp/ping/response/%/locations.metapack
( 33%) Producing: schemas/self/v1/schemas/mcp/ping/response/%/positions.metapack
( 33%) Producing: schemas/self/v1/schemas/mcp/ping/response/%/stats.metapack
( 34%) Producing: schemas/self/v1/schemas/mcp/request/%/dependencies.metapack
( 34%) Producing: schemas/self/v1/schemas/mcp/request/%/locations.metapack
( 34%) Producing: schemas/self/v1/schemas/mcp/request/%/positions.metapack
( 35%) Producing: schemas/self/v1/schemas/mcp/request/%/stats.metapack
( 35%) Producing: schemas/self/v1/schemas/mcp/response/%/dependencies.metapack
( 35%) Producing: schemas/self/v1/schemas/mcp/response/%/locations.metapack
( 36%) Producing: schemas/self/v1/schemas/mcp/response/%/positions.metapack
( 36%) Producing: schemas/self/v1/schemas/mcp/response/%/stats.metapack
( 36%) Producing: schemas/self/v1/schemas/api/error/%/bundle.metapack
( 37%) Producing: schemas/self/v1/schemas/api/error/%/health.metapack
( 37%) Producing: schemas/self/v1/schemas/api/list/response/%/bundle.metapack
( 37%) Producing: schemas/self/v1/schemas/api/list/response/%/health.metapack
( 38%) Producing: schemas/self/v1/schemas/api/schemas/dependencies/response/%/bundle.metapack
( 38%) Producing: schemas/self/v1/schemas/api/schemas/dependencies/response/%/health.metapack
( 38%) Producing: schemas/self/v1/schemas/api/schemas/dependents/response/%/bundle.metapack
( 39%) Producing: schemas/self/v1/schemas/api/schemas/dependents/response/%/health.metapack
( 39%) Producing: schemas/self/v1/schemas/api/schemas/evaluate/response/%/bundle.metapack
( 39%) Producing: schemas/self/v1/schemas/api/schemas/evaluate/response/%/health.metapack
( 40%) Producing: schemas/self/v1/schemas/api/schemas/health/response/%/bundle.metapack
( 40%) Producing: schemas/self/v1/schemas/api/schemas/health/response/%/health.metapack
( 40%) Producing: schemas/self/v1/schemas/api/schemas/locations/response/%/bundle.metapack
( 41%) Producing: schemas/self/v1/schemas/api/schemas/locations/response/%/health.metapack
( 41%) Producing: schemas/self/v1/schemas/api/schemas/metadata/response/%/bundle.metapack
( 41%) Producing: schemas/self/v1/schemas/api/schemas/metadata/response/%/health.metapack
( 41%) Producing: schemas/self/v1/schemas/api/schemas/position/%/bundle.metapack
( 42%) Producing: schemas/self/v1/schemas/api/schemas/position/%/health.metapack
( 42%) Producing: schemas/self/v1/schemas/api/schemas/positions/response/%/bundle.metapack
( 42%) Producing: schemas/self/v1/schemas/api/schemas/positions/response/%/health.metapack
( 43%) Producing: schemas/self/v1/schemas/api/schemas/search/response/%/bundle.metapack
( 43%) Producing: schemas/self/v1/schemas/api/schemas/search/response/%/health.metapack
( 43%) Producing: schemas/self/v1/schemas/api/schemas/stats/response/%/bundle.metapack
( 44%) Producing: schemas/self/v1/schemas/api/schemas/stats/response/%/health.metapack
( 44%) Producing: schemas/self/v1/schemas/api/schemas/trace/response/%/bundle.metapack
( 44%) Producing: schemas/self/v1/schemas/api/schemas/trace/response/%/health.metapack
( 45%) Producing: schemas/self/v1/schemas/mcp/error/%/bundle.metapack
( 45%) Producing: schemas/self/v1/schemas/mcp/error/%/health.metapack
( 45%) Producing: schemas/self/v1/schemas/mcp/initialize/request/%/bundle.metapack
( 46%) Producing: schemas/self/v1/schemas/mcp/initialize/request/%/health.metapack
( 46%) Producing: schemas/self/v1/schemas/mcp/initialize/response/%/bundle.metapack
( 46%) Producing: schemas/self/v1/schemas/mcp/initialize/response/%/health.metapack
( 47%) Producing: schemas/self/v1/schemas/mcp/notifications/cancelled/%/bundle.metapack
( 47%) Producing: schemas/self/v1/schemas/mcp/notifications/cancelled/%/health.metapack
( 47%) Producing: schemas/self/v1/schemas/mcp/notifications/initialized/%/bundle.metapack
( 48%) Producing: schemas/self/v1/schemas/mcp/notifications/initialized/%/health.metapack
( 48%) Producing: schemas/self/v1/schemas/mcp/ping/request/%/bundle.metapack
( 48%) Producing: schemas/self/v1/schemas/mcp/ping/request/%/health.metapack
( 49%) Producing: schemas/self/v1/schemas/mcp/ping/response/%/bundle.metapack
( 49%) Producing: schemas/self/v1/schemas/mcp/ping/response/%/health.metapack
( 49%) Producing: schemas/self/v1/schemas/mcp/request/%/bundle.metapack
( 50%) Producing: schemas/self/v1/schemas/mcp/request/%/health.metapack
( 50%) Producing: schemas/self/v1/schemas/mcp/response/%/bundle.metapack
( 50%) Producing: schemas/self/v1/schemas/mcp/response/%/health.metapack
( 50%) Producing: explorer/self/v1/schemas/api/error/%/schema.metapack
( 51%) Producing: explorer/self/v1/schemas/api/list/response/%/schema.metapack
( 51%) Producing: explorer/self/v1/schemas/api/schemas/dependencies/response/%/schema.metapack
( 51%) Producing: explorer/self/v1/schemas/api/schemas/dependents/response/%/schema.metapack
( 52%) Producing: explorer/self/v1/schemas/api/schemas/evaluate/response/%/schema.metapack
( 52%) Producing: explorer/self/v1/schemas/api/schemas/health/response/%/schema.metapack
( 52%) Producing: explorer/self/v1/schemas/api/schemas/locations/response/%/schema.metapack
( 53%) Producing: explorer/self/v1/schemas/api/schemas/metadata/response/%/schema.metapack
( 53%) Producing: explorer/self/v1/schemas/api/schemas/position/%/schema.metapack
( 53%) Producing: explorer/self/v1/schemas/api/schemas/positions/response/%/schema.metapack
( 54%) Producing: explorer/self/v1/schemas/api/schemas/search/response/%/schema.metapack
( 54%) Producing: explorer/self/v1/schemas/api/schemas/stats/response/%/schema.metapack
( 54%) Producing: explorer/self/v1/schemas/api/schemas/trace/response/%/schema.metapack
( 55%) Producing: explorer/self/v1/schemas/mcp/error/%/schema.metapack
( 55%) Producing: explorer/self/v1/schemas/mcp/initialize/request/%/schema.metapack
( 55%) Producing: explorer/self/v1/schemas/mcp/initialize/response/%/schema.metapack
( 56%) Producing: explorer/self/v1/schemas/mcp/notifications/cancelled/%/schema.metapack
( 56%) Producing: explorer/self/v1/schemas/mcp/notifications/initialized/%/schema.metapack
( 56%) Producing: explorer/self/v1/schemas/mcp/ping/request/%/schema.metapack
( 57%) Producing: explorer/self/v1/schemas/mcp/ping/response/%/schema.metapack
( 57%) Producing: explorer/self/v1/schemas/mcp/request/%/schema.metapack
( 57%) Producing: explorer/self/v1/schemas/mcp/response/%/schema.metapack
( 58%) Producing: schemas/self/v1/schemas/api/error/%/blaze-exhaustive.metapack
( 58%) Producing: schemas/self/v1/schemas/api/error/%/blaze-fast.metapack
( 58%) Producing: schemas/self/v1/schemas/api/error/%/editor.metapack
( 58%) Producing: schemas/self/v1/schemas/api/list/response/%/blaze-exhaustive.metapack
( 59%) Producing: schemas/self/v1/schemas/api/list/response/%/blaze-fast.metapack
( 59%) Producing: schemas/self/v1/schemas/api/list/response/%/editor.metapack
( 59%) Producing: schemas/self/v1/schemas/api/schemas/dependencies/response/%/blaze-exhaustive.metapack
( 60%) Producing: schemas/self/v1/schemas/api/schemas/dependencies/response/%/blaze-fast.metapack
( 60%) Producing: schemas/self/v1/schemas/api/schemas/dependencies/response/%/editor.metapack
( 60%) Producing: schemas/self/v1/schemas/api/schemas/dependents/response/%/blaze-exhaustive.metapack
( 61%) Producing: schemas/self/v1/schemas/api/schemas/dependents/response/%/blaze-fast.metapack
( 61%) Producing: schemas/self/v1/schemas/api/schemas/dependents/response/%/editor.metapack
( 61%) Producing: schemas/self/v1/schemas/api/schemas/evaluate/response/%/blaze-exhaustive.metapack
( 62%) Producing: schemas/self/v1/schemas/api/schemas/evaluate/response/%/blaze-fast.metapack
( 62%) Producing: schemas/self/v1/schemas/api/schemas/evaluate/response/%/editor.metapack
( 62%) Producing: schemas/self/v1/schemas/api/schemas/health/response/%/blaze-exhaustive.metapack
( 63%) Producing: schemas/self/v1/schemas/api/schemas/health/response/%/blaze-fast.metapack
( 63%) Producing: schemas/self/v1/schemas/api/schemas/health/response/%/editor.metapack
( 63%) Producing: schemas/self/v1/schemas/api/schemas/locations/response/%/blaze-exhaustive.metapack
( 64%) Producing: schemas/self/v1/schemas/api/schemas/locations/response/%/blaze-fast.metapack
( 64%) Producing: schemas/self/v1/schemas/api/schemas/locations/response/%/editor.metapack
( 64%) Producing: schemas/self/v1/schemas/api/schemas/metadata/response/%/blaze-exhaustive.metapack
( 65%) Producing: schemas/self/v1/schemas/api/schemas/metadata/response/%/blaze-fast.metapack
( 65%) Producing: schemas/self/v1/schemas/api/schemas/metadata/response/%/editor.metapack
( 65%) Producing: schemas/self/v1/schemas/api/schemas/position/%/blaze-exhaustive.metapack
( 66%) Producing: schemas/self/v1/schemas/api/schemas/position/%/blaze-fast.metapack
( 66%) Producing: schemas/self/v1/schemas/api/schemas/position/%/editor.metapack
( 66%) Producing: schemas/self/v1/schemas/api/schemas/positions/response/%/blaze-exhaustive.metapack
( 66%) Producing: schemas/self/v1/schemas/api/schemas/positions/response/%/blaze-fast.metapack
( 67%) Producing: schemas/self/v1/schemas/api/schemas/positions/response/%/editor.metapack
( 67%) Producing: schemas/self/v1/schemas/api/schemas/search/response/%/blaze-exhaustive.metapack
( 67%) Producing: schemas/self/v1/schemas/api/schemas/search/response/%/blaze-fast.metapack
( 68%) Producing: schemas/self/v1/schemas/api/schemas/search/response/%/editor.metapack
( 68%) Producing: schemas/self/v1/schemas/api/schemas/stats/response/%/blaze-exhaustive.metapack
( 68%) Producing: schemas/self/v1/schemas/api/schemas/stats/response/%/blaze-fast.metapack
( 69%) Producing: schemas/self/v1/schemas/api/schemas/stats/response/%/editor.metapack
( 69%) Producing: schemas/self/v1/schemas/api/schemas/trace/response/%/blaze-exhaustive.metapack
( 69%) Producing: schemas/self/v1/schemas/api/schemas/trace/response/%/blaze-fast.metapack
( 70%) Producing: schemas/self/v1/schemas/api/schemas/trace/response/%/editor.metapack
( 70%) Producing: schemas/self/v1/schemas/mcp/error/%/blaze-exhaustive.metapack
( 70%) Producing: schemas/self/v1/schemas/mcp/error/%/blaze-fast.metapack
( 71%) Producing: schemas/self/v1/schemas/mcp/error/%/editor.metapack
( 71%) Producing: schemas/self/v1/schemas/mcp/initialize/request/%/blaze-exhaustive.metapack
( 71%) Producing: schemas/self/v1/schemas/mcp/initialize/request/%/blaze-fast.metapack
( 72%) Producing: schemas/self/v1/schemas/mcp/initialize/request/%/editor.metapack
( 72%) Producing: schemas/self/v1/schemas/mcp/initialize/response/%/blaze-exhaustive.metapack
( 72%) Producing: schemas/self/v1/schemas/mcp/initialize/response/%/blaze-fast.metapack
( 73%) Producing: schemas/self/v1/schemas/mcp/initialize/response/%/editor.metapack
( 73%) Producing: schemas/self/v1/schemas/mcp/notifications/cancelled/%/blaze-exhaustive.metapack
( 73%) Producing: schemas/self/v1/schemas/mcp/notifications/cancelled/%/blaze-fast.metapack
( 74%) Producing: schemas/self/v1/schemas/mcp/notifications/cancelled/%/editor.metapack
( 74%) Producing: schemas/self/v1/schemas/mcp/notifications/initialized/%/blaze-exhaustive.metapack
( 74%) Producing: schemas/self/v1/schemas/mcp/notifications/initialized/%/blaze-fast.metapack
( 75%) Producing: schemas/self/v1/schemas/mcp/notifications/initialized/%/editor.metapack
( 75%) Producing: schemas/self/v1/schemas/mcp/ping/request/%/blaze-exhaustive.metapack
( 75%) Producing: schemas/self/v1/schemas/mcp/ping/request/%/blaze-fast.metapack
( 75%) Producing: schemas/self/v1/schemas/mcp/ping/request/%/editor.metapack
( 76%) Producing: schemas/self/v1/schemas/mcp/ping/response/%/blaze-exhaustive.metapack
( 76%) Producing: schemas/self/v1/schemas/mcp/ping/response/%/blaze-fast.metapack
( 76%) Producing: schemas/self/v1/schemas/mcp/ping/response/%/editor.metapack
( 77%) Producing: schemas/self/v1/schemas/mcp/request/%/blaze-exhaustive.metapack
( 77%) Producing: schemas/self/v1/schemas/mcp/request/%/blaze-fast.metapack
( 77%) Producing: schemas/self/v1/schemas/mcp/request/%/editor.metapack
( 78%) Producing: schemas/self/v1/schemas/mcp/response/%/blaze-exhaustive.metapack
( 78%) Producing: schemas/self/v1/schemas/mcp/response/%/blaze-fast.metapack
( 78%) Producing: schemas/self/v1/schemas/mcp/response/%/editor.metapack
( 79%) Producing: explorer/self/v1/schemas/api/error/%/schema-html.metapack
( 79%) Producing: explorer/self/v1/schemas/api/list/%/directory.metapack
( 79%) Producing: explorer/self/v1/schemas/api/list/response/%/schema-html.metapack
( 80%) Producing: explorer/self/v1/schemas/api/schemas/dependencies/%/directory.metapack
( 80%) Producing: explorer/self/v1/schemas/api/schemas/dependencies/response/%/schema-html.metapack
( 80%) Producing: explorer/self/v1/schemas/api/schemas/dependents/%/directory.metapack
( 81%) Producing: explorer/self/v1/schemas/api/schemas/dependents/response/%/schema-html.metapack
( 81%) Producing: explorer/self/v1/schemas/api/schemas/evaluate/%/directory.metapack
( 81%) Producing: explorer/self/v1/schemas/api/schemas/evaluate/response/%/schema-html.metapack
( 82%) Producing: explorer/self/v1/schemas/api/schemas/health/%/directory.metapack
( 82%) Producing: explorer/self/v1/schemas/api/schemas/health/response/%/schema-html.metapack
( 82%) Producing: explorer/self/v1/schemas/api/schemas/locations/%/directory.metapack
( 83%) Producing: explorer/self/v1/schemas/api/schemas/locations/response/%/schema-html.metapack
( 83%) Producing: explorer/self/v1/schemas/api/schemas/metadata/%/directory.metapack
( 83%) Producing: explorer/self/v1/schemas/api/schemas/metadata/response/%/schema-html.metapack
( 83%) Producing: explorer/self/v1/schemas/api/schemas/position/%/schema-html.metapack
( 84%) Producing: explorer/self/v1/schemas/api/schemas/positions/%/directory.metapack
( 84%) Producing: explorer/self/v1/schemas/api/schemas/positions/response/%/schema-html.metapack
( 84%) Producing: explorer/self/v1/schemas/api/schemas/search/%/directory.metapack
( 85%) Producing: explorer/self/v1/schemas/api/schemas/search/response/%/schema-html.metapack
( 85%) Producing: explorer/self/v1/schemas/api/schemas/stats/%/directory.metapack
( 85%) Producing: explorer/self/v1/schemas/api/schemas/stats/response/%/schema-html.metapack
( 86%) Producing: explorer/self/v1/schemas/api/schemas/trace/%/directory.metapack
( 86%) Producing: explorer/self/v1/schemas/api/schemas/trace/response/%/schema-html.metapack
( 86%) Producing: explorer/self/v1/schemas/mcp/error/%/schema-html.metapack
( 87%) Producing: explorer/self/v1/schemas/mcp/initialize/%/directory.metapack
( 87%) Producing: explorer/self/v1/schemas/mcp/initialize/request/%/schema-html.metapack
( 87%) Producing: explorer/self/v1/schemas/mcp/initialize/response/%/schema-html.metapack
( 88%) Producing: explorer/self/v1/schemas/mcp/notifications/%/directory.metapack
( 88%) Producing: explorer/self/v1/schemas/mcp/notifications/cancelled/%/schema-html.metapack
( 88%) Producing: explorer/self/v1/schemas/mcp/notifications/initialized/%/schema-html.metapack
( 89%) Producing: explorer/self/v1/schemas/mcp/ping/%/directory.metapack
( 89%) Producing: explorer/self/v1/schemas/mcp/ping/request/%/schema-html.metapack
( 89%) Producing: explorer/self/v1/schemas/mcp/ping/response/%/schema-html.metapack
( 90%) Producing: explorer/self/v1/schemas/mcp/request/%/schema-html.metapack
( 90%) Producing: explorer/self/v1/schemas/mcp/response/%/schema-html.metapack
( 90%) Producing: explorer/self/v1/schemas/api/list/%/directory-html.metapack
( 91%) Producing: explorer/self/v1/schemas/api/schemas/%/directory.metapack
( 91%) Producing: explorer/self/v1/schemas/api/schemas/dependencies/%/directory-html.metapack
( 91%) Producing: explorer/self/v1/schemas/api/schemas/dependents/%/directory-html.metapack
( 91%) Producing: explorer/self/v1/schemas/api/schemas/evaluate/%/directory-html.metapack
( 92%) Producing: explorer/self/v1/schemas/api/schemas/health/%/directory-html.metapack
( 92%) Producing: explorer/self/v1/schemas/api/schemas/locations/%/directory-html.metapack
( 92%) Producing: explorer/self/v1/schemas/api/schemas/metadata/%/directory-html.metapack
( 93%) Producing: explorer/self/v1/schemas/api/schemas/positions/%/directory-html.metapack
( 93%) Producing: explorer/self/v1/schemas/api/schemas/search/%/directory-html.metapack
( 93%) Producing: explorer/self/v1/schemas/api/schemas/stats/%/directory-html.metapack
( 94%) Producing: explorer/self/v1/schemas/api/schemas/trace/%/directory-html.metapack
( 94%) Producing: explorer/self/v1/schemas/mcp/%/directory.metapack
( 94%) Producing: explorer/self/v1/schemas/mcp/initialize/%/directory-html.metapack
( 95%) Producing: explorer/self/v1/schemas/mcp/notifications/%/directory-html.metapack
( 95%) Producing: explorer/self/v1/schemas/mcp/ping/%/directory-html.metapack
( 95%) Producing: explorer/self/v1/schemas/api/%/directory.metapack
( 96%) Producing: explorer/self/v1/schemas/api/schemas/%/directory-html.metapack
( 96%) Producing: explorer/self/v1/schemas/mcp/%/directory-html.metapack
( 96%) Producing: explorer/self/v1/schemas/%/directory.metapack
( 97%) Producing: explorer/self/v1/schemas/api/%/directory-html.metapack
( 97%) Producing: explorer/self/v1/%/directory.metapack
( 97%) Producing: explorer/self/v1/schemas/%/directory-html.metapack
( 98%) Producing: explorer/self/%/directory.metapack
( 98%) Producing: explorer/self/v1/%/directory-html.metapack
( 98%) Producing: explorer/%/directory.metapack
( 99%) Producing: explorer/self/%/directory-html.metapack
( 99%) Producing: explorer/%/directory-html.metapack
( 99%) Producing: explorer/%/search.metapack
(100%) Producing: routes.bin
(  4%) Combining: schemas/self/v1/schemas/api/error/%/dependents.metapack
(  9%) Combining: schemas/self/v1/schemas/api/list/response/%/dependents.metapack
( 13%) Combining: schemas/self/v1/schemas/api/schemas/dependencies/response/%/dependents.metapack
( 18%) Combining: schemas/self/v1/schemas/api/schemas/dependents/response/%/dependents.metapack
( 22%) Combining: schemas/self/v1/schemas/api/schemas/evaluate/response/%/dependents.metapack
( 27%) Combining: schemas/self/v1/schemas/api/schemas/health/response/%/dependents.metapack
( 31%) Combining: schemas/self/v1/schemas/api/schemas/locations/response/%/dependents.metapack
( 36%) Combining: schemas/self/v1/schemas/api/schemas/metadata/response/%/dependents.metapack
( 40%) Combining: schemas/self/v1/schemas/api/schemas/position/%/dependents.metapack
( 45%) Combining: schemas/self/v1/schemas/api/schemas/positions/response/%/dependents.metapack
( 50%) Combining: schemas/self/v1/schemas/api/schemas/search/response/%/dependents.metapack
( 54%) Combining: schemas/self/v1/schemas/api/schemas/stats/response/%/dependents.metapack
( 59%) Combining: schemas/self/v1/schemas/api/schemas/trace/response/%/dependents.metapack
( 63%) Combining: schemas/self/v1/schemas/mcp/error/%/dependents.metapack
( 68%) Combining: schemas/self/v1/schemas/mcp/initialize/request/%/dependents.metapack
( 72%) Combining: schemas/self/v1/schemas/mcp/initialize/response/%/dependents.metapack
( 77%) Combining: schemas/self/v1/schemas/mcp/notifications/cancelled/%/dependents.metapack
( 81%) Combining: schemas/self/v1/schemas/mcp/notifications/initialized/%/dependents.metapack
( 86%) Combining: schemas/self/v1/schemas/mcp/ping/request/%/dependents.metapack
( 90%) Combining: schemas/self/v1/schemas/mcp/ping/response/%/dependents.metapack
( 95%) Combining: schemas/self/v1/schemas/mcp/request/%/dependents.metapack
(100%) Combining: schemas/self/v1/schemas/mcp/response/%/dependents.metapack
EOF

diff "$TMP/output.txt" "$TMP/expected.txt"
