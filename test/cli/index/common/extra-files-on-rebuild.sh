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

cat << 'EOF' > "$TMP/schemas/test.json"
{
  "$schema": "http://json-schema.org/draft-07/schema#",
  "$id": "https://example.com/old.json"
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

"$1" --skip-banner "$TMP/one.json" "$TMP/output" --concurrency 1 2> "$TMP/output.txt"
remove_threads_information "$TMP/output.txt"

cat << EOF > "$TMP/expected.txt"
Writing output to: $(realpath "$TMP")/output
Using configuration: $(realpath "$TMP")/one.json
Detecting: $(realpath "$TMP")/schemas/test.json (#1)
(100%) Resolving: test.json
(  4%) Producing: configuration.json
(  8%) Producing: version.json
( 12%) Producing: explorer/%/404.metapack
( 16%) Producing: schemas/example/schemas/old/%/schema.metapack
( 20%) Producing: schemas/example/schemas/old/%/dependencies.metapack
( 25%) Producing: schemas/example/schemas/old/%/locations.metapack
( 29%) Producing: schemas/example/schemas/old/%/positions.metapack
( 33%) Producing: schemas/example/schemas/old/%/stats.metapack
( 37%) Producing: schemas/example/schemas/old/%/bundle.metapack
( 41%) Producing: schemas/example/schemas/old/%/dependents.metapack
( 45%) Producing: schemas/example/schemas/old/%/health.metapack
( 50%) Producing: explorer/example/schemas/old/%/schema.metapack
( 54%) Producing: schemas/example/schemas/old/%/blaze-exhaustive.metapack
( 58%) Producing: schemas/example/schemas/old/%/blaze-fast.metapack
( 62%) Producing: schemas/example/schemas/old/%/editor.metapack
( 66%) Producing: explorer/%/search.metapack
( 70%) Producing: explorer/example/schemas/%/directory.metapack
( 75%) Producing: explorer/example/schemas/old/%/schema-html.metapack
( 79%) Producing: explorer/example/%/directory.metapack
( 83%) Producing: explorer/example/schemas/%/directory-html.metapack
( 87%) Producing: explorer/%/directory.metapack
( 91%) Producing: explorer/example/%/directory-html.metapack
( 95%) Producing: explorer/%/directory-html.metapack
(100%) Producing: routes.bin
EOF
diff "$TMP/output.txt" "$TMP/expected.txt"

exists() {
  [ -f "$1" ] || (echo "File MUST exist: $1" 1>&2 && exit 1)
}

not_exists() {
  [ ! -f "$1" ] || (echo "File MUST NOT exists: $1" 1>&2 && exit 1)
}

exists "$TMP/output/explorer/example/schemas/old/%/schema.metapack"
exists "$TMP/output/schemas/example/schemas/old/%/bundle.metapack"
exists "$TMP/output/schemas/example/schemas/old/%/editor.metapack"
exists "$TMP/output/schemas/example/schemas/old/%/blaze-exhaustive.metapack"
exists "$TMP/output/schemas/example/schemas/old/%/schema.metapack"

cat << 'EOF' > "$TMP/schemas/test.json"
{
  "$schema": "http://json-schema.org/draft-07/schema#",
  "$id": "https://example.com/new"
}
EOF

"$1" --skip-banner "$TMP/one.json" "$TMP/output" --concurrency 1 2> "$TMP/output.txt"
remove_threads_information "$TMP/output.txt"

cat << EOF > "$TMP/expected.txt"
Writing output to: $(realpath "$TMP")/output
Using configuration: $(realpath "$TMP")/one.json
Detecting: $(realpath "$TMP")/schemas/test.json (#1)
(100%) Resolving: test.json
(  4%) Producing: schemas/example/schemas/new/%/schema.metapack
(  9%) Producing: schemas/example/schemas/new/%/dependencies.metapack
( 13%) Producing: schemas/example/schemas/new/%/locations.metapack
( 18%) Producing: schemas/example/schemas/new/%/positions.metapack
( 22%) Producing: schemas/example/schemas/new/%/stats.metapack
( 27%) Producing: schemas/example/schemas/new/%/bundle.metapack
( 31%) Producing: schemas/example/schemas/new/%/dependents.metapack
( 36%) Producing: schemas/example/schemas/new/%/health.metapack
( 40%) Producing: explorer/example/schemas/new/%/schema.metapack
( 45%) Producing: schemas/example/schemas/new/%/blaze-exhaustive.metapack
( 50%) Producing: schemas/example/schemas/new/%/blaze-fast.metapack
( 54%) Producing: schemas/example/schemas/new/%/editor.metapack
( 59%) Producing: explorer/%/search.metapack
( 63%) Producing: explorer/example/schemas/%/directory.metapack
( 68%) Producing: explorer/example/schemas/new/%/schema-html.metapack
( 72%) Producing: explorer/example/%/directory.metapack
( 77%) Producing: explorer/example/schemas/%/directory-html.metapack
( 81%) Producing: explorer/%/directory.metapack
( 86%) Producing: explorer/example/%/directory-html.metapack
( 90%) Producing: explorer/%/directory-html.metapack
( 95%) Disposing: explorer/example/schemas/old
(100%) Disposing: schemas/example/schemas/old
EOF
diff "$TMP/output.txt" "$TMP/expected.txt"

exists "$TMP/output/explorer/example/schemas/new/%/schema.metapack"
exists "$TMP/output/schemas/example/schemas/new/%/bundle.metapack"
exists "$TMP/output/schemas/example/schemas/new/%/editor.metapack"
exists "$TMP/output/schemas/example/schemas/new/%/blaze-exhaustive.metapack"
exists "$TMP/output/schemas/example/schemas/new/%/schema.metapack"

not_exists "$TMP/output/explorer/example/schemas/old/%/schema.metapack"
not_exists "$TMP/output/schemas/example/schemas/old/%/bundle.metapack"
not_exists "$TMP/output/schemas/example/schemas/old/%/editor.metapack"
not_exists "$TMP/output/schemas/example/schemas/old/%/blaze-exhaustive.metapack"
not_exists "$TMP/output/schemas/example/schemas/old/%/schema.metapack"
