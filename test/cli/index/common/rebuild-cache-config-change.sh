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
(  4%) Producing: configuration.json
(  8%) Producing: version.json
( 12%) Producing: explorer/%/404.metapack
( 16%) Producing: schemas/example/schemas/foo/%/schema.metapack
( 20%) Producing: schemas/example/schemas/foo/%/dependencies.metapack
( 25%) Producing: schemas/example/schemas/foo/%/locations.metapack
( 29%) Producing: schemas/example/schemas/foo/%/positions.metapack
( 33%) Producing: schemas/example/schemas/foo/%/stats.metapack
( 37%) Producing: schemas/example/schemas/foo/%/bundle.metapack
( 41%) Producing: schemas/example/schemas/foo/%/dependents.metapack
( 45%) Producing: schemas/example/schemas/foo/%/health.metapack
( 50%) Producing: explorer/example/schemas/foo/%/schema.metapack
( 54%) Producing: schemas/example/schemas/foo/%/blaze-exhaustive.metapack
( 58%) Producing: schemas/example/schemas/foo/%/blaze-fast.metapack
( 62%) Producing: schemas/example/schemas/foo/%/editor.metapack
( 66%) Producing: explorer/%/search.metapack
( 70%) Producing: explorer/example/schemas/%/directory.metapack
( 75%) Producing: explorer/example/schemas/foo/%/schema-html.metapack
( 79%) Producing: explorer/example/%/directory.metapack
( 83%) Producing: explorer/example/schemas/%/directory-html.metapack
( 87%) Producing: explorer/%/directory.metapack
( 91%) Producing: explorer/example/%/directory-html.metapack
( 95%) Producing: explorer/%/directory-html.metapack
(100%) Producing: routes.bin
EOF
diff "$TMP/output.txt" "$TMP/expected.txt"
