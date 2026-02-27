#!/bin/sh

set -o errexit
set -o nounset

TMP="$(mktemp -d)"
clean() { rm -rf "$TMP"; }
trap clean EXIT

resolve_path_match() {
  RESULT="$("$1" "$2" "$TMP/output" --resolve-path "$3" 2>/dev/null)"
  test "$RESULT" = "$4"
}

resolve_path_match_short() {
  RESULT="$("$1" "$2" "$TMP/output" -r "$3" 2>/dev/null)"
  test "$RESULT" = "$4"
}

resolve_path_match_with_url() {
  RESULT="$("$1" "$2" "$TMP/output" \
    --url "$3" --resolve-path "$4" 2>/dev/null)"
  test "$RESULT" = "$5"
}

resolve_path_no_match() {
  "$1" "$2" "$TMP/output" \
    --resolve-path "$3" && CODE="$?" || CODE="$?"
  test "$CODE" = "1"
}

resolve_path_no_match_with_url() {
  "$1" "$2" "$TMP/output" \
    --url "$3" --resolve-path "$4" && CODE="$?" || CODE="$?"
  test "$CODE" = "1"
}

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

cat << 'EOF' > "$TMP/schemas/foo.json"
{
  "$schema": "http://json-schema.org/draft-07/schema#",
  "$id": "https://example.com/foo"
}
EOF

# Absolute URI match (long option)
resolve_path_match "$1" "$TMP/one.json" \
  "https://sourcemeta.com/schemas/foo.json" \
  "$(realpath "$TMP")/schemas/foo.json"

# Absolute URI match (short option)
resolve_path_match_short "$1" "$TMP/one.json" \
  "https://sourcemeta.com/schemas/foo.json" \
  "$(realpath "$TMP")/schemas/foo.json"

# Path-only match
resolve_path_match "$1" "$TMP/one.json" \
  "/schemas/foo.json" \
  "$(realpath "$TMP")/schemas/foo.json"

# No match
resolve_path_no_match "$1" "$TMP/one.json" "/unknown/foo"

# Wrong origin
resolve_path_no_match "$1" "$TMP/one.json" \
  "https://other.com/schemas/foo.json"

# --url override changes which URIs match
resolve_path_match_with_url "$1" "$TMP/one.json" \
  "https://other.com/" "https://other.com/schemas/foo.json" \
  "$(realpath "$TMP")/schemas/foo.json"

# --url override makes original URL stop matching
resolve_path_no_match_with_url "$1" "$TMP/one.json" \
  "https://other.com/" "https://sourcemeta.com/schemas/foo.json"
