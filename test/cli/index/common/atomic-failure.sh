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

# Index successfully
"$1" "$TMP/one.json" "$TMP/output"

# Snapshot the output directory state
find "$TMP/output" -type f | sort > "$TMP/files_before.txt"
{ find "$TMP/output" -type f -exec md5sum {} \; 2>/dev/null \
  || find "$TMP/output" -type f -exec md5 -r {} \; ; } \
  | sort > "$TMP/checksums_before.txt"

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
find "$TMP/output" -type f | sort > "$TMP/files_after.txt"
{ find "$TMP/output" -type f -exec md5sum {} \; 2>/dev/null \
  || find "$TMP/output" -type f -exec md5 -r {} \; ; } \
  | sort > "$TMP/checksums_after.txt"

diff "$TMP/files_before.txt" "$TMP/files_after.txt"
diff "$TMP/checksums_before.txt" "$TMP/checksums_after.txt"

# Verify no staging directories were left behind
STALE="$(find "$TMP" -maxdepth 1 -type d -name '.sourcemeta-one-*' | wc -l | tr -d ' ')"
test "$STALE" = "0"
