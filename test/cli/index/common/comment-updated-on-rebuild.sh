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

# First run with --comment "first"

"$1" --skip-banner "$TMP/one.json" "$TMP/output" --comment "first" --concurrency 1 2> "$TMP/output.txt"
remove_threads_information "$TMP/output.txt"

cat << EOF > "$TMP/expected.txt"
Writing output to: $(realpath "$TMP")/output
Using configuration: $(realpath "$TMP")/one.json
( 11%) Producing: comment.json
( 22%) Producing: configuration.json
( 33%) Producing: version.json
( 44%) Producing: dependency-tree.metapack
( 55%) Producing: explorer/%/404.metapack
( 66%) Producing: explorer/%/directory.metapack
( 77%) Producing: explorer/%/search.metapack
( 88%) Producing: explorer/%/directory-html.metapack
(100%) Producing: routes.bin
EOF

diff "$TMP/output.txt" "$TMP/expected.txt"

printf '"first"' > "$TMP/expected_comment.txt"
diff "$TMP/output/comment.json" "$TMP/expected_comment.txt"

# Second run with --comment "second"

"$1" --skip-banner "$TMP/one.json" "$TMP/output" --comment "second" --concurrency 1 2> "$TMP/output.txt"
remove_threads_information "$TMP/output.txt"

cat << EOF > "$TMP/expected.txt"
Writing output to: $(realpath "$TMP")/output
Using configuration: $(realpath "$TMP")/one.json
(100%) Producing: comment.json
EOF

diff "$TMP/output.txt" "$TMP/expected.txt"

printf '"second"' > "$TMP/expected_comment.txt"
diff "$TMP/output/comment.json" "$TMP/expected_comment.txt"
