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
    "left": {
      "contents": {
        "left-a": {
          "baseUri": "https://example.com/left/left-a/",
          "path": "./schemas-left-a"
        },
        "left-b": {
          "baseUri": "https://example.com/left/left-b/",
          "path": "./schemas-left-b"
        }
      }
    },
    "right": {
      "contents": {
        "right-a": {
          "baseUri": "https://example.com/right/right-a/",
          "path": "./schemas-right-a"
        },
        "right-b": {
          "baseUri": "https://example.com/right/right-b/",
          "path": "./schemas-right-b"
        }
      }
    }
  }
}
EOF

mkdir "$TMP/schemas-left-a" "$TMP/schemas-left-b" \
      "$TMP/schemas-right-a" "$TMP/schemas-right-b"

cat << 'EOF' > "$TMP/schemas-left-a/s1.json"
{
  "$schema": "http://json-schema.org/draft-07/schema#",
  "$id": "https://example.com/left/left-a/s1"
}
EOF

cat << 'EOF' > "$TMP/schemas-left-b/s2.json"
{
  "$schema": "http://json-schema.org/draft-07/schema#",
  "$id": "https://example.com/left/left-b/s2"
}
EOF

cat << 'EOF' > "$TMP/schemas-right-a/s3.json"
{
  "$schema": "http://json-schema.org/draft-07/schema#",
  "$id": "https://example.com/right/right-a/s3"
}
EOF

cat << 'EOF' > "$TMP/schemas-right-b/s4.json"
{
  "$schema": "http://json-schema.org/draft-07/schema#",
  "$id": "https://example.com/right/right-b/s4"
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

# Run 1: index four schemas across four directories
"$1" --skip-banner "$TMP/one.json" "$TMP/output" --concurrency 1 > /dev/null 2>&1

# Run 2: add a fifth schema to left/left-a
# Only the new schema's artifacts and affected directories should rebuild
cat << 'EOF' > "$TMP/schemas-left-a/s5.json"
{
  "$schema": "http://json-schema.org/draft-07/schema#",
  "$id": "https://example.com/left/left-a/s5"
}
EOF
"$1" --skip-banner "$TMP/one.json" "$TMP/output" --concurrency 1 2> "$TMP/output.txt"
remove_threads_information "$TMP/output.txt"
grep "Producing" "$TMP/output.txt" > "$TMP/output_producing.txt"

cat << 'EOF' > "$TMP/expected.txt"
(  4%) Producing: schemas/left/left-a/s5/%/schema.metapack
(  8%) Producing: schemas/left/left-a/s5/%/dependencies.metapack
( 12%) Producing: schemas/left/left-a/s5/%/locations.metapack
( 16%) Producing: schemas/left/left-a/s5/%/positions.metapack
( 20%) Producing: schemas/left/left-a/s5/%/stats.metapack
( 24%) Producing: dependency-tree.metapack
( 28%) Producing: schemas/left/left-a/s5/%/bundle.metapack
( 32%) Producing: schemas/left/left-a/s5/%/health.metapack
( 36%) Producing: explorer/left/left-a/s5/%/schema.metapack
( 40%) Producing: schemas/left/left-a/s1/%/dependents.metapack
( 44%) Producing: schemas/left/left-a/s5/%/blaze-exhaustive.metapack
( 48%) Producing: schemas/left/left-a/s5/%/blaze-fast.metapack
( 52%) Producing: schemas/left/left-a/s5/%/dependents.metapack
( 56%) Producing: schemas/left/left-a/s5/%/editor.metapack
( 60%) Producing: schemas/left/left-b/s2/%/dependents.metapack
( 64%) Producing: schemas/right/right-a/s3/%/dependents.metapack
( 68%) Producing: schemas/right/right-b/s4/%/dependents.metapack
( 72%) Producing: explorer/%/search.metapack
( 76%) Producing: explorer/left/left-a/%/directory.metapack
( 80%) Producing: explorer/left/left-a/s5/%/schema-html.metapack
( 84%) Producing: explorer/left/%/directory.metapack
( 88%) Producing: explorer/left/left-a/%/directory-html.metapack
( 92%) Producing: explorer/%/directory.metapack
( 96%) Producing: explorer/left/%/directory-html.metapack
(100%) Producing: explorer/%/directory-html.metapack
EOF

diff "$TMP/output_producing.txt" "$TMP/expected.txt"
