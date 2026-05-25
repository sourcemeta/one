#!/bin/sh

set -o errexit
set -o nounset

BINARY="$1"

TMP="$(mktemp -d)"
clean() { rm -rf "$TMP"; }
trap clean EXIT

if [ "$(uname -s)" = "Darwin" ]; then
  # `_main` is the Mach-O mangled name of `main`. We resolve its
  # address, then ask `atos` to symbolicate. `atos` follows the
  # binary's debug map (which can point at a `.dSYM` bundle via
  # UUID through Spotlight, at the original `.o` files, or both)
  # so the test does not depend on where the symbols ended up.
  # When DWARF is reachable the output ends with `(file:line)`;
  # without DWARF it ends with `+ <offset>`
  ADDRESS="$(nm "$BINARY" | awk '$3 == "_main" {print "0x"$1; exit}')"
  test -n "$ADDRESS" || { echo "no main symbol in $BINARY" >&2; exit 1; }
  atos -o "$BINARY" "$ADDRESS" > "$TMP/output.txt"
  grep -qE '\(.+:[0-9]+\)' "$TMP/output.txt" || {
    echo "no source location resolved:" >&2
    cat "$TMP/output.txt" >&2
    exit 1
  }
else
  # `addr2line` follows the binary's `.gnu_debuglink` section to
  # find a `.debug` sidecar in any standard location (binary dir,
  # `.debug` subdir, `/usr/lib/debug/<full-path>`, or by build-id
  # under `/usr/lib/debug/.build-id/`) so the test does not depend
  # on where the sidecar ended up. Without DWARF the output is
  # `??:0` or `<file>:0`
  ADDRESS="$(nm "$BINARY" | awk '$3 == "main" {print "0x"$1; exit}')"
  test -n "$ADDRESS" || { echo "no main symbol in $BINARY" >&2; exit 1; }
  addr2line -e "$BINARY" -f "$ADDRESS" > "$TMP/output.txt"
  tail -1 "$TMP/output.txt" | grep -qE '^[^?]+:[1-9][0-9]*$' || {
    echo "no source location resolved:" >&2
    cat "$TMP/output.txt" >&2
    exit 1
  }
fi
