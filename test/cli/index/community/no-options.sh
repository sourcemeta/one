#!/bin/sh

set -o errexit
set -o nounset

TMP="$(mktemp -d)"
clean() { rm -rf "$TMP"; }
trap clean EXIT

"$1" --skip-banner > "$TMP/output.txt" 2>/dev/null && CODE="$?" || CODE="$?"
test "$CODE" = "1" || exit 1

cat << EOF > "$TMP/expected.txt"
Usage: $(basename "$1") <one.json> <path/to/output/directory>

Global Options:

   --help, -h                     Print this help information and quit
   --configuration, -g            Print the resolved configuration and quit
   --resolve-schema, -r <uri>     Resolve a URI to its schema path and quit
   --skip-banner, -s              Skip the startup banner

Index Options:

   --verbose, -v                  Enable verbose output
   --deterministic, -d            Stable (but slower) log output across platforms
   --concurrency, -c <number>     Set the number of concurrent threads
   --profile, -p                  Output information about slowest steps
   --time, -t                     Output high-level timing information
   --comment, -m <text>           Attach a comment to the build

Advanced Options:

   --maximum-direct-directory-entries <number>

     Set the maximum number of direct entries in a directory listing

For more documentation, visit https://one.sourcemeta.com
EOF

diff "$TMP/output.txt" "$TMP/expected.txt"
