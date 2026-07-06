#!/bin/sh

# A configuration whose `extends` is not an array must fail with a schema
# validation error rather than crash while prepending the self reference.

set -o errexit
set -o nounset

TMP="$(mktemp -d)"
clean() { rm -rf "$TMP"; }
trap clean EXIT

assert_invalid() {
  printf '{ "extends": %s, "url": "https://sourcemeta.com/" }\n' "$2" \
    > "$TMP/one.json"
  "$1" --skip-banner "$TMP/one.json" "$TMP/output" > "$TMP/output.txt" \
    && CODE="$?" || CODE="$?"
  test "$CODE" = "1" || exit 1

  cat << EOF > "$TMP/expected.txt"
error: Invalid configuration
  at path $(realpath "$TMP")/one.json
The value was expected to be of type array but it was of type $3
  at instance location "/extends"
  at evaluate path "/properties/extends/type"
The object value was expected to validate against the 6 defined properties subschemas
  at instance location ""
  at evaluate path "/properties"
EOF
  diff "$TMP/output.txt" "$TMP/expected.txt"
}

assert_invalid "$1" '"foo"' 'string'
assert_invalid "$1" '1' 'integer'
assert_invalid "$1" '1.5' 'number'
assert_invalid "$1" 'true' 'boolean'
assert_invalid "$1" '{}' 'object'
