#!/bin/sh

set -o errexit
set -o nounset

TMP="$(mktemp -d)"
clean() { rm -rf "$TMP"; }
trap clean EXIT

cat << 'EOF' > "$TMP/one.json"
{
  "url": "http://localhost:8000",
  "api": false,
  "html": {
    "name": "Title",
    "description": "Description"
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
The object value {"name":"Title","description":"Description"} was expected to equal the boolean constant false
  at instance location "/html"
  at evaluate path "/dependentSchemas/api/then/properties/html/const"
The object value was expected to validate against the single defined property subschema
  at instance location ""
  at evaluate path "/dependentSchemas/api/then/properties"
The object value defined the property "api"
  at instance location ""
  at evaluate path "/dependentSchemas"
Because the object value defined the property "api", it was also expected to validate against the corresponding subschema
  at instance location ""
  at evaluate path "/dependentSchemas"
EOF

diff "$TMP/output.txt" "$TMP/expected.txt"
