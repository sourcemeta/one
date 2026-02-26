#!/bin/sh

set -o errexit
set -o nounset

TMP="$(mktemp -d)"
clean() { rm -rf "$TMP"; }
trap clean EXIT

cat << EOF > "$TMP/one.json"
{
  "url": "https://sourcemeta1.com/",
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

collect_inodes() {
  find "$1" -type f -exec ls -i {} \; \
    | awk '{print $2, $1}' | sort > "$2"
  # Confirm the file is not empty
  test "$(wc -l < "$2" | tr -d ' ')" -gt 0
  # For debugging
  cat "$2" 1>&2
}

"$1" "$TMP/one.json" "$TMP/output"
collect_inodes "$TMP/output" "$TMP/inodes_before.txt"

# Pin the original inodes by hard-linking every file into a mirror
# directory. This prevents the filesystem from reusing freed inode
# numbers when the indexer removes and recreates files.
cd "$TMP/output"
find . -type d | while read -r directory
do
  mkdir -p "$TMP/mirror/$directory"
done
find . -type f | while read -r file
do
  ln "$file" "$TMP/mirror/$file"
done
cd -

# Change the registry URL and modify the schema to force all targets to rebuild
cat << EOF > "$TMP/one.json"
{
  "url": "https://sourcemeta2.com/",
  "contents": {
    "schemas": {
      "baseUri": "https://example.com/",
      "path": "./schemas"
    }
  }
}
EOF

cat << 'EOF' > "$TMP/schemas/test.json"
{
  "$schema": "http://json-schema.org/draft-07/schema#",
  "$id": "https://example.com/test",
  "type": "string"
}
EOF

"$1" "$TMP/one.json" "$TMP/output"
collect_inodes "$TMP/output" "$TMP/inodes_after.txt"

# Exactly 1 shared inode is expected: the version mark. All other files
# must have been removed and recreated with fresh inodes.
SHARED="$(grep --fixed-strings --line-regexp \
  --file="$TMP/inodes_before.txt" "$TMP/inodes_after.txt" || true)"
test "$(echo "$SHARED" | wc -l | tr -d ' ')" = "1"
echo "$SHARED" | grep --quiet "version.json"
