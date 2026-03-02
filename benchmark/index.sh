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
    "schemas": {
      "baseUri": "https://example.com/",
      "path": "./schemas"
    }
  }
}
EOF

# First run: the schemas directory has a single schema
mkdir "$TMP/schemas"

cat << 'EOF' > "$TMP/schemas/first.json"
{
  "$schema": "http://json-schema.org/draft-07/schema#",
  "$id": "https://example.com/first"
}
EOF

"$1" --skip-banner "$TMP/one.json" "$TMP/output" 2> /dev/null

# Second run: add another schema and measure the rebuild
cat << 'EOF' > "$TMP/schemas/second.json"
{
  "$schema": "http://json-schema.org/draft-07/schema#",
  "$id": "https://example.com/second"
}
EOF

START="$(perl -MTime::HiRes=time -e 'printf "%.9f\n", time')"
"$1" --skip-banner "$TMP/one.json" "$TMP/output" 2> /dev/null
END="$(perl -MTime::HiRes=time -e 'printf "%.9f\n", time')"

ELAPSED="$(perl -e "printf '%.6f', ($END - $START) * 1000")"

cat << EOF
[
    {
        "name": "Index / Add One Schema",
        "unit": "Milliseconds",
        "value": $ELAPSED
    }
]
EOF
