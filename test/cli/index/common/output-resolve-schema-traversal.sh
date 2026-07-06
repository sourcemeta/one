#!/bin/sh

# A percent-encoded traversal in the resolved URI must never escape the
# collection directory into a sibling path.

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

mkdir "$TMP/schemas" "$TMP/secret"

cat << 'EOF' > "$TMP/schemas/foo.json"
{
  "$schema": "http://json-schema.org/draft-07/schema#",
  "$id": "https://example.com/foo"
}
EOF

cat << 'EOF' > "$TMP/secret/passwd.json"
{
  "$schema": "http://json-schema.org/draft-07/schema#",
  "$id": "https://example.com/passwd"
}
EOF

resolve_match() {
  RESULT="$("$1" "$TMP/one.json" "$TMP/output" --resolve-schema "$2" 2>/dev/null)"
  test "$RESULT" = "$3"
}

resolve_no_match() {
  "$1" "$TMP/one.json" "$TMP/output" --resolve-schema "$2" \
    && CODE="$?" || CODE="$?"
  test "$CODE" = "1"
}

# A legitimate resolution inside the collection still works
resolve_match "$1" "https://sourcemeta.com/schemas/foo.json" \
  "$(realpath "$TMP")/schemas/foo.json"

# Dot-segment traversals are removed and never reach the sibling directory
resolve_no_match "$1" "https://sourcemeta.com/schemas/%2E%2E/secret/passwd"
resolve_no_match "$1" \
  "https://sourcemeta.com/schemas/%2E%2E/%2E%2E/secret/passwd"
resolve_no_match "$1" "/schemas/%2E%2E/secret/passwd"

# A fully percent-encoded separator stays a single, contained filename rather
# than escaping into the sibling directory
resolve_match "$1" "https://sourcemeta.com/schemas/%2E%2E%2Fsecret%2Fpasswd" \
  "$(realpath "$TMP")/schemas/..%2Fsecret%2Fpasswd.json"
