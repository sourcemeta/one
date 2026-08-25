#!/bin/sh

set -o errexit
set -o nounset

if [ "$#" -ne 2 ]
then
  echo "Usage: $0 <path/to/index> <path/to/server>" 1>&2
  exit 1
fi

INDEX="$1"
SERVER="$2"
HERE="$(cd "$(dirname "$0")" && pwd)"
ROOT="$(cd "$HERE/.." && pwd)"
PORT="${BENCHMARK_PORT:-8199}"

# A suite says what it wants measured by carrying a script that says it, so
# wiring a new one up is writing that script and nothing else here
RESULTS="$(mktemp)"
clean() { rm -f "$RESULTS"; }
trap clean EXIT

for DECLARATION in "$ROOT"/test/e2e/*/benchmark.sh "$ROOT"/enterprise/e2e/*/benchmark.sh
do
  [ -f "$DECLARATION" ] || continue
  DIRECTORY="$(dirname "$DECLARATION")"
  SUITE="${DIRECTORY#"$ROOT"/}"

  OUTPUT="$(mktemp -d)"
  SERVER_PID=

  echo "Indexing $SUITE..." 1>&2
  "$INDEX" --skip-banner "$DIRECTORY/one.json" "$OUTPUT/index" >&2 2>/dev/null

  # What a suite needs to admit a caller is what it needs to serve one, so the
  # same file the tests run under is the one a measurement runs under
  if [ -f "$DIRECTORY/environment" ]
  then
    set -a
    # shellcheck source=/dev/null
    . "$DIRECTORY/environment"
    set +a
  fi

  "$SERVER" "$OUTPUT/index" "$PORT" > /dev/null 2>&1 &
  SERVER_PID="$!"

  COUNTER=0
  while [ "$COUNTER" -lt 100 ]
  do
    if ! kill -0 "$SERVER_PID" 2>/dev/null
    then
      echo "Server for $SUITE exited before becoming ready" 1>&2
      exit 1
    fi
    if nc -z localhost "$PORT" 2>/dev/null
    then
      break
    fi
    sleep 0.1
    COUNTER=$((COUNTER + 1))
  done

  # A suite names what it measures without naming itself, since where it lives
  # is something this already knows
  "$DECLARATION" "http://localhost:$PORT" \
    | jq --arg suite "$SUITE" 'map(.name |= "\($suite): \(.)")' >> "$RESULTS"

  kill "$SERVER_PID" 2>/dev/null || true
  wait "$SERVER_PID" 2>/dev/null || true
  rm -rf "$OUTPUT"
done

jq --slurp 'add' < "$RESULTS"
