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

cat << 'EOF' > "$TMP/schemas/a.json"
{
  "$schema": "http://json-schema.org/draft-07/schema#",
  "$id": "https://example.com/a"
}
EOF

cat << 'EOF' > "$TMP/schemas/b.json"
{
  "$schema": "http://json-schema.org/draft-07/schema#",
  "$id": "https://example.com/b"
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

# Run 1: index two schemas from scratch
"$1" --skip-banner "$TMP/one.json" "$TMP/output" --concurrency 1 > /dev/null 2>&1

# Run 2: add a third schema
# The existing schemas' dependents and HTML should be cache hits
# because the new schema does not affect the dependency tree
cat << 'EOF' > "$TMP/schemas/c.json"
{
  "$schema": "http://json-schema.org/draft-07/schema#",
  "$id": "https://example.com/c"
}
EOF
"$1" --skip-banner "$TMP/one.json" "$TMP/output" --concurrency 1 2> "$TMP/output.txt"
remove_threads_information "$TMP/output.txt"

grep '(skip)' "$TMP/output.txt" | LC_ALL=C sort > "$TMP/actual_skips.txt"
cat << 'EOF' | LC_ALL=C sort > "$TMP/expected_skips.txt"
(skip) Ingesting: https://sourcemeta.com/example/schemas/a [materialise]
(skip) Ingesting: https://sourcemeta.com/example/schemas/b [materialise]
(skip) Analysing: https://sourcemeta.com/example/schemas/a [positions]
(skip) Analysing: https://sourcemeta.com/example/schemas/a [locations]
(skip) Analysing: https://sourcemeta.com/example/schemas/a [dependencies]
(skip) Analysing: https://sourcemeta.com/example/schemas/a [stats]
(skip) Analysing: https://sourcemeta.com/example/schemas/a [health]
(skip) Analysing: https://sourcemeta.com/example/schemas/a [bundle]
(skip) Analysing: https://sourcemeta.com/example/schemas/a [editor]
(skip) Analysing: https://sourcemeta.com/example/schemas/a [blaze-exhaustive]
(skip) Analysing: https://sourcemeta.com/example/schemas/a [blaze-fast]
(skip) Analysing: https://sourcemeta.com/example/schemas/a [metadata]
(skip) Analysing: https://sourcemeta.com/example/schemas/b [positions]
(skip) Analysing: https://sourcemeta.com/example/schemas/b [locations]
(skip) Analysing: https://sourcemeta.com/example/schemas/b [dependencies]
(skip) Analysing: https://sourcemeta.com/example/schemas/b [stats]
(skip) Analysing: https://sourcemeta.com/example/schemas/b [health]
(skip) Analysing: https://sourcemeta.com/example/schemas/b [bundle]
(skip) Analysing: https://sourcemeta.com/example/schemas/b [editor]
(skip) Analysing: https://sourcemeta.com/example/schemas/b [blaze-exhaustive]
(skip) Analysing: https://sourcemeta.com/example/schemas/b [blaze-fast]
(skip) Analysing: https://sourcemeta.com/example/schemas/b [metadata]
(skip) Rendering: . [not-found]
(skip) Rendering: example/schemas/a [schema]
(skip) Rendering: example/schemas/b [schema]
(skip) Producing: routes.bin [routes]
EOF

diff "$TMP/actual_skips.txt" "$TMP/expected_skips.txt"

# Run 3: re-index with no changes. All three schemas should be fully cached.
"$1" --skip-banner "$TMP/one.json" "$TMP/output" --concurrency 1 2> "$TMP/output.txt"
remove_threads_information "$TMP/output.txt"
grep '(skip) Ingesting:' "$TMP/output.txt" | sort > "$TMP/ingest_actual.txt"

cat << 'EOF' | sort > "$TMP/ingest_expected.txt"
(skip) Ingesting: https://sourcemeta.com/example/schemas/a [materialise]
(skip) Ingesting: https://sourcemeta.com/example/schemas/b [materialise]
(skip) Ingesting: https://sourcemeta.com/example/schemas/c [materialise]
EOF

diff "$TMP/ingest_actual.txt" "$TMP/ingest_expected.txt"

cd "$TMP/output"
find . -mindepth 1 | LC_ALL=C sort > "$TMP/manifest.txt"
cd - > /dev/null

cat << 'EOF' > "$TMP/expected_manifest.txt"
./configuration.json
./dependency-tree.metapack
./deps.txt
./explorer
./explorer/%
./explorer/%/404.metapack
./explorer/%/directory-html.metapack
./explorer/%/directory.metapack
./explorer/%/search.metapack
./explorer/example
./explorer/example/%
./explorer/example/%/directory-html.metapack
./explorer/example/%/directory.metapack
./explorer/example/schemas
./explorer/example/schemas/%
./explorer/example/schemas/%/directory-html.metapack
./explorer/example/schemas/%/directory.metapack
./explorer/example/schemas/a
./explorer/example/schemas/a/%
./explorer/example/schemas/a/%/schema-html.metapack
./explorer/example/schemas/a/%/schema.metapack
./explorer/example/schemas/b
./explorer/example/schemas/b/%
./explorer/example/schemas/b/%/schema-html.metapack
./explorer/example/schemas/b/%/schema.metapack
./explorer/example/schemas/c
./explorer/example/schemas/c/%
./explorer/example/schemas/c/%/schema-html.metapack
./explorer/example/schemas/c/%/schema.metapack
./routes.bin
./schemas
./schemas/example
./schemas/example/schemas
./schemas/example/schemas/a
./schemas/example/schemas/a/%
./schemas/example/schemas/a/%/blaze-exhaustive.metapack
./schemas/example/schemas/a/%/blaze-fast.metapack
./schemas/example/schemas/a/%/bundle.metapack
./schemas/example/schemas/a/%/dependencies.metapack
./schemas/example/schemas/a/%/dependents.metapack
./schemas/example/schemas/a/%/editor.metapack
./schemas/example/schemas/a/%/health.metapack
./schemas/example/schemas/a/%/locations.metapack
./schemas/example/schemas/a/%/positions.metapack
./schemas/example/schemas/a/%/schema.metapack
./schemas/example/schemas/a/%/stats.metapack
./schemas/example/schemas/b
./schemas/example/schemas/b/%
./schemas/example/schemas/b/%/blaze-exhaustive.metapack
./schemas/example/schemas/b/%/blaze-fast.metapack
./schemas/example/schemas/b/%/bundle.metapack
./schemas/example/schemas/b/%/dependencies.metapack
./schemas/example/schemas/b/%/dependents.metapack
./schemas/example/schemas/b/%/editor.metapack
./schemas/example/schemas/b/%/health.metapack
./schemas/example/schemas/b/%/locations.metapack
./schemas/example/schemas/b/%/positions.metapack
./schemas/example/schemas/b/%/schema.metapack
./schemas/example/schemas/b/%/stats.metapack
./schemas/example/schemas/c
./schemas/example/schemas/c/%
./schemas/example/schemas/c/%/blaze-exhaustive.metapack
./schemas/example/schemas/c/%/blaze-fast.metapack
./schemas/example/schemas/c/%/bundle.metapack
./schemas/example/schemas/c/%/dependencies.metapack
./schemas/example/schemas/c/%/dependents.metapack
./schemas/example/schemas/c/%/editor.metapack
./schemas/example/schemas/c/%/health.metapack
./schemas/example/schemas/c/%/locations.metapack
./schemas/example/schemas/c/%/positions.metapack
./schemas/example/schemas/c/%/schema.metapack
./schemas/example/schemas/c/%/stats.metapack
./version.json
EOF

diff "$TMP/manifest.txt" "$TMP/expected_manifest.txt"
