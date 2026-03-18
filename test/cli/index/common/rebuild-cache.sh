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

"$1" --skip-banner "$TMP/one.json" "$TMP/output" --concurrency 1 2> "$TMP/output.txt"
remove_threads_information "$TMP/output.txt"
cat << EOF > "$TMP/expected.txt"
Writing output to: $(realpath "$TMP")/output
Using configuration: $(realpath "$TMP")/one.json
Detecting: $(realpath "$TMP")/schemas/foo.json (#1)
(100%) Resolving: foo.json
(  4%) Producing: configuration.json
(  8%) Producing: version.json
( 13%) Producing: explorer/%/404.metapack
( 17%) Producing: schemas/example/schemas/foo/%/schema.metapack
( 21%) Producing: schemas/example/schemas/foo/%/dependencies.metapack
( 26%) Producing: schemas/example/schemas/foo/%/locations.metapack
( 30%) Producing: schemas/example/schemas/foo/%/positions.metapack
( 34%) Producing: schemas/example/schemas/foo/%/stats.metapack
( 39%) Producing: schemas/example/schemas/foo/%/bundle.metapack
( 43%) Producing: schemas/example/schemas/foo/%/health.metapack
( 47%) Producing: explorer/example/schemas/foo/%/schema.metapack
( 52%) Producing: schemas/example/schemas/foo/%/blaze-exhaustive.metapack
( 56%) Producing: schemas/example/schemas/foo/%/blaze-fast.metapack
( 60%) Producing: schemas/example/schemas/foo/%/editor.metapack
( 65%) Producing: explorer/example/schemas/%/directory.metapack
( 69%) Producing: explorer/example/schemas/foo/%/schema-html.metapack
( 73%) Producing: explorer/example/%/directory.metapack
( 78%) Producing: explorer/example/schemas/%/directory-html.metapack
( 82%) Producing: explorer/%/directory.metapack
( 86%) Producing: explorer/example/%/directory-html.metapack
( 91%) Producing: explorer/%/directory-html.metapack
( 95%) Producing: explorer/%/search.metapack
(100%) Producing: routes.bin
(100%) Combining: schemas/example/schemas/foo/%/dependents.metapack
EOF
diff "$TMP/output.txt" "$TMP/expected.txt"

# Run it once more

"$1" --skip-banner "$TMP/one.json" "$TMP/output" --concurrency 1 2> "$TMP/output.txt"
remove_threads_information "$TMP/output.txt"
cat << EOF > "$TMP/expected.txt"
Writing output to: $(realpath "$TMP")/output
Using configuration: $(realpath "$TMP")/one.json
Detecting: $(realpath "$TMP")/schemas/foo.json (#1)
EOF
diff "$TMP/output.txt" "$TMP/expected.txt"

# Update the file
cat << 'EOF' > "$TMP/schemas/foo.json"
{
  "$schema": "http://json-schema.org/draft-07/schema#",
  "$id": "https://example.com/foo",
  "type": "string"
}
EOF
"$1" --skip-banner "$TMP/one.json" "$TMP/output" --concurrency 1 2> "$TMP/output.txt"
remove_threads_information "$TMP/output.txt"
cat << EOF > "$TMP/expected.txt"
Writing output to: $(realpath "$TMP")/output
Using configuration: $(realpath "$TMP")/one.json
Detecting: $(realpath "$TMP")/schemas/foo.json (#1)
(100%) Resolving: foo.json
(  5%) Producing: schemas/example/schemas/foo/%/schema.metapack
( 10%) Producing: schemas/example/schemas/foo/%/dependencies.metapack
( 15%) Producing: schemas/example/schemas/foo/%/locations.metapack
( 21%) Producing: schemas/example/schemas/foo/%/positions.metapack
( 26%) Producing: schemas/example/schemas/foo/%/stats.metapack
( 31%) Producing: schemas/example/schemas/foo/%/bundle.metapack
( 36%) Producing: schemas/example/schemas/foo/%/health.metapack
( 42%) Producing: explorer/example/schemas/foo/%/schema.metapack
( 47%) Producing: schemas/example/schemas/foo/%/blaze-exhaustive.metapack
( 52%) Producing: schemas/example/schemas/foo/%/blaze-fast.metapack
( 57%) Producing: schemas/example/schemas/foo/%/editor.metapack
( 63%) Producing: explorer/example/schemas/%/directory.metapack
( 68%) Producing: explorer/example/schemas/foo/%/schema-html.metapack
( 73%) Producing: explorer/example/%/directory.metapack
( 78%) Producing: explorer/example/schemas/%/directory-html.metapack
( 84%) Producing: explorer/%/directory.metapack
( 89%) Producing: explorer/example/%/directory-html.metapack
( 94%) Producing: explorer/%/directory-html.metapack
(100%) Producing: explorer/%/search.metapack
EOF
diff "$TMP/output.txt" "$TMP/expected.txt"

# Touch an output file (the indexer trusts its own marks, so this is a no-op)
touch "$TMP/output/configuration.json"
"$1" --skip-banner "$TMP/one.json" "$TMP/output" --concurrency 1 2> "$TMP/output.txt"
remove_threads_information "$TMP/output.txt"
cat << EOF > "$TMP/expected.txt"
Writing output to: $(realpath "$TMP")/output
Using configuration: $(realpath "$TMP")/one.json
Detecting: $(realpath "$TMP")/schemas/foo.json (#1)
EOF
diff "$TMP/output.txt" "$TMP/expected.txt"
