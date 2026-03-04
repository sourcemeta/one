#!/bin/sh

set -o errexit
set -o nounset

if [ "$#" -ne 1 ]
then
  echo "Usage: $0 <output>" 1>&2
  exit 1
fi

OUTPUT="$1"

# To test how the server deals with corrupted data
echo "Applying chaos corruptions..." 1>&2

echo "  Corrupting root node first_literal_child to an out-of-bounds index..." 1>&2
test -f "$OUTPUT/routes.bin"
printf '\x00\x00\x10\x00' | \
  dd of="$OUTPUT/routes.bin" bs=1 seek=24 conv=notrunc 2>/dev/null

echo "  Corrupting second node string_offset to point past the file end..." 1>&2
printf '\x00\x00\x00\x7f' | \
  dd of="$OUTPUT/routes.bin" bs=1 seek=40 conv=notrunc 2>/dev/null

echo "  Replacing directory.metapack metadata with wrong types..." 1>&2
test -f "$OUTPUT/explorer/%/directory.metapack"
printf '{"version":1,"checksum":99999,"lastModified":0,"mime":false,"bytes":1,"duration":1,"encoding":"identity"}' \
  > "$OUTPUT/explorer/%/directory.metapack"

echo "  Deleting search.metapack to trigger missing file assert..." 1>&2
test -f "$OUTPUT/explorer/%/search.metapack"
rm -f "$OUTPUT/explorer/%/search.metapack"

echo "  Replacing locations.metapack with empty object for test/schemas/string..." 1>&2
test -f "$OUTPUT/schemas/test/schemas/string/%/locations.metapack"
printf '{"version":1,"checksum":"x","lastModified":"Thu, 01 Jan 2025 00:00:00 GMT","mime":"application/json","bytes":2,"duration":1,"encoding":"identity"}{}' \
  > "$OUTPUT/schemas/test/schemas/string/%/locations.metapack"
