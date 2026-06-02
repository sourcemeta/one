#!/bin/sh

set -o errexit
set -o nounset
set -o pipefail

if [ "$#" -ne 1 ]
then
  echo "Usage: $0 <path/to/sourcemeta-one-index>" 1>&2
  exit 1
fi

INDEX="$1"
HERE="$(cd "$(dirname "$0")" && pwd)"

{
  "$HERE/index-add-update-rebuild.sh" "$INDEX"
  "$HERE/index-n.sh" "$INDEX" 100
  "$HERE/index-n.sh" "$INDEX" 1000
  "$HERE/index-n.sh" "$INDEX" 10000
  "$HERE/index-custom-meta-schema.sh" "$INDEX" 10000
} | jq --slurp 'add'
