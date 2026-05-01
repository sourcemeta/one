#!/bin/sh

# Verify that metadata.json is generated on initial build, NOT regenerated on a
# cached rebuild, and IS regenerated when the configuration URL changes (since
# its contents are derived from the configured URL).

set -o errexit
set -o nounset

TMP="$(mktemp -d)"
clean() { rm -rf "$TMP"; }
trap clean EXIT

cat << EOF > "$TMP/one.json"
{
  "url": "https://example.com/v1"
}
EOF

remove_threads_information() {
  expr='s/ \[[^]]*[^a-z-][^]]*\]//g'
  if [ "$(uname -s)" = "Darwin" ]; then
    sed -i '' "$expr" "$1"
  else
    sed -i "$expr" "$1"
  fi
}

# Run 1: initial build emits metadata.json with the URL and origin
"$1" --skip-banner --deterministic "$TMP/one.json" "$TMP/output" --concurrency 1 > /dev/null 2>&1

test -f "$TMP/output/metadata.json"

printf '%s' '{"url":"https://example.com/v1","origin":"https://example.com"}' \
  > "$TMP/expected_metadata_v1.txt"
diff "$TMP/output/metadata.json" "$TMP/expected_metadata_v1.txt"

# Capture the metadata.json contents as the cache witness for Run 2
cp "$TMP/output/metadata.json" "$TMP/metadata-after-run1.txt"

# Run 2: cached rebuild with no changes must NOT regenerate metadata.json
"$1" --skip-banner --deterministic "$TMP/one.json" "$TMP/output" --concurrency 1 2> "$TMP/output.txt"
remove_threads_information "$TMP/output.txt"

# A cached rebuild produces no work, so no Producing lines at all
grep -q "Producing:" "$TMP/output.txt" && exit 1

# And the file must be byte-identical to the Run 1 version
diff "$TMP/output/metadata.json" "$TMP/metadata-after-run1.txt"

# Run 3: change the configured URL so origin changes too
cat << EOF > "$TMP/one.json"
{
  "url": "https://other.example.com/v2"
}
EOF

"$1" --skip-banner --deterministic "$TMP/one.json" "$TMP/output" --concurrency 1 2> "$TMP/output.txt"
remove_threads_information "$TMP/output.txt"

# A configuration change forces a full rebuild that includes metadata.json
grep -q "Producing: metadata.json" "$TMP/output.txt"

# The file must now reflect the new URL and origin
printf '%s' '{"url":"https://other.example.com/v2","origin":"https://other.example.com"}' \
  > "$TMP/expected_metadata_v2.txt"
diff "$TMP/output/metadata.json" "$TMP/expected_metadata_v2.txt"
