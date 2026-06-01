#!/bin/sh

set -o errexit
set -o nounset

TMP="$(mktemp -d)"
clean() { rm -rf "$TMP"; }
trap clean EXIT

mkdir -p "$TMP/schemas" "$TMP/rules"

cat << 'EOF' > "$TMP/schemas/test.json"
{ "$schema": "https://json-schema.org/draft/2020-12/schema" }
EOF

cat << 'EOF' > "$TMP/rules/rule.json"
{ "$schema": "https://json-schema.org/draft/2020-12/schema" }
EOF

cat << EOF > "$TMP/one.json"
{
  "url": "https://example.com",
  "contents": {
    "test": {
      "path": "./schemas",
      "lint": { "rules": [ "./rules/rule.json" ] }
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
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/api/schemas/evaluate/request.json (#5)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/api/schemas/evaluate/response.json (#6)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/api/schemas/health/response.json (#7)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/api/schemas/locations/response.json (#8)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/api/schemas/metadata/response.json (#9)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/api/schemas/position.json (#10)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/api/schemas/positions/response.json (#11)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/api/schemas/search/response.json (#12)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/api/schemas/stats/response.json (#13)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/api/schemas/trace/request.json (#14)
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
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/tools/call/evaluate-schema/request.json (#31)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/tools/call/evaluate-schema/response.json (#32)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/tools/call/get-schema-dependencies/request.json (#33)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/tools/call/get-schema-dependencies/response.json (#34)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/tools/call/get-schema-dependents/request.json (#35)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/tools/call/get-schema-dependents/response.json (#36)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/tools/call/get-schema-health/request.json (#37)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/tools/call/get-schema-health/response.json (#38)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/tools/call/get-schema-locations/request.json (#39)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/tools/call/get-schema-locations/response.json (#40)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/tools/call/get-schema-metadata/request.json (#41)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/tools/call/get-schema-metadata/response.json (#42)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/tools/call/get-schema-positions/request.json (#43)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/tools/call/get-schema-positions/response.json (#44)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/tools/call/get-schema-stats/request.json (#45)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/tools/call/get-schema-stats/response.json (#46)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/tools/call/list-directory/request.json (#47)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/tools/call/list-directory/response.json (#48)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/tools/call/request.json (#49)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/tools/call/response.json (#50)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/tools/call/search-schemas/request.json (#51)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/tools/call/search-schemas/response.json (#52)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/tools/call/trace-schema-evaluation/request.json (#53)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/tools/call/trace-schema-evaluation/response.json (#54)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/tools/list/request.json (#55)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/mcp/tools/list/response.json (#56)
Detecting: $(realpath "$TMP")/schemas/test.json (#57)
(  1%) Resolving: self/v1/schemas/api/error.json
(  3%) Resolving: self/v1/schemas/api/list/response.json
(  5%) Resolving: self/v1/schemas/api/schemas/dependencies/response.json
(  7%) Resolving: self/v1/schemas/api/schemas/dependents/response.json
(  8%) Resolving: self/v1/schemas/api/schemas/evaluate/request.json
( 10%) Resolving: self/v1/schemas/api/schemas/evaluate/response.json
( 12%) Resolving: self/v1/schemas/api/schemas/health/response.json
( 14%) Resolving: self/v1/schemas/api/schemas/locations/response.json
( 15%) Resolving: self/v1/schemas/api/schemas/metadata/response.json
( 17%) Resolving: self/v1/schemas/api/schemas/position.json
( 19%) Resolving: self/v1/schemas/api/schemas/positions/response.json
( 21%) Resolving: self/v1/schemas/api/schemas/search/response.json
( 22%) Resolving: self/v1/schemas/api/schemas/stats/response.json
( 24%) Resolving: self/v1/schemas/api/schemas/trace/request.json
( 26%) Resolving: self/v1/schemas/api/schemas/trace/response.json
( 28%) Resolving: self/v1/schemas/mcp/error.json
( 29%) Resolving: self/v1/schemas/mcp/initialize/request.json
( 31%) Resolving: self/v1/schemas/mcp/initialize/response.json
( 33%) Resolving: self/v1/schemas/mcp/notifications/cancelled.json
( 35%) Resolving: self/v1/schemas/mcp/notifications/initialized.json
( 36%) Resolving: self/v1/schemas/mcp/ping/request.json
( 38%) Resolving: self/v1/schemas/mcp/ping/response.json
( 40%) Resolving: self/v1/schemas/mcp/request.json
( 42%) Resolving: self/v1/schemas/mcp/resources/list/request.json
( 43%) Resolving: self/v1/schemas/mcp/resources/list/response.json
( 45%) Resolving: self/v1/schemas/mcp/resources/read/request.json
( 47%) Resolving: self/v1/schemas/mcp/resources/read/response.json
( 49%) Resolving: self/v1/schemas/mcp/resources/templates/list/request.json
( 50%) Resolving: self/v1/schemas/mcp/resources/templates/list/response.json
( 52%) Resolving: self/v1/schemas/mcp/response.json
( 54%) Resolving: self/v1/schemas/mcp/tools/call/evaluate-schema/request.json
( 56%) Resolving: self/v1/schemas/mcp/tools/call/evaluate-schema/response.json
( 57%) Resolving: self/v1/schemas/mcp/tools/call/get-schema-dependencies/request.json
( 59%) Resolving: self/v1/schemas/mcp/tools/call/get-schema-dependencies/response.json
( 61%) Resolving: self/v1/schemas/mcp/tools/call/get-schema-dependents/request.json
( 63%) Resolving: self/v1/schemas/mcp/tools/call/get-schema-dependents/response.json
( 64%) Resolving: self/v1/schemas/mcp/tools/call/get-schema-health/request.json
( 66%) Resolving: self/v1/schemas/mcp/tools/call/get-schema-health/response.json
( 68%) Resolving: self/v1/schemas/mcp/tools/call/get-schema-locations/request.json
( 70%) Resolving: self/v1/schemas/mcp/tools/call/get-schema-locations/response.json
( 71%) Resolving: self/v1/schemas/mcp/tools/call/get-schema-metadata/request.json
( 73%) Resolving: self/v1/schemas/mcp/tools/call/get-schema-metadata/response.json
( 75%) Resolving: self/v1/schemas/mcp/tools/call/get-schema-positions/request.json
( 77%) Resolving: self/v1/schemas/mcp/tools/call/get-schema-positions/response.json
( 78%) Resolving: self/v1/schemas/mcp/tools/call/get-schema-stats/request.json
( 80%) Resolving: self/v1/schemas/mcp/tools/call/get-schema-stats/response.json
( 82%) Resolving: self/v1/schemas/mcp/tools/call/list-directory/request.json
( 84%) Resolving: self/v1/schemas/mcp/tools/call/list-directory/response.json
( 85%) Resolving: self/v1/schemas/mcp/tools/call/request.json
( 87%) Resolving: self/v1/schemas/mcp/tools/call/response.json
( 89%) Resolving: self/v1/schemas/mcp/tools/call/search-schemas/request.json
( 91%) Resolving: self/v1/schemas/mcp/tools/call/search-schemas/response.json
( 92%) Resolving: self/v1/schemas/mcp/tools/call/trace-schema-evaluation/request.json
( 94%) Resolving: self/v1/schemas/mcp/tools/call/trace-schema-evaluation/response.json
( 96%) Resolving: self/v1/schemas/mcp/tools/list/request.json
( 98%) Resolving: self/v1/schemas/mcp/tools/list/response.json
(100%) Resolving: test/test.json
(  0%) Producing: configuration.json
(  0%) Producing: version.json
(  0%) Producing: routes.bin
(  0%) Producing: explorer/%/404.metapack
(  0%) Producing: schemas/self/v1/schemas/api/error/%/schema.metapack
(  0%) Producing: schemas/self/v1/schemas/api/list/response/%/schema.metapack
(  0%) Producing: schemas/self/v1/schemas/api/schemas/dependencies/response/%/schema.metapack
(  1%) Producing: schemas/self/v1/schemas/api/schemas/dependents/response/%/schema.metapack
(  1%) Producing: schemas/self/v1/schemas/api/schemas/evaluate/request/%/schema.metapack
(  1%) Producing: schemas/self/v1/schemas/api/schemas/evaluate/response/%/schema.metapack
(  1%) Producing: schemas/self/v1/schemas/api/schemas/health/response/%/schema.metapack
(  1%) Producing: schemas/self/v1/schemas/api/schemas/locations/response/%/schema.metapack
(  1%) Producing: schemas/self/v1/schemas/api/schemas/metadata/response/%/schema.metapack
(  1%) Producing: schemas/self/v1/schemas/api/schemas/position/%/schema.metapack
(  1%) Producing: schemas/self/v1/schemas/api/schemas/positions/response/%/schema.metapack
(  2%) Producing: schemas/self/v1/schemas/api/schemas/search/response/%/schema.metapack
(  2%) Producing: schemas/self/v1/schemas/api/schemas/stats/response/%/schema.metapack
(  2%) Producing: schemas/self/v1/schemas/api/schemas/trace/request/%/schema.metapack
(  2%) Producing: schemas/self/v1/schemas/api/schemas/trace/response/%/schema.metapack
(  2%) Producing: schemas/self/v1/schemas/mcp/error/%/schema.metapack
(  2%) Producing: schemas/self/v1/schemas/mcp/initialize/request/%/schema.metapack
(  2%) Producing: schemas/self/v1/schemas/mcp/initialize/response/%/schema.metapack
(  2%) Producing: schemas/self/v1/schemas/mcp/notifications/cancelled/%/schema.metapack
(  3%) Producing: schemas/self/v1/schemas/mcp/notifications/initialized/%/schema.metapack
(  3%) Producing: schemas/self/v1/schemas/mcp/ping/request/%/schema.metapack
(  3%) Producing: schemas/self/v1/schemas/mcp/ping/response/%/schema.metapack
(  3%) Producing: schemas/self/v1/schemas/mcp/request/%/schema.metapack
(  3%) Producing: schemas/self/v1/schemas/mcp/resources/list/request/%/schema.metapack
(  3%) Producing: schemas/self/v1/schemas/mcp/resources/list/response/%/schema.metapack
(  3%) Producing: schemas/self/v1/schemas/mcp/resources/read/request/%/schema.metapack
(  4%) Producing: schemas/self/v1/schemas/mcp/resources/read/response/%/schema.metapack
(  4%) Producing: schemas/self/v1/schemas/mcp/resources/templates/list/request/%/schema.metapack
(  4%) Producing: schemas/self/v1/schemas/mcp/resources/templates/list/response/%/schema.metapack
(  4%) Producing: schemas/self/v1/schemas/mcp/response/%/schema.metapack
(  4%) Producing: schemas/self/v1/schemas/mcp/tools/call/evaluate-schema/request/%/schema.metapack
(  4%) Producing: schemas/self/v1/schemas/mcp/tools/call/evaluate-schema/response/%/schema.metapack
(  4%) Producing: schemas/self/v1/schemas/mcp/tools/call/get-schema-dependencies/request/%/schema.metapack
(  4%) Producing: schemas/self/v1/schemas/mcp/tools/call/get-schema-dependencies/response/%/schema.metapack
(  5%) Producing: schemas/self/v1/schemas/mcp/tools/call/get-schema-dependents/request/%/schema.metapack
(  5%) Producing: schemas/self/v1/schemas/mcp/tools/call/get-schema-dependents/response/%/schema.metapack
(  5%) Producing: schemas/self/v1/schemas/mcp/tools/call/get-schema-health/request/%/schema.metapack
(  5%) Producing: schemas/self/v1/schemas/mcp/tools/call/get-schema-health/response/%/schema.metapack
(  5%) Producing: schemas/self/v1/schemas/mcp/tools/call/get-schema-locations/request/%/schema.metapack
(  5%) Producing: schemas/self/v1/schemas/mcp/tools/call/get-schema-locations/response/%/schema.metapack
(  5%) Producing: schemas/self/v1/schemas/mcp/tools/call/get-schema-metadata/request/%/schema.metapack
(  5%) Producing: schemas/self/v1/schemas/mcp/tools/call/get-schema-metadata/response/%/schema.metapack
(  6%) Producing: schemas/self/v1/schemas/mcp/tools/call/get-schema-positions/request/%/schema.metapack
(  6%) Producing: schemas/self/v1/schemas/mcp/tools/call/get-schema-positions/response/%/schema.metapack
(  6%) Producing: schemas/self/v1/schemas/mcp/tools/call/get-schema-stats/request/%/schema.metapack
(  6%) Producing: schemas/self/v1/schemas/mcp/tools/call/get-schema-stats/response/%/schema.metapack
(  6%) Producing: schemas/self/v1/schemas/mcp/tools/call/list-directory/request/%/schema.metapack
(  6%) Producing: schemas/self/v1/schemas/mcp/tools/call/list-directory/response/%/schema.metapack
(  6%) Producing: schemas/self/v1/schemas/mcp/tools/call/request/%/schema.metapack
(  6%) Producing: schemas/self/v1/schemas/mcp/tools/call/response/%/schema.metapack
(  7%) Producing: schemas/self/v1/schemas/mcp/tools/call/search-schemas/request/%/schema.metapack
(  7%) Producing: schemas/self/v1/schemas/mcp/tools/call/search-schemas/response/%/schema.metapack
(  7%) Producing: schemas/self/v1/schemas/mcp/tools/call/trace-schema-evaluation/request/%/schema.metapack
(  7%) Producing: schemas/self/v1/schemas/mcp/tools/call/trace-schema-evaluation/response/%/schema.metapack
(  7%) Producing: schemas/self/v1/schemas/mcp/tools/list/request/%/schema.metapack
(  7%) Producing: schemas/self/v1/schemas/mcp/tools/list/response/%/schema.metapack
(  7%) Producing: schemas/test/test/%/schema.metapack
(  8%) Producing: schemas/self/v1/schemas/api/error/%/dependencies.metapack
(  8%) Producing: schemas/self/v1/schemas/api/error/%/locations.metapack
(  8%) Producing: schemas/self/v1/schemas/api/error/%/positions.metapack
(  8%) Producing: schemas/self/v1/schemas/api/error/%/stats.metapack
(  8%) Producing: schemas/self/v1/schemas/api/list/response/%/dependencies.metapack
(  8%) Producing: schemas/self/v1/schemas/api/list/response/%/locations.metapack
(  8%) Producing: schemas/self/v1/schemas/api/list/response/%/positions.metapack
(  8%) Producing: schemas/self/v1/schemas/api/list/response/%/stats.metapack
(  9%) Producing: schemas/self/v1/schemas/api/schemas/dependencies/response/%/dependencies.metapack
(  9%) Producing: schemas/self/v1/schemas/api/schemas/dependencies/response/%/locations.metapack
(  9%) Producing: schemas/self/v1/schemas/api/schemas/dependencies/response/%/positions.metapack
(  9%) Producing: schemas/self/v1/schemas/api/schemas/dependencies/response/%/stats.metapack
(  9%) Producing: schemas/self/v1/schemas/api/schemas/dependents/response/%/dependencies.metapack
(  9%) Producing: schemas/self/v1/schemas/api/schemas/dependents/response/%/locations.metapack
(  9%) Producing: schemas/self/v1/schemas/api/schemas/dependents/response/%/positions.metapack
(  9%) Producing: schemas/self/v1/schemas/api/schemas/dependents/response/%/stats.metapack
( 10%) Producing: schemas/self/v1/schemas/api/schemas/evaluate/request/%/dependencies.metapack
( 10%) Producing: schemas/self/v1/schemas/api/schemas/evaluate/request/%/locations.metapack
( 10%) Producing: schemas/self/v1/schemas/api/schemas/evaluate/request/%/positions.metapack
( 10%) Producing: schemas/self/v1/schemas/api/schemas/evaluate/request/%/stats.metapack
( 10%) Producing: schemas/self/v1/schemas/api/schemas/evaluate/response/%/dependencies.metapack
( 10%) Producing: schemas/self/v1/schemas/api/schemas/evaluate/response/%/locations.metapack
( 10%) Producing: schemas/self/v1/schemas/api/schemas/evaluate/response/%/positions.metapack
( 11%) Producing: schemas/self/v1/schemas/api/schemas/evaluate/response/%/stats.metapack
( 11%) Producing: schemas/self/v1/schemas/api/schemas/health/response/%/dependencies.metapack
( 11%) Producing: schemas/self/v1/schemas/api/schemas/health/response/%/locations.metapack
( 11%) Producing: schemas/self/v1/schemas/api/schemas/health/response/%/positions.metapack
( 11%) Producing: schemas/self/v1/schemas/api/schemas/health/response/%/stats.metapack
( 11%) Producing: schemas/self/v1/schemas/api/schemas/locations/response/%/dependencies.metapack
( 11%) Producing: schemas/self/v1/schemas/api/schemas/locations/response/%/locations.metapack
( 11%) Producing: schemas/self/v1/schemas/api/schemas/locations/response/%/positions.metapack
( 12%) Producing: schemas/self/v1/schemas/api/schemas/locations/response/%/stats.metapack
( 12%) Producing: schemas/self/v1/schemas/api/schemas/metadata/response/%/dependencies.metapack
( 12%) Producing: schemas/self/v1/schemas/api/schemas/metadata/response/%/locations.metapack
( 12%) Producing: schemas/self/v1/schemas/api/schemas/metadata/response/%/positions.metapack
( 12%) Producing: schemas/self/v1/schemas/api/schemas/metadata/response/%/stats.metapack
( 12%) Producing: schemas/self/v1/schemas/api/schemas/position/%/dependencies.metapack
( 12%) Producing: schemas/self/v1/schemas/api/schemas/position/%/locations.metapack
( 12%) Producing: schemas/self/v1/schemas/api/schemas/position/%/positions.metapack
( 13%) Producing: schemas/self/v1/schemas/api/schemas/position/%/stats.metapack
( 13%) Producing: schemas/self/v1/schemas/api/schemas/positions/response/%/dependencies.metapack
( 13%) Producing: schemas/self/v1/schemas/api/schemas/positions/response/%/locations.metapack
( 13%) Producing: schemas/self/v1/schemas/api/schemas/positions/response/%/positions.metapack
( 13%) Producing: schemas/self/v1/schemas/api/schemas/positions/response/%/stats.metapack
( 13%) Producing: schemas/self/v1/schemas/api/schemas/search/response/%/dependencies.metapack
( 13%) Producing: schemas/self/v1/schemas/api/schemas/search/response/%/locations.metapack
( 13%) Producing: schemas/self/v1/schemas/api/schemas/search/response/%/positions.metapack
( 14%) Producing: schemas/self/v1/schemas/api/schemas/search/response/%/stats.metapack
( 14%) Producing: schemas/self/v1/schemas/api/schemas/stats/response/%/dependencies.metapack
( 14%) Producing: schemas/self/v1/schemas/api/schemas/stats/response/%/locations.metapack
( 14%) Producing: schemas/self/v1/schemas/api/schemas/stats/response/%/positions.metapack
( 14%) Producing: schemas/self/v1/schemas/api/schemas/stats/response/%/stats.metapack
( 14%) Producing: schemas/self/v1/schemas/api/schemas/trace/request/%/dependencies.metapack
( 14%) Producing: schemas/self/v1/schemas/api/schemas/trace/request/%/locations.metapack
( 15%) Producing: schemas/self/v1/schemas/api/schemas/trace/request/%/positions.metapack
( 15%) Producing: schemas/self/v1/schemas/api/schemas/trace/request/%/stats.metapack
( 15%) Producing: schemas/self/v1/schemas/api/schemas/trace/response/%/dependencies.metapack
( 15%) Producing: schemas/self/v1/schemas/api/schemas/trace/response/%/locations.metapack
( 15%) Producing: schemas/self/v1/schemas/api/schemas/trace/response/%/positions.metapack
( 15%) Producing: schemas/self/v1/schemas/api/schemas/trace/response/%/stats.metapack
( 15%) Producing: schemas/self/v1/schemas/mcp/error/%/dependencies.metapack
( 15%) Producing: schemas/self/v1/schemas/mcp/error/%/locations.metapack
( 16%) Producing: schemas/self/v1/schemas/mcp/error/%/positions.metapack
( 16%) Producing: schemas/self/v1/schemas/mcp/error/%/stats.metapack
( 16%) Producing: schemas/self/v1/schemas/mcp/initialize/request/%/dependencies.metapack
( 16%) Producing: schemas/self/v1/schemas/mcp/initialize/request/%/locations.metapack
( 16%) Producing: schemas/self/v1/schemas/mcp/initialize/request/%/positions.metapack
( 16%) Producing: schemas/self/v1/schemas/mcp/initialize/request/%/stats.metapack
( 16%) Producing: schemas/self/v1/schemas/mcp/initialize/response/%/dependencies.metapack
( 16%) Producing: schemas/self/v1/schemas/mcp/initialize/response/%/locations.metapack
( 17%) Producing: schemas/self/v1/schemas/mcp/initialize/response/%/positions.metapack
( 17%) Producing: schemas/self/v1/schemas/mcp/initialize/response/%/stats.metapack
( 17%) Producing: schemas/self/v1/schemas/mcp/notifications/cancelled/%/dependencies.metapack
( 17%) Producing: schemas/self/v1/schemas/mcp/notifications/cancelled/%/locations.metapack
( 17%) Producing: schemas/self/v1/schemas/mcp/notifications/cancelled/%/positions.metapack
( 17%) Producing: schemas/self/v1/schemas/mcp/notifications/cancelled/%/stats.metapack
( 17%) Producing: schemas/self/v1/schemas/mcp/notifications/initialized/%/dependencies.metapack
( 18%) Producing: schemas/self/v1/schemas/mcp/notifications/initialized/%/locations.metapack
( 18%) Producing: schemas/self/v1/schemas/mcp/notifications/initialized/%/positions.metapack
( 18%) Producing: schemas/self/v1/schemas/mcp/notifications/initialized/%/stats.metapack
( 18%) Producing: schemas/self/v1/schemas/mcp/ping/request/%/dependencies.metapack
( 18%) Producing: schemas/self/v1/schemas/mcp/ping/request/%/locations.metapack
( 18%) Producing: schemas/self/v1/schemas/mcp/ping/request/%/positions.metapack
( 18%) Producing: schemas/self/v1/schemas/mcp/ping/request/%/stats.metapack
( 18%) Producing: schemas/self/v1/schemas/mcp/ping/response/%/dependencies.metapack
( 19%) Producing: schemas/self/v1/schemas/mcp/ping/response/%/locations.metapack
( 19%) Producing: schemas/self/v1/schemas/mcp/ping/response/%/positions.metapack
( 19%) Producing: schemas/self/v1/schemas/mcp/ping/response/%/stats.metapack
( 19%) Producing: schemas/self/v1/schemas/mcp/request/%/dependencies.metapack
( 19%) Producing: schemas/self/v1/schemas/mcp/request/%/locations.metapack
( 19%) Producing: schemas/self/v1/schemas/mcp/request/%/positions.metapack
( 19%) Producing: schemas/self/v1/schemas/mcp/request/%/stats.metapack
( 19%) Producing: schemas/self/v1/schemas/mcp/resources/list/request/%/dependencies.metapack
( 20%) Producing: schemas/self/v1/schemas/mcp/resources/list/request/%/locations.metapack
( 20%) Producing: schemas/self/v1/schemas/mcp/resources/list/request/%/positions.metapack
( 20%) Producing: schemas/self/v1/schemas/mcp/resources/list/request/%/stats.metapack
( 20%) Producing: schemas/self/v1/schemas/mcp/resources/list/response/%/dependencies.metapack
( 20%) Producing: schemas/self/v1/schemas/mcp/resources/list/response/%/locations.metapack
( 20%) Producing: schemas/self/v1/schemas/mcp/resources/list/response/%/positions.metapack
( 20%) Producing: schemas/self/v1/schemas/mcp/resources/list/response/%/stats.metapack
( 20%) Producing: schemas/self/v1/schemas/mcp/resources/read/request/%/dependencies.metapack
( 21%) Producing: schemas/self/v1/schemas/mcp/resources/read/request/%/locations.metapack
( 21%) Producing: schemas/self/v1/schemas/mcp/resources/read/request/%/positions.metapack
( 21%) Producing: schemas/self/v1/schemas/mcp/resources/read/request/%/stats.metapack
( 21%) Producing: schemas/self/v1/schemas/mcp/resources/read/response/%/dependencies.metapack
( 21%) Producing: schemas/self/v1/schemas/mcp/resources/read/response/%/locations.metapack
( 21%) Producing: schemas/self/v1/schemas/mcp/resources/read/response/%/positions.metapack
( 21%) Producing: schemas/self/v1/schemas/mcp/resources/read/response/%/stats.metapack
( 22%) Producing: schemas/self/v1/schemas/mcp/resources/templates/list/request/%/dependencies.metapack
( 22%) Producing: schemas/self/v1/schemas/mcp/resources/templates/list/request/%/locations.metapack
( 22%) Producing: schemas/self/v1/schemas/mcp/resources/templates/list/request/%/positions.metapack
( 22%) Producing: schemas/self/v1/schemas/mcp/resources/templates/list/request/%/stats.metapack
( 22%) Producing: schemas/self/v1/schemas/mcp/resources/templates/list/response/%/dependencies.metapack
( 22%) Producing: schemas/self/v1/schemas/mcp/resources/templates/list/response/%/locations.metapack
( 22%) Producing: schemas/self/v1/schemas/mcp/resources/templates/list/response/%/positions.metapack
( 22%) Producing: schemas/self/v1/schemas/mcp/resources/templates/list/response/%/stats.metapack
( 23%) Producing: schemas/self/v1/schemas/mcp/response/%/dependencies.metapack
( 23%) Producing: schemas/self/v1/schemas/mcp/response/%/locations.metapack
( 23%) Producing: schemas/self/v1/schemas/mcp/response/%/positions.metapack
( 23%) Producing: schemas/self/v1/schemas/mcp/response/%/stats.metapack
( 23%) Producing: schemas/self/v1/schemas/mcp/tools/call/evaluate-schema/request/%/dependencies.metapack
( 23%) Producing: schemas/self/v1/schemas/mcp/tools/call/evaluate-schema/request/%/locations.metapack
( 23%) Producing: schemas/self/v1/schemas/mcp/tools/call/evaluate-schema/request/%/positions.metapack
( 23%) Producing: schemas/self/v1/schemas/mcp/tools/call/evaluate-schema/request/%/stats.metapack
( 24%) Producing: schemas/self/v1/schemas/mcp/tools/call/evaluate-schema/response/%/dependencies.metapack
( 24%) Producing: schemas/self/v1/schemas/mcp/tools/call/evaluate-schema/response/%/locations.metapack
( 24%) Producing: schemas/self/v1/schemas/mcp/tools/call/evaluate-schema/response/%/positions.metapack
( 24%) Producing: schemas/self/v1/schemas/mcp/tools/call/evaluate-schema/response/%/stats.metapack
( 24%) Producing: schemas/self/v1/schemas/mcp/tools/call/get-schema-dependencies/request/%/dependencies.metapack
( 24%) Producing: schemas/self/v1/schemas/mcp/tools/call/get-schema-dependencies/request/%/locations.metapack
( 24%) Producing: schemas/self/v1/schemas/mcp/tools/call/get-schema-dependencies/request/%/positions.metapack
( 25%) Producing: schemas/self/v1/schemas/mcp/tools/call/get-schema-dependencies/request/%/stats.metapack
( 25%) Producing: schemas/self/v1/schemas/mcp/tools/call/get-schema-dependencies/response/%/dependencies.metapack
( 25%) Producing: schemas/self/v1/schemas/mcp/tools/call/get-schema-dependencies/response/%/locations.metapack
( 25%) Producing: schemas/self/v1/schemas/mcp/tools/call/get-schema-dependencies/response/%/positions.metapack
( 25%) Producing: schemas/self/v1/schemas/mcp/tools/call/get-schema-dependencies/response/%/stats.metapack
( 25%) Producing: schemas/self/v1/schemas/mcp/tools/call/get-schema-dependents/request/%/dependencies.metapack
( 25%) Producing: schemas/self/v1/schemas/mcp/tools/call/get-schema-dependents/request/%/locations.metapack
( 25%) Producing: schemas/self/v1/schemas/mcp/tools/call/get-schema-dependents/request/%/positions.metapack
( 26%) Producing: schemas/self/v1/schemas/mcp/tools/call/get-schema-dependents/request/%/stats.metapack
( 26%) Producing: schemas/self/v1/schemas/mcp/tools/call/get-schema-dependents/response/%/dependencies.metapack
( 26%) Producing: schemas/self/v1/schemas/mcp/tools/call/get-schema-dependents/response/%/locations.metapack
( 26%) Producing: schemas/self/v1/schemas/mcp/tools/call/get-schema-dependents/response/%/positions.metapack
( 26%) Producing: schemas/self/v1/schemas/mcp/tools/call/get-schema-dependents/response/%/stats.metapack
( 26%) Producing: schemas/self/v1/schemas/mcp/tools/call/get-schema-health/request/%/dependencies.metapack
( 26%) Producing: schemas/self/v1/schemas/mcp/tools/call/get-schema-health/request/%/locations.metapack
( 26%) Producing: schemas/self/v1/schemas/mcp/tools/call/get-schema-health/request/%/positions.metapack
( 27%) Producing: schemas/self/v1/schemas/mcp/tools/call/get-schema-health/request/%/stats.metapack
( 27%) Producing: schemas/self/v1/schemas/mcp/tools/call/get-schema-health/response/%/dependencies.metapack
( 27%) Producing: schemas/self/v1/schemas/mcp/tools/call/get-schema-health/response/%/locations.metapack
( 27%) Producing: schemas/self/v1/schemas/mcp/tools/call/get-schema-health/response/%/positions.metapack
( 27%) Producing: schemas/self/v1/schemas/mcp/tools/call/get-schema-health/response/%/stats.metapack
( 27%) Producing: schemas/self/v1/schemas/mcp/tools/call/get-schema-locations/request/%/dependencies.metapack
( 27%) Producing: schemas/self/v1/schemas/mcp/tools/call/get-schema-locations/request/%/locations.metapack
( 27%) Producing: schemas/self/v1/schemas/mcp/tools/call/get-schema-locations/request/%/positions.metapack
( 28%) Producing: schemas/self/v1/schemas/mcp/tools/call/get-schema-locations/request/%/stats.metapack
( 28%) Producing: schemas/self/v1/schemas/mcp/tools/call/get-schema-locations/response/%/dependencies.metapack
( 28%) Producing: schemas/self/v1/schemas/mcp/tools/call/get-schema-locations/response/%/locations.metapack
( 28%) Producing: schemas/self/v1/schemas/mcp/tools/call/get-schema-locations/response/%/positions.metapack
( 28%) Producing: schemas/self/v1/schemas/mcp/tools/call/get-schema-locations/response/%/stats.metapack
( 28%) Producing: schemas/self/v1/schemas/mcp/tools/call/get-schema-metadata/request/%/dependencies.metapack
( 28%) Producing: schemas/self/v1/schemas/mcp/tools/call/get-schema-metadata/request/%/locations.metapack
( 29%) Producing: schemas/self/v1/schemas/mcp/tools/call/get-schema-metadata/request/%/positions.metapack
( 29%) Producing: schemas/self/v1/schemas/mcp/tools/call/get-schema-metadata/request/%/stats.metapack
( 29%) Producing: schemas/self/v1/schemas/mcp/tools/call/get-schema-metadata/response/%/dependencies.metapack
( 29%) Producing: schemas/self/v1/schemas/mcp/tools/call/get-schema-metadata/response/%/locations.metapack
( 29%) Producing: schemas/self/v1/schemas/mcp/tools/call/get-schema-metadata/response/%/positions.metapack
( 29%) Producing: schemas/self/v1/schemas/mcp/tools/call/get-schema-metadata/response/%/stats.metapack
( 29%) Producing: schemas/self/v1/schemas/mcp/tools/call/get-schema-positions/request/%/dependencies.metapack
( 29%) Producing: schemas/self/v1/schemas/mcp/tools/call/get-schema-positions/request/%/locations.metapack
( 30%) Producing: schemas/self/v1/schemas/mcp/tools/call/get-schema-positions/request/%/positions.metapack
( 30%) Producing: schemas/self/v1/schemas/mcp/tools/call/get-schema-positions/request/%/stats.metapack
( 30%) Producing: schemas/self/v1/schemas/mcp/tools/call/get-schema-positions/response/%/dependencies.metapack
( 30%) Producing: schemas/self/v1/schemas/mcp/tools/call/get-schema-positions/response/%/locations.metapack
( 30%) Producing: schemas/self/v1/schemas/mcp/tools/call/get-schema-positions/response/%/positions.metapack
( 30%) Producing: schemas/self/v1/schemas/mcp/tools/call/get-schema-positions/response/%/stats.metapack
( 30%) Producing: schemas/self/v1/schemas/mcp/tools/call/get-schema-stats/request/%/dependencies.metapack
( 30%) Producing: schemas/self/v1/schemas/mcp/tools/call/get-schema-stats/request/%/locations.metapack
( 31%) Producing: schemas/self/v1/schemas/mcp/tools/call/get-schema-stats/request/%/positions.metapack
( 31%) Producing: schemas/self/v1/schemas/mcp/tools/call/get-schema-stats/request/%/stats.metapack
( 31%) Producing: schemas/self/v1/schemas/mcp/tools/call/get-schema-stats/response/%/dependencies.metapack
( 31%) Producing: schemas/self/v1/schemas/mcp/tools/call/get-schema-stats/response/%/locations.metapack
( 31%) Producing: schemas/self/v1/schemas/mcp/tools/call/get-schema-stats/response/%/positions.metapack
( 31%) Producing: schemas/self/v1/schemas/mcp/tools/call/get-schema-stats/response/%/stats.metapack
( 31%) Producing: schemas/self/v1/schemas/mcp/tools/call/list-directory/request/%/dependencies.metapack
( 31%) Producing: schemas/self/v1/schemas/mcp/tools/call/list-directory/request/%/locations.metapack
( 32%) Producing: schemas/self/v1/schemas/mcp/tools/call/list-directory/request/%/positions.metapack
( 32%) Producing: schemas/self/v1/schemas/mcp/tools/call/list-directory/request/%/stats.metapack
( 32%) Producing: schemas/self/v1/schemas/mcp/tools/call/list-directory/response/%/dependencies.metapack
( 32%) Producing: schemas/self/v1/schemas/mcp/tools/call/list-directory/response/%/locations.metapack
( 32%) Producing: schemas/self/v1/schemas/mcp/tools/call/list-directory/response/%/positions.metapack
( 32%) Producing: schemas/self/v1/schemas/mcp/tools/call/list-directory/response/%/stats.metapack
( 32%) Producing: schemas/self/v1/schemas/mcp/tools/call/request/%/dependencies.metapack
( 33%) Producing: schemas/self/v1/schemas/mcp/tools/call/request/%/locations.metapack
( 33%) Producing: schemas/self/v1/schemas/mcp/tools/call/request/%/positions.metapack
( 33%) Producing: schemas/self/v1/schemas/mcp/tools/call/request/%/stats.metapack
( 33%) Producing: schemas/self/v1/schemas/mcp/tools/call/response/%/dependencies.metapack
( 33%) Producing: schemas/self/v1/schemas/mcp/tools/call/response/%/locations.metapack
( 33%) Producing: schemas/self/v1/schemas/mcp/tools/call/response/%/positions.metapack
( 33%) Producing: schemas/self/v1/schemas/mcp/tools/call/response/%/stats.metapack
( 33%) Producing: schemas/self/v1/schemas/mcp/tools/call/search-schemas/request/%/dependencies.metapack
( 34%) Producing: schemas/self/v1/schemas/mcp/tools/call/search-schemas/request/%/locations.metapack
( 34%) Producing: schemas/self/v1/schemas/mcp/tools/call/search-schemas/request/%/positions.metapack
( 34%) Producing: schemas/self/v1/schemas/mcp/tools/call/search-schemas/request/%/stats.metapack
( 34%) Producing: schemas/self/v1/schemas/mcp/tools/call/search-schemas/response/%/dependencies.metapack
( 34%) Producing: schemas/self/v1/schemas/mcp/tools/call/search-schemas/response/%/locations.metapack
( 34%) Producing: schemas/self/v1/schemas/mcp/tools/call/search-schemas/response/%/positions.metapack
( 34%) Producing: schemas/self/v1/schemas/mcp/tools/call/search-schemas/response/%/stats.metapack
( 34%) Producing: schemas/self/v1/schemas/mcp/tools/call/trace-schema-evaluation/request/%/dependencies.metapack
( 35%) Producing: schemas/self/v1/schemas/mcp/tools/call/trace-schema-evaluation/request/%/locations.metapack
( 35%) Producing: schemas/self/v1/schemas/mcp/tools/call/trace-schema-evaluation/request/%/positions.metapack
( 35%) Producing: schemas/self/v1/schemas/mcp/tools/call/trace-schema-evaluation/request/%/stats.metapack
( 35%) Producing: schemas/self/v1/schemas/mcp/tools/call/trace-schema-evaluation/response/%/dependencies.metapack
( 35%) Producing: schemas/self/v1/schemas/mcp/tools/call/trace-schema-evaluation/response/%/locations.metapack
( 35%) Producing: schemas/self/v1/schemas/mcp/tools/call/trace-schema-evaluation/response/%/positions.metapack
( 35%) Producing: schemas/self/v1/schemas/mcp/tools/call/trace-schema-evaluation/response/%/stats.metapack
( 36%) Producing: schemas/self/v1/schemas/mcp/tools/list/request/%/dependencies.metapack
( 36%) Producing: schemas/self/v1/schemas/mcp/tools/list/request/%/locations.metapack
( 36%) Producing: schemas/self/v1/schemas/mcp/tools/list/request/%/positions.metapack
( 36%) Producing: schemas/self/v1/schemas/mcp/tools/list/request/%/stats.metapack
( 36%) Producing: schemas/self/v1/schemas/mcp/tools/list/response/%/dependencies.metapack
( 36%) Producing: schemas/self/v1/schemas/mcp/tools/list/response/%/locations.metapack
( 36%) Producing: schemas/self/v1/schemas/mcp/tools/list/response/%/positions.metapack
( 36%) Producing: schemas/self/v1/schemas/mcp/tools/list/response/%/stats.metapack
( 37%) Producing: schemas/test/test/%/dependencies.metapack
( 37%) Producing: schemas/test/test/%/locations.metapack
( 37%) Producing: schemas/test/test/%/positions.metapack
( 37%) Producing: schemas/test/test/%/stats.metapack
( 37%) Producing: schemas/self/v1/schemas/api/error/%/bundle.metapack
( 37%) Producing: schemas/self/v1/schemas/api/error/%/health.metapack
( 37%) Producing: schemas/self/v1/schemas/api/list/response/%/bundle.metapack
( 37%) Producing: schemas/self/v1/schemas/api/list/response/%/health.metapack
( 38%) Producing: schemas/self/v1/schemas/api/schemas/dependencies/response/%/bundle.metapack
( 38%) Producing: schemas/self/v1/schemas/api/schemas/dependencies/response/%/health.metapack
( 38%) Producing: schemas/self/v1/schemas/api/schemas/dependents/response/%/bundle.metapack
( 38%) Producing: schemas/self/v1/schemas/api/schemas/dependents/response/%/health.metapack
( 38%) Producing: schemas/self/v1/schemas/api/schemas/evaluate/request/%/bundle.metapack
( 38%) Producing: schemas/self/v1/schemas/api/schemas/evaluate/request/%/health.metapack
( 38%) Producing: schemas/self/v1/schemas/api/schemas/evaluate/response/%/bundle.metapack
( 38%) Producing: schemas/self/v1/schemas/api/schemas/evaluate/response/%/health.metapack
( 39%) Producing: schemas/self/v1/schemas/api/schemas/health/response/%/bundle.metapack
( 39%) Producing: schemas/self/v1/schemas/api/schemas/health/response/%/health.metapack
( 39%) Producing: schemas/self/v1/schemas/api/schemas/locations/response/%/bundle.metapack
( 39%) Producing: schemas/self/v1/schemas/api/schemas/locations/response/%/health.metapack
( 39%) Producing: schemas/self/v1/schemas/api/schemas/metadata/response/%/bundle.metapack
( 39%) Producing: schemas/self/v1/schemas/api/schemas/metadata/response/%/health.metapack
( 39%) Producing: schemas/self/v1/schemas/api/schemas/position/%/bundle.metapack
( 40%) Producing: schemas/self/v1/schemas/api/schemas/position/%/health.metapack
( 40%) Producing: schemas/self/v1/schemas/api/schemas/positions/response/%/bundle.metapack
( 40%) Producing: schemas/self/v1/schemas/api/schemas/positions/response/%/health.metapack
( 40%) Producing: schemas/self/v1/schemas/api/schemas/search/response/%/bundle.metapack
( 40%) Producing: schemas/self/v1/schemas/api/schemas/search/response/%/health.metapack
( 40%) Producing: schemas/self/v1/schemas/api/schemas/stats/response/%/bundle.metapack
( 40%) Producing: schemas/self/v1/schemas/api/schemas/stats/response/%/health.metapack
( 40%) Producing: schemas/self/v1/schemas/api/schemas/trace/request/%/bundle.metapack
( 41%) Producing: schemas/self/v1/schemas/api/schemas/trace/request/%/health.metapack
( 41%) Producing: schemas/self/v1/schemas/api/schemas/trace/response/%/bundle.metapack
( 41%) Producing: schemas/self/v1/schemas/api/schemas/trace/response/%/health.metapack
( 41%) Producing: schemas/self/v1/schemas/mcp/error/%/bundle.metapack
( 41%) Producing: schemas/self/v1/schemas/mcp/error/%/health.metapack
( 41%) Producing: schemas/self/v1/schemas/mcp/initialize/request/%/bundle.metapack
( 41%) Producing: schemas/self/v1/schemas/mcp/initialize/request/%/health.metapack
( 41%) Producing: schemas/self/v1/schemas/mcp/initialize/response/%/bundle.metapack
( 42%) Producing: schemas/self/v1/schemas/mcp/initialize/response/%/health.metapack
( 42%) Producing: schemas/self/v1/schemas/mcp/notifications/cancelled/%/bundle.metapack
( 42%) Producing: schemas/self/v1/schemas/mcp/notifications/cancelled/%/health.metapack
( 42%) Producing: schemas/self/v1/schemas/mcp/notifications/initialized/%/bundle.metapack
( 42%) Producing: schemas/self/v1/schemas/mcp/notifications/initialized/%/health.metapack
( 42%) Producing: schemas/self/v1/schemas/mcp/ping/request/%/bundle.metapack
( 42%) Producing: schemas/self/v1/schemas/mcp/ping/request/%/health.metapack
( 43%) Producing: schemas/self/v1/schemas/mcp/ping/response/%/bundle.metapack
( 43%) Producing: schemas/self/v1/schemas/mcp/ping/response/%/health.metapack
( 43%) Producing: schemas/self/v1/schemas/mcp/request/%/bundle.metapack
( 43%) Producing: schemas/self/v1/schemas/mcp/request/%/health.metapack
( 43%) Producing: schemas/self/v1/schemas/mcp/resources/list/request/%/bundle.metapack
( 43%) Producing: schemas/self/v1/schemas/mcp/resources/list/request/%/health.metapack
( 43%) Producing: schemas/self/v1/schemas/mcp/resources/list/response/%/bundle.metapack
( 43%) Producing: schemas/self/v1/schemas/mcp/resources/list/response/%/health.metapack
( 44%) Producing: schemas/self/v1/schemas/mcp/resources/read/request/%/bundle.metapack
( 44%) Producing: schemas/self/v1/schemas/mcp/resources/read/request/%/health.metapack
( 44%) Producing: schemas/self/v1/schemas/mcp/resources/read/response/%/bundle.metapack
( 44%) Producing: schemas/self/v1/schemas/mcp/resources/read/response/%/health.metapack
( 44%) Producing: schemas/self/v1/schemas/mcp/resources/templates/list/request/%/bundle.metapack
( 44%) Producing: schemas/self/v1/schemas/mcp/resources/templates/list/request/%/health.metapack
( 44%) Producing: schemas/self/v1/schemas/mcp/resources/templates/list/response/%/bundle.metapack
( 44%) Producing: schemas/self/v1/schemas/mcp/resources/templates/list/response/%/health.metapack
( 45%) Producing: schemas/self/v1/schemas/mcp/response/%/bundle.metapack
( 45%) Producing: schemas/self/v1/schemas/mcp/response/%/health.metapack
( 45%) Producing: schemas/self/v1/schemas/mcp/tools/call/evaluate-schema/request/%/bundle.metapack
( 45%) Producing: schemas/self/v1/schemas/mcp/tools/call/evaluate-schema/request/%/health.metapack
( 45%) Producing: schemas/self/v1/schemas/mcp/tools/call/evaluate-schema/response/%/bundle.metapack
( 45%) Producing: schemas/self/v1/schemas/mcp/tools/call/evaluate-schema/response/%/health.metapack
( 45%) Producing: schemas/self/v1/schemas/mcp/tools/call/get-schema-dependencies/request/%/bundle.metapack
( 45%) Producing: schemas/self/v1/schemas/mcp/tools/call/get-schema-dependencies/request/%/health.metapack
( 46%) Producing: schemas/self/v1/schemas/mcp/tools/call/get-schema-dependencies/response/%/bundle.metapack
( 46%) Producing: schemas/self/v1/schemas/mcp/tools/call/get-schema-dependencies/response/%/health.metapack
( 46%) Producing: schemas/self/v1/schemas/mcp/tools/call/get-schema-dependents/request/%/bundle.metapack
( 46%) Producing: schemas/self/v1/schemas/mcp/tools/call/get-schema-dependents/request/%/health.metapack
( 46%) Producing: schemas/self/v1/schemas/mcp/tools/call/get-schema-dependents/response/%/bundle.metapack
( 46%) Producing: schemas/self/v1/schemas/mcp/tools/call/get-schema-dependents/response/%/health.metapack
( 46%) Producing: schemas/self/v1/schemas/mcp/tools/call/get-schema-health/request/%/bundle.metapack
( 47%) Producing: schemas/self/v1/schemas/mcp/tools/call/get-schema-health/request/%/health.metapack
( 47%) Producing: schemas/self/v1/schemas/mcp/tools/call/get-schema-health/response/%/bundle.metapack
( 47%) Producing: schemas/self/v1/schemas/mcp/tools/call/get-schema-health/response/%/health.metapack
( 47%) Producing: schemas/self/v1/schemas/mcp/tools/call/get-schema-locations/request/%/bundle.metapack
( 47%) Producing: schemas/self/v1/schemas/mcp/tools/call/get-schema-locations/request/%/health.metapack
( 47%) Producing: schemas/self/v1/schemas/mcp/tools/call/get-schema-locations/response/%/bundle.metapack
( 47%) Producing: schemas/self/v1/schemas/mcp/tools/call/get-schema-locations/response/%/health.metapack
( 47%) Producing: schemas/self/v1/schemas/mcp/tools/call/get-schema-metadata/request/%/bundle.metapack
( 48%) Producing: schemas/self/v1/schemas/mcp/tools/call/get-schema-metadata/request/%/health.metapack
( 48%) Producing: schemas/self/v1/schemas/mcp/tools/call/get-schema-metadata/response/%/bundle.metapack
( 48%) Producing: schemas/self/v1/schemas/mcp/tools/call/get-schema-metadata/response/%/health.metapack
( 48%) Producing: schemas/self/v1/schemas/mcp/tools/call/get-schema-positions/request/%/bundle.metapack
( 48%) Producing: schemas/self/v1/schemas/mcp/tools/call/get-schema-positions/request/%/health.metapack
( 48%) Producing: schemas/self/v1/schemas/mcp/tools/call/get-schema-positions/response/%/bundle.metapack
( 48%) Producing: schemas/self/v1/schemas/mcp/tools/call/get-schema-positions/response/%/health.metapack
( 48%) Producing: schemas/self/v1/schemas/mcp/tools/call/get-schema-stats/request/%/bundle.metapack
( 49%) Producing: schemas/self/v1/schemas/mcp/tools/call/get-schema-stats/request/%/health.metapack
( 49%) Producing: schemas/self/v1/schemas/mcp/tools/call/get-schema-stats/response/%/bundle.metapack
( 49%) Producing: schemas/self/v1/schemas/mcp/tools/call/get-schema-stats/response/%/health.metapack
( 49%) Producing: schemas/self/v1/schemas/mcp/tools/call/list-directory/request/%/bundle.metapack
( 49%) Producing: schemas/self/v1/schemas/mcp/tools/call/list-directory/request/%/health.metapack
( 49%) Producing: schemas/self/v1/schemas/mcp/tools/call/list-directory/response/%/bundle.metapack
( 49%) Producing: schemas/self/v1/schemas/mcp/tools/call/list-directory/response/%/health.metapack
( 50%) Producing: schemas/self/v1/schemas/mcp/tools/call/request/%/bundle.metapack
( 50%) Producing: schemas/self/v1/schemas/mcp/tools/call/request/%/health.metapack
( 50%) Producing: schemas/self/v1/schemas/mcp/tools/call/response/%/bundle.metapack
( 50%) Producing: schemas/self/v1/schemas/mcp/tools/call/response/%/health.metapack
( 50%) Producing: schemas/self/v1/schemas/mcp/tools/call/search-schemas/request/%/bundle.metapack
( 50%) Producing: schemas/self/v1/schemas/mcp/tools/call/search-schemas/request/%/health.metapack
( 50%) Producing: schemas/self/v1/schemas/mcp/tools/call/search-schemas/response/%/bundle.metapack
( 50%) Producing: schemas/self/v1/schemas/mcp/tools/call/search-schemas/response/%/health.metapack
( 51%) Producing: schemas/self/v1/schemas/mcp/tools/call/trace-schema-evaluation/request/%/bundle.metapack
( 51%) Producing: schemas/self/v1/schemas/mcp/tools/call/trace-schema-evaluation/request/%/health.metapack
( 51%) Producing: schemas/self/v1/schemas/mcp/tools/call/trace-schema-evaluation/response/%/bundle.metapack
( 51%) Producing: schemas/self/v1/schemas/mcp/tools/call/trace-schema-evaluation/response/%/health.metapack
( 51%) Producing: schemas/self/v1/schemas/mcp/tools/list/request/%/bundle.metapack
( 51%) Producing: schemas/self/v1/schemas/mcp/tools/list/request/%/health.metapack
( 51%) Producing: schemas/self/v1/schemas/mcp/tools/list/response/%/bundle.metapack
( 51%) Producing: schemas/self/v1/schemas/mcp/tools/list/response/%/health.metapack
( 52%) Producing: schemas/test/test/%/bundle.metapack
( 52%) Producing: schemas/test/test/%/health.metapack
error: The schema rule is missing a title
  at path $(realpath "$TMP")/rules/rule.json
EOF

diff "$TMP/output.txt" "$TMP/expected.txt"
