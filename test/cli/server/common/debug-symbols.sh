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
  # `nm --line-numbers` annotates each symbol with `<file>:<line>`
  # when source mapping is available. It uses BFD, which follows
  # the binary's `.gnu_debuglink` section to locate a `.debug`
  # sidecar in any standard location, so the test does not depend
  # on where the sidecar ended up. We do not pin to a single symbol
  # because LTO routinely splits or stubs out specific symbols
  # (notably `main`), losing their line mapping while leaving the
  # rest of the binary fully annotated. As long as some symbol
  # resolves, the sidecar is reachable and DWARF is intact
  nm --line-numbers "$BINARY" > "$TMP/output.txt"
  grep -qE ':[1-9][0-9]*$' "$TMP/output.txt" || {
    echo "no symbol resolved to a source location" >&2
    head -5 "$TMP/output.txt" >&2
    exit 1
  }
fi
