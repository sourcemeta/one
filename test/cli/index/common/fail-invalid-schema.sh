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

cat << 'EOF' > "$TMP/schemas/test.json"
{
  "$schema": "http://json-schema.org/draft-07/schema#",
  "$id": "https://example.com/test.json",
  "type": 1
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
Detecting: $(realpath "$TMP")/schemas/test.json (#1)
Detecting: $ONE_PREFIX/share/sourcemeta/one/collections/self/v1/schemas/configuration/path.json (#2)
Detecting: $ONE_PREFIX/share/sourcemeta/one/collections/self/v1/schemas/configuration/extends.json (#3)
Detecting: $ONE_PREFIX/share/sourcemeta/one/collections/self/v1/schemas/configuration/configuration.json (#4)
Detecting: $ONE_PREFIX/share/sourcemeta/one/collections/self/v1/schemas/configuration/contents.json (#5)
Detecting: $ONE_PREFIX/share/sourcemeta/one/collections/self/v1/schemas/configuration/collection.json (#6)
Detecting: $ONE_PREFIX/share/sourcemeta/one/collections/self/v1/schemas/configuration/rpath.json (#7)
Detecting: $ONE_PREFIX/share/sourcemeta/one/collections/self/v1/schemas/configuration/page.json (#8)
Detecting: $ONE_PREFIX/share/sourcemeta/one/collections/self/v1/schemas/api/error.json (#9)
Detecting: $ONE_PREFIX/share/sourcemeta/one/collections/self/v1/schemas/api/schemas/trace/response.json (#10)
Detecting: $ONE_PREFIX/share/sourcemeta/one/collections/self/v1/schemas/api/schemas/health/response.json (#11)
Detecting: $ONE_PREFIX/share/sourcemeta/one/collections/self/v1/schemas/api/schemas/dependents/response.json (#12)
Detecting: $ONE_PREFIX/share/sourcemeta/one/collections/self/v1/schemas/api/schemas/dependencies/response.json (#13)
Detecting: $ONE_PREFIX/share/sourcemeta/one/collections/self/v1/schemas/api/schemas/position.json (#14)
Detecting: $ONE_PREFIX/share/sourcemeta/one/collections/self/v1/schemas/api/schemas/search/response.json (#15)
Detecting: $ONE_PREFIX/share/sourcemeta/one/collections/self/v1/schemas/api/schemas/evaluate/response.json (#16)
Detecting: $ONE_PREFIX/share/sourcemeta/one/collections/self/v1/schemas/api/schemas/locations/response.json (#17)
Detecting: $ONE_PREFIX/share/sourcemeta/one/collections/self/v1/schemas/api/schemas/positions/response.json (#18)
Detecting: $ONE_PREFIX/share/sourcemeta/one/collections/self/v1/schemas/api/schemas/metadata/response.json (#19)
Detecting: $ONE_PREFIX/share/sourcemeta/one/collections/self/v1/schemas/api/schemas/stats/response.json (#20)
Detecting: $ONE_PREFIX/share/sourcemeta/one/collections/self/v1/schemas/api/list/response.json (#21)
(  4%) Resolving: example/schemas/test.json
(  9%) Resolving: self/v1/schemas/configuration/path.json
( 14%) Resolving: self/v1/schemas/configuration/extends.json
( 19%) Resolving: self/v1/schemas/configuration/configuration.json
( 23%) Resolving: self/v1/schemas/configuration/contents.json
( 28%) Resolving: self/v1/schemas/configuration/collection.json
( 33%) Resolving: self/v1/schemas/configuration/rpath.json
( 38%) Resolving: self/v1/schemas/configuration/page.json
( 42%) Resolving: self/v1/schemas/api/error.json
( 47%) Resolving: self/v1/schemas/api/schemas/trace/response.json
( 52%) Resolving: self/v1/schemas/api/schemas/health/response.json
( 57%) Resolving: self/v1/schemas/api/schemas/dependents/response.json
( 61%) Resolving: self/v1/schemas/api/schemas/dependencies/response.json
( 66%) Resolving: self/v1/schemas/api/schemas/position.json
( 71%) Resolving: self/v1/schemas/api/schemas/search/response.json
( 76%) Resolving: self/v1/schemas/api/schemas/evaluate/response.json
( 80%) Resolving: self/v1/schemas/api/schemas/locations/response.json
( 85%) Resolving: self/v1/schemas/api/schemas/positions/response.json
( 90%) Resolving: self/v1/schemas/api/schemas/metadata/response.json
( 95%) Resolving: self/v1/schemas/api/schemas/stats/response.json
(100%) Resolving: self/v1/schemas/api/list/response.json
(  0%) Producing: configuration.json
(  0%) Producing: version.json
(  1%) Producing: explorer/%/404.metapack
(  1%) Producing: schemas/example/schemas/test/%/schema.metapack
error: The schema does not adhere to its metaschema
The integer value 1 was expected to equal one of the following values: "array", "boolean", "integer", "null", "number", "object", and "string"
  at instance location "/type"
  at evaluate path "/properties/type/anyOf/0/\$ref/enum"
The integer value was expected to validate against the referenced schema
  at instance location "/type"
  at evaluate path "/properties/type/anyOf/0/\$ref"
The value was expected to be of type array but it was of type integer
  at instance location "/type"
  at evaluate path "/properties/type/anyOf/1/type"
The integer value was expected to validate against at least one of the 2 given subschemas
  at instance location "/type"
  at evaluate path "/properties/type/anyOf"
The object value was expected to validate against the 46 defined properties subschemas
  at instance location ""
  at evaluate path "/properties"
EOF

diff "$TMP/output.txt" "$TMP/expected.txt"
