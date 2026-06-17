#!/bin/sh

set -o errexit
set -o nounset

if [ "$#" -ne 3 ]
then
  echo "Usage: $0 <sandbox> <edition> <port>" 1>&2
  exit 1
fi

SANDBOX="$1"
EDITION="$2"
PORT="$3"

ROOT="$(pwd)"
OUTPUT="$ROOT/build/sandbox"

SERVER_PID=
kill_server() {
  if [ -n "$SERVER_PID" ] && kill -0 "$SERVER_PID" 2>/dev/null
  then
    kill "$SERVER_PID" 2>/dev/null || true
    wait "$SERVER_PID" 2>/dev/null || true
  fi
  SERVER_PID=
}
trap kill_server EXIT INT TERM

wait_for_ready() {
  COUNTER=0
  while [ "$COUNTER" -lt 100 ]
  do
    if ! kill -0 "$SERVER_PID" 2>/dev/null
    then
      echo "Server exited before becoming ready" 1>&2
      return 1
    fi
    if nc -z localhost "$PORT" 2>/dev/null
    then
      return 0
    fi
    sleep 0.1
    COUNTER=$((COUNTER + 1))
  done
  echo "Server did not become ready within 10 seconds" 1>&2
  return 1
}

echo "=========================================" 1>&2
echo "RUNNING: $SANDBOX" 1>&2
echo "=========================================" 1>&2
rm -rf "$OUTPUT"
"$ROOT/build/dist/bin/sourcemeta-one-index" "$ROOT/$SANDBOX/one.json" "$OUTPUT"
if [ -f "$ROOT/$SANDBOX/post-index.sh" ]
then
  "$ROOT/$SANDBOX/post-index.sh" "$OUTPUT"
fi

if [ -f "$ROOT/$SANDBOX/environment" ]
then
  set -a
  # shellcheck source=/dev/null
  . "$ROOT/$SANDBOX/environment"
  set +a
fi

"$ROOT/build/dist/bin/sourcemeta-one-server" "$OUTPUT" "$PORT" &
SERVER_PID="$!"
wait_for_ready
make -C "$SANDBOX" test-hurl EDITION="$EDITION" PORT="$PORT"
make -C "$SANDBOX" test-playwright EDITION="$EDITION" PORT="$PORT"
