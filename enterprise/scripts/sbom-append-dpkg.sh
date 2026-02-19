#!/bin/sh

# Appends system (dpkg) packages to an existing SPDX 2.3 tag-value SBOM

set -o errexit
set -o nounset

if [ $# -ne 1 ]; then
  echo "Usage: $0 <spdx-file>" 1>&2
  exit 1
fi

SPDX_FILE="$1"

# Determine starting index from existing package entries
INDEX="$(grep -c 'SPDXID: SPDXRef-Package-' "$SPDX_FILE")"

DPKG_FILE="$(mktemp)"
trap 'rm -f "$DPKG_FILE"' EXIT
dpkg-query -W -f '${Package}\t${Version}\t${Homepage}\n' > "$DPKG_FILE"

while IFS="$(printf '\t')" read -r name version homepage; do
  if [ -z "$name" ]; then
    continue
  fi

  INDEX=$((INDEX + 1))
  spdxid="SPDXRef-Package-${INDEX}"
  url="${homepage:-NOASSERTION}"

  cat >> "$SPDX_FILE" <<SPDXPKG

PackageName: ${name}
SPDXID: ${spdxid}
PackageVersion: ${version}
PackageDownloadLocation: ${url}
FilesAnalyzed: false
PackageLicenseConcluded: NOASSERTION
PackageLicenseDeclared: NOASSERTION
SPDXPKG

  printf 'Relationship: SPDXRef-RootPackage DEPENDS_ON %s\n' "$spdxid" >> "$SPDX_FILE"
done < "$DPKG_FILE"
