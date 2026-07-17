#!/bin/sh

set -o errexit
set -o nounset

TMP="$(mktemp -d)"
clean() { rm -rf "$TMP"; }
trap clean EXIT

cat << 'EOF' > "$TMP/one.json"
{
  "url": "http://localhost:8000",
  "authentication": [
    { "type": "apiKey", "algorithm": "identity", "name": "internal", "paths": [ "/internal" ], "keys": [] }
  ],
  "contents": {}
}
EOF

"$1" --skip-banner "$TMP/one.json" "$TMP/output" \
  > "$TMP/output.txt" && CODE="$?" || CODE="$?"
test "$CODE" = "1" || exit 1

cat << EOF > "$TMP/expected.txt"
error: Invalid configuration
  at path $(realpath "$TMP")/one.json
The array value was expected to contain at least 1 item but it contained 0 items
  at instance location "/authentication/0/keys"
  at evaluate path "/properties/authentication/items/anyOf/0/properties/keys/minItems"
The object value was expected to validate against the defined properties subschemas
  at instance location "/authentication/0"
  at evaluate path "/properties/authentication/items/anyOf/0/properties"
The value was expected to be an object that defines properties "algorithms", "audience", "issuer", "name", "paths", and "type"
  at instance location "/authentication/0"
  at evaluate path "/properties/authentication/items/anyOf/1/required"
The value was expected to be an object that defines properties "clientId", "clientSecret", "issuer", "name", "paths", "sessionSecret", and "type"
  at instance location "/authentication/0"
  at evaluate path "/properties/authentication/items/anyOf/2/required"
The object value was expected to validate against at least one of the 3 given subschemas
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
