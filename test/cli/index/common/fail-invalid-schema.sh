#!/bin/sh

set -o errexit
set -o nounset

TMP="$(mktemp -d)"
clean() { rm -rf "$TMP"; }
trap clean EXIT

cat << EOF > "$TMP/one.json"
{
  "url": "https://sourcemeta.com/",
  "contents": {
    "example": {
      "contents": {
        "schemas": {
          "baseUri": "https://example.com/",
          "path": "./schemas"
        }
      }
    }
  }
}
EOF

mkdir "$TMP/schemas"

cat << 'EOF' > "$TMP/schemas/test.json"
{
  "$schema": "http://json-schema.org/draft-07/schema#",
  "$id": "https://example.com/test.json",
  "type": 1
}
EOF

"$1" --skip-banner "$TMP/one.json" "$TMP/output" > "$TMP/output.txt" && CODE="$?" || CODE="$?"
test "$CODE" = "1" || exit 1

cat << EOF > "$TMP/expected.txt"
error: The schema does not adhere to its metaschema
The integer value 1 was expected to equal one of the following values: "array", "boolean", "integer", "null", "number", "object", and "string"
  at instance location "/type"
  at evaluate path "/properties/type/anyOf/0/\$ref/enum"
The integer value was expected to validate against the referenced schema
  at instance location "/type"
  at evaluate path "/properties/type/anyOf/0/\$ref"
The value was expected to be of type array but it was of type integer
  at instance location "/type"
  at evaluate path "/properties/type/anyOf/1/type"
The integer value was expected to validate against at least one of the 2 given subschemas
  at instance location "/type"
  at evaluate path "/properties/type/anyOf"
The object value was expected to validate against the 46 defined properties subschemas
  at instance location ""
  at evaluate path "/properties"
EOF

diff "$TMP/output.txt" "$TMP/expected.txt"
