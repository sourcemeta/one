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

cat << 'EOF' > "$TMP/schemas/custom-meta.json"
{
  "$schema": "https://json-schema.org/draft/2020-12/schema",
  "$id": "https://example.com/custom-meta",
  "$vocabulary": "not-an-object"
}
EOF

cat << 'EOF' > "$TMP/schemas/test.json"
{
  "$schema": "https://example.com/custom-meta",
  "$id": "https://example.com/test",
  "type": "string"
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

"$1" --skip-banner --deterministic "$TMP/one.json" "$TMP/output" --concurrency 1 2> "$TMP/output.txt" && CODE="$?" || CODE="$?"
test "$CODE" = "1" || exit 1
remove_threads_information "$TMP/output.txt"

cat << EOF > "$TMP/expected.txt"
Writing output to: $(realpath "$TMP")/output
Using configuration: $(realpath "$TMP")/one.json
Detecting: $(realpath "$TMP")/schemas/custom-meta.json (#1)
Detecting: $(realpath "$TMP")/schemas/test.json (#2)
Detecting: $ONE_PREFIX/share/sourcemeta/one/collections/self/v1/schemas/api/error.json (#3)
Detecting: $ONE_PREFIX/share/sourcemeta/one/collections/self/v1/schemas/api/list/response.json (#4)
Detecting: $ONE_PREFIX/share/sourcemeta/one/collections/self/v1/schemas/api/schemas/dependencies/response.json (#5)
Detecting: $ONE_PREFIX/share/sourcemeta/one/collections/self/v1/schemas/api/schemas/dependents/response.json (#6)
Detecting: $ONE_PREFIX/share/sourcemeta/one/collections/self/v1/schemas/api/schemas/evaluate/response.json (#7)
Detecting: $ONE_PREFIX/share/sourcemeta/one/collections/self/v1/schemas/api/schemas/health/response.json (#8)
Detecting: $ONE_PREFIX/share/sourcemeta/one/collections/self/v1/schemas/api/schemas/locations/response.json (#9)
Detecting: $ONE_PREFIX/share/sourcemeta/one/collections/self/v1/schemas/api/schemas/metadata/response.json (#10)
Detecting: $ONE_PREFIX/share/sourcemeta/one/collections/self/v1/schemas/api/schemas/position.json (#11)
Detecting: $ONE_PREFIX/share/sourcemeta/one/collections/self/v1/schemas/api/schemas/positions/response.json (#12)
Detecting: $ONE_PREFIX/share/sourcemeta/one/collections/self/v1/schemas/api/schemas/search/response.json (#13)
Detecting: $ONE_PREFIX/share/sourcemeta/one/collections/self/v1/schemas/api/schemas/stats/response.json (#14)
Detecting: $ONE_PREFIX/share/sourcemeta/one/collections/self/v1/schemas/api/schemas/trace/response.json (#15)
Detecting: $ONE_PREFIX/share/sourcemeta/one/collections/self/v1/schemas/configuration/collection.json (#16)
Detecting: $ONE_PREFIX/share/sourcemeta/one/collections/self/v1/schemas/configuration/configuration.json (#17)
Detecting: $ONE_PREFIX/share/sourcemeta/one/collections/self/v1/schemas/configuration/contents.json (#18)
Detecting: $ONE_PREFIX/share/sourcemeta/one/collections/self/v1/schemas/configuration/extends.json (#19)
Detecting: $ONE_PREFIX/share/sourcemeta/one/collections/self/v1/schemas/configuration/page.json (#20)
Detecting: $ONE_PREFIX/share/sourcemeta/one/collections/self/v1/schemas/configuration/path.json (#21)
(  4%) Resolving: example/schemas/custom-meta.json
(  9%) Resolving: example/schemas/test.json
( 14%) Resolving: self/v1/schemas/api/error.json
( 19%) Resolving: self/v1/schemas/api/list/response.json
( 23%) Resolving: self/v1/schemas/api/schemas/dependencies/response.json
( 28%) Resolving: self/v1/schemas/api/schemas/dependents/response.json
( 33%) Resolving: self/v1/schemas/api/schemas/evaluate/response.json
( 38%) Resolving: self/v1/schemas/api/schemas/health/response.json
( 42%) Resolving: self/v1/schemas/api/schemas/locations/response.json
( 47%) Resolving: self/v1/schemas/api/schemas/metadata/response.json
( 52%) Resolving: self/v1/schemas/api/schemas/position.json
( 57%) Resolving: self/v1/schemas/api/schemas/positions/response.json
( 61%) Resolving: self/v1/schemas/api/schemas/search/response.json
( 66%) Resolving: self/v1/schemas/api/schemas/stats/response.json
( 71%) Resolving: self/v1/schemas/api/schemas/trace/response.json
( 76%) Resolving: self/v1/schemas/configuration/collection.json
( 80%) Resolving: self/v1/schemas/configuration/configuration.json
( 85%) Resolving: self/v1/schemas/configuration/contents.json
( 90%) Resolving: self/v1/schemas/configuration/extends.json
( 95%) Resolving: self/v1/schemas/configuration/page.json
(100%) Resolving: self/v1/schemas/configuration/path.json
(  0%) Producing: configuration.json
(  0%) Producing: version.json
(  1%) Producing: explorer/%/404.metapack
(  1%) Producing: schemas/example/schemas/custom-meta/%/schema.metapack
error: The schema does not adhere to its metaschema
The value was expected to be of type object but it was of type string
  at instance location "/\$vocabulary"
  at evaluate path "/allOf/0/\$ref/properties/\$vocabulary/type"
The object value was expected to validate against the 9 defined properties subschemas
  at instance location ""
  at evaluate path "/allOf/0/\$ref/properties"
The object value was expected to validate against the referenced schema
  at instance location ""
  at evaluate path "/allOf/0/\$ref"
The object value was expected to validate against the 7 given subschemas
  at instance location ""
  at evaluate path "/allOf"
EOF

diff "$TMP/output.txt" "$TMP/expected.txt"
