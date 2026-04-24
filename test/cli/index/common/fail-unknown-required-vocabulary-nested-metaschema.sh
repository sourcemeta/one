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

# A custom metaschema that only uses known vocabularies
cat << 'EOF' > "$TMP/schemas/meta-a.json"
{
  "$schema": "https://json-schema.org/draft/2020-12/schema",
  "$id": "https://example.com/meta-a",
  "$vocabulary": {
    "https://json-schema.org/draft/2020-12/vocab/core": true,
    "https://json-schema.org/draft/2020-12/vocab/applicator": true,
    "https://json-schema.org/draft/2020-12/vocab/validation": true
  }
}
EOF

# A custom metaschema that uses meta-a as its dialect
# and declares an unknown required vocabulary
cat << 'EOF' > "$TMP/schemas/meta-b.json"
{
  "$schema": "https://example.com/meta-a",
  "$id": "https://example.com/meta-b",
  "$vocabulary": {
    "https://json-schema.org/draft/2020-12/vocab/core": true,
    "https://example.com/vocab/totally-unknown": true
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

"$1" --skip-banner "$TMP/one.json" "$TMP/output" --concurrency 1 2> "$TMP/output.txt" && CODE="$?" || CODE="$?"
test "$CODE" = "1"
remove_threads_information "$TMP/output.txt"

cat << EOF > "$TMP/expected1.txt"
Writing output to: $(realpath "$TMP")/output
Using configuration: $(realpath "$TMP")/one.json
Detecting: $(realpath "$TMP")/schemas/meta-b.json (#1)
Detecting: $(realpath "$TMP")/schemas/meta-a.json (#2)
Detecting: $ONE_PREFIX/share/sourcemeta/one/collections/self/v1/schemas/configuration/path.json (#3)
Detecting: $ONE_PREFIX/share/sourcemeta/one/collections/self/v1/schemas/configuration/extends.json (#4)
Detecting: $ONE_PREFIX/share/sourcemeta/one/collections/self/v1/schemas/configuration/configuration.json (#5)
Detecting: $ONE_PREFIX/share/sourcemeta/one/collections/self/v1/schemas/configuration/contents.json (#6)
Detecting: $ONE_PREFIX/share/sourcemeta/one/collections/self/v1/schemas/configuration/collection.json (#7)
Detecting: $ONE_PREFIX/share/sourcemeta/one/collections/self/v1/schemas/configuration/rpath.json (#8)
Detecting: $ONE_PREFIX/share/sourcemeta/one/collections/self/v1/schemas/configuration/page.json (#9)
Detecting: $ONE_PREFIX/share/sourcemeta/one/collections/self/v1/schemas/api/error.json (#10)
Detecting: $ONE_PREFIX/share/sourcemeta/one/collections/self/v1/schemas/api/schemas/trace/response.json (#11)
Detecting: $ONE_PREFIX/share/sourcemeta/one/collections/self/v1/schemas/api/schemas/health/response.json (#12)
Detecting: $ONE_PREFIX/share/sourcemeta/one/collections/self/v1/schemas/api/schemas/dependents/response.json (#13)
Detecting: $ONE_PREFIX/share/sourcemeta/one/collections/self/v1/schemas/api/schemas/dependencies/response.json (#14)
Detecting: $ONE_PREFIX/share/sourcemeta/one/collections/self/v1/schemas/api/schemas/position.json (#15)
Detecting: $ONE_PREFIX/share/sourcemeta/one/collections/self/v1/schemas/api/schemas/search/response.json (#16)
Detecting: $ONE_PREFIX/share/sourcemeta/one/collections/self/v1/schemas/api/schemas/evaluate/response.json (#17)
Detecting: $ONE_PREFIX/share/sourcemeta/one/collections/self/v1/schemas/api/schemas/locations/response.json (#18)
Detecting: $ONE_PREFIX/share/sourcemeta/one/collections/self/v1/schemas/api/schemas/positions/response.json (#19)
Detecting: $ONE_PREFIX/share/sourcemeta/one/collections/self/v1/schemas/api/schemas/metadata/response.json (#20)
Detecting: $ONE_PREFIX/share/sourcemeta/one/collections/self/v1/schemas/api/schemas/stats/response.json (#21)
Detecting: $ONE_PREFIX/share/sourcemeta/one/collections/self/v1/schemas/api/list/response.json (#22)
(  4%) Resolving: example/schemas/meta-b.json
(  9%) Resolving: example/schemas/meta-a.json
( 13%) Resolving: self/v1/schemas/configuration/path.json
( 18%) Resolving: self/v1/schemas/configuration/extends.json
( 22%) Resolving: self/v1/schemas/configuration/configuration.json
( 27%) Resolving: self/v1/schemas/configuration/contents.json
( 31%) Resolving: self/v1/schemas/configuration/collection.json
( 36%) Resolving: self/v1/schemas/configuration/rpath.json
( 40%) Resolving: self/v1/schemas/configuration/page.json
( 45%) Resolving: self/v1/schemas/api/error.json
( 50%) Resolving: self/v1/schemas/api/schemas/trace/response.json
( 54%) Resolving: self/v1/schemas/api/schemas/health/response.json
( 59%) Resolving: self/v1/schemas/api/schemas/dependents/response.json
( 63%) Resolving: self/v1/schemas/api/schemas/dependencies/response.json
( 68%) Resolving: self/v1/schemas/api/schemas/position.json
( 72%) Resolving: self/v1/schemas/api/schemas/search/response.json
( 77%) Resolving: self/v1/schemas/api/schemas/evaluate/response.json
( 81%) Resolving: self/v1/schemas/api/schemas/locations/response.json
( 86%) Resolving: self/v1/schemas/api/schemas/positions/response.json
( 90%) Resolving: self/v1/schemas/api/schemas/metadata/response.json
( 95%) Resolving: self/v1/schemas/api/schemas/stats/response.json
(100%) Resolving: self/v1/schemas/api/list/response.json
(  0%) Producing: configuration.json
(  0%) Producing: version.json
(  0%) Producing: explorer/%/404.metapack
(  1%) Producing: schemas/example/schemas/meta-a/%/schema.metapack
(  1%) Producing: schemas/example/schemas/meta-b/%/schema.metapack
error: The metaschema requires an unrecognised vocabulary
  at vocabulary https://example.com/vocab/totally-unknown
  at path $(realpath "$TMP")/schemas/meta-b.json
EOF

cat << EOF > "$TMP/expected2.txt"
Writing output to: $(realpath "$TMP")/output
Using configuration: $(realpath "$TMP")/one.json
Detecting: $(realpath "$TMP")/schemas/meta-b.json (#1)
Detecting: $(realpath "$TMP")/schemas/meta-a.json (#2)
( 50%) Resolving: meta-b.json
(100%) Resolving: meta-a.json
(  2%) Producing: configuration.json
(  5%) Producing: version.json
(  8%) Producing: explorer/%/404.metapack
( 11%) Producing: schemas/example/schemas/meta-a/%/schema.metapack
( 14%) Producing: schemas/example/schemas/meta-b/%/schema.metapack
error: The metaschema requires an unrecognised vocabulary
  at vocabulary https://example.com/vocab/totally-unknown
  at path $(realpath "$TMP")/schemas/meta-b.json
EOF

diff "$TMP/output.txt" "$TMP/expected1.txt" || diff "$TMP/output.txt" "$TMP/expected2.txt"
