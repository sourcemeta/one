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

normalize_staging_path() {
  expr='s|\.sourcemeta-one-[^/ ]*|.sourcemeta-one-XXXXXX|g'
  if [ "$(uname -s)" = "Darwin" ]; then
    sed -i '' "$expr" "$1"
  else
    sed -i "$expr" "$1"
  fi
}

# Simulate stale staging entries left behind by crashed previous runs
mkdir "$TMP/.sourcemeta-one-aaa"
mkdir "$TMP/.sourcemeta-one-bbb"
mkdir "$TMP/.sourcemeta-one-ccc"
echo "garbage" > "$TMP/.sourcemeta-one-aaa/file.txt"
echo "garbage" > "$TMP/.sourcemeta-one-ddd"

# Hidden entries that do NOT match the staging prefix must be preserved
mkdir "$TMP/.hidden-directory"
echo "keep" > "$TMP/.hidden-directory/data.txt"
mkdir "$TMP/.another-hidden"
echo "keep" > "$TMP/.dotfile"

"$1" "$TMP/one.json" "$TMP/output" --concurrency 1 2> "$TMP/output.txt"
remove_threads_information "$TMP/output.txt"
normalize_staging_path "$TMP/output.txt"

# The directory_iterator order is unspecified, so sort the stale removal lines
grep -v "Removing stale" "$TMP/output.txt" > "$TMP/output_main.txt"
grep "Removing stale" "$TMP/output.txt" | sort >> "$TMP/output_main.txt"
mv "$TMP/output_main.txt" "$TMP/output.txt"

cat << EOF > "$TMP/expected.txt"
Writing output to: $(realpath "$TMP")/output
Using configuration: $(realpath "$TMP")/one.json
Detecting: $(realpath "$TMP")/schemas/foo.json (#1)
(100%) Ingesting: https://sourcemeta.com/example/schemas/foo
(100%) Analysing: https://sourcemeta.com/example/schemas/foo
( 50%) Reviewing: schemas
(100%) Reviewing: schemas
(100%) Reworking: https://sourcemeta.com/example/schemas/foo
(  0%) Producing: explorer
( 33%) Producing: example/schemas
( 66%) Producing: example
(100%) Producing: .
( 25%) Rendering: example/schemas
( 50%) Rendering: example
( 75%) Rendering: .
(100%) Rendering: example/schemas/foo
Committing: $(realpath "$TMP")/.sourcemeta-one-XXXXXX => $(realpath "$TMP")/output
Removing stale staging entry: $(realpath "$TMP")/.sourcemeta-one-XXXXXX
Removing stale staging entry: $(realpath "$TMP")/.sourcemeta-one-XXXXXX
Removing stale staging entry: $(realpath "$TMP")/.sourcemeta-one-XXXXXX
Removing stale staging entry: $(realpath "$TMP")/.sourcemeta-one-XXXXXX
EOF
diff "$TMP/output.txt" "$TMP/expected.txt"

# Verify the stale staging entries were removed
test ! -d "$TMP/.sourcemeta-one-aaa"
test ! -d "$TMP/.sourcemeta-one-bbb"
test ! -d "$TMP/.sourcemeta-one-ccc"
test ! -f "$TMP/.sourcemeta-one-ddd"

# Verify non-matching hidden entries were preserved
test -d "$TMP/.hidden-directory"
test -f "$TMP/.hidden-directory/data.txt"
test -d "$TMP/.another-hidden"
test -f "$TMP/.dotfile"
