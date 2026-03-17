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

# Run 1: initial build
"$1" --skip-banner "$TMP/one.json" "$TMP/output" --concurrency 1 > /dev/null 2>&1

# Run 2: cached rebuild (no changes) and resolver cache gets a hit
"$1" --skip-banner "$TMP/one.json" "$TMP/output" --concurrency 1 2> "$TMP/output.txt"
remove_threads_information "$TMP/output.txt"
cat << EOF > "$TMP/expected.txt"
Writing output to: $(realpath "$TMP")/output
Using configuration: $(realpath "$TMP")/one.json
Detecting: $(realpath "$TMP")/schemas/foo.json (#1)
Building...
Resolving dependents...
EOF
diff "$TMP/output.txt" "$TMP/expected.txt"

# Run 3: change the server URL in the configuration
cat << EOF > "$TMP/one.json"
{
  "url": "https://other.example.com/",
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

# The entire thing must be invalidated because the configuration changed
"$1" --skip-banner "$TMP/one.json" "$TMP/output" --concurrency 1 2> "$TMP/output.txt"
remove_threads_information "$TMP/output.txt"
cat << EOF > "$TMP/expected.txt"
Writing output to: $(realpath "$TMP")/output
Using configuration: $(realpath "$TMP")/one.json
Detecting: $(realpath "$TMP")/schemas/foo.json (#1)
(100%) Resolving: foo.json
Building...
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
( 65%) Producing: explorer/%/search.metapack
( 69%) Producing: explorer/example/schemas/%/directory.metapack
( 73%) Producing: explorer/example/schemas/foo/%/schema-html.metapack
( 78%) Producing: explorer/example/%/directory.metapack
( 82%) Producing: explorer/example/schemas/%/directory-html.metapack
( 86%) Producing: explorer/%/directory.metapack
( 91%) Producing: explorer/example/%/directory-html.metapack
( 95%) Producing: explorer/%/directory-html.metapack
(100%) Producing: routes.bin
Resolving dependents...
EOF
diff "$TMP/output.txt" "$TMP/expected.txt"
