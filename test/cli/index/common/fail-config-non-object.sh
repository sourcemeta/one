#!/bin/sh

# A configuration whose root is not an object must fail with a schema
# validation error rather than crash while normalising the document.

set -o errexit
set -o nounset

TMP="$(mktemp -d)"
clean() { rm -rf "$TMP"; }
trap clean EXIT

assert_invalid() {
  printf '%s\n' "$2" > "$TMP/one.json"
  "$1" --skip-banner "$TMP/one.json" "$TMP/output" > "$TMP/output.txt" \
    && CODE="$?" || CODE="$?"
  test "$CODE" = "1" || exit 1

  cat << EOF > "$TMP/expected.txt"
error: Invalid configuration
  at path $(realpath "$TMP")/one.json
The value was expected to be an object that defines the property "url"
  at instance location ""
  at evaluate path "/required"
EOF
  diff "$TMP/output.txt" "$TMP/expected.txt"
}

assert_invalid "$1" '[]'
assert_invalid "$1" '"sourcemeta"'
assert_invalid "$1" '1'
assert_invalid "$1" 'true'
assert_invalid "$1" 'null'
