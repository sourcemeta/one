#!/bin/sh

set -o errexit
set -o nounset

BINARY="$1"
INDEXER="$2"
EDITION="$3"
VERSION="$4"

TMP="$(mktemp -d)"
clean() {
  if [ -n "${SERVER_PID:-}" ] && kill -0 "$SERVER_PID" 2>/dev/null; then
    kill -KILL "$SERVER_PID" 2>/dev/null || true
  fi
  rm -rf "$TMP"
}
trap clean EXIT

echo '{ "url": "http://localhost:8000" }' > "$TMP/one.json"
"$INDEXER" "$TMP/one.json" "$TMP/output" > /dev/null 2>&1

PORT=39873
"$BINARY" "$TMP/output" "$PORT" > "$TMP/log.txt" 2>&1 &
SERVER_PID="$!"

WAITED=0
until grep -q "Listening on port" "$TMP/log.txt" 2>/dev/null; do
  WAITED=$((WAITED + 1))
  if [ "$WAITED" -gt 50 ]; then
    cat "$TMP/log.txt" >&2
    exit 1
  fi
  sleep 0.1
done

kill -TERM "$SERVER_PID"
WAITED=0
while kill -0 "$SERVER_PID" 2>/dev/null; do
  WAITED=$((WAITED + 1))
  if [ "$WAITED" -gt 50 ]; then
    cat "$TMP/log.txt" >&2
    exit 1
  fi
  sleep 0.1
done

wait "$SERVER_PID" && CODE="$?" || CODE="$?"
SERVER_PID=""
test "$CODE" = "0" || { cat "$TMP/log.txt" >&2; exit 1; }

# Normalize variable bits (timestamp, thread id, port, elapsed ms) and
# collapse duplicate worker startup lines so the diff is stable across
# scheduling and hardware concurrency.
sed -E \
  -e 's/\[[A-Za-z]{3}, [0-9]{2} [A-Za-z]{3} [0-9]{4} [0-9]{2}:[0-9]{2}:[0-9]{2} GMT\]/[DATE]/' \
  -e 's/0x[0-9a-f]+/0xTID/' \
  -e "s/Listening on port $PORT in [0-9]+ ms/Listening on port PORT in MS ms/" \
  "$TMP/log.txt" | LC_ALL=C sort -u > "$TMP/actual.txt"

cat << EOF | LC_ALL=C sort -u > "$TMP/expected.txt"
Sourcemeta One ${EDITION} v${VERSION}
[DATE] 0xTID Listening on port PORT in MS ms
Terminating on requested signal
[DATE] 0xTID The server stopped gracefully
EOF

diff "$TMP/actual.txt" "$TMP/expected.txt"
