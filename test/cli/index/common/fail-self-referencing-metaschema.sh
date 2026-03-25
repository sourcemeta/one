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
  "$schema": "https://example.com/my-metaschema",
  "$id": "https://example.com/test"
}
EOF
cat << 'EOF' > "$TMP/schemas/my-metaschema.json"
{
  "$schema": "https://example.com/my-metaschema",
  "$id": "https://example.com/my-metaschema"
}
EOF
"$1" --skip-banner "$TMP/one.json" "$TMP/output" --concurrency 1 2> "$TMP/output.txt" && CODE="$?" || CODE="$?"
test "$CODE" = "1" || exit 1
# Remove thread information
if [ "$(uname)" = "Darwin" ]
then
  sed -i '' 's/ \[.*\]//g' "$TMP/output.txt"
else
  sed -i 's/ \[.*\]//g' "$TMP/output.txt"
fi
cat << EOF > "$TMP/expected.txt"
Writing output to: $(realpath "$TMP")/output
Using configuration: $(realpath "$TMP")/one.json
Detecting: $(realpath "$TMP")/schemas/test.json (#1)
Detecting: $(realpath "$TMP")/schemas/my-metaschema.json (#2)
( 50%) Resolving: test.json
(100%) Resolving: my-metaschema.json
(  2%) Producing: configuration.json
(  5%) Producing: version.json
(  8%) Producing: explorer/%/404.metapack
( 11%) Producing: schemas/example/schemas/my-metaschema/%/schema.metapack
error: Could not resolve the metaschema of the schema
  at identifier https://sourcemeta.com/example/schemas/my-metaschema
EOF
diff "$TMP/output.txt" "$TMP/expected.txt"
