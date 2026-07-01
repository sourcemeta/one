#!/bin/sh

set -o errexit
set -o nounset

TMP="$(mktemp -d)"
clean() { rm -rf "$TMP"; }
trap clean EXIT

mkdir "$TMP/private"

cat << 'EOF' > "$TMP/private/secret.json"
{
  "$schema": "https://json-schema.org/draft/2020-12/schema",
  "type": "object"
}
EOF

cat << 'EOF' > "$TMP/one.json"
{
  "url": "http://localhost:8000",
  "authentication": [
    {
      "type": "apiKey",
      "algorithm": "md5",
      "name": "reports",
      "paths": [ "/private" ],
      "keys": [ { "environmentVariable": "ONE_TEST_KEY_REPORTS" } ]
    }
  ],
  "contents": {
    "private": { "path": "./private" }
  }
}
EOF

"$1" --skip-banner "$TMP/one.json" "$TMP/output" \
  > "$TMP/output.txt" && CODE="$?" || CODE="$?"
test "$CODE" = "1" || exit 1

cat << EOF > "$TMP/expected.txt"
error: Invalid configuration
  at path $(realpath "$TMP")/one.json
The string value "md5" was expected to equal one of the following values: "identity", and "sha256"
  at instance location "/authentication/0/algorithm"
  at evaluate path "/properties/authentication/items/anyOf/0/properties/algorithm/enum"
The object value was expected to validate against the defined properties subschemas
  at instance location "/authentication/0"
  at evaluate path "/properties/authentication/items/anyOf/0/properties"
The value was expected to be an object that defines properties "algorithms", "audience", "issuer", "name", "paths", and "type"
  at instance location "/authentication/0"
  at evaluate path "/properties/authentication/items/anyOf/1/required"
The object value was expected to validate against at least one of the 2 given subschemas
  at instance location "/authentication/0"
  at evaluate path "/properties/authentication/items/anyOf"
Every item in the array value was expected to validate against the given subschema
  at instance location "/authentication"
  at evaluate path "/properties/authentication/items"
The object value was expected to validate against the 6 defined properties subschemas
  at instance location ""
  at evaluate path "/properties"
EOF

diff "$TMP/output.txt" "$TMP/expected.txt"
