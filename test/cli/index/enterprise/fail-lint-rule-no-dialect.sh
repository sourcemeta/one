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
{ "title": "custom/my_rule" }
EOF

cat << EOF > "$TMP/one.json"
{
  "extends": [ "@self/v1" ],
  "url": "https://example.com",
  "contents": {
    "test": {
      "path": "./schemas",
      "lint": { "rules": [ "./rules/rule.json" ] }
    }
  }
}
EOF

"$1" --skip-banner "$TMP/one.json" "$TMP/output" --concurrency 1 2> "$TMP/output.txt" && CODE="$?" || CODE="$?"
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
Detecting: $ONE_PREFIX/share/sourcemeta/one/collections/self/v1/schemas/configuration/path.json (#1)
Detecting: $ONE_PREFIX/share/sourcemeta/one/collections/self/v1/schemas/configuration/extends.json (#2)
Detecting: $ONE_PREFIX/share/sourcemeta/one/collections/self/v1/schemas/configuration/configuration.json (#3)
Detecting: $ONE_PREFIX/share/sourcemeta/one/collections/self/v1/schemas/configuration/contents.json (#4)
Detecting: $ONE_PREFIX/share/sourcemeta/one/collections/self/v1/schemas/configuration/collection.json (#5)
Detecting: $ONE_PREFIX/share/sourcemeta/one/collections/self/v1/schemas/configuration/rpath.json (#6)
Detecting: $ONE_PREFIX/share/sourcemeta/one/collections/self/v1/schemas/configuration/page.json (#7)
Detecting: $ONE_PREFIX/share/sourcemeta/one/collections/self/v1/schemas/api/error.json (#8)
Detecting: $ONE_PREFIX/share/sourcemeta/one/collections/self/v1/schemas/api/schemas/trace/response.json (#9)
Detecting: $ONE_PREFIX/share/sourcemeta/one/collections/self/v1/schemas/api/schemas/health/response.json (#10)
Detecting: $ONE_PREFIX/share/sourcemeta/one/collections/self/v1/schemas/api/schemas/dependents/response.json (#11)
Detecting: $ONE_PREFIX/share/sourcemeta/one/collections/self/v1/schemas/api/schemas/dependencies/response.json (#12)
Detecting: $ONE_PREFIX/share/sourcemeta/one/collections/self/v1/schemas/api/schemas/position.json (#13)
Detecting: $ONE_PREFIX/share/sourcemeta/one/collections/self/v1/schemas/api/schemas/search/response.json (#14)
Detecting: $ONE_PREFIX/share/sourcemeta/one/collections/self/v1/schemas/api/schemas/evaluate/response.json (#15)
Detecting: $ONE_PREFIX/share/sourcemeta/one/collections/self/v1/schemas/api/schemas/locations/response.json (#16)
Detecting: $ONE_PREFIX/share/sourcemeta/one/collections/self/v1/schemas/api/schemas/positions/response.json (#17)
Detecting: $ONE_PREFIX/share/sourcemeta/one/collections/self/v1/schemas/api/schemas/metadata/response.json (#18)
Detecting: $ONE_PREFIX/share/sourcemeta/one/collections/self/v1/schemas/api/schemas/stats/response.json (#19)
Detecting: $ONE_PREFIX/share/sourcemeta/one/collections/self/v1/schemas/api/list/response.json (#20)
Detecting: $(realpath "$TMP")/schemas/test.json (#21)
(  4%) Resolving: self/v1/schemas/configuration/path.json
(  9%) Resolving: self/v1/schemas/configuration/extends.json
( 14%) Resolving: self/v1/schemas/configuration/configuration.json
( 19%) Resolving: self/v1/schemas/configuration/contents.json
( 23%) Resolving: self/v1/schemas/configuration/collection.json
( 28%) Resolving: self/v1/schemas/configuration/rpath.json
( 33%) Resolving: self/v1/schemas/configuration/page.json
( 38%) Resolving: self/v1/schemas/api/error.json
( 42%) Resolving: self/v1/schemas/api/schemas/trace/response.json
( 47%) Resolving: self/v1/schemas/api/schemas/health/response.json
( 52%) Resolving: self/v1/schemas/api/schemas/dependents/response.json
( 57%) Resolving: self/v1/schemas/api/schemas/dependencies/response.json
( 61%) Resolving: self/v1/schemas/api/schemas/position.json
( 66%) Resolving: self/v1/schemas/api/schemas/search/response.json
( 71%) Resolving: self/v1/schemas/api/schemas/evaluate/response.json
( 76%) Resolving: self/v1/schemas/api/schemas/locations/response.json
( 80%) Resolving: self/v1/schemas/api/schemas/positions/response.json
( 85%) Resolving: self/v1/schemas/api/schemas/metadata/response.json
( 90%) Resolving: self/v1/schemas/api/schemas/stats/response.json
( 95%) Resolving: self/v1/schemas/api/list/response.json
(100%) Resolving: test/test.json
(  0%) Producing: configuration.json
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
(  5%) Producing: schemas/self/v1/schemas/configuration/collection/%/schema.metapack
(  6%) Producing: schemas/self/v1/schemas/configuration/configuration/%/schema.metapack
(  6%) Producing: schemas/self/v1/schemas/configuration/contents/%/schema.metapack
(  6%) Producing: schemas/self/v1/schemas/configuration/extends/%/schema.metapack
(  7%) Producing: schemas/self/v1/schemas/configuration/page/%/schema.metapack
(  7%) Producing: schemas/self/v1/schemas/configuration/path/%/schema.metapack
(  7%) Producing: schemas/self/v1/schemas/configuration/rpath/%/schema.metapack
(  8%) Producing: schemas/test/test/%/schema.metapack
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
( 17%) Producing: schemas/self/v1/schemas/api/schemas/locations/response/%/positions.metapack
( 17%) Producing: schemas/self/v1/schemas/api/schemas/locations/response/%/stats.metapack
( 17%) Producing: schemas/self/v1/schemas/api/schemas/metadata/response/%/dependencies.metapack
( 18%) Producing: schemas/self/v1/schemas/api/schemas/metadata/response/%/locations.metapack
( 18%) Producing: schemas/self/v1/schemas/api/schemas/metadata/response/%/positions.metapack
( 18%) Producing: schemas/self/v1/schemas/api/schemas/metadata/response/%/stats.metapack
( 19%) Producing: schemas/self/v1/schemas/api/schemas/position/%/dependencies.metapack
( 19%) Producing: schemas/self/v1/schemas/api/schemas/position/%/locations.metapack
( 20%) Producing: schemas/self/v1/schemas/api/schemas/position/%/positions.metapack
( 20%) Producing: schemas/self/v1/schemas/api/schemas/position/%/stats.metapack
( 20%) Producing: schemas/self/v1/schemas/api/schemas/positions/response/%/dependencies.metapack
( 21%) Producing: schemas/self/v1/schemas/api/schemas/positions/response/%/locations.metapack
( 21%) Producing: schemas/self/v1/schemas/api/schemas/positions/response/%/positions.metapack
( 21%) Producing: schemas/self/v1/schemas/api/schemas/positions/response/%/stats.metapack
( 22%) Producing: schemas/self/v1/schemas/api/schemas/search/response/%/dependencies.metapack
( 22%) Producing: schemas/self/v1/schemas/api/schemas/search/response/%/locations.metapack
( 22%) Producing: schemas/self/v1/schemas/api/schemas/search/response/%/positions.metapack
( 23%) Producing: schemas/self/v1/schemas/api/schemas/search/response/%/stats.metapack
( 23%) Producing: schemas/self/v1/schemas/api/schemas/stats/response/%/dependencies.metapack
( 23%) Producing: schemas/self/v1/schemas/api/schemas/stats/response/%/locations.metapack
( 24%) Producing: schemas/self/v1/schemas/api/schemas/stats/response/%/positions.metapack
( 24%) Producing: schemas/self/v1/schemas/api/schemas/stats/response/%/stats.metapack
( 24%) Producing: schemas/self/v1/schemas/api/schemas/trace/response/%/dependencies.metapack
( 25%) Producing: schemas/self/v1/schemas/api/schemas/trace/response/%/locations.metapack
( 25%) Producing: schemas/self/v1/schemas/api/schemas/trace/response/%/positions.metapack
( 25%) Producing: schemas/self/v1/schemas/api/schemas/trace/response/%/stats.metapack
( 26%) Producing: schemas/self/v1/schemas/configuration/collection/%/dependencies.metapack
( 26%) Producing: schemas/self/v1/schemas/configuration/collection/%/locations.metapack
( 26%) Producing: schemas/self/v1/schemas/configuration/collection/%/positions.metapack
( 27%) Producing: schemas/self/v1/schemas/configuration/collection/%/stats.metapack
( 27%) Producing: schemas/self/v1/schemas/configuration/configuration/%/dependencies.metapack
( 27%) Producing: schemas/self/v1/schemas/configuration/configuration/%/locations.metapack
( 28%) Producing: schemas/self/v1/schemas/configuration/configuration/%/positions.metapack
( 28%) Producing: schemas/self/v1/schemas/configuration/configuration/%/stats.metapack
( 28%) Producing: schemas/self/v1/schemas/configuration/contents/%/dependencies.metapack
( 29%) Producing: schemas/self/v1/schemas/configuration/contents/%/locations.metapack
( 29%) Producing: schemas/self/v1/schemas/configuration/contents/%/positions.metapack
( 29%) Producing: schemas/self/v1/schemas/configuration/contents/%/stats.metapack
( 30%) Producing: schemas/self/v1/schemas/configuration/extends/%/dependencies.metapack
( 30%) Producing: schemas/self/v1/schemas/configuration/extends/%/locations.metapack
( 30%) Producing: schemas/self/v1/schemas/configuration/extends/%/positions.metapack
( 31%) Producing: schemas/self/v1/schemas/configuration/extends/%/stats.metapack
( 31%) Producing: schemas/self/v1/schemas/configuration/page/%/dependencies.metapack
( 31%) Producing: schemas/self/v1/schemas/configuration/page/%/locations.metapack
( 32%) Producing: schemas/self/v1/schemas/configuration/page/%/positions.metapack
( 32%) Producing: schemas/self/v1/schemas/configuration/page/%/stats.metapack
( 32%) Producing: schemas/self/v1/schemas/configuration/path/%/dependencies.metapack
( 33%) Producing: schemas/self/v1/schemas/configuration/path/%/locations.metapack
( 33%) Producing: schemas/self/v1/schemas/configuration/path/%/positions.metapack
( 33%) Producing: schemas/self/v1/schemas/configuration/path/%/stats.metapack
( 34%) Producing: schemas/self/v1/schemas/configuration/rpath/%/dependencies.metapack
( 34%) Producing: schemas/self/v1/schemas/configuration/rpath/%/locations.metapack
( 34%) Producing: schemas/self/v1/schemas/configuration/rpath/%/positions.metapack
( 35%) Producing: schemas/self/v1/schemas/configuration/rpath/%/stats.metapack
( 35%) Producing: schemas/test/test/%/dependencies.metapack
( 35%) Producing: schemas/test/test/%/locations.metapack
( 36%) Producing: schemas/test/test/%/positions.metapack
( 36%) Producing: schemas/test/test/%/stats.metapack
( 36%) Producing: schemas/self/v1/schemas/api/error/%/bundle.metapack
( 37%) Producing: schemas/self/v1/schemas/api/error/%/health.metapack
( 37%) Producing: schemas/self/v1/schemas/api/list/response/%/bundle.metapack
( 37%) Producing: schemas/self/v1/schemas/api/list/response/%/health.metapack
( 38%) Producing: schemas/self/v1/schemas/api/schemas/dependencies/response/%/bundle.metapack
( 38%) Producing: schemas/self/v1/schemas/api/schemas/dependencies/response/%/health.metapack
( 38%) Producing: schemas/self/v1/schemas/api/schemas/dependents/response/%/bundle.metapack
( 39%) Producing: schemas/self/v1/schemas/api/schemas/dependents/response/%/health.metapack
( 39%) Producing: schemas/self/v1/schemas/api/schemas/evaluate/response/%/bundle.metapack
( 40%) Producing: schemas/self/v1/schemas/api/schemas/evaluate/response/%/health.metapack
( 40%) Producing: schemas/self/v1/schemas/api/schemas/health/response/%/bundle.metapack
( 40%) Producing: schemas/self/v1/schemas/api/schemas/health/response/%/health.metapack
( 41%) Producing: schemas/self/v1/schemas/api/schemas/locations/response/%/bundle.metapack
( 41%) Producing: schemas/self/v1/schemas/api/schemas/locations/response/%/health.metapack
( 41%) Producing: schemas/self/v1/schemas/api/schemas/metadata/response/%/bundle.metapack
( 42%) Producing: schemas/self/v1/schemas/api/schemas/metadata/response/%/health.metapack
( 42%) Producing: schemas/self/v1/schemas/api/schemas/position/%/bundle.metapack
( 42%) Producing: schemas/self/v1/schemas/api/schemas/position/%/health.metapack
( 43%) Producing: schemas/self/v1/schemas/api/schemas/positions/response/%/bundle.metapack
( 43%) Producing: schemas/self/v1/schemas/api/schemas/positions/response/%/health.metapack
( 43%) Producing: schemas/self/v1/schemas/api/schemas/search/response/%/bundle.metapack
( 44%) Producing: schemas/self/v1/schemas/api/schemas/search/response/%/health.metapack
( 44%) Producing: schemas/self/v1/schemas/api/schemas/stats/response/%/bundle.metapack
( 44%) Producing: schemas/self/v1/schemas/api/schemas/stats/response/%/health.metapack
( 45%) Producing: schemas/self/v1/schemas/api/schemas/trace/response/%/bundle.metapack
( 45%) Producing: schemas/self/v1/schemas/api/schemas/trace/response/%/health.metapack
( 45%) Producing: schemas/self/v1/schemas/configuration/collection/%/bundle.metapack
( 46%) Producing: schemas/self/v1/schemas/configuration/collection/%/health.metapack
( 46%) Producing: schemas/self/v1/schemas/configuration/configuration/%/bundle.metapack
( 46%) Producing: schemas/self/v1/schemas/configuration/configuration/%/health.metapack
( 47%) Producing: schemas/self/v1/schemas/configuration/contents/%/bundle.metapack
( 47%) Producing: schemas/self/v1/schemas/configuration/contents/%/health.metapack
( 47%) Producing: schemas/self/v1/schemas/configuration/extends/%/bundle.metapack
( 48%) Producing: schemas/self/v1/schemas/configuration/extends/%/health.metapack
( 48%) Producing: schemas/self/v1/schemas/configuration/page/%/bundle.metapack
( 48%) Producing: schemas/self/v1/schemas/configuration/page/%/health.metapack
( 49%) Producing: schemas/self/v1/schemas/configuration/path/%/bundle.metapack
( 49%) Producing: schemas/self/v1/schemas/configuration/path/%/health.metapack
( 49%) Producing: schemas/self/v1/schemas/configuration/rpath/%/bundle.metapack
( 50%) Producing: schemas/self/v1/schemas/configuration/rpath/%/health.metapack
( 50%) Producing: schemas/test/test/%/bundle.metapack
( 50%) Producing: schemas/test/test/%/health.metapack
error: Could not determine the base dialect of the schema
  at path $(realpath "$TMP")/rules/rule.json
EOF

diff "$TMP/output.txt" "$TMP/expected.txt"
