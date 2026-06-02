#!/bin/sh

# Measures one cold indexer run over a registry of N trivial schemas,
# all using the standard draft 2020-12 dialect. Pairs with
# `index-custom-meta-schema.sh` to surface the per-schema overhead of a
# user-defined meta-schema vs the bundled standard one.

set -o errexit
set -o nounset

if [ "$#" -ne 2 ]
then
  echo "Usage: $0 <path/to/sourcemeta-one-index> <schema-count>" 1>&2
  exit 1
fi

INDEX="$1"
COUNT="$2"

if [ "$(uname -s)" = "Darwin" ]
then
  IS_DARWIN=1
else
  IS_DARWIN=0
fi

nanoseconds() {
  if [ "$IS_DARWIN" = "1" ]
  then
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

mkdir "$TMP/schemas"

echo "Filling registry to ${COUNT} schemas..." >&2
index=0
while [ "$index" -lt "$COUNT" ]
do
  cat << EOF > "$TMP/schemas/schema-$index.json"
{
  "\$schema": "https://json-schema.org/draft/2020-12/schema",
  "\$id": "https://example.com/schema-$index"
}
EOF
  index=$((index + 1))
done

echo "Measuring: index ${COUNT} schemas..." >&2
START="$(nanoseconds)"
"$INDEX" --skip-banner --maximum-direct-directory-entries 0 \
  "$TMP/one.json" "$TMP/output" --time >&2 2>/dev/null
END="$(nanoseconds)"
RESULT="$(( (END - START) / 1000000 ))"
echo "  Result: ${RESULT}ms" >&2

cat << EOF
[
  {
    "name": "Index ${COUNT} schemas",
    "unit": "ms",
    "value": ${RESULT}
  }
]
EOF
