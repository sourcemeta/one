#!/bin/sh

# Generates an SPDX 2.3 tag-value Software Bill of Materials (SBOM) for the
# Sourcemeta One Enterprise container image, covering vendored C++ libraries,
# npm frontend packages, and system packages

set -o errexit
set -o nounset

if [ $# -ne 2 ]; then
  echo "Usage: $0 <root> <version>" 1>&2
  exit 1
fi

ROOT="$1"
VERSION="$2"

# License map: dependency name -> SPDX identifier
license_for() {
  case "$1" in
    core|blaze|hydra|codegen|jsonbinpack) echo "AGPL-3.0-or-later" ;;
    jsonschema) echo "AGPL-3.0-only" ;;
    uwebsockets) echo "Apache-2.0" ;;
    bootstrap|bootstrap-icons) echo "MIT" ;;
    pcre2) echo "BSD-3-Clause" ;;
    mbedtls) echo "Apache-2.0" ;;
    zlib) echo "Zlib" ;;
    curl) echo "curl" ;;
    nghttp2|cpr|c-ares|libpsl) echo "MIT" ;;
    mpdecimal) echo "BSD-2-Clause" ;;
    @codemirror/*) echo "MIT" ;;
    openssl) echo "Apache-2.0" ;;
    *)
      echo "ERROR: No license mapping for dependency: $1" 1>&2
      exit 1
      ;;
  esac
}

# Blacklist check: returns 0 (true) if the name should be skipped
is_blacklisted() {
  case "$1" in
    vendorpull|googletest|googlebenchmark) return 0 ;;
    jsontestsuite|yaml-test-suite|jsonschema-test-suite) return 0 ;;
    referencing-suite|uritemplate-test) return 0 ;;
    pyca-cryptography|ctrf) return 0 ;;
    mbedtls) return 0 ;;
    public/*|collections/*) return 0 ;;
    jsonschema-draft*|jsonschema-2019*|jsonschema-2020*) return 0 ;;
    openapi*) return 0 ;;
    *) return 1 ;;
  esac
}

# Collect packages: name, version, url
# Using temp files to accumulate state across subshells
PACKAGES_FILE="$(mktemp)"
SEEN_URLS_FILE="$(mktemp)"
RELATIONSHIPS_FILE="$(mktemp)"
INDEX_FILE="$(mktemp)"
printf '0' > "$INDEX_FILE"
trap 'rm -f "$PACKAGES_FILE" "$SEEN_URLS_FILE" "$RELATIONSHIPS_FILE" "$INDEX_FILE"' EXIT

add_package() {
  _name="$1"
  _version="$2"
  _url="$3"
  _license="$4"

  _index="$(cat "$INDEX_FILE")"
  _index=$((_index + 1))
  printf '%s' "$_index" > "$INDEX_FILE"
  _spdxid="SPDXRef-Package-${_index}"

  cat >> "$PACKAGES_FILE" <<SPDXPKG

PackageName: ${_name}
SPDXID: ${_spdxid}
PackageVersion: ${_version}
PackageDownloadLocation: ${_url}
FilesAnalyzed: false
PackageLicenseConcluded: ${_license}
PackageLicenseDeclared: ${_license}
SPDXPKG

  printf 'Relationship: SPDXRef-RootPackage DEPENDS_ON %s\n' "$_spdxid" >> "$RELATIONSHIPS_FILE"
}

# Discover all DEPENDENCIES files in the repository
DEPS_LIST_FILE="$(mktemp)"
trap 'rm -f "$PACKAGES_FILE" "$SEEN_URLS_FILE" "$RELATIONSHIPS_FILE" "$INDEX_FILE" "$DEPS_LIST_FILE"' EXIT
find "$ROOT" -name DEPENDENCIES -type f -not -path '*/.git/*' | sort > "$DEPS_LIST_FILE"

while read -r deps_file; do
  while read -r name url version; do
    if [ -z "$name" ]; then
      continue
    fi

    # Always blacklist first
    if is_blacklisted "$name"; then
      continue
    fi

    # Deduplicate by URL
    if grep -qFx "$url" "$SEEN_URLS_FILE" 2>/dev/null; then
      continue
    fi
    printf '%s\n' "$url" >> "$SEEN_URLS_FILE"

    _license="$(license_for "$name")"
    add_package "$name" "$version" "$url" "$_license"
  done < "$deps_file"
done < "$DEPS_LIST_FILE"

# Hardcoded: mpdecimal (vendored but not in any DEPENDENCIES file)
add_package "mpdecimal" "4.0.0" "https://www.bytereef.org/mpdecimal/" "BSD-2-Clause"

# npm runtime deps: @codemirror/* packages from package.json
for row in $(jq -r '.dependencies | to_entries[] | select(.key | startswith("@codemirror/")) | "\(.key)=\(.value)"' "$ROOT/package.json"); do
  _pkg_name="${row%%=*}"
  _pkg_version="${row#*=}"
  # Strip leading ^ or ~ from version
  _pkg_version="${_pkg_version#^}"
  _pkg_version="${_pkg_version#\~}"
  _npm_url="https://www.npmjs.com/package/${_pkg_name}"
  add_package "$_pkg_name" "$_pkg_version" "$_npm_url" "MIT"
done

# Emit SPDX document
cat <<SPDXHEADER
SPDXVersion: SPDX-2.3
DataLicense: CC0-1.0
SPDXID: SPDXRef-DOCUMENT
DocumentName: sourcemeta-one-enterprise
DocumentNamespace: https://sourcemeta.com/spdx/${VERSION}
Creator: Tool: enterprise/scripts/sbom.sh

PackageName: sourcemeta-one-enterprise
SPDXID: SPDXRef-RootPackage
PackageVersion: ${VERSION}
PackageDownloadLocation: https://github.com/sourcemeta/one
FilesAnalyzed: false
PackageLicenseConcluded: NOASSERTION
PackageLicenseDeclared: NOASSERTION

Relationship: SPDXRef-DOCUMENT DESCRIBES SPDXRef-RootPackage
SPDXHEADER

cat "$PACKAGES_FILE"
printf '\n'
cat "$RELATIONSHIPS_FILE"
