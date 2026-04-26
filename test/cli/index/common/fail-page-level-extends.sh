#!/bin/sh

set -o errexit
set -o nounset

TMP="$(mktemp -d)"
clean() { rm -rf "$TMP"; }
trap clean EXIT

cat << 'EOF' > "$TMP/one.json"
{
  "url": "http://localhost:8000",
  "contents": {
    "example": {
      "title": "Test",
      "extends": [ "./other.json" ]
    }
  }
}
EOF

"$1" --skip-banner "$TMP/one.json" "$TMP/output" 2> "$TMP/output.txt" && CODE="$?" || CODE="$?"
test "$CODE" = "1" || exit 1

cat << EOF > "$TMP/expected.txt"
Writing output to: $(realpath "$TMP")/output
Using configuration: $(realpath "$TMP")/one.json
error: Invalid configuration
  at path $(realpath "$TMP")/one.json
The value was expected to be an object that defines the property "include"
  at instance location "/contents/example"
  at evaluate path "/properties/contents/\$ref/additionalProperties/anyOf/0/required"
The object value was not expected to define the property "extends"
  at instance location "/contents/example/extends"
  at evaluate path "/properties/contents/\$ref/additionalProperties/anyOf/1/\$ref/additionalProperties"
The object value was not expected to define additional properties
  at instance location "/contents/example"
  at evaluate path "/properties/contents/\$ref/additionalProperties/anyOf/1/\$ref/additionalProperties"
The object value was expected to validate against the referenced schema
  at instance location "/contents/example"
  at evaluate path "/properties/contents/\$ref/additionalProperties/anyOf/1/\$ref"
The object value was not expected to define the property "extends"
  at instance location "/contents/example/extends"
  at evaluate path "/properties/contents/\$ref/additionalProperties/anyOf/2/\$ref/additionalProperties"
The object value was not expected to define additional properties
  at instance location "/contents/example"
  at evaluate path "/properties/contents/\$ref/additionalProperties/anyOf/2/\$ref/additionalProperties"
The object value was expected to validate against the referenced schema
  at instance location "/contents/example"
  at evaluate path "/properties/contents/\$ref/additionalProperties/anyOf/2/\$ref"
The object value was expected to validate against at least one of the 3 given subschemas
  at instance location "/contents/example"
  at evaluate path "/properties/contents/\$ref/additionalProperties/anyOf"
The object properties not covered by other adjacent object keywords were expected to validate against this subschema
  at instance location "/contents"
  at evaluate path "/properties/contents/\$ref/additionalProperties"
The object value was expected to validate against the referenced schema
  at instance location "/contents"
  at evaluate path "/properties/contents/\$ref"
The object value was expected to validate against the 5 defined properties subschemas
  at instance location ""
  at evaluate path "/properties"
EOF

diff "$TMP/output.txt" "$TMP/expected.txt"
