#!/bin/sh

set -o errexit
set -o nounset

TMP="$(mktemp -d)"
clean() { rm -rf "$TMP"; }
trap clean EXIT

mkdir -p "$TMP/schemas-a" "$TMP/schemas-b" "$TMP/docs"

cat << 'EOF' > "$TMP/schemas-a/test.json"
{ "$schema": "https://json-schema.org/draft/2020-12/schema" }
EOF

cat << 'EOF' > "$TMP/schemas-b/test.json"
{ "$schema": "https://json-schema.org/draft/2020-12/schema" }
EOF

cat << 'EOF' > "$TMP/docs/readme.md"
# Shared
EOF

cat << EOF > "$TMP/one.json"
{
  "url": "https://example.com",
  "contents": {
    "alpha": {
      "path": "./schemas-a",
      "x-sourcemeta-one:documentation": "./docs/readme.md"
    },
    "beta": {
      "path": "./schemas-b",
      "x-sourcemeta-one:documentation": "./docs/readme.md"
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

RESOLVED_DOCS_PATH="$(realpath "$TMP/docs/readme.md")"
HASH="$(printf '%s' "$RESOLVED_DOCS_PATH" | shasum -a 256 | cut -d' ' -f1)"

# Run 1: Full build
"$1" --skip-banner "$TMP/one.json" "$TMP/output" --concurrency 1 2> "$TMP/output1.txt"

# Run 2: Cached
"$1" --skip-banner "$TMP/one.json" "$TMP/output" --concurrency 1 2> "$TMP/output2.txt"
remove_threads_information "$TMP/output2.txt"

cat << EOF > "$TMP/expected2.txt"
Writing output to: $(realpath "$TMP")/output
Using configuration: $(realpath "$TMP")/one.json
Detecting: $(realpath "$TMP")/schemas-b/test.json (#1)
Detecting: $(realpath "$TMP")/schemas-a/test.json (#2)
EOF

diff "$TMP/output2.txt" "$TMP/expected2.txt"

# Run 3: Modify shared documentation file
echo '# Updated Shared' > "$TMP/docs/readme.md"

"$1" --skip-banner "$TMP/one.json" "$TMP/output" --concurrency 1 2> "$TMP/output3.txt"
remove_threads_information "$TMP/output3.txt"

cat << EOF > "$TMP/expected3.txt"
Writing output to: $(realpath "$TMP")/output
Using configuration: $(realpath "$TMP")/one.json
Detecting: $(realpath "$TMP")/schemas-b/test.json (#1)
Detecting: $(realpath "$TMP")/schemas-a/test.json (#2)
( 12%) Producing: static/${HASH}.metapack
( 25%) Producing: explorer/alpha/%/directory.metapack
( 37%) Producing: explorer/beta/%/directory.metapack
( 50%) Producing: explorer/%/directory.metapack
( 62%) Producing: explorer/alpha/%/directory-html.metapack
( 75%) Producing: explorer/beta/%/directory-html.metapack
( 87%) Producing: explorer/%/directory-html.metapack
(100%) Producing: explorer/%/search.metapack
EOF

diff "$TMP/output3.txt" "$TMP/expected3.txt"
