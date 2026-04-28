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
(  7%) Resolving: self/v1/schemas/api/error.json
( 15%) Resolving: self/v1/schemas/api/list/response.json
( 23%) Resolving: self/v1/schemas/api/schemas/dependencies/response.json
( 30%) Resolving: self/v1/schemas/api/schemas/dependents/response.json
( 38%) Resolving: self/v1/schemas/api/schemas/evaluate/response.json
( 46%) Resolving: self/v1/schemas/api/schemas/health/response.json
( 53%) Resolving: self/v1/schemas/api/schemas/locations/response.json
( 61%) Resolving: self/v1/schemas/api/schemas/metadata/response.json
( 69%) Resolving: self/v1/schemas/api/schemas/position.json
( 76%) Resolving: self/v1/schemas/api/schemas/positions/response.json
( 84%) Resolving: self/v1/schemas/api/schemas/search/response.json
( 92%) Resolving: self/v1/schemas/api/schemas/stats/response.json
(100%) Resolving: self/v1/schemas/api/schemas/trace/response.json
(  0%) Producing: configuration.json
(  1%) Producing: version.json
(  1%) Producing: explorer/%/404.metapack
(  2%) Producing: schemas/self/v1/schemas/api/error/%/schema.metapack
(  2%) Producing: schemas/self/v1/schemas/api/list/response/%/schema.metapack
(  3%) Producing: schemas/self/v1/schemas/api/schemas/dependencies/response/%/schema.metapack
(  3%) Producing: schemas/self/v1/schemas/api/schemas/dependents/response/%/schema.metapack
(  4%) Producing: schemas/self/v1/schemas/api/schemas/evaluate/response/%/schema.metapack
(  4%) Producing: schemas/self/v1/schemas/api/schemas/health/response/%/schema.metapack
(  5%) Producing: schemas/self/v1/schemas/api/schemas/locations/response/%/schema.metapack
(  5%) Producing: schemas/self/v1/schemas/api/schemas/metadata/response/%/schema.metapack
(  6%) Producing: schemas/self/v1/schemas/api/schemas/position/%/schema.metapack
(  6%) Producing: schemas/self/v1/schemas/api/schemas/positions/response/%/schema.metapack
(  7%) Producing: schemas/self/v1/schemas/api/schemas/search/response/%/schema.metapack
(  7%) Producing: schemas/self/v1/schemas/api/schemas/stats/response/%/schema.metapack
(  8%) Producing: schemas/self/v1/schemas/api/schemas/trace/response/%/schema.metapack
(  8%) Producing: schemas/self/v1/schemas/api/error/%/dependencies.metapack
(  9%) Producing: schemas/self/v1/schemas/api/error/%/locations.metapack
(  9%) Producing: schemas/self/v1/schemas/api/error/%/positions.metapack
( 10%) Producing: schemas/self/v1/schemas/api/error/%/stats.metapack
( 10%) Producing: schemas/self/v1/schemas/api/list/response/%/dependencies.metapack
( 11%) Producing: schemas/self/v1/schemas/api/list/response/%/locations.metapack
( 11%) Producing: schemas/self/v1/schemas/api/list/response/%/positions.metapack
( 12%) Producing: schemas/self/v1/schemas/api/list/response/%/stats.metapack
( 12%) Producing: schemas/self/v1/schemas/api/schemas/dependencies/response/%/dependencies.metapack
( 13%) Producing: schemas/self/v1/schemas/api/schemas/dependencies/response/%/locations.metapack
( 13%) Producing: schemas/self/v1/schemas/api/schemas/dependencies/response/%/positions.metapack
( 14%) Producing: schemas/self/v1/schemas/api/schemas/dependencies/response/%/stats.metapack
( 14%) Producing: schemas/self/v1/schemas/api/schemas/dependents/response/%/dependencies.metapack
( 15%) Producing: schemas/self/v1/schemas/api/schemas/dependents/response/%/locations.metapack
( 15%) Producing: schemas/self/v1/schemas/api/schemas/dependents/response/%/positions.metapack
( 16%) Producing: schemas/self/v1/schemas/api/schemas/dependents/response/%/stats.metapack
( 16%) Producing: schemas/self/v1/schemas/api/schemas/evaluate/response/%/dependencies.metapack
( 17%) Producing: schemas/self/v1/schemas/api/schemas/evaluate/response/%/locations.metapack
( 17%) Producing: schemas/self/v1/schemas/api/schemas/evaluate/response/%/positions.metapack
( 18%) Producing: schemas/self/v1/schemas/api/schemas/evaluate/response/%/stats.metapack
( 18%) Producing: schemas/self/v1/schemas/api/schemas/health/response/%/dependencies.metapack
( 19%) Producing: schemas/self/v1/schemas/api/schemas/health/response/%/locations.metapack
( 20%) Producing: schemas/self/v1/schemas/api/schemas/health/response/%/positions.metapack
( 20%) Producing: schemas/self/v1/schemas/api/schemas/health/response/%/stats.metapack
( 21%) Producing: schemas/self/v1/schemas/api/schemas/locations/response/%/dependencies.metapack
( 21%) Producing: schemas/self/v1/schemas/api/schemas/locations/response/%/locations.metapack
( 22%) Producing: schemas/self/v1/schemas/api/schemas/locations/response/%/positions.metapack
( 22%) Producing: schemas/self/v1/schemas/api/schemas/locations/response/%/stats.metapack
( 23%) Producing: schemas/self/v1/schemas/api/schemas/metadata/response/%/dependencies.metapack
( 23%) Producing: schemas/self/v1/schemas/api/schemas/metadata/response/%/locations.metapack
( 24%) Producing: schemas/self/v1/schemas/api/schemas/metadata/response/%/positions.metapack
( 24%) Producing: schemas/self/v1/schemas/api/schemas/metadata/response/%/stats.metapack
( 25%) Producing: schemas/self/v1/schemas/api/schemas/position/%/dependencies.metapack
( 25%) Producing: schemas/self/v1/schemas/api/schemas/position/%/locations.metapack
( 26%) Producing: schemas/self/v1/schemas/api/schemas/position/%/positions.metapack
( 26%) Producing: schemas/self/v1/schemas/api/schemas/position/%/stats.metapack
( 27%) Producing: schemas/self/v1/schemas/api/schemas/positions/response/%/dependencies.metapack
( 27%) Producing: schemas/self/v1/schemas/api/schemas/positions/response/%/locations.metapack
( 28%) Producing: schemas/self/v1/schemas/api/schemas/positions/response/%/positions.metapack
( 28%) Producing: schemas/self/v1/schemas/api/schemas/positions/response/%/stats.metapack
( 29%) Producing: schemas/self/v1/schemas/api/schemas/search/response/%/dependencies.metapack
( 29%) Producing: schemas/self/v1/schemas/api/schemas/search/response/%/locations.metapack
( 30%) Producing: schemas/self/v1/schemas/api/schemas/search/response/%/positions.metapack
( 30%) Producing: schemas/self/v1/schemas/api/schemas/search/response/%/stats.metapack
( 31%) Producing: schemas/self/v1/schemas/api/schemas/stats/response/%/dependencies.metapack
( 31%) Producing: schemas/self/v1/schemas/api/schemas/stats/response/%/locations.metapack
( 32%) Producing: schemas/self/v1/schemas/api/schemas/stats/response/%/positions.metapack
( 32%) Producing: schemas/self/v1/schemas/api/schemas/stats/response/%/stats.metapack
( 33%) Producing: schemas/self/v1/schemas/api/schemas/trace/response/%/dependencies.metapack
( 33%) Producing: schemas/self/v1/schemas/api/schemas/trace/response/%/locations.metapack
( 34%) Producing: schemas/self/v1/schemas/api/schemas/trace/response/%/positions.metapack
( 34%) Producing: schemas/self/v1/schemas/api/schemas/trace/response/%/stats.metapack
( 35%) Producing: schemas/self/v1/schemas/api/error/%/bundle.metapack
( 35%) Producing: schemas/self/v1/schemas/api/error/%/health.metapack
( 36%) Producing: schemas/self/v1/schemas/api/list/response/%/bundle.metapack
( 36%) Producing: schemas/self/v1/schemas/api/list/response/%/health.metapack
( 37%) Producing: schemas/self/v1/schemas/api/schemas/dependencies/response/%/bundle.metapack
( 37%) Producing: schemas/self/v1/schemas/api/schemas/dependencies/response/%/health.metapack
( 38%) Producing: schemas/self/v1/schemas/api/schemas/dependents/response/%/bundle.metapack
( 38%) Producing: schemas/self/v1/schemas/api/schemas/dependents/response/%/health.metapack
( 39%) Producing: schemas/self/v1/schemas/api/schemas/evaluate/response/%/bundle.metapack
( 40%) Producing: schemas/self/v1/schemas/api/schemas/evaluate/response/%/health.metapack
( 40%) Producing: schemas/self/v1/schemas/api/schemas/health/response/%/bundle.metapack
( 41%) Producing: schemas/self/v1/schemas/api/schemas/health/response/%/health.metapack
( 41%) Producing: schemas/self/v1/schemas/api/schemas/locations/response/%/bundle.metapack
( 42%) Producing: schemas/self/v1/schemas/api/schemas/locations/response/%/health.metapack
( 42%) Producing: schemas/self/v1/schemas/api/schemas/metadata/response/%/bundle.metapack
( 43%) Producing: schemas/self/v1/schemas/api/schemas/metadata/response/%/health.metapack
( 43%) Producing: schemas/self/v1/schemas/api/schemas/position/%/bundle.metapack
( 44%) Producing: schemas/self/v1/schemas/api/schemas/position/%/health.metapack
( 44%) Producing: schemas/self/v1/schemas/api/schemas/positions/response/%/bundle.metapack
( 45%) Producing: schemas/self/v1/schemas/api/schemas/positions/response/%/health.metapack
( 45%) Producing: schemas/self/v1/schemas/api/schemas/search/response/%/bundle.metapack
( 46%) Producing: schemas/self/v1/schemas/api/schemas/search/response/%/health.metapack
( 46%) Producing: schemas/self/v1/schemas/api/schemas/stats/response/%/bundle.metapack
( 47%) Producing: schemas/self/v1/schemas/api/schemas/stats/response/%/health.metapack
( 47%) Producing: schemas/self/v1/schemas/api/schemas/trace/response/%/bundle.metapack
( 48%) Producing: schemas/self/v1/schemas/api/schemas/trace/response/%/health.metapack
( 48%) Producing: explorer/self/v1/schemas/api/error/%/schema.metapack
( 49%) Producing: explorer/self/v1/schemas/api/list/response/%/schema.metapack
( 49%) Producing: explorer/self/v1/schemas/api/schemas/dependencies/response/%/schema.metapack
( 50%) Producing: explorer/self/v1/schemas/api/schemas/dependents/response/%/schema.metapack
( 50%) Producing: explorer/self/v1/schemas/api/schemas/evaluate/response/%/schema.metapack
( 51%) Producing: explorer/self/v1/schemas/api/schemas/health/response/%/schema.metapack
( 51%) Producing: explorer/self/v1/schemas/api/schemas/locations/response/%/schema.metapack
( 52%) Producing: explorer/self/v1/schemas/api/schemas/metadata/response/%/schema.metapack
( 52%) Producing: explorer/self/v1/schemas/api/schemas/position/%/schema.metapack
( 53%) Producing: explorer/self/v1/schemas/api/schemas/positions/response/%/schema.metapack
( 53%) Producing: explorer/self/v1/schemas/api/schemas/search/response/%/schema.metapack
( 54%) Producing: explorer/self/v1/schemas/api/schemas/stats/response/%/schema.metapack
( 54%) Producing: explorer/self/v1/schemas/api/schemas/trace/response/%/schema.metapack
( 55%) Producing: schemas/self/v1/schemas/api/error/%/blaze-exhaustive.metapack
( 55%) Producing: schemas/self/v1/schemas/api/error/%/blaze-fast.metapack
( 56%) Producing: schemas/self/v1/schemas/api/error/%/editor.metapack
( 56%) Producing: schemas/self/v1/schemas/api/list/response/%/blaze-exhaustive.metapack
( 57%) Producing: schemas/self/v1/schemas/api/list/response/%/blaze-fast.metapack
( 57%) Producing: schemas/self/v1/schemas/api/list/response/%/editor.metapack
( 58%) Producing: schemas/self/v1/schemas/api/schemas/dependencies/response/%/blaze-exhaustive.metapack
( 58%) Producing: schemas/self/v1/schemas/api/schemas/dependencies/response/%/blaze-fast.metapack
( 59%) Producing: schemas/self/v1/schemas/api/schemas/dependencies/response/%/editor.metapack
( 60%) Producing: schemas/self/v1/schemas/api/schemas/dependents/response/%/blaze-exhaustive.metapack
( 60%) Producing: schemas/self/v1/schemas/api/schemas/dependents/response/%/blaze-fast.metapack
( 61%) Producing: schemas/self/v1/schemas/api/schemas/dependents/response/%/editor.metapack
( 61%) Producing: schemas/self/v1/schemas/api/schemas/evaluate/response/%/blaze-exhaustive.metapack
( 62%) Producing: schemas/self/v1/schemas/api/schemas/evaluate/response/%/blaze-fast.metapack
( 62%) Producing: schemas/self/v1/schemas/api/schemas/evaluate/response/%/editor.metapack
( 63%) Producing: schemas/self/v1/schemas/api/schemas/health/response/%/blaze-exhaustive.metapack
( 63%) Producing: schemas/self/v1/schemas/api/schemas/health/response/%/blaze-fast.metapack
( 64%) Producing: schemas/self/v1/schemas/api/schemas/health/response/%/editor.metapack
( 64%) Producing: schemas/self/v1/schemas/api/schemas/locations/response/%/blaze-exhaustive.metapack
( 65%) Producing: schemas/self/v1/schemas/api/schemas/locations/response/%/blaze-fast.metapack
( 65%) Producing: schemas/self/v1/schemas/api/schemas/locations/response/%/editor.metapack
( 66%) Producing: schemas/self/v1/schemas/api/schemas/metadata/response/%/blaze-exhaustive.metapack
( 66%) Producing: schemas/self/v1/schemas/api/schemas/metadata/response/%/blaze-fast.metapack
( 67%) Producing: schemas/self/v1/schemas/api/schemas/metadata/response/%/editor.metapack
( 67%) Producing: schemas/self/v1/schemas/api/schemas/position/%/blaze-exhaustive.metapack
( 68%) Producing: schemas/self/v1/schemas/api/schemas/position/%/blaze-fast.metapack
( 68%) Producing: schemas/self/v1/schemas/api/schemas/position/%/editor.metapack
( 69%) Producing: schemas/self/v1/schemas/api/schemas/positions/response/%/blaze-exhaustive.metapack
( 69%) Producing: schemas/self/v1/schemas/api/schemas/positions/response/%/blaze-fast.metapack
( 70%) Producing: schemas/self/v1/schemas/api/schemas/positions/response/%/editor.metapack
( 70%) Producing: schemas/self/v1/schemas/api/schemas/search/response/%/blaze-exhaustive.metapack
( 71%) Producing: schemas/self/v1/schemas/api/schemas/search/response/%/blaze-fast.metapack
( 71%) Producing: schemas/self/v1/schemas/api/schemas/search/response/%/editor.metapack
( 72%) Producing: schemas/self/v1/schemas/api/schemas/stats/response/%/blaze-exhaustive.metapack
( 72%) Producing: schemas/self/v1/schemas/api/schemas/stats/response/%/blaze-fast.metapack
( 73%) Producing: schemas/self/v1/schemas/api/schemas/stats/response/%/editor.metapack
( 73%) Producing: schemas/self/v1/schemas/api/schemas/trace/response/%/blaze-exhaustive.metapack
( 74%) Producing: schemas/self/v1/schemas/api/schemas/trace/response/%/blaze-fast.metapack
( 74%) Producing: schemas/self/v1/schemas/api/schemas/trace/response/%/editor.metapack
( 75%) Producing: explorer/self/v1/schemas/api/error/%/schema-html.metapack
( 75%) Producing: explorer/self/v1/schemas/api/list/%/directory.metapack
( 76%) Producing: explorer/self/v1/schemas/api/list/response/%/schema-html.metapack
( 76%) Producing: explorer/self/v1/schemas/api/schemas/dependencies/%/directory.metapack
( 77%) Producing: explorer/self/v1/schemas/api/schemas/dependencies/response/%/schema-html.metapack
( 77%) Producing: explorer/self/v1/schemas/api/schemas/dependents/%/directory.metapack
( 78%) Producing: explorer/self/v1/schemas/api/schemas/dependents/response/%/schema-html.metapack
( 78%) Producing: explorer/self/v1/schemas/api/schemas/evaluate/%/directory.metapack
( 79%) Producing: explorer/self/v1/schemas/api/schemas/evaluate/response/%/schema-html.metapack
( 80%) Producing: explorer/self/v1/schemas/api/schemas/health/%/directory.metapack
( 80%) Producing: explorer/self/v1/schemas/api/schemas/health/response/%/schema-html.metapack
( 81%) Producing: explorer/self/v1/schemas/api/schemas/locations/%/directory.metapack
( 81%) Producing: explorer/self/v1/schemas/api/schemas/locations/response/%/schema-html.metapack
( 82%) Producing: explorer/self/v1/schemas/api/schemas/metadata/%/directory.metapack
( 82%) Producing: explorer/self/v1/schemas/api/schemas/metadata/response/%/schema-html.metapack
( 83%) Producing: explorer/self/v1/schemas/api/schemas/position/%/schema-html.metapack
( 83%) Producing: explorer/self/v1/schemas/api/schemas/positions/%/directory.metapack
( 84%) Producing: explorer/self/v1/schemas/api/schemas/positions/response/%/schema-html.metapack
( 84%) Producing: explorer/self/v1/schemas/api/schemas/search/%/directory.metapack
( 85%) Producing: explorer/self/v1/schemas/api/schemas/search/response/%/schema-html.metapack
( 85%) Producing: explorer/self/v1/schemas/api/schemas/stats/%/directory.metapack
( 86%) Producing: explorer/self/v1/schemas/api/schemas/stats/response/%/schema-html.metapack
( 86%) Producing: explorer/self/v1/schemas/api/schemas/trace/%/directory.metapack
( 87%) Producing: explorer/self/v1/schemas/api/schemas/trace/response/%/schema-html.metapack
( 87%) Producing: explorer/self/v1/schemas/api/list/%/directory-html.metapack
( 88%) Producing: explorer/self/v1/schemas/api/schemas/%/directory.metapack
( 88%) Producing: explorer/self/v1/schemas/api/schemas/dependencies/%/directory-html.metapack
( 89%) Producing: explorer/self/v1/schemas/api/schemas/dependents/%/directory-html.metapack
( 89%) Producing: explorer/self/v1/schemas/api/schemas/evaluate/%/directory-html.metapack
( 90%) Producing: explorer/self/v1/schemas/api/schemas/health/%/directory-html.metapack
( 90%) Producing: explorer/self/v1/schemas/api/schemas/locations/%/directory-html.metapack
( 91%) Producing: explorer/self/v1/schemas/api/schemas/metadata/%/directory-html.metapack
( 91%) Producing: explorer/self/v1/schemas/api/schemas/positions/%/directory-html.metapack
( 92%) Producing: explorer/self/v1/schemas/api/schemas/search/%/directory-html.metapack
( 92%) Producing: explorer/self/v1/schemas/api/schemas/stats/%/directory-html.metapack
( 93%) Producing: explorer/self/v1/schemas/api/schemas/trace/%/directory-html.metapack
( 93%) Producing: explorer/self/v1/schemas/api/%/directory.metapack
( 94%) Producing: explorer/self/v1/schemas/api/schemas/%/directory-html.metapack
( 94%) Producing: explorer/self/v1/schemas/%/directory.metapack
( 95%) Producing: explorer/self/v1/schemas/api/%/directory-html.metapack
( 95%) Producing: explorer/self/v1/%/directory.metapack
( 96%) Producing: explorer/self/v1/schemas/%/directory-html.metapack
( 96%) Producing: explorer/self/%/directory.metapack
( 97%) Producing: explorer/self/v1/%/directory-html.metapack
( 97%) Producing: explorer/%/directory.metapack
( 98%) Producing: explorer/self/%/directory-html.metapack
( 98%) Producing: explorer/%/directory-html.metapack
( 99%) Producing: explorer/%/search.metapack
(100%) Producing: routes.bin
(  7%) Combining: schemas/self/v1/schemas/api/error/%/dependents.metapack
( 15%) Combining: schemas/self/v1/schemas/api/list/response/%/dependents.metapack
( 23%) Combining: schemas/self/v1/schemas/api/schemas/dependencies/response/%/dependents.metapack
( 30%) Combining: schemas/self/v1/schemas/api/schemas/dependents/response/%/dependents.metapack
( 38%) Combining: schemas/self/v1/schemas/api/schemas/evaluate/response/%/dependents.metapack
( 46%) Combining: schemas/self/v1/schemas/api/schemas/health/response/%/dependents.metapack
( 53%) Combining: schemas/self/v1/schemas/api/schemas/locations/response/%/dependents.metapack
( 61%) Combining: schemas/self/v1/schemas/api/schemas/metadata/response/%/dependents.metapack
( 69%) Combining: schemas/self/v1/schemas/api/schemas/position/%/dependents.metapack
( 76%) Combining: schemas/self/v1/schemas/api/schemas/positions/response/%/dependents.metapack
( 84%) Combining: schemas/self/v1/schemas/api/schemas/search/response/%/dependents.metapack
( 92%) Combining: schemas/self/v1/schemas/api/schemas/stats/response/%/dependents.metapack
(100%) Combining: schemas/self/v1/schemas/api/schemas/trace/response/%/dependents.metapack
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
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/api/schemas/dependencies/response.json (#4)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/api/schemas/dependents/response.json (#5)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/api/schemas/evaluate/response.json (#6)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/api/schemas/health/response.json (#7)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/api/schemas/locations/response.json (#8)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/api/schemas/metadata/response.json (#9)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/api/schemas/position.json (#10)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/api/schemas/positions/response.json (#11)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/api/schemas/search/response.json (#12)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/api/schemas/stats/response.json (#13)
Detecting: $ONE_PREFIX/share/sourcemeta/one/self/v1/schemas/api/schemas/trace/response.json (#14)
(100%) Resolving: schemas/test.json
(  5%) Producing: schemas/schemas/test/%/schema.metapack
( 11%) Producing: schemas/schemas/test/%/dependencies.metapack
( 17%) Producing: schemas/schemas/test/%/locations.metapack
( 23%) Producing: schemas/schemas/test/%/positions.metapack
( 29%) Producing: schemas/schemas/test/%/stats.metapack
( 35%) Producing: schemas/schemas/test/%/bundle.metapack
( 41%) Producing: schemas/schemas/test/%/health.metapack
( 47%) Producing: explorer/schemas/test/%/schema.metapack
( 52%) Producing: schemas/schemas/test/%/blaze-exhaustive.metapack
( 58%) Producing: schemas/schemas/test/%/blaze-fast.metapack
( 64%) Producing: schemas/schemas/test/%/editor.metapack
( 70%) Producing: explorer/schemas/%/directory.metapack
( 76%) Producing: explorer/schemas/test/%/schema-html.metapack
( 82%) Producing: explorer/%/directory.metapack
( 88%) Producing: explorer/schemas/%/directory-html.metapack
( 94%) Producing: explorer/%/directory-html.metapack
(100%) Producing: explorer/%/search.metapack
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
./state.bin
./version.json
EOF
diff "$TMP/manifest.txt" "$TMP/expected_manifest.txt"
