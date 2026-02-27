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

mkdir "$TMP/schemas"

cat << 'EOF' > "$TMP/schemas/test.json"
{
  "$schema": "http://json-schema.org/draft-07/schema#",
  "$id": "https://example.com/test"
}
EOF

hash_directory() {
  { find "$1" -type f -exec md5sum {} \; 2>/dev/null \
    || find "$1" -type f -exec md5 -r {} \; ; } \
    | sort > "$2"
}

# Index successfully
"$1" "$TMP/one.json" "$TMP/output"
hash_directory "$TMP/output" "$TMP/snapshot_before.txt"

# Break the schema so the next index run fails
cat << 'EOF' > "$TMP/schemas/test.json"
{
  "$schema": "http://json-schema.org/draft-07/schema#",
  "$id": "https://example.com/test",
  "type": 1
}
EOF

# Re-index, expecting failure
"$1" "$TMP/one.json" "$TMP/output" 2>/dev/null && CODE="$?" || CODE="$?"
test "$CODE" = "1"

# Verify the output directory is untouched
hash_directory "$TMP/output" "$TMP/snapshot_after.txt"
diff "$TMP/snapshot_before.txt" "$TMP/snapshot_after.txt"

# Verify no hidden files or directories were left behind
HIDDEN="$(find "$TMP" -maxdepth 1 -name '.*' ! -name '.' | wc -l | tr -d ' ')"
test "$HIDDEN" = "0"
