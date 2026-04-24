#!/bin/sh

set -o errexit
set -o nounset

TMP="$(mktemp -d)"
clean() { rm -rf "$TMP"; }
trap clean EXIT

cat << EOF > "$TMP/one.json"
{
  "extends": [ "@self/v1" ],
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

# First run: the schemas directory has a single schema
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
Detecting: $ONE_PREFIX/share/sourcemeta/one/collections/self/v1/schemas/api/error.json (#2)
Detecting: $ONE_PREFIX/share/sourcemeta/one/collections/self/v1/schemas/api/list/response.json (#3)
Detecting: $ONE_PREFIX/share/sourcemeta/one/collections/self/v1/schemas/api/schemas/dependencies/response.json (#4)
Detecting: $ONE_PREFIX/share/sourcemeta/one/collections/self/v1/schemas/api/schemas/dependents/response.json (#5)
Detecting: $ONE_PREFIX/share/sourcemeta/one/collections/self/v1/schemas/api/schemas/evaluate/response.json (#6)
Detecting: $ONE_PREFIX/share/sourcemeta/one/collections/self/v1/schemas/api/schemas/health/response.json (#7)
Detecting: $ONE_PREFIX/share/sourcemeta/one/collections/self/v1/schemas/api/schemas/locations/response.json (#8)
Detecting: $ONE_PREFIX/share/sourcemeta/one/collections/self/v1/schemas/api/schemas/metadata/response.json (#9)
Detecting: $ONE_PREFIX/share/sourcemeta/one/collections/self/v1/schemas/api/schemas/position.json (#10)
Detecting: $ONE_PREFIX/share/sourcemeta/one/collections/self/v1/schemas/api/schemas/positions/response.json (#11)
Detecting: $ONE_PREFIX/share/sourcemeta/one/collections/self/v1/schemas/api/schemas/search/response.json (#12)
Detecting: $ONE_PREFIX/share/sourcemeta/one/collections/self/v1/schemas/api/schemas/stats/response.json (#13)
Detecting: $ONE_PREFIX/share/sourcemeta/one/collections/self/v1/schemas/api/schemas/trace/response.json (#14)
Detecting: $ONE_PREFIX/share/sourcemeta/one/collections/self/v1/schemas/configuration/collection.json (#15)
Detecting: $ONE_PREFIX/share/sourcemeta/one/collections/self/v1/schemas/configuration/configuration.json (#16)
Detecting: $ONE_PREFIX/share/sourcemeta/one/collections/self/v1/schemas/configuration/contents.json (#17)
Detecting: $ONE_PREFIX/share/sourcemeta/one/collections/self/v1/schemas/configuration/extends.json (#18)
Detecting: $ONE_PREFIX/share/sourcemeta/one/collections/self/v1/schemas/configuration/page.json (#19)
Detecting: $ONE_PREFIX/share/sourcemeta/one/collections/self/v1/schemas/configuration/path.json (#20)
Detecting: $ONE_PREFIX/share/sourcemeta/one/collections/self/v1/schemas/configuration/rpath.json (#21)
(  4%) Resolving: schemas/test.json
(  9%) Resolving: self/v1/schemas/api/error.json
( 14%) Resolving: self/v1/schemas/api/list/response.json
( 19%) Resolving: self/v1/schemas/api/schemas/dependencies/response.json
( 23%) Resolving: self/v1/schemas/api/schemas/dependents/response.json
( 28%) Resolving: self/v1/schemas/api/schemas/evaluate/response.json
( 33%) Resolving: self/v1/schemas/api/schemas/health/response.json
( 38%) Resolving: self/v1/schemas/api/schemas/locations/response.json
( 42%) Resolving: self/v1/schemas/api/schemas/metadata/response.json
( 47%) Resolving: self/v1/schemas/api/schemas/position.json
( 52%) Resolving: self/v1/schemas/api/schemas/positions/response.json
( 57%) Resolving: self/v1/schemas/api/schemas/search/response.json
( 61%) Resolving: self/v1/schemas/api/schemas/stats/response.json
( 66%) Resolving: self/v1/schemas/api/schemas/trace/response.json
( 71%) Resolving: self/v1/schemas/configuration/collection.json
( 76%) Resolving: self/v1/schemas/configuration/configuration.json
( 80%) Resolving: self/v1/schemas/configuration/contents.json
( 85%) Resolving: self/v1/schemas/configuration/extends.json
( 90%) Resolving: self/v1/schemas/configuration/page.json
( 95%) Resolving: self/v1/schemas/configuration/path.json
(100%) Resolving: self/v1/schemas/configuration/rpath.json
(  0%) Producing: configuration.json
(  0%) Producing: version.json
(  1%) Producing: explorer/%/404.metapack
(  1%) Producing: schemas/schemas/test/%/schema.metapack
(  1%) Producing: schemas/self/v1/schemas/api/error/%/schema.metapack
(  2%) Producing: schemas/self/v1/schemas/api/list/response/%/schema.metapack
(  2%) Producing: schemas/self/v1/schemas/api/schemas/dependencies/response/%/schema.metapack
(  2%) Producing: schemas/self/v1/schemas/api/schemas/dependents/response/%/schema.metapack
(  3%) Producing: schemas/self/v1/schemas/api/schemas/evaluate/response/%/schema.metapack
(  3%) Producing: schemas/self/v1/schemas/api/schemas/health/response/%/schema.metapack
(  3%) Producing: schemas/self/v1/schemas/api/schemas/locations/response/%/schema.metapack
(  4%) Producing: schemas/self/v1/schemas/api/schemas/metadata/response/%/schema.metapack
(  4%) Producing: schemas/self/v1/schemas/api/schemas/position/%/schema.metapack
(  4%) Producing: schemas/self/v1/schemas/api/schemas/positions/response/%/schema.metapack
(  5%) Producing: schemas/self/v1/schemas/api/schemas/search/response/%/schema.metapack
(  5%) Producing: schemas/self/v1/schemas/api/schemas/stats/response/%/schema.metapack
(  5%) Producing: schemas/self/v1/schemas/api/schemas/trace/response/%/schema.metapack
(  6%) Producing: schemas/self/v1/schemas/configuration/collection/%/schema.metapack
(  6%) Producing: schemas/self/v1/schemas/configuration/configuration/%/schema.metapack
(  6%) Producing: schemas/self/v1/schemas/configuration/contents/%/schema.metapack
(  7%) Producing: schemas/self/v1/schemas/configuration/extends/%/schema.metapack
(  7%) Producing: schemas/self/v1/schemas/configuration/page/%/schema.metapack
(  7%) Producing: schemas/self/v1/schemas/configuration/path/%/schema.metapack
(  8%) Producing: schemas/self/v1/schemas/configuration/rpath/%/schema.metapack
(  8%) Producing: schemas/schemas/test/%/dependencies.metapack
(  8%) Producing: schemas/schemas/test/%/locations.metapack
(  9%) Producing: schemas/schemas/test/%/positions.metapack
(  9%) Producing: schemas/schemas/test/%/stats.metapack
(  9%) Producing: schemas/self/v1/schemas/api/error/%/dependencies.metapack
( 10%) Producing: schemas/self/v1/schemas/api/error/%/locations.metapack
( 10%) Producing: schemas/self/v1/schemas/api/error/%/positions.metapack
( 10%) Producing: schemas/self/v1/schemas/api/error/%/stats.metapack
( 11%) Producing: schemas/self/v1/schemas/api/list/response/%/dependencies.metapack
( 11%) Producing: schemas/self/v1/schemas/api/list/response/%/locations.metapack
( 11%) Producing: schemas/self/v1/schemas/api/list/response/%/positions.metapack
( 12%) Producing: schemas/self/v1/schemas/api/list/response/%/stats.metapack
( 12%) Producing: schemas/self/v1/schemas/api/schemas/dependencies/response/%/dependencies.metapack
( 12%) Producing: schemas/self/v1/schemas/api/schemas/dependencies/response/%/locations.metapack
( 13%) Producing: schemas/self/v1/schemas/api/schemas/dependencies/response/%/positions.metapack
( 13%) Producing: schemas/self/v1/schemas/api/schemas/dependencies/response/%/stats.metapack
( 13%) Producing: schemas/self/v1/schemas/api/schemas/dependents/response/%/dependencies.metapack
( 14%) Producing: schemas/self/v1/schemas/api/schemas/dependents/response/%/locations.metapack
( 14%) Producing: schemas/self/v1/schemas/api/schemas/dependents/response/%/positions.metapack
( 14%) Producing: schemas/self/v1/schemas/api/schemas/dependents/response/%/stats.metapack
( 15%) Producing: schemas/self/v1/schemas/api/schemas/evaluate/response/%/dependencies.metapack
( 15%) Producing: schemas/self/v1/schemas/api/schemas/evaluate/response/%/locations.metapack
( 15%) Producing: schemas/self/v1/schemas/api/schemas/evaluate/response/%/positions.metapack
( 16%) Producing: schemas/self/v1/schemas/api/schemas/evaluate/response/%/stats.metapack
( 16%) Producing: schemas/self/v1/schemas/api/schemas/health/response/%/dependencies.metapack
( 16%) Producing: schemas/self/v1/schemas/api/schemas/health/response/%/locations.metapack
( 17%) Producing: schemas/self/v1/schemas/api/schemas/health/response/%/positions.metapack
( 17%) Producing: schemas/self/v1/schemas/api/schemas/health/response/%/stats.metapack
( 17%) Producing: schemas/self/v1/schemas/api/schemas/locations/response/%/dependencies.metapack
( 18%) Producing: schemas/self/v1/schemas/api/schemas/locations/response/%/locations.metapack
( 18%) Producing: schemas/self/v1/schemas/api/schemas/locations/response/%/positions.metapack
( 18%) Producing: schemas/self/v1/schemas/api/schemas/locations/response/%/stats.metapack
( 19%) Producing: schemas/self/v1/schemas/api/schemas/metadata/response/%/dependencies.metapack
( 19%) Producing: schemas/self/v1/schemas/api/schemas/metadata/response/%/locations.metapack
( 20%) Producing: schemas/self/v1/schemas/api/schemas/metadata/response/%/positions.metapack
( 20%) Producing: schemas/self/v1/schemas/api/schemas/metadata/response/%/stats.metapack
( 20%) Producing: schemas/self/v1/schemas/api/schemas/position/%/dependencies.metapack
( 21%) Producing: schemas/self/v1/schemas/api/schemas/position/%/locations.metapack
( 21%) Producing: schemas/self/v1/schemas/api/schemas/position/%/positions.metapack
( 21%) Producing: schemas/self/v1/schemas/api/schemas/position/%/stats.metapack
( 22%) Producing: schemas/self/v1/schemas/api/schemas/positions/response/%/dependencies.metapack
( 22%) Producing: schemas/self/v1/schemas/api/schemas/positions/response/%/locations.metapack
( 22%) Producing: schemas/self/v1/schemas/api/schemas/positions/response/%/positions.metapack
( 23%) Producing: schemas/self/v1/schemas/api/schemas/positions/response/%/stats.metapack
( 23%) Producing: schemas/self/v1/schemas/api/schemas/search/response/%/dependencies.metapack
( 23%) Producing: schemas/self/v1/schemas/api/schemas/search/response/%/locations.metapack
( 24%) Producing: schemas/self/v1/schemas/api/schemas/search/response/%/positions.metapack
( 24%) Producing: schemas/self/v1/schemas/api/schemas/search/response/%/stats.metapack
( 24%) Producing: schemas/self/v1/schemas/api/schemas/stats/response/%/dependencies.metapack
( 25%) Producing: schemas/self/v1/schemas/api/schemas/stats/response/%/locations.metapack
( 25%) Producing: schemas/self/v1/schemas/api/schemas/stats/response/%/positions.metapack
( 25%) Producing: schemas/self/v1/schemas/api/schemas/stats/response/%/stats.metapack
( 26%) Producing: schemas/self/v1/schemas/api/schemas/trace/response/%/dependencies.metapack
( 26%) Producing: schemas/self/v1/schemas/api/schemas/trace/response/%/locations.metapack
( 26%) Producing: schemas/self/v1/schemas/api/schemas/trace/response/%/positions.metapack
( 27%) Producing: schemas/self/v1/schemas/api/schemas/trace/response/%/stats.metapack
( 27%) Producing: schemas/self/v1/schemas/configuration/collection/%/dependencies.metapack
( 27%) Producing: schemas/self/v1/schemas/configuration/collection/%/locations.metapack
( 28%) Producing: schemas/self/v1/schemas/configuration/collection/%/positions.metapack
( 28%) Producing: schemas/self/v1/schemas/configuration/collection/%/stats.metapack
( 28%) Producing: schemas/self/v1/schemas/configuration/configuration/%/dependencies.metapack
( 29%) Producing: schemas/self/v1/schemas/configuration/configuration/%/locations.metapack
( 29%) Producing: schemas/self/v1/schemas/configuration/configuration/%/positions.metapack
( 29%) Producing: schemas/self/v1/schemas/configuration/configuration/%/stats.metapack
( 30%) Producing: schemas/self/v1/schemas/configuration/contents/%/dependencies.metapack
( 30%) Producing: schemas/self/v1/schemas/configuration/contents/%/locations.metapack
( 30%) Producing: schemas/self/v1/schemas/configuration/contents/%/positions.metapack
( 31%) Producing: schemas/self/v1/schemas/configuration/contents/%/stats.metapack
( 31%) Producing: schemas/self/v1/schemas/configuration/extends/%/dependencies.metapack
( 31%) Producing: schemas/self/v1/schemas/configuration/extends/%/locations.metapack
( 32%) Producing: schemas/self/v1/schemas/configuration/extends/%/positions.metapack
( 32%) Producing: schemas/self/v1/schemas/configuration/extends/%/stats.metapack
( 32%) Producing: schemas/self/v1/schemas/configuration/page/%/dependencies.metapack
( 33%) Producing: schemas/self/v1/schemas/configuration/page/%/locations.metapack
( 33%) Producing: schemas/self/v1/schemas/configuration/page/%/positions.metapack
( 33%) Producing: schemas/self/v1/schemas/configuration/page/%/stats.metapack
( 34%) Producing: schemas/self/v1/schemas/configuration/path/%/dependencies.metapack
( 34%) Producing: schemas/self/v1/schemas/configuration/path/%/locations.metapack
( 34%) Producing: schemas/self/v1/schemas/configuration/path/%/positions.metapack
( 35%) Producing: schemas/self/v1/schemas/configuration/path/%/stats.metapack
( 35%) Producing: schemas/self/v1/schemas/configuration/rpath/%/dependencies.metapack
( 35%) Producing: schemas/self/v1/schemas/configuration/rpath/%/locations.metapack
( 36%) Producing: schemas/self/v1/schemas/configuration/rpath/%/positions.metapack
( 36%) Producing: schemas/self/v1/schemas/configuration/rpath/%/stats.metapack
( 36%) Producing: schemas/schemas/test/%/bundle.metapack
( 37%) Producing: schemas/schemas/test/%/health.metapack
( 37%) Producing: schemas/self/v1/schemas/api/error/%/bundle.metapack
( 37%) Producing: schemas/self/v1/schemas/api/error/%/health.metapack
( 38%) Producing: schemas/self/v1/schemas/api/list/response/%/bundle.metapack
( 38%) Producing: schemas/self/v1/schemas/api/list/response/%/health.metapack
( 38%) Producing: schemas/self/v1/schemas/api/schemas/dependencies/response/%/bundle.metapack
( 39%) Producing: schemas/self/v1/schemas/api/schemas/dependencies/response/%/health.metapack
( 39%) Producing: schemas/self/v1/schemas/api/schemas/dependents/response/%/bundle.metapack
( 40%) Producing: schemas/self/v1/schemas/api/schemas/dependents/response/%/health.metapack
( 40%) Producing: schemas/self/v1/schemas/api/schemas/evaluate/response/%/bundle.metapack
( 40%) Producing: schemas/self/v1/schemas/api/schemas/evaluate/response/%/health.metapack
( 41%) Producing: schemas/self/v1/schemas/api/schemas/health/response/%/bundle.metapack
( 41%) Producing: schemas/self/v1/schemas/api/schemas/health/response/%/health.metapack
( 41%) Producing: schemas/self/v1/schemas/api/schemas/locations/response/%/bundle.metapack
( 42%) Producing: schemas/self/v1/schemas/api/schemas/locations/response/%/health.metapack
( 42%) Producing: schemas/self/v1/schemas/api/schemas/metadata/response/%/bundle.metapack
( 42%) Producing: schemas/self/v1/schemas/api/schemas/metadata/response/%/health.metapack
( 43%) Producing: schemas/self/v1/schemas/api/schemas/position/%/bundle.metapack
( 43%) Producing: schemas/self/v1/schemas/api/schemas/position/%/health.metapack
( 43%) Producing: schemas/self/v1/schemas/api/schemas/positions/response/%/bundle.metapack
( 44%) Producing: schemas/self/v1/schemas/api/schemas/positions/response/%/health.metapack
( 44%) Producing: schemas/self/v1/schemas/api/schemas/search/response/%/bundle.metapack
( 44%) Producing: schemas/self/v1/schemas/api/schemas/search/response/%/health.metapack
( 45%) Producing: schemas/self/v1/schemas/api/schemas/stats/response/%/bundle.metapack
( 45%) Producing: schemas/self/v1/schemas/api/schemas/stats/response/%/health.metapack
( 45%) Producing: schemas/self/v1/schemas/api/schemas/trace/response/%/bundle.metapack
( 46%) Producing: schemas/self/v1/schemas/api/schemas/trace/response/%/health.metapack
( 46%) Producing: schemas/self/v1/schemas/configuration/collection/%/bundle.metapack
( 46%) Producing: schemas/self/v1/schemas/configuration/collection/%/health.metapack
( 47%) Producing: schemas/self/v1/schemas/configuration/configuration/%/bundle.metapack
( 47%) Producing: schemas/self/v1/schemas/configuration/configuration/%/health.metapack
( 47%) Producing: schemas/self/v1/schemas/configuration/contents/%/bundle.metapack
( 48%) Producing: schemas/self/v1/schemas/configuration/contents/%/health.metapack
( 48%) Producing: schemas/self/v1/schemas/configuration/extends/%/bundle.metapack
( 48%) Producing: schemas/self/v1/schemas/configuration/extends/%/health.metapack
( 49%) Producing: schemas/self/v1/schemas/configuration/page/%/bundle.metapack
( 49%) Producing: schemas/self/v1/schemas/configuration/page/%/health.metapack
( 49%) Producing: schemas/self/v1/schemas/configuration/path/%/bundle.metapack
( 50%) Producing: schemas/self/v1/schemas/configuration/path/%/health.metapack
( 50%) Producing: schemas/self/v1/schemas/configuration/rpath/%/bundle.metapack
( 50%) Producing: schemas/self/v1/schemas/configuration/rpath/%/health.metapack
( 51%) Producing: explorer/schemas/test/%/schema.metapack
( 51%) Producing: explorer/self/v1/schemas/api/error/%/schema.metapack
( 51%) Producing: explorer/self/v1/schemas/api/list/response/%/schema.metapack
( 52%) Producing: explorer/self/v1/schemas/api/schemas/dependencies/response/%/schema.metapack
( 52%) Producing: explorer/self/v1/schemas/api/schemas/dependents/response/%/schema.metapack
( 52%) Producing: explorer/self/v1/schemas/api/schemas/evaluate/response/%/schema.metapack
( 53%) Producing: explorer/self/v1/schemas/api/schemas/health/response/%/schema.metapack
( 53%) Producing: explorer/self/v1/schemas/api/schemas/locations/response/%/schema.metapack
( 53%) Producing: explorer/self/v1/schemas/api/schemas/metadata/response/%/schema.metapack
( 54%) Producing: explorer/self/v1/schemas/api/schemas/position/%/schema.metapack
( 54%) Producing: explorer/self/v1/schemas/api/schemas/positions/response/%/schema.metapack
( 54%) Producing: explorer/self/v1/schemas/api/schemas/search/response/%/schema.metapack
( 55%) Producing: explorer/self/v1/schemas/api/schemas/stats/response/%/schema.metapack
( 55%) Producing: explorer/self/v1/schemas/api/schemas/trace/response/%/schema.metapack
( 55%) Producing: explorer/self/v1/schemas/configuration/collection/%/schema.metapack
( 56%) Producing: explorer/self/v1/schemas/configuration/configuration/%/schema.metapack
( 56%) Producing: explorer/self/v1/schemas/configuration/contents/%/schema.metapack
( 56%) Producing: explorer/self/v1/schemas/configuration/extends/%/schema.metapack
( 57%) Producing: explorer/self/v1/schemas/configuration/page/%/schema.metapack
( 57%) Producing: explorer/self/v1/schemas/configuration/path/%/schema.metapack
( 57%) Producing: explorer/self/v1/schemas/configuration/rpath/%/schema.metapack
( 58%) Producing: schemas/schemas/test/%/blaze-exhaustive.metapack
( 58%) Producing: schemas/schemas/test/%/blaze-fast.metapack
( 58%) Producing: schemas/schemas/test/%/editor.metapack
( 59%) Producing: schemas/self/v1/schemas/api/error/%/blaze-exhaustive.metapack
( 59%) Producing: schemas/self/v1/schemas/api/error/%/blaze-fast.metapack
( 60%) Producing: schemas/self/v1/schemas/api/error/%/editor.metapack
( 60%) Producing: schemas/self/v1/schemas/api/list/response/%/blaze-exhaustive.metapack
( 60%) Producing: schemas/self/v1/schemas/api/list/response/%/blaze-fast.metapack
( 61%) Producing: schemas/self/v1/schemas/api/list/response/%/editor.metapack
( 61%) Producing: schemas/self/v1/schemas/api/schemas/dependencies/response/%/blaze-exhaustive.metapack
( 61%) Producing: schemas/self/v1/schemas/api/schemas/dependencies/response/%/blaze-fast.metapack
( 62%) Producing: schemas/self/v1/schemas/api/schemas/dependencies/response/%/editor.metapack
( 62%) Producing: schemas/self/v1/schemas/api/schemas/dependents/response/%/blaze-exhaustive.metapack
( 62%) Producing: schemas/self/v1/schemas/api/schemas/dependents/response/%/blaze-fast.metapack
( 63%) Producing: schemas/self/v1/schemas/api/schemas/dependents/response/%/editor.metapack
( 63%) Producing: schemas/self/v1/schemas/api/schemas/evaluate/response/%/blaze-exhaustive.metapack
( 63%) Producing: schemas/self/v1/schemas/api/schemas/evaluate/response/%/blaze-fast.metapack
( 64%) Producing: schemas/self/v1/schemas/api/schemas/evaluate/response/%/editor.metapack
( 64%) Producing: schemas/self/v1/schemas/api/schemas/health/response/%/blaze-exhaustive.metapack
( 64%) Producing: schemas/self/v1/schemas/api/schemas/health/response/%/blaze-fast.metapack
( 65%) Producing: schemas/self/v1/schemas/api/schemas/health/response/%/editor.metapack
( 65%) Producing: schemas/self/v1/schemas/api/schemas/locations/response/%/blaze-exhaustive.metapack
( 65%) Producing: schemas/self/v1/schemas/api/schemas/locations/response/%/blaze-fast.metapack
( 66%) Producing: schemas/self/v1/schemas/api/schemas/locations/response/%/editor.metapack
( 66%) Producing: schemas/self/v1/schemas/api/schemas/metadata/response/%/blaze-exhaustive.metapack
( 66%) Producing: schemas/self/v1/schemas/api/schemas/metadata/response/%/blaze-fast.metapack
( 67%) Producing: schemas/self/v1/schemas/api/schemas/metadata/response/%/editor.metapack
( 67%) Producing: schemas/self/v1/schemas/api/schemas/position/%/blaze-exhaustive.metapack
( 67%) Producing: schemas/self/v1/schemas/api/schemas/position/%/blaze-fast.metapack
( 68%) Producing: schemas/self/v1/schemas/api/schemas/position/%/editor.metapack
( 68%) Producing: schemas/self/v1/schemas/api/schemas/positions/response/%/blaze-exhaustive.metapack
( 68%) Producing: schemas/self/v1/schemas/api/schemas/positions/response/%/blaze-fast.metapack
( 69%) Producing: schemas/self/v1/schemas/api/schemas/positions/response/%/editor.metapack
( 69%) Producing: schemas/self/v1/schemas/api/schemas/search/response/%/blaze-exhaustive.metapack
( 69%) Producing: schemas/self/v1/schemas/api/schemas/search/response/%/blaze-fast.metapack
( 70%) Producing: schemas/self/v1/schemas/api/schemas/search/response/%/editor.metapack
( 70%) Producing: schemas/self/v1/schemas/api/schemas/stats/response/%/blaze-exhaustive.metapack
( 70%) Producing: schemas/self/v1/schemas/api/schemas/stats/response/%/blaze-fast.metapack
( 71%) Producing: schemas/self/v1/schemas/api/schemas/stats/response/%/editor.metapack
( 71%) Producing: schemas/self/v1/schemas/api/schemas/trace/response/%/blaze-exhaustive.metapack
( 71%) Producing: schemas/self/v1/schemas/api/schemas/trace/response/%/blaze-fast.metapack
( 72%) Producing: schemas/self/v1/schemas/api/schemas/trace/response/%/editor.metapack
( 72%) Producing: schemas/self/v1/schemas/configuration/collection/%/blaze-exhaustive.metapack
( 72%) Producing: schemas/self/v1/schemas/configuration/collection/%/blaze-fast.metapack
( 73%) Producing: schemas/self/v1/schemas/configuration/collection/%/editor.metapack
( 73%) Producing: schemas/self/v1/schemas/configuration/configuration/%/blaze-exhaustive.metapack
( 73%) Producing: schemas/self/v1/schemas/configuration/configuration/%/blaze-fast.metapack
( 74%) Producing: schemas/self/v1/schemas/configuration/configuration/%/editor.metapack
( 74%) Producing: schemas/self/v1/schemas/configuration/contents/%/blaze-exhaustive.metapack
( 74%) Producing: schemas/self/v1/schemas/configuration/contents/%/blaze-fast.metapack
( 75%) Producing: schemas/self/v1/schemas/configuration/contents/%/editor.metapack
( 75%) Producing: schemas/self/v1/schemas/configuration/extends/%/blaze-exhaustive.metapack
( 75%) Producing: schemas/self/v1/schemas/configuration/extends/%/blaze-fast.metapack
( 76%) Producing: schemas/self/v1/schemas/configuration/extends/%/editor.metapack
( 76%) Producing: schemas/self/v1/schemas/configuration/page/%/blaze-exhaustive.metapack
( 76%) Producing: schemas/self/v1/schemas/configuration/page/%/blaze-fast.metapack
( 77%) Producing: schemas/self/v1/schemas/configuration/page/%/editor.metapack
( 77%) Producing: schemas/self/v1/schemas/configuration/path/%/blaze-exhaustive.metapack
( 77%) Producing: schemas/self/v1/schemas/configuration/path/%/blaze-fast.metapack
( 78%) Producing: schemas/self/v1/schemas/configuration/path/%/editor.metapack
( 78%) Producing: schemas/self/v1/schemas/configuration/rpath/%/blaze-exhaustive.metapack
( 78%) Producing: schemas/self/v1/schemas/configuration/rpath/%/blaze-fast.metapack
( 79%) Producing: schemas/self/v1/schemas/configuration/rpath/%/editor.metapack
( 79%) Producing: explorer/schemas/%/directory.metapack
( 80%) Producing: explorer/schemas/test/%/schema-html.metapack
( 80%) Producing: explorer/self/v1/schemas/api/error/%/schema-html.metapack
( 80%) Producing: explorer/self/v1/schemas/api/list/%/directory.metapack
( 81%) Producing: explorer/self/v1/schemas/api/list/response/%/schema-html.metapack
( 81%) Producing: explorer/self/v1/schemas/api/schemas/dependencies/%/directory.metapack
( 81%) Producing: explorer/self/v1/schemas/api/schemas/dependencies/response/%/schema-html.metapack
( 82%) Producing: explorer/self/v1/schemas/api/schemas/dependents/%/directory.metapack
( 82%) Producing: explorer/self/v1/schemas/api/schemas/dependents/response/%/schema-html.metapack
( 82%) Producing: explorer/self/v1/schemas/api/schemas/evaluate/%/directory.metapack
( 83%) Producing: explorer/self/v1/schemas/api/schemas/evaluate/response/%/schema-html.metapack
( 83%) Producing: explorer/self/v1/schemas/api/schemas/health/%/directory.metapack
( 83%) Producing: explorer/self/v1/schemas/api/schemas/health/response/%/schema-html.metapack
( 84%) Producing: explorer/self/v1/schemas/api/schemas/locations/%/directory.metapack
( 84%) Producing: explorer/self/v1/schemas/api/schemas/locations/response/%/schema-html.metapack
( 84%) Producing: explorer/self/v1/schemas/api/schemas/metadata/%/directory.metapack
( 85%) Producing: explorer/self/v1/schemas/api/schemas/metadata/response/%/schema-html.metapack
( 85%) Producing: explorer/self/v1/schemas/api/schemas/position/%/schema-html.metapack
( 85%) Producing: explorer/self/v1/schemas/api/schemas/positions/%/directory.metapack
( 86%) Producing: explorer/self/v1/schemas/api/schemas/positions/response/%/schema-html.metapack
( 86%) Producing: explorer/self/v1/schemas/api/schemas/search/%/directory.metapack
( 86%) Producing: explorer/self/v1/schemas/api/schemas/search/response/%/schema-html.metapack
( 87%) Producing: explorer/self/v1/schemas/api/schemas/stats/%/directory.metapack
( 87%) Producing: explorer/self/v1/schemas/api/schemas/stats/response/%/schema-html.metapack
( 87%) Producing: explorer/self/v1/schemas/api/schemas/trace/%/directory.metapack
( 88%) Producing: explorer/self/v1/schemas/api/schemas/trace/response/%/schema-html.metapack
( 88%) Producing: explorer/self/v1/schemas/configuration/%/directory.metapack
( 88%) Producing: explorer/self/v1/schemas/configuration/collection/%/schema-html.metapack
( 89%) Producing: explorer/self/v1/schemas/configuration/configuration/%/schema-html.metapack
( 89%) Producing: explorer/self/v1/schemas/configuration/contents/%/schema-html.metapack
( 89%) Producing: explorer/self/v1/schemas/configuration/extends/%/schema-html.metapack
( 90%) Producing: explorer/self/v1/schemas/configuration/page/%/schema-html.metapack
( 90%) Producing: explorer/self/v1/schemas/configuration/path/%/schema-html.metapack
( 90%) Producing: explorer/self/v1/schemas/configuration/rpath/%/schema-html.metapack
( 91%) Producing: explorer/schemas/%/directory-html.metapack
( 91%) Producing: explorer/self/v1/schemas/api/list/%/directory-html.metapack
( 91%) Producing: explorer/self/v1/schemas/api/schemas/%/directory.metapack
( 92%) Producing: explorer/self/v1/schemas/api/schemas/dependencies/%/directory-html.metapack
( 92%) Producing: explorer/self/v1/schemas/api/schemas/dependents/%/directory-html.metapack
( 92%) Producing: explorer/self/v1/schemas/api/schemas/evaluate/%/directory-html.metapack
( 93%) Producing: explorer/self/v1/schemas/api/schemas/health/%/directory-html.metapack
( 93%) Producing: explorer/self/v1/schemas/api/schemas/locations/%/directory-html.metapack
( 93%) Producing: explorer/self/v1/schemas/api/schemas/metadata/%/directory-html.metapack
( 94%) Producing: explorer/self/v1/schemas/api/schemas/positions/%/directory-html.metapack
( 94%) Producing: explorer/self/v1/schemas/api/schemas/search/%/directory-html.metapack
( 94%) Producing: explorer/self/v1/schemas/api/schemas/stats/%/directory-html.metapack
( 95%) Producing: explorer/self/v1/schemas/api/schemas/trace/%/directory-html.metapack
( 95%) Producing: explorer/self/v1/schemas/configuration/%/directory-html.metapack
( 95%) Producing: explorer/self/v1/schemas/api/%/directory.metapack
( 96%) Producing: explorer/self/v1/schemas/api/schemas/%/directory-html.metapack
( 96%) Producing: explorer/self/v1/schemas/%/directory.metapack
( 96%) Producing: explorer/self/v1/schemas/api/%/directory-html.metapack
( 97%) Producing: explorer/self/v1/%/directory.metapack
( 97%) Producing: explorer/self/v1/schemas/%/directory-html.metapack
( 97%) Producing: explorer/self/%/directory.metapack
( 98%) Producing: explorer/self/v1/%/directory-html.metapack
( 98%) Producing: explorer/%/directory.metapack
( 98%) Producing: explorer/self/%/directory-html.metapack
( 99%) Producing: explorer/%/directory-html.metapack
( 99%) Producing: explorer/%/search.metapack
(100%) Producing: routes.bin
(  4%) Combining: schemas/schemas/test/%/dependents.metapack
(  9%) Combining: schemas/self/v1/schemas/api/error/%/dependents.metapack
( 14%) Combining: schemas/self/v1/schemas/api/list/response/%/dependents.metapack
( 19%) Combining: schemas/self/v1/schemas/api/schemas/dependencies/response/%/dependents.metapack
( 23%) Combining: schemas/self/v1/schemas/api/schemas/dependents/response/%/dependents.metapack
( 28%) Combining: schemas/self/v1/schemas/api/schemas/evaluate/response/%/dependents.metapack
( 33%) Combining: schemas/self/v1/schemas/api/schemas/health/response/%/dependents.metapack
( 38%) Combining: schemas/self/v1/schemas/api/schemas/locations/response/%/dependents.metapack
( 42%) Combining: schemas/self/v1/schemas/api/schemas/metadata/response/%/dependents.metapack
( 47%) Combining: schemas/self/v1/schemas/api/schemas/position/%/dependents.metapack
( 52%) Combining: schemas/self/v1/schemas/api/schemas/positions/response/%/dependents.metapack
( 57%) Combining: schemas/self/v1/schemas/api/schemas/search/response/%/dependents.metapack
( 61%) Combining: schemas/self/v1/schemas/api/schemas/stats/response/%/dependents.metapack
( 66%) Combining: schemas/self/v1/schemas/api/schemas/trace/response/%/dependents.metapack
( 71%) Combining: schemas/self/v1/schemas/configuration/collection/%/dependents.metapack
( 76%) Combining: schemas/self/v1/schemas/configuration/configuration/%/dependents.metapack
( 80%) Combining: schemas/self/v1/schemas/configuration/contents/%/dependents.metapack
( 85%) Combining: schemas/self/v1/schemas/configuration/extends/%/dependents.metapack
( 90%) Combining: schemas/self/v1/schemas/configuration/page/%/dependents.metapack
( 95%) Combining: schemas/self/v1/schemas/configuration/path/%/dependents.metapack
(100%) Combining: schemas/self/v1/schemas/configuration/rpath/%/dependents.metapack
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
./explorer/self/v1/schemas/api/schemas/dependents
./explorer/self/v1/schemas/api/schemas/dependents/%
./explorer/self/v1/schemas/api/schemas/dependents/%/directory-html.metapack
./explorer/self/v1/schemas/api/schemas/dependents/%/directory.metapack
./explorer/self/v1/schemas/api/schemas/dependents/response
./explorer/self/v1/schemas/api/schemas/dependents/response/%
./explorer/self/v1/schemas/api/schemas/dependents/response/%/schema-html.metapack
./explorer/self/v1/schemas/api/schemas/dependents/response/%/schema.metapack
./explorer/self/v1/schemas/api/schemas/evaluate
./explorer/self/v1/schemas/api/schemas/evaluate/%
./explorer/self/v1/schemas/api/schemas/evaluate/%/directory-html.metapack
./explorer/self/v1/schemas/api/schemas/evaluate/%/directory.metapack
./explorer/self/v1/schemas/api/schemas/evaluate/response
./explorer/self/v1/schemas/api/schemas/evaluate/response/%
./explorer/self/v1/schemas/api/schemas/evaluate/response/%/schema-html.metapack
./explorer/self/v1/schemas/api/schemas/evaluate/response/%/schema.metapack
./explorer/self/v1/schemas/api/schemas/health
./explorer/self/v1/schemas/api/schemas/health/%
./explorer/self/v1/schemas/api/schemas/health/%/directory-html.metapack
./explorer/self/v1/schemas/api/schemas/health/%/directory.metapack
./explorer/self/v1/schemas/api/schemas/health/response
./explorer/self/v1/schemas/api/schemas/health/response/%
./explorer/self/v1/schemas/api/schemas/health/response/%/schema-html.metapack
./explorer/self/v1/schemas/api/schemas/health/response/%/schema.metapack
./explorer/self/v1/schemas/api/schemas/locations
./explorer/self/v1/schemas/api/schemas/locations/%
./explorer/self/v1/schemas/api/schemas/locations/%/directory-html.metapack
./explorer/self/v1/schemas/api/schemas/locations/%/directory.metapack
./explorer/self/v1/schemas/api/schemas/locations/response
./explorer/self/v1/schemas/api/schemas/locations/response/%
./explorer/self/v1/schemas/api/schemas/locations/response/%/schema-html.metapack
./explorer/self/v1/schemas/api/schemas/locations/response/%/schema.metapack
./explorer/self/v1/schemas/api/schemas/metadata
./explorer/self/v1/schemas/api/schemas/metadata/%
./explorer/self/v1/schemas/api/schemas/metadata/%/directory-html.metapack
./explorer/self/v1/schemas/api/schemas/metadata/%/directory.metapack
./explorer/self/v1/schemas/api/schemas/metadata/response
./explorer/self/v1/schemas/api/schemas/metadata/response/%
./explorer/self/v1/schemas/api/schemas/metadata/response/%/schema-html.metapack
./explorer/self/v1/schemas/api/schemas/metadata/response/%/schema.metapack
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
./explorer/self/v1/schemas/api/schemas/search
./explorer/self/v1/schemas/api/schemas/search/%
./explorer/self/v1/schemas/api/schemas/search/%/directory-html.metapack
./explorer/self/v1/schemas/api/schemas/search/%/directory.metapack
./explorer/self/v1/schemas/api/schemas/search/response
./explorer/self/v1/schemas/api/schemas/search/response/%
./explorer/self/v1/schemas/api/schemas/search/response/%/schema-html.metapack
./explorer/self/v1/schemas/api/schemas/search/response/%/schema.metapack
./explorer/self/v1/schemas/api/schemas/stats
./explorer/self/v1/schemas/api/schemas/stats/%
./explorer/self/v1/schemas/api/schemas/stats/%/directory-html.metapack
./explorer/self/v1/schemas/api/schemas/stats/%/directory.metapack
./explorer/self/v1/schemas/api/schemas/stats/response
./explorer/self/v1/schemas/api/schemas/stats/response/%
./explorer/self/v1/schemas/api/schemas/stats/response/%/schema-html.metapack
./explorer/self/v1/schemas/api/schemas/stats/response/%/schema.metapack
./explorer/self/v1/schemas/api/schemas/trace
./explorer/self/v1/schemas/api/schemas/trace/%
./explorer/self/v1/schemas/api/schemas/trace/%/directory-html.metapack
./explorer/self/v1/schemas/api/schemas/trace/%/directory.metapack
./explorer/self/v1/schemas/api/schemas/trace/response
./explorer/self/v1/schemas/api/schemas/trace/response/%
./explorer/self/v1/schemas/api/schemas/trace/response/%/schema-html.metapack
./explorer/self/v1/schemas/api/schemas/trace/response/%/schema.metapack
./explorer/self/v1/schemas/configuration
./explorer/self/v1/schemas/configuration/%
./explorer/self/v1/schemas/configuration/%/directory-html.metapack
./explorer/self/v1/schemas/configuration/%/directory.metapack
./explorer/self/v1/schemas/configuration/collection
./explorer/self/v1/schemas/configuration/collection/%
./explorer/self/v1/schemas/configuration/collection/%/schema-html.metapack
./explorer/self/v1/schemas/configuration/collection/%/schema.metapack
./explorer/self/v1/schemas/configuration/configuration
./explorer/self/v1/schemas/configuration/configuration/%
./explorer/self/v1/schemas/configuration/configuration/%/schema-html.metapack
./explorer/self/v1/schemas/configuration/configuration/%/schema.metapack
./explorer/self/v1/schemas/configuration/contents
./explorer/self/v1/schemas/configuration/contents/%
./explorer/self/v1/schemas/configuration/contents/%/schema-html.metapack
./explorer/self/v1/schemas/configuration/contents/%/schema.metapack
./explorer/self/v1/schemas/configuration/extends
./explorer/self/v1/schemas/configuration/extends/%
./explorer/self/v1/schemas/configuration/extends/%/schema-html.metapack
./explorer/self/v1/schemas/configuration/extends/%/schema.metapack
./explorer/self/v1/schemas/configuration/page
./explorer/self/v1/schemas/configuration/page/%
./explorer/self/v1/schemas/configuration/page/%/schema-html.metapack
./explorer/self/v1/schemas/configuration/page/%/schema.metapack
./explorer/self/v1/schemas/configuration/path
./explorer/self/v1/schemas/configuration/path/%
./explorer/self/v1/schemas/configuration/path/%/schema-html.metapack
./explorer/self/v1/schemas/configuration/path/%/schema.metapack
./explorer/self/v1/schemas/configuration/rpath
./explorer/self/v1/schemas/configuration/rpath/%
./explorer/self/v1/schemas/configuration/rpath/%/schema-html.metapack
./explorer/self/v1/schemas/configuration/rpath/%/schema.metapack
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
./schemas/self/v1/schemas/api/schemas/evaluate
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
./schemas/self/v1/schemas/api/schemas/trace
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
./schemas/self/v1/schemas/configuration
./schemas/self/v1/schemas/configuration/collection
./schemas/self/v1/schemas/configuration/collection/%
./schemas/self/v1/schemas/configuration/collection/%/blaze-exhaustive.metapack
./schemas/self/v1/schemas/configuration/collection/%/blaze-fast.metapack
./schemas/self/v1/schemas/configuration/collection/%/bundle.metapack
./schemas/self/v1/schemas/configuration/collection/%/dependencies.metapack
./schemas/self/v1/schemas/configuration/collection/%/dependents.metapack
./schemas/self/v1/schemas/configuration/collection/%/editor.metapack
./schemas/self/v1/schemas/configuration/collection/%/health.metapack
./schemas/self/v1/schemas/configuration/collection/%/locations.metapack
./schemas/self/v1/schemas/configuration/collection/%/positions.metapack
./schemas/self/v1/schemas/configuration/collection/%/schema.metapack
./schemas/self/v1/schemas/configuration/collection/%/stats.metapack
./schemas/self/v1/schemas/configuration/configuration
./schemas/self/v1/schemas/configuration/configuration/%
./schemas/self/v1/schemas/configuration/configuration/%/blaze-exhaustive.metapack
./schemas/self/v1/schemas/configuration/configuration/%/blaze-fast.metapack
./schemas/self/v1/schemas/configuration/configuration/%/bundle.metapack
./schemas/self/v1/schemas/configuration/configuration/%/dependencies.metapack
./schemas/self/v1/schemas/configuration/configuration/%/dependents.metapack
./schemas/self/v1/schemas/configuration/configuration/%/editor.metapack
./schemas/self/v1/schemas/configuration/configuration/%/health.metapack
./schemas/self/v1/schemas/configuration/configuration/%/locations.metapack
./schemas/self/v1/schemas/configuration/configuration/%/positions.metapack
./schemas/self/v1/schemas/configuration/configuration/%/schema.metapack
./schemas/self/v1/schemas/configuration/configuration/%/stats.metapack
./schemas/self/v1/schemas/configuration/contents
./schemas/self/v1/schemas/configuration/contents/%
./schemas/self/v1/schemas/configuration/contents/%/blaze-exhaustive.metapack
./schemas/self/v1/schemas/configuration/contents/%/blaze-fast.metapack
./schemas/self/v1/schemas/configuration/contents/%/bundle.metapack
./schemas/self/v1/schemas/configuration/contents/%/dependencies.metapack
./schemas/self/v1/schemas/configuration/contents/%/dependents.metapack
./schemas/self/v1/schemas/configuration/contents/%/editor.metapack
./schemas/self/v1/schemas/configuration/contents/%/health.metapack
./schemas/self/v1/schemas/configuration/contents/%/locations.metapack
./schemas/self/v1/schemas/configuration/contents/%/positions.metapack
./schemas/self/v1/schemas/configuration/contents/%/schema.metapack
./schemas/self/v1/schemas/configuration/contents/%/stats.metapack
./schemas/self/v1/schemas/configuration/extends
./schemas/self/v1/schemas/configuration/extends/%
./schemas/self/v1/schemas/configuration/extends/%/blaze-exhaustive.metapack
./schemas/self/v1/schemas/configuration/extends/%/blaze-fast.metapack
./schemas/self/v1/schemas/configuration/extends/%/bundle.metapack
./schemas/self/v1/schemas/configuration/extends/%/dependencies.metapack
./schemas/self/v1/schemas/configuration/extends/%/dependents.metapack
./schemas/self/v1/schemas/configuration/extends/%/editor.metapack
./schemas/self/v1/schemas/configuration/extends/%/health.metapack
./schemas/self/v1/schemas/configuration/extends/%/locations.metapack
./schemas/self/v1/schemas/configuration/extends/%/positions.metapack
./schemas/self/v1/schemas/configuration/extends/%/schema.metapack
./schemas/self/v1/schemas/configuration/extends/%/stats.metapack
./schemas/self/v1/schemas/configuration/page
./schemas/self/v1/schemas/configuration/page/%
./schemas/self/v1/schemas/configuration/page/%/blaze-exhaustive.metapack
./schemas/self/v1/schemas/configuration/page/%/blaze-fast.metapack
./schemas/self/v1/schemas/configuration/page/%/bundle.metapack
./schemas/self/v1/schemas/configuration/page/%/dependencies.metapack
./schemas/self/v1/schemas/configuration/page/%/dependents.metapack
./schemas/self/v1/schemas/configuration/page/%/editor.metapack
./schemas/self/v1/schemas/configuration/page/%/health.metapack
./schemas/self/v1/schemas/configuration/page/%/locations.metapack
./schemas/self/v1/schemas/configuration/page/%/positions.metapack
./schemas/self/v1/schemas/configuration/page/%/schema.metapack
./schemas/self/v1/schemas/configuration/page/%/stats.metapack
./schemas/self/v1/schemas/configuration/path
./schemas/self/v1/schemas/configuration/path/%
./schemas/self/v1/schemas/configuration/path/%/blaze-exhaustive.metapack
./schemas/self/v1/schemas/configuration/path/%/blaze-fast.metapack
./schemas/self/v1/schemas/configuration/path/%/bundle.metapack
./schemas/self/v1/schemas/configuration/path/%/dependencies.metapack
./schemas/self/v1/schemas/configuration/path/%/dependents.metapack
./schemas/self/v1/schemas/configuration/path/%/editor.metapack
./schemas/self/v1/schemas/configuration/path/%/health.metapack
./schemas/self/v1/schemas/configuration/path/%/locations.metapack
./schemas/self/v1/schemas/configuration/path/%/positions.metapack
./schemas/self/v1/schemas/configuration/path/%/schema.metapack
./schemas/self/v1/schemas/configuration/path/%/stats.metapack
./schemas/self/v1/schemas/configuration/rpath
./schemas/self/v1/schemas/configuration/rpath/%
./schemas/self/v1/schemas/configuration/rpath/%/blaze-exhaustive.metapack
./schemas/self/v1/schemas/configuration/rpath/%/blaze-fast.metapack
./schemas/self/v1/schemas/configuration/rpath/%/bundle.metapack
./schemas/self/v1/schemas/configuration/rpath/%/dependencies.metapack
./schemas/self/v1/schemas/configuration/rpath/%/dependents.metapack
./schemas/self/v1/schemas/configuration/rpath/%/editor.metapack
./schemas/self/v1/schemas/configuration/rpath/%/health.metapack
./schemas/self/v1/schemas/configuration/rpath/%/locations.metapack
./schemas/self/v1/schemas/configuration/rpath/%/positions.metapack
./schemas/self/v1/schemas/configuration/rpath/%/schema.metapack
./schemas/self/v1/schemas/configuration/rpath/%/stats.metapack
./state.bin
./version.json
EOF
diff "$TMP/manifest.txt" "$TMP/expected_manifest.txt"

# Second run: delete the schema file
rm "$TMP/schemas/test.json"

"$1" --skip-banner --deterministic "$TMP/one.json" "$TMP/output" --concurrency 1 2> "$TMP/output.txt"
remove_threads_information "$TMP/output.txt"

cat << EOF > "$TMP/expected.txt"
Writing output to: $(realpath "$TMP")/output
Using configuration: $(realpath "$TMP")/one.json
Detecting: $ONE_PREFIX/share/sourcemeta/one/collections/self/v1/schemas/api/error.json (#1)
Detecting: $ONE_PREFIX/share/sourcemeta/one/collections/self/v1/schemas/api/list/response.json (#2)
Detecting: $ONE_PREFIX/share/sourcemeta/one/collections/self/v1/schemas/api/schemas/dependencies/response.json (#3)
Detecting: $ONE_PREFIX/share/sourcemeta/one/collections/self/v1/schemas/api/schemas/dependents/response.json (#4)
Detecting: $ONE_PREFIX/share/sourcemeta/one/collections/self/v1/schemas/api/schemas/evaluate/response.json (#5)
Detecting: $ONE_PREFIX/share/sourcemeta/one/collections/self/v1/schemas/api/schemas/health/response.json (#6)
Detecting: $ONE_PREFIX/share/sourcemeta/one/collections/self/v1/schemas/api/schemas/locations/response.json (#7)
Detecting: $ONE_PREFIX/share/sourcemeta/one/collections/self/v1/schemas/api/schemas/metadata/response.json (#8)
Detecting: $ONE_PREFIX/share/sourcemeta/one/collections/self/v1/schemas/api/schemas/position.json (#9)
Detecting: $ONE_PREFIX/share/sourcemeta/one/collections/self/v1/schemas/api/schemas/positions/response.json (#10)
Detecting: $ONE_PREFIX/share/sourcemeta/one/collections/self/v1/schemas/api/schemas/search/response.json (#11)
Detecting: $ONE_PREFIX/share/sourcemeta/one/collections/self/v1/schemas/api/schemas/stats/response.json (#12)
Detecting: $ONE_PREFIX/share/sourcemeta/one/collections/self/v1/schemas/api/schemas/trace/response.json (#13)
Detecting: $ONE_PREFIX/share/sourcemeta/one/collections/self/v1/schemas/configuration/collection.json (#14)
Detecting: $ONE_PREFIX/share/sourcemeta/one/collections/self/v1/schemas/configuration/configuration.json (#15)
Detecting: $ONE_PREFIX/share/sourcemeta/one/collections/self/v1/schemas/configuration/contents.json (#16)
Detecting: $ONE_PREFIX/share/sourcemeta/one/collections/self/v1/schemas/configuration/extends.json (#17)
Detecting: $ONE_PREFIX/share/sourcemeta/one/collections/self/v1/schemas/configuration/page.json (#18)
Detecting: $ONE_PREFIX/share/sourcemeta/one/collections/self/v1/schemas/configuration/path.json (#19)
Detecting: $ONE_PREFIX/share/sourcemeta/one/collections/self/v1/schemas/configuration/rpath.json (#20)
( 20%) Producing: explorer/%/directory.metapack
( 40%) Producing: explorer/%/directory-html.metapack
( 60%) Producing: explorer/%/search.metapack
( 80%) Disposing: explorer/schemas
(100%) Disposing: schemas/schemas
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
./explorer/self/v1/schemas/api/schemas/dependents
./explorer/self/v1/schemas/api/schemas/dependents/%
./explorer/self/v1/schemas/api/schemas/dependents/%/directory-html.metapack
./explorer/self/v1/schemas/api/schemas/dependents/%/directory.metapack
./explorer/self/v1/schemas/api/schemas/dependents/response
./explorer/self/v1/schemas/api/schemas/dependents/response/%
./explorer/self/v1/schemas/api/schemas/dependents/response/%/schema-html.metapack
./explorer/self/v1/schemas/api/schemas/dependents/response/%/schema.metapack
./explorer/self/v1/schemas/api/schemas/evaluate
./explorer/self/v1/schemas/api/schemas/evaluate/%
./explorer/self/v1/schemas/api/schemas/evaluate/%/directory-html.metapack
./explorer/self/v1/schemas/api/schemas/evaluate/%/directory.metapack
./explorer/self/v1/schemas/api/schemas/evaluate/response
./explorer/self/v1/schemas/api/schemas/evaluate/response/%
./explorer/self/v1/schemas/api/schemas/evaluate/response/%/schema-html.metapack
./explorer/self/v1/schemas/api/schemas/evaluate/response/%/schema.metapack
./explorer/self/v1/schemas/api/schemas/health
./explorer/self/v1/schemas/api/schemas/health/%
./explorer/self/v1/schemas/api/schemas/health/%/directory-html.metapack
./explorer/self/v1/schemas/api/schemas/health/%/directory.metapack
./explorer/self/v1/schemas/api/schemas/health/response
./explorer/self/v1/schemas/api/schemas/health/response/%
./explorer/self/v1/schemas/api/schemas/health/response/%/schema-html.metapack
./explorer/self/v1/schemas/api/schemas/health/response/%/schema.metapack
./explorer/self/v1/schemas/api/schemas/locations
./explorer/self/v1/schemas/api/schemas/locations/%
./explorer/self/v1/schemas/api/schemas/locations/%/directory-html.metapack
./explorer/self/v1/schemas/api/schemas/locations/%/directory.metapack
./explorer/self/v1/schemas/api/schemas/locations/response
./explorer/self/v1/schemas/api/schemas/locations/response/%
./explorer/self/v1/schemas/api/schemas/locations/response/%/schema-html.metapack
./explorer/self/v1/schemas/api/schemas/locations/response/%/schema.metapack
./explorer/self/v1/schemas/api/schemas/metadata
./explorer/self/v1/schemas/api/schemas/metadata/%
./explorer/self/v1/schemas/api/schemas/metadata/%/directory-html.metapack
./explorer/self/v1/schemas/api/schemas/metadata/%/directory.metapack
./explorer/self/v1/schemas/api/schemas/metadata/response
./explorer/self/v1/schemas/api/schemas/metadata/response/%
./explorer/self/v1/schemas/api/schemas/metadata/response/%/schema-html.metapack
./explorer/self/v1/schemas/api/schemas/metadata/response/%/schema.metapack
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
./explorer/self/v1/schemas/api/schemas/search
./explorer/self/v1/schemas/api/schemas/search/%
./explorer/self/v1/schemas/api/schemas/search/%/directory-html.metapack
./explorer/self/v1/schemas/api/schemas/search/%/directory.metapack
./explorer/self/v1/schemas/api/schemas/search/response
./explorer/self/v1/schemas/api/schemas/search/response/%
./explorer/self/v1/schemas/api/schemas/search/response/%/schema-html.metapack
./explorer/self/v1/schemas/api/schemas/search/response/%/schema.metapack
./explorer/self/v1/schemas/api/schemas/stats
./explorer/self/v1/schemas/api/schemas/stats/%
./explorer/self/v1/schemas/api/schemas/stats/%/directory-html.metapack
./explorer/self/v1/schemas/api/schemas/stats/%/directory.metapack
./explorer/self/v1/schemas/api/schemas/stats/response
./explorer/self/v1/schemas/api/schemas/stats/response/%
./explorer/self/v1/schemas/api/schemas/stats/response/%/schema-html.metapack
./explorer/self/v1/schemas/api/schemas/stats/response/%/schema.metapack
./explorer/self/v1/schemas/api/schemas/trace
./explorer/self/v1/schemas/api/schemas/trace/%
./explorer/self/v1/schemas/api/schemas/trace/%/directory-html.metapack
./explorer/self/v1/schemas/api/schemas/trace/%/directory.metapack
./explorer/self/v1/schemas/api/schemas/trace/response
./explorer/self/v1/schemas/api/schemas/trace/response/%
./explorer/self/v1/schemas/api/schemas/trace/response/%/schema-html.metapack
./explorer/self/v1/schemas/api/schemas/trace/response/%/schema.metapack
./explorer/self/v1/schemas/configuration
./explorer/self/v1/schemas/configuration/%
./explorer/self/v1/schemas/configuration/%/directory-html.metapack
./explorer/self/v1/schemas/configuration/%/directory.metapack
./explorer/self/v1/schemas/configuration/collection
./explorer/self/v1/schemas/configuration/collection/%
./explorer/self/v1/schemas/configuration/collection/%/schema-html.metapack
./explorer/self/v1/schemas/configuration/collection/%/schema.metapack
./explorer/self/v1/schemas/configuration/configuration
./explorer/self/v1/schemas/configuration/configuration/%
./explorer/self/v1/schemas/configuration/configuration/%/schema-html.metapack
./explorer/self/v1/schemas/configuration/configuration/%/schema.metapack
./explorer/self/v1/schemas/configuration/contents
./explorer/self/v1/schemas/configuration/contents/%
./explorer/self/v1/schemas/configuration/contents/%/schema-html.metapack
./explorer/self/v1/schemas/configuration/contents/%/schema.metapack
./explorer/self/v1/schemas/configuration/extends
./explorer/self/v1/schemas/configuration/extends/%
./explorer/self/v1/schemas/configuration/extends/%/schema-html.metapack
./explorer/self/v1/schemas/configuration/extends/%/schema.metapack
./explorer/self/v1/schemas/configuration/page
./explorer/self/v1/schemas/configuration/page/%
./explorer/self/v1/schemas/configuration/page/%/schema-html.metapack
./explorer/self/v1/schemas/configuration/page/%/schema.metapack
./explorer/self/v1/schemas/configuration/path
./explorer/self/v1/schemas/configuration/path/%
./explorer/self/v1/schemas/configuration/path/%/schema-html.metapack
./explorer/self/v1/schemas/configuration/path/%/schema.metapack
./explorer/self/v1/schemas/configuration/rpath
./explorer/self/v1/schemas/configuration/rpath/%
./explorer/self/v1/schemas/configuration/rpath/%/schema-html.metapack
./explorer/self/v1/schemas/configuration/rpath/%/schema.metapack
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
./schemas/self/v1/schemas/api/schemas/evaluate
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
./schemas/self/v1/schemas/api/schemas/trace
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
./schemas/self/v1/schemas/configuration
./schemas/self/v1/schemas/configuration/collection
./schemas/self/v1/schemas/configuration/collection/%
./schemas/self/v1/schemas/configuration/collection/%/blaze-exhaustive.metapack
./schemas/self/v1/schemas/configuration/collection/%/blaze-fast.metapack
./schemas/self/v1/schemas/configuration/collection/%/bundle.metapack
./schemas/self/v1/schemas/configuration/collection/%/dependencies.metapack
./schemas/self/v1/schemas/configuration/collection/%/dependents.metapack
./schemas/self/v1/schemas/configuration/collection/%/editor.metapack
./schemas/self/v1/schemas/configuration/collection/%/health.metapack
./schemas/self/v1/schemas/configuration/collection/%/locations.metapack
./schemas/self/v1/schemas/configuration/collection/%/positions.metapack
./schemas/self/v1/schemas/configuration/collection/%/schema.metapack
./schemas/self/v1/schemas/configuration/collection/%/stats.metapack
./schemas/self/v1/schemas/configuration/configuration
./schemas/self/v1/schemas/configuration/configuration/%
./schemas/self/v1/schemas/configuration/configuration/%/blaze-exhaustive.metapack
./schemas/self/v1/schemas/configuration/configuration/%/blaze-fast.metapack
./schemas/self/v1/schemas/configuration/configuration/%/bundle.metapack
./schemas/self/v1/schemas/configuration/configuration/%/dependencies.metapack
./schemas/self/v1/schemas/configuration/configuration/%/dependents.metapack
./schemas/self/v1/schemas/configuration/configuration/%/editor.metapack
./schemas/self/v1/schemas/configuration/configuration/%/health.metapack
./schemas/self/v1/schemas/configuration/configuration/%/locations.metapack
./schemas/self/v1/schemas/configuration/configuration/%/positions.metapack
./schemas/self/v1/schemas/configuration/configuration/%/schema.metapack
./schemas/self/v1/schemas/configuration/configuration/%/stats.metapack
./schemas/self/v1/schemas/configuration/contents
./schemas/self/v1/schemas/configuration/contents/%
./schemas/self/v1/schemas/configuration/contents/%/blaze-exhaustive.metapack
./schemas/self/v1/schemas/configuration/contents/%/blaze-fast.metapack
./schemas/self/v1/schemas/configuration/contents/%/bundle.metapack
./schemas/self/v1/schemas/configuration/contents/%/dependencies.metapack
./schemas/self/v1/schemas/configuration/contents/%/dependents.metapack
./schemas/self/v1/schemas/configuration/contents/%/editor.metapack
./schemas/self/v1/schemas/configuration/contents/%/health.metapack
./schemas/self/v1/schemas/configuration/contents/%/locations.metapack
./schemas/self/v1/schemas/configuration/contents/%/positions.metapack
./schemas/self/v1/schemas/configuration/contents/%/schema.metapack
./schemas/self/v1/schemas/configuration/contents/%/stats.metapack
./schemas/self/v1/schemas/configuration/extends
./schemas/self/v1/schemas/configuration/extends/%
./schemas/self/v1/schemas/configuration/extends/%/blaze-exhaustive.metapack
./schemas/self/v1/schemas/configuration/extends/%/blaze-fast.metapack
./schemas/self/v1/schemas/configuration/extends/%/bundle.metapack
./schemas/self/v1/schemas/configuration/extends/%/dependencies.metapack
./schemas/self/v1/schemas/configuration/extends/%/dependents.metapack
./schemas/self/v1/schemas/configuration/extends/%/editor.metapack
./schemas/self/v1/schemas/configuration/extends/%/health.metapack
./schemas/self/v1/schemas/configuration/extends/%/locations.metapack
./schemas/self/v1/schemas/configuration/extends/%/positions.metapack
./schemas/self/v1/schemas/configuration/extends/%/schema.metapack
./schemas/self/v1/schemas/configuration/extends/%/stats.metapack
./schemas/self/v1/schemas/configuration/page
./schemas/self/v1/schemas/configuration/page/%
./schemas/self/v1/schemas/configuration/page/%/blaze-exhaustive.metapack
./schemas/self/v1/schemas/configuration/page/%/blaze-fast.metapack
./schemas/self/v1/schemas/configuration/page/%/bundle.metapack
./schemas/self/v1/schemas/configuration/page/%/dependencies.metapack
./schemas/self/v1/schemas/configuration/page/%/dependents.metapack
./schemas/self/v1/schemas/configuration/page/%/editor.metapack
./schemas/self/v1/schemas/configuration/page/%/health.metapack
./schemas/self/v1/schemas/configuration/page/%/locations.metapack
./schemas/self/v1/schemas/configuration/page/%/positions.metapack
./schemas/self/v1/schemas/configuration/page/%/schema.metapack
./schemas/self/v1/schemas/configuration/page/%/stats.metapack
./schemas/self/v1/schemas/configuration/path
./schemas/self/v1/schemas/configuration/path/%
./schemas/self/v1/schemas/configuration/path/%/blaze-exhaustive.metapack
./schemas/self/v1/schemas/configuration/path/%/blaze-fast.metapack
./schemas/self/v1/schemas/configuration/path/%/bundle.metapack
./schemas/self/v1/schemas/configuration/path/%/dependencies.metapack
./schemas/self/v1/schemas/configuration/path/%/dependents.metapack
./schemas/self/v1/schemas/configuration/path/%/editor.metapack
./schemas/self/v1/schemas/configuration/path/%/health.metapack
./schemas/self/v1/schemas/configuration/path/%/locations.metapack
./schemas/self/v1/schemas/configuration/path/%/positions.metapack
./schemas/self/v1/schemas/configuration/path/%/schema.metapack
./schemas/self/v1/schemas/configuration/path/%/stats.metapack
./schemas/self/v1/schemas/configuration/rpath
./schemas/self/v1/schemas/configuration/rpath/%
./schemas/self/v1/schemas/configuration/rpath/%/blaze-exhaustive.metapack
./schemas/self/v1/schemas/configuration/rpath/%/blaze-fast.metapack
./schemas/self/v1/schemas/configuration/rpath/%/bundle.metapack
./schemas/self/v1/schemas/configuration/rpath/%/dependencies.metapack
./schemas/self/v1/schemas/configuration/rpath/%/dependents.metapack
./schemas/self/v1/schemas/configuration/rpath/%/editor.metapack
./schemas/self/v1/schemas/configuration/rpath/%/health.metapack
./schemas/self/v1/schemas/configuration/rpath/%/locations.metapack
./schemas/self/v1/schemas/configuration/rpath/%/positions.metapack
./schemas/self/v1/schemas/configuration/rpath/%/schema.metapack
./schemas/self/v1/schemas/configuration/rpath/%/stats.metapack
./state.bin
./version.json
EOF
diff "$TMP/manifest.txt" "$TMP/expected_manifest.txt"
