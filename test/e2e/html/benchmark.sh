#!/bin/sh

set -o errexit
set -o nounset

BASE="$1"
HERE="$(cd "$(dirname "$0")" && pwd)"
MEASURE="$HERE/../../../benchmark/measure.py"

"$MEASURE" --name "Schema Fetch" --url "$BASE/test/bundling/single.json"
