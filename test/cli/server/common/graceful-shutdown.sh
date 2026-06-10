#!/bin/sh

set -o errexit
set -o nounset

BINARY="$1"
# `ONE_PREFIX` points at the install prefix used by the build. The
# indexer ends up under `${ONE_PREFIX}/bin/sourcemeta-one-index`
# in both the build tree and a real install.
INDEXER="${ONE_PREFIX}/bin/sourcemeta-one-index"

TMP="$(mktemp -d)"
clean() {
  if [ -n "${SERVER_PID:-}" ] && kill -0 "$SERVER_PID" 2>/dev/null; then
    kill -KILL "$SERVER_PID" 2>/dev/null || true
  fi
  rm -rf "$TMP"
}
trap clean EXIT

cat << 'EOF' > "$TMP/one.json"
{ "url": "http://localhost:8000" }
EOF

"$INDEXER" "$TMP/one.json" "$TMP/output" > /dev/null 2>&1

# Pick a port deliberately far from the default so we do not collide
# with any locally-running server. The test only cares about exit
# code on shutdown, not about the chosen port.
PORT=39873

"$BINARY" "$TMP/output" "$PORT" > "$TMP/log.txt" 2>&1 &
SERVER_PID="$!"

# Wait for the server to bind. The log line goes out the moment the
# listen call returns successfully, so polling the log is enough.
WAITED=0
until grep -q "Listening on port" "$TMP/log.txt" 2>/dev/null; do
  WAITED=$((WAITED + 1))
  if [ "$WAITED" -gt 50 ]; then
    echo "Server failed to bind within timeout" >&2
    cat "$TMP/log.txt" >&2
    exit 1
  fi
  sleep 0.1
done

# RFC-style cooperative shutdown via SIGTERM. The process must exit
# cleanly and within a reasonable bound. Anything longer than a few
# seconds means the watcher thread did not see the flag or a worker
# refused to drain its event loop.
kill -TERM "$SERVER_PID"
WAITED=0
while kill -0 "$SERVER_PID" 2>/dev/null; do
  WAITED=$((WAITED + 1))
  if [ "$WAITED" -gt 50 ]; then
    echo "Server did not exit within 5 seconds of SIGTERM" >&2
    cat "$TMP/log.txt" >&2
    exit 1
  fi
  sleep 0.1
done

wait "$SERVER_PID" && CODE="$?" || CODE="$?"
SERVER_PID=""

test "$CODE" = "0" || {
  echo "Server exited with $CODE (expected 0)" >&2
  cat "$TMP/log.txt" >&2
  exit 1
}

grep -q "Terminating on requested signal" "$TMP/log.txt" || {
  echo "Missing async-signal-safe terminate banner" >&2
  cat "$TMP/log.txt" >&2
  exit 1
}

grep -q "The server stopped gracefully" "$TMP/log.txt" || {
  echo "Missing graceful-shutdown log line" >&2
  cat "$TMP/log.txt" >&2
  exit 1
}
