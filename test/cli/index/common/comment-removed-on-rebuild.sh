#!/bin/sh

set -o errexit
set -o nounset

TMP="$(mktemp -d)"
clean() { rm -rf "$TMP"; }
trap clean EXIT

cat << EOF > "$TMP/one.json"
{
  "url": "http://localhost:8000"
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

# First run with --comment

"$1" --skip-banner "$TMP/one.json" "$TMP/output" --comment "hello" --concurrency 1 2> "$TMP/output.txt"
remove_threads_information "$TMP/output.txt"

cat << EOF > "$TMP/expected.txt"
Writing output to: $(realpath "$TMP")/output
Using configuration: $(realpath "$TMP")/one.json
( 12%) Producing: comment.json
( 25%) Producing: configuration.json
( 37%) Producing: version.json
( 50%) Producing: explorer/%/404.metapack
( 62%) Producing: explorer/%/directory.metapack
( 75%) Producing: explorer/%/directory-html.metapack
( 87%) Producing: explorer/%/search.metapack
(100%) Producing: routes.bin
EOF

diff "$TMP/output.txt" "$TMP/expected.txt"

printf '"hello"' > "$TMP/expected_comment.txt"
diff "$TMP/output/comment.json" "$TMP/expected_comment.txt"

# Second run without --comment

"$1" --skip-banner "$TMP/one.json" "$TMP/output" --concurrency 1 2> "$TMP/output.txt"
remove_threads_information "$TMP/output.txt"

cat << EOF > "$TMP/expected.txt"
Writing output to: $(realpath "$TMP")/output
Using configuration: $(realpath "$TMP")/one.json
(100%) Disposing: comment.json
EOF

diff "$TMP/output.txt" "$TMP/expected.txt"

! test -f "$TMP/output/comment.json"
