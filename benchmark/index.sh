#!/bin/sh

set -o errexit
set -o nounset

if [ "$(uname -s)" = "Darwin" ]
then
  IS_DARWIN=1
else
  IS_DARWIN=0
fi

nanoseconds() {
  if [ "$IS_DARWIN" = "1" ]; then
    perl -MTime::HiRes=time -e 'printf "%d\n", time * 1000000000'
  else
    date +%s%N
  fi
}

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

START="$(nanoseconds)"
"$1" --skip-banner "$TMP/one.json" "$TMP/output" 2> /dev/null
END="$(nanoseconds)"

ELAPSED="$(( (END - START) / 1000000 ))"

cat << EOF
[
    {
        "name": "Add One Schema",
        "unit": "ms",
        "value": $ELAPSED
    }
]
EOF
