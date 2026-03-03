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
( 50%) Reviewing: schemas
(100%) Reviewing: schemas
(  0%) Producing: explorer
(100%) Producing: .
(100%) Rendering: .
Committing: $(realpath "$TMP")/output.staging => $(realpath "$TMP")/output
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
Hardlinking: $(realpath "$TMP")/output => $(realpath "$TMP")/output.staging
( 50%) Reviewing: schemas
(100%) Reviewing: schemas
(  0%) Producing: explorer
(100%) Producing: .
(skip) Producing: . [directory]
(100%) Rendering: .
(skip) Rendering: . [index]
(skip) Rendering: . [not-found]
(skip) Producing: routes.bin [routes]
Committing: $(realpath "$TMP")/output.staging => $(realpath "$TMP")/output
EOF

diff "$TMP/output.txt" "$TMP/expected.txt"

! test -f "$TMP/output/comment.json"
