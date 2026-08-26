#!/bin/sh

set -o errexit
set -o nounset

# What the indexer measured of itself was taken when this image was built. What
# the suites measured of a running instance was taken on the way here, and is
# mounted rather than built in, since it could only be taken with a server up.
# Both are said at once, with the indexer first, so the answer reads as one
# group after another
set -- /benchmark.json

for RESULT in /results/*.json
do
  if [ -f "$RESULT" ]
  then
    set -- "$@" "$RESULT"
  fi
done

jq --slurp 'add' "$@"
