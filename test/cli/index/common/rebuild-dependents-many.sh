#!/bin/sh

# Modifying a schema that many others reference marks every dependent dirty in
# one propagation pass. That pass must not crash or corrupt the index as the
# dirty set grows and rehashes, and the incremental result must match a build
# from scratch.

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

cat << 'EOF' > "$TMP/schemas/common.json"
{
  "$schema": "http://json-schema.org/draft-07/schema#",
  "$id": "https://example.com/common",
  "type": "object"
}
EOF

# Many schemas reference the common one, so a change to it dirties all of them
index=1
while [ "$index" -le 20 ]; do
  cat << EOF > "$TMP/schemas/dependent-$index.json"
{
  "\$schema": "http://json-schema.org/draft-07/schema#",
  "\$id": "https://example.com/dependent-$index",
  "properties": {
    "value": { "\$ref": "https://example.com/common" }
  }
}
EOF
  index=$((index + 1))
done

extract_search_paths() {
  strings "$1/explorer/%/search.metapack" \
    | grep -oE '/example/[^[:space:]"]*' \
    | sed 's|https*://.*$||' \
    | LC_ALL=C sort -u
}

# Run 1: full build from scratch
"$1" --skip-banner "$TMP/one.json" "$TMP/output" > /dev/null 2>&1

# Run 2: modify the common schema so every dependent must rebuild
cat << 'EOF' > "$TMP/schemas/common.json"
{
  "$schema": "http://json-schema.org/draft-07/schema#",
  "$id": "https://example.com/common",
  "type": "object",
  "additionalProperties": false
}
EOF
"$1" --skip-banner "$TMP/one.json" "$TMP/output" > /dev/null 2>&1 \
  && CODE="$?" || CODE="$?"
test "$CODE" = "0" || exit 1

# A build from scratch of the final sources must produce the same catalog
"$1" --skip-banner "$TMP/one.json" "$TMP/scratch" > /dev/null 2>&1

# The full expected catalog, so an empty or truncated extraction fails rather
# than silently matching another empty extraction
{
  echo "/example/schemas/common"
  index=1
  while [ "$index" -le 20 ]; do
    echo "/example/schemas/dependent-$index"
    index=$((index + 1))
  done
} | LC_ALL=C sort -u > "$TMP/expected.txt"

extract_search_paths "$TMP/output" > "$TMP/incremental.txt"
diff "$TMP/incremental.txt" "$TMP/expected.txt"

extract_search_paths "$TMP/scratch" > "$TMP/scratch.txt"
diff "$TMP/scratch.txt" "$TMP/expected.txt"
