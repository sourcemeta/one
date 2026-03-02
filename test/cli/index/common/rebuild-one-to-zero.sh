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

normalize_staging_path() {
  expr='s|\.sourcemeta-one-[^/ ]*|.sourcemeta-one-XXXXXX|g'
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

"$1" --skip-banner "$TMP/one.json" "$TMP/output" --concurrency 1 2> "$TMP/output.txt"
remove_threads_information "$TMP/output.txt"
normalize_staging_path "$TMP/output.txt"

cat << EOF > "$TMP/expected.txt"
Writing output to: $(realpath "$TMP")/output
Using configuration: $(realpath "$TMP")/one.json
Detecting: $(realpath "$TMP")/schemas/test.json (#1)
(100%) Ingesting: https://sourcemeta.com/schemas/test
(100%) Analysing: https://sourcemeta.com/schemas/test
( 50%) Reviewing: schemas
(100%) Reviewing: schemas
(100%) Reworking: https://sourcemeta.com/schemas/test
(  0%) Producing: explorer
( 50%) Producing: schemas
(100%) Producing: .
( 33%) Rendering: schemas
( 66%) Rendering: .
(100%) Rendering: schemas/test
Committing: $(realpath "$TMP")/.sourcemeta-one-XXXXXX => $(realpath "$TMP")/output
EOF
diff "$TMP/output.txt" "$TMP/expected.txt"

cd "$TMP/output"
find . -mindepth 1 | LC_ALL=C sort > "$TMP/manifest.txt"
cd - > /dev/null

cat << 'EOF' > "$TMP/expected_manifest.txt"
./configuration.json
./dependency-tree.metapack
./dependency-tree.metapack.deps
./explorer
./explorer/%
./explorer/%/404.metapack
./explorer/%/404.metapack.deps
./explorer/%/directory-html.metapack
./explorer/%/directory-html.metapack.deps
./explorer/%/directory.metapack
./explorer/%/directory.metapack.deps
./explorer/%/search.metapack
./explorer/%/search.metapack.deps
./explorer/schemas
./explorer/schemas/%
./explorer/schemas/%/directory-html.metapack
./explorer/schemas/%/directory-html.metapack.deps
./explorer/schemas/%/directory.metapack
./explorer/schemas/%/directory.metapack.deps
./explorer/schemas/test
./explorer/schemas/test/%
./explorer/schemas/test/%/schema-html.metapack
./explorer/schemas/test/%/schema-html.metapack.deps
./explorer/schemas/test/%/schema.metapack
./explorer/schemas/test/%/schema.metapack.deps
./routes.bin
./routes.bin.deps
./schemas
./schemas/schemas
./schemas/schemas/test
./schemas/schemas/test/%
./schemas/schemas/test/%/blaze-exhaustive.metapack
./schemas/schemas/test/%/blaze-exhaustive.metapack.deps
./schemas/schemas/test/%/blaze-fast.metapack
./schemas/schemas/test/%/blaze-fast.metapack.deps
./schemas/schemas/test/%/bundle.metapack
./schemas/schemas/test/%/bundle.metapack.deps
./schemas/schemas/test/%/dependencies.metapack
./schemas/schemas/test/%/dependencies.metapack.deps
./schemas/schemas/test/%/dependents.metapack
./schemas/schemas/test/%/dependents.metapack.deps
./schemas/schemas/test/%/editor.metapack
./schemas/schemas/test/%/editor.metapack.deps
./schemas/schemas/test/%/health.metapack
./schemas/schemas/test/%/health.metapack.deps
./schemas/schemas/test/%/locations.metapack
./schemas/schemas/test/%/locations.metapack.deps
./schemas/schemas/test/%/positions.metapack
./schemas/schemas/test/%/positions.metapack.deps
./schemas/schemas/test/%/schema.metapack
./schemas/schemas/test/%/schema.metapack.deps
./schemas/schemas/test/%/stats.metapack
./schemas/schemas/test/%/stats.metapack.deps
./version.json
EOF
diff "$TMP/manifest.txt" "$TMP/expected_manifest.txt"

# Second run: delete the schema file
rm "$TMP/schemas/test.json"

"$1" --skip-banner "$TMP/one.json" "$TMP/output" --concurrency 1 2> "$TMP/output.txt"
remove_threads_information "$TMP/output.txt"
normalize_staging_path "$TMP/output.txt"

cat << EOF > "$TMP/expected.txt"
Writing output to: $(realpath "$TMP")/output
Using configuration: $(realpath "$TMP")/one.json
Hardlinking: $(realpath "$TMP")/output => $(realpath "$TMP")/.sourcemeta-one-XXXXXX
( 50%) Reviewing: schemas
(100%) Reviewing: schemas
(  0%) Producing: explorer
(100%) Producing: .
(100%) Rendering: .
(skip) Rendering: . [not-found]
(skip) Producing: routes.bin [routes]
Committing: $(realpath "$TMP")/.sourcemeta-one-XXXXXX => $(realpath "$TMP")/output
EOF
diff "$TMP/output.txt" "$TMP/expected.txt"

cd "$TMP/output"
find . -mindepth 1 | LC_ALL=C sort > "$TMP/manifest.txt"
cd - > /dev/null

cat << 'EOF' > "$TMP/expected_manifest.txt"
./configuration.json
./dependency-tree.metapack
./dependency-tree.metapack.deps
./explorer
./explorer/%
./explorer/%/404.metapack
./explorer/%/404.metapack.deps
./explorer/%/directory-html.metapack
./explorer/%/directory-html.metapack.deps
./explorer/%/directory.metapack
./explorer/%/directory.metapack.deps
./explorer/%/search.metapack
./explorer/%/search.metapack.deps
./routes.bin
./routes.bin.deps
./version.json
EOF
diff "$TMP/manifest.txt" "$TMP/expected_manifest.txt"
