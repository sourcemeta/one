#!/bin/sh

set -o errexit
set -o nounset

TMP="$(mktemp -d)"
clean() { rm -rf "$TMP"; }
trap clean EXIT

cat << EOF > "$TMP/one.json"
{
  "url": "https://example.com/schemas",
  "contents": {
    "example": {
      "baseUri": "https://example.com",
      "path": "./schemas"
    }
  }
}
EOF

mkdir "$TMP/schemas"

cat << 'EOF' > "$TMP/schemas/foo.json"
{
  "$schema": "https://json-schema.org/draft/2020-12/schema",
  "$id": "https://example.com/foo",
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

# Run 1: build with path URL
"$1" --skip-banner "$TMP/one.json" "$TMP/output" --concurrency 1 > /dev/null 2>&1

# Run 2: cached rebuild (no changes)
"$1" --skip-banner "$TMP/one.json" "$TMP/output" --concurrency 1 2> "$TMP/output.txt"
remove_threads_information "$TMP/output.txt"
cat << EOF > "$TMP/expected.txt"
Writing output to: $(realpath "$TMP")/output
Using configuration: $(realpath "$TMP")/one.json
Detecting: $(realpath "$TMP")/schemas/foo.json (#1)
EOF
diff "$TMP/output.txt" "$TMP/expected.txt"

# Run 3: change URL from path to no-path
cat << EOF > "$TMP/one.json"
{
  "url": "https://example.com",
  "contents": {
    "example": {
      "baseUri": "https://example.com",
      "path": "./schemas"
    }
  }
}
EOF

# Must trigger full rebuild because configuration changed
"$1" --skip-banner "$TMP/one.json" "$TMP/output" --concurrency 1 2> "$TMP/output.txt"
remove_threads_information "$TMP/output.txt"
cat << EOF > "$TMP/expected.txt"
Writing output to: $(realpath "$TMP")/output
Using configuration: $(realpath "$TMP")/one.json
Detecting: $(realpath "$TMP")/schemas/foo.json (#1)
(100%) Resolving: foo.json
(  4%) Producing: configuration.json
(  9%) Producing: version.json
( 14%) Producing: explorer/%/404.metapack
( 19%) Producing: schemas/example/foo/%/schema.metapack
( 23%) Producing: schemas/example/foo/%/dependencies.metapack
( 28%) Producing: schemas/example/foo/%/locations.metapack
( 33%) Producing: schemas/example/foo/%/positions.metapack
( 38%) Producing: schemas/example/foo/%/stats.metapack
( 42%) Producing: schemas/example/foo/%/bundle.metapack
( 47%) Producing: schemas/example/foo/%/health.metapack
( 52%) Producing: explorer/example/foo/%/schema.metapack
( 57%) Producing: schemas/example/foo/%/blaze-exhaustive.metapack
( 61%) Producing: schemas/example/foo/%/blaze-fast.metapack
( 66%) Producing: schemas/example/foo/%/editor.metapack
( 71%) Producing: explorer/example/%/directory.metapack
( 76%) Producing: explorer/example/foo/%/schema-html.metapack
( 80%) Producing: explorer/%/directory.metapack
( 85%) Producing: explorer/example/%/directory-html.metapack
( 90%) Producing: explorer/%/directory-html.metapack
( 95%) Producing: explorer/%/search.metapack
(100%) Producing: routes.bin
EOF
diff "$TMP/output.txt" "$TMP/expected.txt"
