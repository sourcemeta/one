#!/bin/sh

# When modifying a schema in one directory of a nested layout, the search
# index must still contain schemas from ALL directories, not just the
# affected one.

set -o errexit
set -o nounset

TMP="$(mktemp -d)"
clean() { rm -rf "$TMP"; }
trap clean EXIT

cat << EOF > "$TMP/one.json"
{
  "extends": [ "@self/v1" ],
  "url": "https://sourcemeta.com/",
  "contents": {
    "left": {
      "contents": {
        "schemas": {
          "baseUri": "https://example.com/left/",
          "path": "./schemas-left"
        }
      }
    },
    "right": {
      "contents": {
        "schemas": {
          "baseUri": "https://example.com/right/",
          "path": "./schemas-right"
        }
      }
    }
  }
}
EOF

mkdir "$TMP/schemas-left" "$TMP/schemas-right"

cat << 'EOF' > "$TMP/schemas-left/a.json"
{
  "$schema": "http://json-schema.org/draft-07/schema#",
  "$id": "https://example.com/left/a"
}
EOF

cat << 'EOF' > "$TMP/schemas-right/b.json"
{
  "$schema": "http://json-schema.org/draft-07/schema#",
  "$id": "https://example.com/right/b"
}
EOF

SEARCH="$TMP/output/explorer/%/search.metapack"

extract_search_paths() {
  strings "$1" | grep -E '^/(left|right)/' | LC_ALL=C sort
}

# Run 1: full build with two schemas in separate directories
"$1" --skip-banner "$TMP/one.json" "$TMP/output" --concurrency 1 > /dev/null 2>&1

extract_search_paths "$SEARCH" > "$TMP/search_actual.txt"
cat << 'EOF' > "$TMP/search_expected.txt"
/left/schemas/a
/right/schemas/b
EOF
diff "$TMP/search_actual.txt" "$TMP/search_expected.txt"

# Run 2: modify only the left schema
cat << 'EOF' > "$TMP/schemas-left/a.json"
{
  "$schema": "http://json-schema.org/draft-07/schema#",
  "$id": "https://example.com/left/a",
  "type": "string"
}
EOF

"$1" --skip-banner "$TMP/one.json" "$TMP/output" --concurrency 1 > /dev/null 2>&1

extract_search_paths "$SEARCH" > "$TMP/search_actual.txt"
cat << 'EOF' > "$TMP/search_expected.txt"
/left/schemas/a
/right/schemas/b
EOF
diff "$TMP/search_actual.txt" "$TMP/search_expected.txt"

# Run 3: add a third schema to the right directory
cat << 'EOF' > "$TMP/schemas-right/c.json"
{
  "$schema": "http://json-schema.org/draft-07/schema#",
  "$id": "https://example.com/right/c"
}
EOF

"$1" --skip-banner "$TMP/one.json" "$TMP/output" --concurrency 1 > /dev/null 2>&1

extract_search_paths "$SEARCH" > "$TMP/search_actual.txt"
cat << 'EOF' > "$TMP/search_expected.txt"
/left/schemas/a
/right/schemas/b
/right/schemas/c
EOF
diff "$TMP/search_actual.txt" "$TMP/search_expected.txt"
