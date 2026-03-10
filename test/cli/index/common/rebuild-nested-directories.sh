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

cat << EOF > "$TMP/expected_a.txt"
Writing output to: $(realpath "$TMP")/output
Using configuration: $(realpath "$TMP")/one.json
Detecting: $(realpath "$TMP")/schemas-left-b/s2.json (#1)
Detecting: $(realpath "$TMP")/schemas-left-a/s1.json (#2)
Detecting: $(realpath "$TMP")/schemas-left-a/s5.json (#3)
Detecting: $(realpath "$TMP")/schemas-right-b/s4.json (#4)
Detecting: $(realpath "$TMP")/schemas-right-a/s3.json (#5)
( 20%) Resolving: s2.json
( 40%) Resolving: s1.json
( 60%) Resolving: s5.json
( 80%) Resolving: s4.json
(100%) Resolving: s3.json
(  4%) Producing: schemas/left/left-a/s5/%/schema.metapack
(  9%) Producing: schemas/left/left-a/s5/%/dependencies.metapack
( 14%) Producing: schemas/left/left-a/s5/%/locations.metapack
( 19%) Producing: schemas/left/left-a/s5/%/positions.metapack
( 23%) Producing: schemas/left/left-a/s5/%/stats.metapack
( 28%) Producing: dependency-tree.metapack
( 33%) Producing: schemas/left/left-a/s5/%/bundle.metapack
( 38%) Producing: schemas/left/left-a/s5/%/health.metapack
( 42%) Producing: explorer/left/left-a/s5/%/schema.metapack
( 47%) Producing: schemas/left/left-a/s5/%/blaze-exhaustive.metapack
( 52%) Producing: schemas/left/left-a/s5/%/blaze-fast.metapack
( 57%) Producing: schemas/left/left-a/s5/%/dependents.metapack
( 61%) Producing: schemas/left/left-a/s5/%/editor.metapack
( 66%) Producing: explorer/%/search.metapack
( 71%) Producing: explorer/left/left-a/%/directory.metapack
( 76%) Producing: explorer/left/left-a/s5/%/schema-html.metapack
( 80%) Producing: explorer/left/%/directory.metapack
( 85%) Producing: explorer/left/left-a/%/directory-html.metapack
( 90%) Producing: explorer/%/directory.metapack
( 95%) Producing: explorer/left/%/directory-html.metapack
(100%) Producing: explorer/%/directory-html.metapack
EOF

cat << EOF > "$TMP/expected_b.txt"
Writing output to: $(realpath "$TMP")/output
Using configuration: $(realpath "$TMP")/one.json
Detecting: $(realpath "$TMP")/schemas-right-b/s4.json (#1)
Detecting: $(realpath "$TMP")/schemas-right-a/s3.json (#2)
Detecting: $(realpath "$TMP")/schemas-left-b/s2.json (#3)
Detecting: $(realpath "$TMP")/schemas-left-a/s1.json (#4)
Detecting: $(realpath "$TMP")/schemas-left-a/s5.json (#5)
( 20%) Resolving: s4.json
( 40%) Resolving: s3.json
( 60%) Resolving: s2.json
( 80%) Resolving: s1.json
(100%) Resolving: s5.json
(  4%) Producing: schemas/left/left-a/s5/%/schema.metapack
(  9%) Producing: schemas/left/left-a/s5/%/dependencies.metapack
( 14%) Producing: schemas/left/left-a/s5/%/locations.metapack
( 19%) Producing: schemas/left/left-a/s5/%/positions.metapack
( 23%) Producing: schemas/left/left-a/s5/%/stats.metapack
( 28%) Producing: dependency-tree.metapack
( 33%) Producing: schemas/left/left-a/s5/%/bundle.metapack
( 38%) Producing: schemas/left/left-a/s5/%/health.metapack
( 42%) Producing: explorer/left/left-a/s5/%/schema.metapack
( 47%) Producing: schemas/left/left-a/s5/%/blaze-exhaustive.metapack
( 52%) Producing: schemas/left/left-a/s5/%/blaze-fast.metapack
( 57%) Producing: schemas/left/left-a/s5/%/dependents.metapack
( 61%) Producing: schemas/left/left-a/s5/%/editor.metapack
( 66%) Producing: explorer/%/search.metapack
( 71%) Producing: explorer/left/left-a/%/directory.metapack
( 76%) Producing: explorer/left/left-a/s5/%/schema-html.metapack
( 80%) Producing: explorer/left/%/directory.metapack
( 85%) Producing: explorer/left/left-a/%/directory-html.metapack
( 90%) Producing: explorer/%/directory.metapack
( 95%) Producing: explorer/left/%/directory-html.metapack
(100%) Producing: explorer/%/directory-html.metapack
EOF

diff "$TMP/output.txt" "$TMP/expected_a.txt" || diff "$TMP/output.txt" "$TMP/expected_b.txt"
