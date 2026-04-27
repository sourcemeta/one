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

cat << 'EOF' > "$TMP/schemas/foo.json"
{
  "$schema": "http://json-schema.org/draft-07/schema#",
  "$id": "https://example.com/foo"
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

"$1" --skip-banner --deterministic "$TMP/one.json" "$TMP/output" -v --concurrency 1 2> "$TMP/output.txt"
remove_threads_information "$TMP/output.txt"
cat << EOF > "$TMP/expected.txt"
Writing output to: $(realpath "$TMP")/output
Using configuration: $(realpath "$TMP")/one.json
Detecting: $(realpath "$TMP")/schemas/foo.json (#1)
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
Detecting: $ONE_PREFIX/share/sourcemeta/one/collections/self/v1/schemas/configuration/page.json (#18)
(  5%) Resolving: example/schemas/foo.json
https://example.com/foo => https://sourcemeta.com/example/schemas/foo
( 11%) Resolving: self/v1/schemas/api/error.json
https://sourcemeta.com/api/error => https://sourcemeta.com/self/v1/schemas/api/error
( 16%) Resolving: self/v1/schemas/api/list/response.json
https://sourcemeta.com/api/list/response => https://sourcemeta.com/self/v1/schemas/api/list/response
( 22%) Resolving: self/v1/schemas/api/schemas/dependencies/response.json
https://sourcemeta.com/api/schemas/dependencies/response => https://sourcemeta.com/self/v1/schemas/api/schemas/dependencies/response
( 27%) Resolving: self/v1/schemas/api/schemas/dependents/response.json
https://sourcemeta.com/api/schemas/dependents/response => https://sourcemeta.com/self/v1/schemas/api/schemas/dependents/response
( 33%) Resolving: self/v1/schemas/api/schemas/evaluate/response.json
https://sourcemeta.com/api/schemas/evaluate/response => https://sourcemeta.com/self/v1/schemas/api/schemas/evaluate/response
( 38%) Resolving: self/v1/schemas/api/schemas/health/response.json
https://sourcemeta.com/api/schemas/health/response => https://sourcemeta.com/self/v1/schemas/api/schemas/health/response
( 44%) Resolving: self/v1/schemas/api/schemas/locations/response.json
https://sourcemeta.com/api/schemas/locations/response => https://sourcemeta.com/self/v1/schemas/api/schemas/locations/response
( 50%) Resolving: self/v1/schemas/api/schemas/metadata/response.json
https://sourcemeta.com/api/schemas/metadata/response => https://sourcemeta.com/self/v1/schemas/api/schemas/metadata/response
( 55%) Resolving: self/v1/schemas/api/schemas/position.json
https://sourcemeta.com/api/schemas/position => https://sourcemeta.com/self/v1/schemas/api/schemas/position
( 61%) Resolving: self/v1/schemas/api/schemas/positions/response.json
https://sourcemeta.com/api/schemas/positions/response => https://sourcemeta.com/self/v1/schemas/api/schemas/positions/response
( 66%) Resolving: self/v1/schemas/api/schemas/search/response.json
https://sourcemeta.com/api/schemas/search/response => https://sourcemeta.com/self/v1/schemas/api/schemas/search/response
( 72%) Resolving: self/v1/schemas/api/schemas/stats/response.json
https://sourcemeta.com/api/schemas/stats/response => https://sourcemeta.com/self/v1/schemas/api/schemas/stats/response
( 77%) Resolving: self/v1/schemas/api/schemas/trace/response.json
https://sourcemeta.com/api/schemas/trace/response => https://sourcemeta.com/self/v1/schemas/api/schemas/trace/response
( 83%) Resolving: self/v1/schemas/configuration/collection.json
https://sourcemeta.com/configuration/collection => https://sourcemeta.com/self/v1/schemas/configuration/collection
( 88%) Resolving: self/v1/schemas/configuration/configuration.json
https://sourcemeta.com/configuration/configuration => https://sourcemeta.com/self/v1/schemas/configuration/configuration
( 94%) Resolving: self/v1/schemas/configuration/contents.json
https://sourcemeta.com/configuration/contents => https://sourcemeta.com/self/v1/schemas/configuration/contents
(100%) Resolving: self/v1/schemas/configuration/page.json
https://sourcemeta.com/configuration/page => https://sourcemeta.com/self/v1/schemas/configuration/page
(  0%) Producing: configuration.json
(  0%) Producing: version.json
(  1%) Producing: explorer/%/404.metapack
(  1%) Producing: schemas/example/schemas/foo/%/schema.metapack
(  1%) Producing: schemas/self/v1/schemas/api/error/%/schema.metapack
(  2%) Producing: schemas/self/v1/schemas/api/list/response/%/schema.metapack
(  2%) Producing: schemas/self/v1/schemas/api/schemas/dependencies/response/%/schema.metapack
(  3%) Producing: schemas/self/v1/schemas/api/schemas/dependents/response/%/schema.metapack
(  3%) Producing: schemas/self/v1/schemas/api/schemas/evaluate/response/%/schema.metapack
(  3%) Producing: schemas/self/v1/schemas/api/schemas/health/response/%/schema.metapack
(  4%) Producing: schemas/self/v1/schemas/api/schemas/locations/response/%/schema.metapack
(  4%) Producing: schemas/self/v1/schemas/api/schemas/metadata/response/%/schema.metapack
(  4%) Producing: schemas/self/v1/schemas/api/schemas/position/%/schema.metapack
(  5%) Producing: schemas/self/v1/schemas/api/schemas/positions/response/%/schema.metapack
(  5%) Producing: schemas/self/v1/schemas/api/schemas/search/response/%/schema.metapack
(  6%) Producing: schemas/self/v1/schemas/api/schemas/stats/response/%/schema.metapack
(  6%) Producing: schemas/self/v1/schemas/api/schemas/trace/response/%/schema.metapack
(  6%) Producing: schemas/self/v1/schemas/configuration/collection/%/schema.metapack
(  7%) Producing: schemas/self/v1/schemas/configuration/configuration/%/schema.metapack
(  7%) Producing: schemas/self/v1/schemas/configuration/contents/%/schema.metapack
(  8%) Producing: schemas/self/v1/schemas/configuration/page/%/schema.metapack
(  8%) Producing: schemas/example/schemas/foo/%/dependencies.metapack
(  8%) Producing: schemas/example/schemas/foo/%/locations.metapack
(  9%) Producing: schemas/example/schemas/foo/%/positions.metapack
(  9%) Producing: schemas/example/schemas/foo/%/stats.metapack
(  9%) Producing: schemas/self/v1/schemas/api/error/%/dependencies.metapack
( 10%) Producing: schemas/self/v1/schemas/api/error/%/locations.metapack
( 10%) Producing: schemas/self/v1/schemas/api/error/%/positions.metapack
( 11%) Producing: schemas/self/v1/schemas/api/error/%/stats.metapack
( 11%) Producing: schemas/self/v1/schemas/api/list/response/%/dependencies.metapack
( 11%) Producing: schemas/self/v1/schemas/api/list/response/%/locations.metapack
( 12%) Producing: schemas/self/v1/schemas/api/list/response/%/positions.metapack
( 12%) Producing: schemas/self/v1/schemas/api/list/response/%/stats.metapack
( 13%) Producing: schemas/self/v1/schemas/api/schemas/dependencies/response/%/dependencies.metapack
( 13%) Producing: schemas/self/v1/schemas/api/schemas/dependencies/response/%/locations.metapack
( 13%) Producing: schemas/self/v1/schemas/api/schemas/dependencies/response/%/positions.metapack
( 14%) Producing: schemas/self/v1/schemas/api/schemas/dependencies/response/%/stats.metapack
( 14%) Producing: schemas/self/v1/schemas/api/schemas/dependents/response/%/dependencies.metapack
( 14%) Producing: schemas/self/v1/schemas/api/schemas/dependents/response/%/locations.metapack
( 15%) Producing: schemas/self/v1/schemas/api/schemas/dependents/response/%/positions.metapack
( 15%) Producing: schemas/self/v1/schemas/api/schemas/dependents/response/%/stats.metapack
( 16%) Producing: schemas/self/v1/schemas/api/schemas/evaluate/response/%/dependencies.metapack
( 16%) Producing: schemas/self/v1/schemas/api/schemas/evaluate/response/%/locations.metapack
( 16%) Producing: schemas/self/v1/schemas/api/schemas/evaluate/response/%/positions.metapack
( 17%) Producing: schemas/self/v1/schemas/api/schemas/evaluate/response/%/stats.metapack
( 17%) Producing: schemas/self/v1/schemas/api/schemas/health/response/%/dependencies.metapack
( 18%) Producing: schemas/self/v1/schemas/api/schemas/health/response/%/locations.metapack
( 18%) Producing: schemas/self/v1/schemas/api/schemas/health/response/%/positions.metapack
( 18%) Producing: schemas/self/v1/schemas/api/schemas/health/response/%/stats.metapack
( 19%) Producing: schemas/self/v1/schemas/api/schemas/locations/response/%/dependencies.metapack
( 19%) Producing: schemas/self/v1/schemas/api/schemas/locations/response/%/locations.metapack
( 19%) Producing: schemas/self/v1/schemas/api/schemas/locations/response/%/positions.metapack
( 20%) Producing: schemas/self/v1/schemas/api/schemas/locations/response/%/stats.metapack
( 20%) Producing: schemas/self/v1/schemas/api/schemas/metadata/response/%/dependencies.metapack
( 21%) Producing: schemas/self/v1/schemas/api/schemas/metadata/response/%/locations.metapack
( 21%) Producing: schemas/self/v1/schemas/api/schemas/metadata/response/%/positions.metapack
( 21%) Producing: schemas/self/v1/schemas/api/schemas/metadata/response/%/stats.metapack
( 22%) Producing: schemas/self/v1/schemas/api/schemas/position/%/dependencies.metapack
( 22%) Producing: schemas/self/v1/schemas/api/schemas/position/%/locations.metapack
( 22%) Producing: schemas/self/v1/schemas/api/schemas/position/%/positions.metapack
( 23%) Producing: schemas/self/v1/schemas/api/schemas/position/%/stats.metapack
( 23%) Producing: schemas/self/v1/schemas/api/schemas/positions/response/%/dependencies.metapack
( 24%) Producing: schemas/self/v1/schemas/api/schemas/positions/response/%/locations.metapack
( 24%) Producing: schemas/self/v1/schemas/api/schemas/positions/response/%/positions.metapack
( 24%) Producing: schemas/self/v1/schemas/api/schemas/positions/response/%/stats.metapack
( 25%) Producing: schemas/self/v1/schemas/api/schemas/search/response/%/dependencies.metapack
( 25%) Producing: schemas/self/v1/schemas/api/schemas/search/response/%/locations.metapack
( 26%) Producing: schemas/self/v1/schemas/api/schemas/search/response/%/positions.metapack
( 26%) Producing: schemas/self/v1/schemas/api/schemas/search/response/%/stats.metapack
( 26%) Producing: schemas/self/v1/schemas/api/schemas/stats/response/%/dependencies.metapack
( 27%) Producing: schemas/self/v1/schemas/api/schemas/stats/response/%/locations.metapack
( 27%) Producing: schemas/self/v1/schemas/api/schemas/stats/response/%/positions.metapack
( 27%) Producing: schemas/self/v1/schemas/api/schemas/stats/response/%/stats.metapack
( 28%) Producing: schemas/self/v1/schemas/api/schemas/trace/response/%/dependencies.metapack
( 28%) Producing: schemas/self/v1/schemas/api/schemas/trace/response/%/locations.metapack
( 29%) Producing: schemas/self/v1/schemas/api/schemas/trace/response/%/positions.metapack
( 29%) Producing: schemas/self/v1/schemas/api/schemas/trace/response/%/stats.metapack
( 29%) Producing: schemas/self/v1/schemas/configuration/collection/%/dependencies.metapack
( 30%) Producing: schemas/self/v1/schemas/configuration/collection/%/locations.metapack
( 30%) Producing: schemas/self/v1/schemas/configuration/collection/%/positions.metapack
( 31%) Producing: schemas/self/v1/schemas/configuration/collection/%/stats.metapack
( 31%) Producing: schemas/self/v1/schemas/configuration/configuration/%/dependencies.metapack
( 31%) Producing: schemas/self/v1/schemas/configuration/configuration/%/locations.metapack
( 32%) Producing: schemas/self/v1/schemas/configuration/configuration/%/positions.metapack
( 32%) Producing: schemas/self/v1/schemas/configuration/configuration/%/stats.metapack
( 32%) Producing: schemas/self/v1/schemas/configuration/contents/%/dependencies.metapack
( 33%) Producing: schemas/self/v1/schemas/configuration/contents/%/locations.metapack
( 33%) Producing: schemas/self/v1/schemas/configuration/contents/%/positions.metapack
( 34%) Producing: schemas/self/v1/schemas/configuration/contents/%/stats.metapack
( 34%) Producing: schemas/self/v1/schemas/configuration/page/%/dependencies.metapack
( 34%) Producing: schemas/self/v1/schemas/configuration/page/%/locations.metapack
( 35%) Producing: schemas/self/v1/schemas/configuration/page/%/positions.metapack
( 35%) Producing: schemas/self/v1/schemas/configuration/page/%/stats.metapack
( 36%) Producing: schemas/example/schemas/foo/%/bundle.metapack
( 36%) Producing: schemas/example/schemas/foo/%/health.metapack
( 36%) Producing: schemas/self/v1/schemas/api/error/%/bundle.metapack
( 37%) Producing: schemas/self/v1/schemas/api/error/%/health.metapack
( 37%) Producing: schemas/self/v1/schemas/api/list/response/%/bundle.metapack
( 37%) Producing: schemas/self/v1/schemas/api/list/response/%/health.metapack
( 38%) Producing: schemas/self/v1/schemas/api/schemas/dependencies/response/%/bundle.metapack
( 38%) Producing: schemas/self/v1/schemas/api/schemas/dependencies/response/%/health.metapack
( 39%) Producing: schemas/self/v1/schemas/api/schemas/dependents/response/%/bundle.metapack
( 39%) Producing: schemas/self/v1/schemas/api/schemas/dependents/response/%/health.metapack
( 39%) Producing: schemas/self/v1/schemas/api/schemas/evaluate/response/%/bundle.metapack
( 40%) Producing: schemas/self/v1/schemas/api/schemas/evaluate/response/%/health.metapack
( 40%) Producing: schemas/self/v1/schemas/api/schemas/health/response/%/bundle.metapack
( 40%) Producing: schemas/self/v1/schemas/api/schemas/health/response/%/health.metapack
( 41%) Producing: schemas/self/v1/schemas/api/schemas/locations/response/%/bundle.metapack
( 41%) Producing: schemas/self/v1/schemas/api/schemas/locations/response/%/health.metapack
( 42%) Producing: schemas/self/v1/schemas/api/schemas/metadata/response/%/bundle.metapack
( 42%) Producing: schemas/self/v1/schemas/api/schemas/metadata/response/%/health.metapack
( 42%) Producing: schemas/self/v1/schemas/api/schemas/position/%/bundle.metapack
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
( 47%) Producing: schemas/self/v1/schemas/configuration/collection/%/health.metapack
( 47%) Producing: schemas/self/v1/schemas/configuration/configuration/%/bundle.metapack
( 47%) Producing: schemas/self/v1/schemas/configuration/configuration/%/health.metapack
( 48%) Producing: schemas/self/v1/schemas/configuration/contents/%/bundle.metapack
( 48%) Producing: schemas/self/v1/schemas/configuration/contents/%/health.metapack
( 49%) Producing: schemas/self/v1/schemas/configuration/page/%/bundle.metapack
( 49%) Producing: schemas/self/v1/schemas/configuration/page/%/health.metapack
( 49%) Producing: explorer/example/schemas/foo/%/schema.metapack
( 50%) Producing: explorer/self/v1/schemas/api/error/%/schema.metapack
( 50%) Producing: explorer/self/v1/schemas/api/list/response/%/schema.metapack
( 50%) Producing: explorer/self/v1/schemas/api/schemas/dependencies/response/%/schema.metapack
( 51%) Producing: explorer/self/v1/schemas/api/schemas/dependents/response/%/schema.metapack
( 51%) Producing: explorer/self/v1/schemas/api/schemas/evaluate/response/%/schema.metapack
( 52%) Producing: explorer/self/v1/schemas/api/schemas/health/response/%/schema.metapack
( 52%) Producing: explorer/self/v1/schemas/api/schemas/locations/response/%/schema.metapack
( 52%) Producing: explorer/self/v1/schemas/api/schemas/metadata/response/%/schema.metapack
( 53%) Producing: explorer/self/v1/schemas/api/schemas/position/%/schema.metapack
( 53%) Producing: explorer/self/v1/schemas/api/schemas/positions/response/%/schema.metapack
( 54%) Producing: explorer/self/v1/schemas/api/schemas/search/response/%/schema.metapack
( 54%) Producing: explorer/self/v1/schemas/api/schemas/stats/response/%/schema.metapack
( 54%) Producing: explorer/self/v1/schemas/api/schemas/trace/response/%/schema.metapack
( 55%) Producing: explorer/self/v1/schemas/configuration/collection/%/schema.metapack
( 55%) Producing: explorer/self/v1/schemas/configuration/configuration/%/schema.metapack
( 55%) Producing: explorer/self/v1/schemas/configuration/contents/%/schema.metapack
( 56%) Producing: explorer/self/v1/schemas/configuration/page/%/schema.metapack
( 56%) Producing: schemas/example/schemas/foo/%/blaze-exhaustive.metapack
( 57%) Producing: schemas/example/schemas/foo/%/blaze-fast.metapack
( 57%) Producing: schemas/example/schemas/foo/%/editor.metapack
( 57%) Producing: schemas/self/v1/schemas/api/error/%/blaze-exhaustive.metapack
( 58%) Producing: schemas/self/v1/schemas/api/error/%/blaze-fast.metapack
( 58%) Producing: schemas/self/v1/schemas/api/error/%/editor.metapack
( 59%) Producing: schemas/self/v1/schemas/api/list/response/%/blaze-exhaustive.metapack
( 59%) Producing: schemas/self/v1/schemas/api/list/response/%/blaze-fast.metapack
( 59%) Producing: schemas/self/v1/schemas/api/list/response/%/editor.metapack
( 60%) Producing: schemas/self/v1/schemas/api/schemas/dependencies/response/%/blaze-exhaustive.metapack
( 60%) Producing: schemas/self/v1/schemas/api/schemas/dependencies/response/%/blaze-fast.metapack
( 60%) Producing: schemas/self/v1/schemas/api/schemas/dependencies/response/%/editor.metapack
( 61%) Producing: schemas/self/v1/schemas/api/schemas/dependents/response/%/blaze-exhaustive.metapack
( 61%) Producing: schemas/self/v1/schemas/api/schemas/dependents/response/%/blaze-fast.metapack
( 62%) Producing: schemas/self/v1/schemas/api/schemas/dependents/response/%/editor.metapack
( 62%) Producing: schemas/self/v1/schemas/api/schemas/evaluate/response/%/blaze-exhaustive.metapack
( 62%) Producing: schemas/self/v1/schemas/api/schemas/evaluate/response/%/blaze-fast.metapack
( 63%) Producing: schemas/self/v1/schemas/api/schemas/evaluate/response/%/editor.metapack
( 63%) Producing: schemas/self/v1/schemas/api/schemas/health/response/%/blaze-exhaustive.metapack
( 63%) Producing: schemas/self/v1/schemas/api/schemas/health/response/%/blaze-fast.metapack
( 64%) Producing: schemas/self/v1/schemas/api/schemas/health/response/%/editor.metapack
( 64%) Producing: schemas/self/v1/schemas/api/schemas/locations/response/%/blaze-exhaustive.metapack
( 65%) Producing: schemas/self/v1/schemas/api/schemas/locations/response/%/blaze-fast.metapack
( 65%) Producing: schemas/self/v1/schemas/api/schemas/locations/response/%/editor.metapack
( 65%) Producing: schemas/self/v1/schemas/api/schemas/metadata/response/%/blaze-exhaustive.metapack
( 66%) Producing: schemas/self/v1/schemas/api/schemas/metadata/response/%/blaze-fast.metapack
( 66%) Producing: schemas/self/v1/schemas/api/schemas/metadata/response/%/editor.metapack
( 67%) Producing: schemas/self/v1/schemas/api/schemas/position/%/blaze-exhaustive.metapack
( 67%) Producing: schemas/self/v1/schemas/api/schemas/position/%/blaze-fast.metapack
( 67%) Producing: schemas/self/v1/schemas/api/schemas/position/%/editor.metapack
( 68%) Producing: schemas/self/v1/schemas/api/schemas/positions/response/%/blaze-exhaustive.metapack
( 68%) Producing: schemas/self/v1/schemas/api/schemas/positions/response/%/blaze-fast.metapack
( 68%) Producing: schemas/self/v1/schemas/api/schemas/positions/response/%/editor.metapack
( 69%) Producing: schemas/self/v1/schemas/api/schemas/search/response/%/blaze-exhaustive.metapack
( 69%) Producing: schemas/self/v1/schemas/api/schemas/search/response/%/blaze-fast.metapack
( 70%) Producing: schemas/self/v1/schemas/api/schemas/search/response/%/editor.metapack
( 70%) Producing: schemas/self/v1/schemas/api/schemas/stats/response/%/blaze-exhaustive.metapack
( 70%) Producing: schemas/self/v1/schemas/api/schemas/stats/response/%/blaze-fast.metapack
( 71%) Producing: schemas/self/v1/schemas/api/schemas/stats/response/%/editor.metapack
( 71%) Producing: schemas/self/v1/schemas/api/schemas/trace/response/%/blaze-exhaustive.metapack
( 72%) Producing: schemas/self/v1/schemas/api/schemas/trace/response/%/blaze-fast.metapack
( 72%) Producing: schemas/self/v1/schemas/api/schemas/trace/response/%/editor.metapack
( 72%) Producing: schemas/self/v1/schemas/configuration/collection/%/blaze-exhaustive.metapack
( 73%) Producing: schemas/self/v1/schemas/configuration/collection/%/blaze-fast.metapack
( 73%) Producing: schemas/self/v1/schemas/configuration/collection/%/editor.metapack
( 73%) Producing: schemas/self/v1/schemas/configuration/configuration/%/blaze-exhaustive.metapack
( 74%) Producing: schemas/self/v1/schemas/configuration/configuration/%/blaze-fast.metapack
( 74%) Producing: schemas/self/v1/schemas/configuration/configuration/%/editor.metapack
( 75%) Producing: schemas/self/v1/schemas/configuration/contents/%/blaze-exhaustive.metapack
( 75%) Producing: schemas/self/v1/schemas/configuration/contents/%/blaze-fast.metapack
( 75%) Producing: schemas/self/v1/schemas/configuration/contents/%/editor.metapack
( 76%) Producing: schemas/self/v1/schemas/configuration/page/%/blaze-exhaustive.metapack
( 76%) Producing: schemas/self/v1/schemas/configuration/page/%/blaze-fast.metapack
( 77%) Producing: schemas/self/v1/schemas/configuration/page/%/editor.metapack
( 77%) Producing: explorer/example/schemas/%/directory.metapack
( 77%) Producing: explorer/example/schemas/foo/%/schema-html.metapack
( 78%) Producing: explorer/self/v1/schemas/api/error/%/schema-html.metapack
( 78%) Producing: explorer/self/v1/schemas/api/list/%/directory.metapack
( 78%) Producing: explorer/self/v1/schemas/api/list/response/%/schema-html.metapack
( 79%) Producing: explorer/self/v1/schemas/api/schemas/dependencies/%/directory.metapack
( 79%) Producing: explorer/self/v1/schemas/api/schemas/dependencies/response/%/schema-html.metapack
( 80%) Producing: explorer/self/v1/schemas/api/schemas/dependents/%/directory.metapack
( 80%) Producing: explorer/self/v1/schemas/api/schemas/dependents/response/%/schema-html.metapack
( 80%) Producing: explorer/self/v1/schemas/api/schemas/evaluate/%/directory.metapack
( 81%) Producing: explorer/self/v1/schemas/api/schemas/evaluate/response/%/schema-html.metapack
( 81%) Producing: explorer/self/v1/schemas/api/schemas/health/%/directory.metapack
( 81%) Producing: explorer/self/v1/schemas/api/schemas/health/response/%/schema-html.metapack
( 82%) Producing: explorer/self/v1/schemas/api/schemas/locations/%/directory.metapack
( 82%) Producing: explorer/self/v1/schemas/api/schemas/locations/response/%/schema-html.metapack
( 83%) Producing: explorer/self/v1/schemas/api/schemas/metadata/%/directory.metapack
( 83%) Producing: explorer/self/v1/schemas/api/schemas/metadata/response/%/schema-html.metapack
( 83%) Producing: explorer/self/v1/schemas/api/schemas/position/%/schema-html.metapack
( 84%) Producing: explorer/self/v1/schemas/api/schemas/positions/%/directory.metapack
( 84%) Producing: explorer/self/v1/schemas/api/schemas/positions/response/%/schema-html.metapack
( 85%) Producing: explorer/self/v1/schemas/api/schemas/search/%/directory.metapack
( 85%) Producing: explorer/self/v1/schemas/api/schemas/search/response/%/schema-html.metapack
( 85%) Producing: explorer/self/v1/schemas/api/schemas/stats/%/directory.metapack
( 86%) Producing: explorer/self/v1/schemas/api/schemas/stats/response/%/schema-html.metapack
( 86%) Producing: explorer/self/v1/schemas/api/schemas/trace/%/directory.metapack
( 86%) Producing: explorer/self/v1/schemas/api/schemas/trace/response/%/schema-html.metapack
( 87%) Producing: explorer/self/v1/schemas/configuration/%/directory.metapack
( 87%) Producing: explorer/self/v1/schemas/configuration/collection/%/schema-html.metapack
( 88%) Producing: explorer/self/v1/schemas/configuration/configuration/%/schema-html.metapack
( 88%) Producing: explorer/self/v1/schemas/configuration/contents/%/schema-html.metapack
( 88%) Producing: explorer/self/v1/schemas/configuration/page/%/schema-html.metapack
( 89%) Producing: explorer/example/%/directory.metapack
( 89%) Producing: explorer/example/schemas/%/directory-html.metapack
( 90%) Producing: explorer/self/v1/schemas/api/list/%/directory-html.metapack
( 90%) Producing: explorer/self/v1/schemas/api/schemas/%/directory.metapack
( 90%) Producing: explorer/self/v1/schemas/api/schemas/dependencies/%/directory-html.metapack
( 91%) Producing: explorer/self/v1/schemas/api/schemas/dependents/%/directory-html.metapack
( 91%) Producing: explorer/self/v1/schemas/api/schemas/evaluate/%/directory-html.metapack
( 91%) Producing: explorer/self/v1/schemas/api/schemas/health/%/directory-html.metapack
( 92%) Producing: explorer/self/v1/schemas/api/schemas/locations/%/directory-html.metapack
( 92%) Producing: explorer/self/v1/schemas/api/schemas/metadata/%/directory-html.metapack
( 93%) Producing: explorer/self/v1/schemas/api/schemas/positions/%/directory-html.metapack
( 93%) Producing: explorer/self/v1/schemas/api/schemas/search/%/directory-html.metapack
( 93%) Producing: explorer/self/v1/schemas/api/schemas/stats/%/directory-html.metapack
( 94%) Producing: explorer/self/v1/schemas/api/schemas/trace/%/directory-html.metapack
( 94%) Producing: explorer/self/v1/schemas/configuration/%/directory-html.metapack
( 95%) Producing: explorer/example/%/directory-html.metapack
( 95%) Producing: explorer/self/v1/schemas/api/%/directory.metapack
( 95%) Producing: explorer/self/v1/schemas/api/schemas/%/directory-html.metapack
( 96%) Producing: explorer/self/v1/schemas/%/directory.metapack
( 96%) Producing: explorer/self/v1/schemas/api/%/directory-html.metapack
( 96%) Producing: explorer/self/v1/%/directory.metapack
( 97%) Producing: explorer/self/v1/schemas/%/directory-html.metapack
( 97%) Producing: explorer/self/%/directory.metapack
( 98%) Producing: explorer/self/v1/%/directory-html.metapack
( 98%) Producing: explorer/%/directory.metapack
( 98%) Producing: explorer/self/%/directory-html.metapack
( 99%) Producing: explorer/%/directory-html.metapack
( 99%) Producing: explorer/%/search.metapack
(100%) Producing: routes.bin
(  5%) Combining: schemas/example/schemas/foo/%/dependents.metapack
( 11%) Combining: schemas/self/v1/schemas/api/error/%/dependents.metapack
( 16%) Combining: schemas/self/v1/schemas/api/list/response/%/dependents.metapack
( 22%) Combining: schemas/self/v1/schemas/api/schemas/dependencies/response/%/dependents.metapack
( 27%) Combining: schemas/self/v1/schemas/api/schemas/dependents/response/%/dependents.metapack
( 33%) Combining: schemas/self/v1/schemas/api/schemas/evaluate/response/%/dependents.metapack
( 38%) Combining: schemas/self/v1/schemas/api/schemas/health/response/%/dependents.metapack
( 44%) Combining: schemas/self/v1/schemas/api/schemas/locations/response/%/dependents.metapack
( 50%) Combining: schemas/self/v1/schemas/api/schemas/metadata/response/%/dependents.metapack
( 55%) Combining: schemas/self/v1/schemas/api/schemas/position/%/dependents.metapack
( 61%) Combining: schemas/self/v1/schemas/api/schemas/positions/response/%/dependents.metapack
( 66%) Combining: schemas/self/v1/schemas/api/schemas/search/response/%/dependents.metapack
( 72%) Combining: schemas/self/v1/schemas/api/schemas/stats/response/%/dependents.metapack
( 77%) Combining: schemas/self/v1/schemas/api/schemas/trace/response/%/dependents.metapack
( 83%) Combining: schemas/self/v1/schemas/configuration/collection/%/dependents.metapack
( 88%) Combining: schemas/self/v1/schemas/configuration/configuration/%/dependents.metapack
( 94%) Combining: schemas/self/v1/schemas/configuration/contents/%/dependents.metapack
(100%) Combining: schemas/self/v1/schemas/configuration/page/%/dependents.metapack
EOF
diff "$TMP/output.txt" "$TMP/expected.txt"
