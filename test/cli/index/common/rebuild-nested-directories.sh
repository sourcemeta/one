#!/bin/sh

set -o errexit
set -o nounset

TMP="$(mktemp -d)"
clean() { rm -rf "$TMP"; }
trap clean EXIT

cat << EOF > "$TMP/one.json"
{
  "extends": [ "@self/v1" ],
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
grep -E "Producing|Combining" "$TMP/output.txt" > "$TMP/output_producing.txt"

# TODO(over-rebuild): Adding s5 causes dependents.metapack to rebuild for
# s1, s2, s3, and s4 even though none of these schemas reference each other.
# The dependency-tree.metapack global aggregate triggers ForceOnGraphChange
# for all dependents, not just the ones whose graph actually changed.
cat << 'EOF' > "$TMP/expected.txt"
(  5%) Producing: schemas/left/left-a/s5/%/schema.metapack
( 10%) Producing: schemas/left/left-a/s5/%/dependencies.metapack
( 15%) Producing: schemas/left/left-a/s5/%/locations.metapack
( 21%) Producing: schemas/left/left-a/s5/%/positions.metapack
( 26%) Producing: schemas/left/left-a/s5/%/stats.metapack
( 31%) Producing: schemas/left/left-a/s5/%/bundle.metapack
( 36%) Producing: schemas/left/left-a/s5/%/health.metapack
( 42%) Producing: explorer/left/left-a/s5/%/schema.metapack
( 47%) Producing: schemas/left/left-a/s5/%/blaze-exhaustive.metapack
( 52%) Producing: schemas/left/left-a/s5/%/blaze-fast.metapack
( 57%) Producing: schemas/left/left-a/s5/%/editor.metapack
( 63%) Producing: explorer/left/left-a/%/directory.metapack
( 68%) Producing: explorer/left/left-a/s5/%/schema-html.metapack
( 73%) Producing: explorer/left/%/directory.metapack
( 78%) Producing: explorer/left/left-a/%/directory-html.metapack
( 84%) Producing: explorer/%/directory.metapack
( 89%) Producing: explorer/left/%/directory-html.metapack
( 94%) Producing: explorer/%/directory-html.metapack
(100%) Producing: explorer/%/search.metapack
(100%) Combining: schemas/left/left-a/s5/%/dependents.metapack
EOF

diff "$TMP/output_producing.txt" "$TMP/expected.txt"
