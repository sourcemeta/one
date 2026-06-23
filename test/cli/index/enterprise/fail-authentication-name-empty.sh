#!/bin/sh

set -o errexit
set -o nounset

TMP="$(mktemp -d)"
clean() { rm -rf "$TMP"; }
trap clean EXIT

mkdir "$TMP/internal"

cat << 'EOF' > "$TMP/internal/secret.json"
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
      "algorithm": "identity",
      "name": "",
      "paths": [ "/internal" ],
      "keys": [ { "environmentVariable": "ONE_TEST_KEY_INTERNAL" } ]
    }
  ],
  "contents": {
    "internal": { "path": "./internal" }
  }
}
EOF

"$1" --skip-banner "$TMP/one.json" "$TMP/output" \
  > "$TMP/output.txt" && CODE="$?" || CODE="$?"
test "$CODE" = "1" || exit 1

cat << EOF > "$TMP/expected.txt"
error: Invalid configuration
  at path $(realpath "$TMP")/one.json
The object value was expected to only define properties "paths", and "type", but it also defines properties "algorithm", "keys", and "name"
  at instance location "/authentication/0"
  at evaluate path "/properties/authentication/items/anyOf/0/required"
The string value "" was expected to match the regular expression "^[a-z0-9-]+\$"
  at instance location "/authentication/0/name"
  at evaluate path "/properties/authentication/items/anyOf/1/properties/name/pattern"
The object value was expected to validate against the defined properties subschemas
  at instance location "/authentication/0"
  at evaluate path "/properties/authentication/items/anyOf/1/properties"
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
