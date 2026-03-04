#!/bin/sh

# Run a command inside a transactional overlayfs overlay.
#
# Usage:
#   $0 <output-dir> <command> [args...]
#
# Mounts a kernel overlayfs over <output-dir>, then executes the given
# command with any argument that exactly matches <output-dir> replaced
# by the overlay merged path. On success the upper layer is merged back
# into <output-dir>. On failure the overlay is discarded and the
# original exit code is returned.
#
# Requirements:
#   - Linux kernel with overlayfs support
#   - Ability to mount filesystems (root + CAP_SYS_ADMIN or --privileged)

set -o errexit
set -o nounset

if [ $# -lt 2 ]
then
  echo "Usage: $0 <output-dir> <command> [args...]" >&2
  exit 2
fi

OUTPUT_DIR="$1"
shift

OVERLAY_DIR=$(mktemp -d)
OVERLAY_UPPER="$OVERLAY_DIR/upper"
OVERLAY_WORK="$OVERLAY_DIR/work"
OVERLAY_MERGED="$OVERLAY_DIR/merged"
mkdir -p "$OVERLAY_UPPER" "$OVERLAY_WORK" "$OVERLAY_MERGED"

OVERLAY_MOUNTED=0
cleanup() {
  if [ "$OVERLAY_MOUNTED" -eq 1 ]
  then
    umount "$OVERLAY_MERGED" 2>/dev/null || true
  fi
  rm -rf "$OVERLAY_DIR"
}
trap cleanup EXIT

TIME_START=$(date +%s%3N)
mount -t overlay overlay \
  -o "lowerdir=$OUTPUT_DIR" \
  -o "upperdir=$OVERLAY_UPPER" \
  -o "workdir=$OVERLAY_WORK" \
  "$OVERLAY_MERGED"
OVERLAY_MOUNTED=1
TIME_END=$(date +%s%3N)
echo "transaction(overlayfs): mount took $((TIME_END - TIME_START))ms" >&2

ARGUMENT_COUNT=$#
ARGUMENT_INDEX=0
while [ $ARGUMENT_INDEX -lt $ARGUMENT_COUNT ]
do
  ARGUMENT_INDEX=$((ARGUMENT_INDEX + 1))
  CURRENT="$1"
  shift
  if [ "$CURRENT" = "$OUTPUT_DIR" ]
  then
    set -- "$@" "$OVERLAY_MERGED"
  else
    set -- "$@" "$CURRENT"
  fi
done

TIME_START=$(date +%s%3N)
"$@" && EXIT_CODE=$? || EXIT_CODE=$?
TIME_END=$(date +%s%3N)
echo "transaction(overlayfs): command took $((TIME_END - TIME_START))ms" >&2

TIME_START=$(date +%s%3N)
umount "$OVERLAY_MERGED"
OVERLAY_MOUNTED=0
TIME_END=$(date +%s%3N)
echo "transaction(overlayfs): umount took $((TIME_END - TIME_START))ms" >&2

if [ "$EXIT_CODE" -ne 0 ]
then
  exit "$EXIT_CODE"
fi

TIME_START=$(date +%s%3N)

# Process overlayfs whiteouts (file deletions) in the upper layer.
# Whiteouts are character devices with major:minor 0:0
find "$OVERLAY_UPPER" -type c | while read -r WHITEOUT_PATH
do
  RELATIVE_PATH="${WHITEOUT_PATH#$OVERLAY_UPPER/}"
  rm -rf "${OUTPUT_DIR:?}/$RELATIVE_PATH"
  rm "$WHITEOUT_PATH"
done

cp -a "$OVERLAY_UPPER"/. "$OUTPUT_DIR/"
TIME_END=$(date +%s%3N)
echo "transaction(overlayfs): merge took $((TIME_END - TIME_START))ms" >&2
