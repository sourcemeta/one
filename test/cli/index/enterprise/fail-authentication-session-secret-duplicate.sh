#!/bin/sh

# The secrets a policy names are tried in order, so naming the same variable
# twice adds nothing and reads as the mistake it is: somebody meant to name the
# secret being replaced and named the one replacing it again, leaving a
# rotation that retires nothing while appearing to have been carried out.

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
      "type": "oidc",
      "name": "corporate",
      "paths": [ "/internal" ],
      "issuer": "https://login.example.com",
      "clientId": "registry",
      "clientSecret": { "environmentVariable": "ONE_TEST_OIDC_CLIENT" },
      "sessionSecrets": [
        { "environmentVariable": "ONE_TEST_OIDC_SESSION" },
        { "environmentVariable": "ONE_TEST_OIDC_SESSION" }
      ]
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
The object value was expected to only define properties "algorithm", "keys", "name", "paths", and "type", but it also defines properties "clientId", "clientSecret", "issuer", and "sessionSecrets"
  at instance location "/authentication/0"
  at evaluate path "/properties/authentication/items/anyOf/0/required"
The value was expected to be an object that defines properties "algorithms", "audience", "issuer", "name", "paths", and "type"
  at instance location "/authentication/0"
  at evaluate path "/properties/authentication/items/anyOf/1/required"
The array value contained the following duplicate item: {"environmentVariable":"ONE_TEST_OIDC_SESSION"}
  at instance location "/authentication/0/sessionSecrets"
  at evaluate path "/properties/authentication/items/anyOf/2/properties/sessionSecrets/uniqueItems"
The object value was expected to validate against the defined properties subschemas
  at instance location "/authentication/0"
  at evaluate path "/properties/authentication/items/anyOf/2/properties"
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
