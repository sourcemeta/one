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
"$1" --skip-banner "$TMP/one.json" "$TMP/output" --concurrency 1 2> "$TMP/output.txt"
remove_threads_information "$TMP/output.txt"

cat << EOF > "$TMP/expected.txt"
Writing output to: $(realpath "$TMP")/output
Using configuration: $(realpath "$TMP")/one.json
Building...
( 14%) Producing: configuration.json
( 28%) Producing: version.json
( 42%) Producing: explorer/%/404.metapack
( 57%) Producing: explorer/%/directory.metapack
( 71%) Producing: explorer/%/search.metapack
( 85%) Producing: explorer/%/directory-html.metapack
(100%) Producing: routes.bin
Resolving dependents...
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
./routes.bin
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

"$1" --skip-banner "$TMP/one.json" "$TMP/output" --concurrency 1 2> "$TMP/output.txt"
remove_threads_information "$TMP/output.txt"

cat << EOF > "$TMP/expected.txt"
Writing output to: $(realpath "$TMP")/output
Using configuration: $(realpath "$TMP")/one.json
Detecting: $(realpath "$TMP")/schemas/test.json (#1)
(100%) Resolving: test.json
Building...
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
( 70%) Producing: explorer/%/search.metapack
( 76%) Producing: explorer/schemas/%/directory.metapack
( 82%) Producing: explorer/schemas/test/%/schema-html.metapack
( 88%) Producing: explorer/%/directory.metapack
( 94%) Producing: explorer/schemas/%/directory-html.metapack
(100%) Producing: explorer/%/directory-html.metapack
Resolving dependents...
(100%) Producing: schemas/schemas/test/%/dependents.metapack
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
./state.bin
./version.json
EOF
diff "$TMP/manifest.txt" "$TMP/expected_manifest.txt"
