#!/bin/sh

set -o errexit
set -o nounset

if [ "$#" -ne 1 ]
then
  echo "Usage: $0 <path/to/results/directory>" 1>&2
  exit 1
fi

DIRECTORY="$1"

# A run where no suite declared anything is not a failure, it is a run with
# nothing to say, and an empty array is how it says so
if [ ! -d "$DIRECTORY" ] || [ -z "$(ls -A "$DIRECTORY" 2>/dev/null)" ]
then
  echo "[]"
  exit 0
fi

jq --slurp 'add' "$DIRECTORY"/*.json
