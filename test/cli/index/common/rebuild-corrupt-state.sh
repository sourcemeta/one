#!/bin/sh

# The state file is only an incremental-build cache. A corrupt, truncated, or
# empty one must be discarded in favour of a full rebuild rather than crash the
# indexer.

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

cat << 'EOF' > "$TMP/schemas/a.json"
{
  "$schema": "http://json-schema.org/draft-07/schema#",
  "$id": "https://example.com/a"
}
EOF

STATE="$TMP/output/state.bin"

reindex_survives() {
  "$1" --skip-banner "$TMP/one.json" "$TMP/output" > /dev/null 2>&1 \
    && CODE="$?" || CODE="$?"
  test "$CODE" = "0" || exit 1
  test -s "$STATE"
}

assert_rebuilt() {
  strings "$TMP/output/explorer/%/search.metapack" \
    | grep -oE '/example/[^[:space:]"]*' \
    | sed 's|https*://.*$||' \
    | LC_ALL=C sort -u > "$TMP/actual.txt"
  cat << 'EOF' > "$TMP/expected.txt"
/example/schemas/a
EOF
  diff "$TMP/actual.txt" "$TMP/expected.txt"
}

# Baseline: index once so a valid state file exists to corrupt
reindex_survives "$1"
assert_rebuilt

# An empty file cannot be mapped at all
: > "$STATE"
reindex_survives "$1"
assert_rebuilt

# A header that survives but a body truncated away
head -c 100 "$STATE" > "$STATE.truncated"
mv "$STATE.truncated" "$STATE"
reindex_survives "$1"
assert_rebuilt

# A non-power-of-two capacity breaks the probe mask
printf '\x07\x00\x00\x00' | dd of="$STATE" bs=1 seek=12 conv=notrunc 2>/dev/null
reindex_survives "$1"
assert_rebuilt

# A capacity whose table would run past the end of the file
printf '\xff\xff\xff\x7f' | dd of="$STATE" bs=1 seek=12 conv=notrunc 2>/dev/null
reindex_survives "$1"
assert_rebuilt
