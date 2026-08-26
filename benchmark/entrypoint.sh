#!/bin/sh

set -o errexit
set -o nounset

# What the indexer measured of itself was taken when this image was built. What
# the suites measured of a running instance was taken on the way here, and is
# mounted rather than built in, since it could only be taken with a server up.
# Both are said at once, with the indexer first, so the answer reads as one
# group after another
RESULTS="$(mktemp -d)"
clean() { rm -rf "$RESULTS"; }
trap clean EXIT

cp /benchmark.json "$RESULTS/index.json"

if [ -d /results ]
then
  find /results -maxdepth 1 -name '*.json' -exec cp {} "$RESULTS/" \;
fi

/benchmark/merge.sh "$RESULTS"
