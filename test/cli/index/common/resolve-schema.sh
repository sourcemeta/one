#!/bin/sh

set -o errexit
set -o nounset

TMP="$(mktemp -d)"
clean() { rm -rf "$TMP"; }
trap clean EXIT

resolve_schema_match() {
  RESULT="$("$1" "$2" "$TMP/output" --resolve-schema "$3" 2>/dev/null)"
  test "$RESULT" = "$4"
}

resolve_schema_match_short() {
  RESULT="$("$1" "$2" "$TMP/output" -r "$3" 2>/dev/null)"
  test "$RESULT" = "$4"
}

resolve_schema_no_match() {
  "$1" "$2" "$TMP/output" \
    --resolve-schema "$3" && CODE="$?" || CODE="$?"
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
resolve_schema_match "$1" "$TMP/one.json" \
  "https://sourcemeta.com/schemas/foo.json" \
  "$(realpath "$TMP")/schemas/foo.json"

# Absolute URI match (short option)
resolve_schema_match_short "$1" "$TMP/one.json" \
  "https://sourcemeta.com/schemas/foo.json" \
  "$(realpath "$TMP")/schemas/foo.json"

# Path-only match
resolve_schema_match "$1" "$TMP/one.json" \
  "/schemas/foo.json" \
  "$(realpath "$TMP")/schemas/foo.json"

# Path without .json extension appends .json
resolve_schema_match "$1" "$TMP/one.json" \
  "/schemas/foo" \
  "$(realpath "$TMP")/schemas/foo.json"

# Non-.json extension gets .json appended
resolve_schema_match "$1" "$TMP/one.json" \
  "/schemas/foo.schema" \
  "$(realpath "$TMP")/schemas/foo.schema.json"

# .yaml extension gets .json appended
resolve_schema_match "$1" "$TMP/one.json" \
  "/schemas/foo.yaml" \
  "$(realpath "$TMP")/schemas/foo.yaml.json"

# Dotted version-like name gets .json appended
resolve_schema_match "$1" "$TMP/one.json" \
  "/schemas/v1.2.3" \
  "$(realpath "$TMP")/schemas/v1.2.3.json"

# No match
resolve_schema_no_match "$1" "$TMP/one.json" "/unknown/foo"

# Wrong origin
resolve_schema_no_match "$1" "$TMP/one.json" \
  "https://other.com/schemas/foo.json"

# A full run to assert on standard error output
"$1" "$TMP/one.json" "$TMP/output" \
  --skip-banner \
  --resolve-schema "https://sourcemeta.com/schemas/foo.json" > "$TMP/output.txt" 2>&1
cat << EOF > "$TMP/expected.txt"
Writing output to: $(realpath "$TMP")/output
Using configuration: $(realpath "$TMP")/one.json
Resolving schema for URI: https://sourcemeta.com/schemas/foo.json
$(realpath "$TMP")/schemas/foo.json
EOF
diff "$TMP/output.txt" "$TMP/expected.txt"
