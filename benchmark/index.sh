#!/bin/sh

set -o errexit
set -o nounset

if [ "$#" -ne 1 ]
then
  echo "Usage: $0 <path/to/sourcemeta-one-index>" 1>&2
  exit 1
fi

INDEX="$1"
HERE="$(cd "$(dirname "$0")" && pwd)"

# Each case is collected before any of it is summed, so that a case which fails
# stops the run rather than being summed away into a partial result that reads
# like a complete one
RESULTS="$(mktemp)"
clean() { rm -f "$RESULTS"; }
trap clean EXIT

{
  "$HERE/index-add-update-rebuild.sh" "$INDEX"
  "$HERE/index-n.sh" "$INDEX" 100
  "$HERE/index-n.sh" "$INDEX" 1000
  "$HERE/index-n.sh" "$INDEX" 10000
  "$HERE/index-custom-meta-schema.sh" "$INDEX" 10000
  "$HERE/index-ref-fanout.sh" "$INDEX" 10000
} >> "$RESULTS"

jq --slurp 'add' < "$RESULTS"
