#!/bin/sh

set -o errexit
set -o nounset

usage() {
  echo "Usage: $0 <major|minor|patch>" 1>&2
  exit 1
}

if [ "$#" -ne 1 ]; then
  usage
fi

BUMP_TYPE="$1"

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
VERSION_FILE="${SCRIPT_DIR}/VERSION"

CURRENT_VERSION="$(cat "$VERSION_FILE")"
MAJOR="$(echo "$CURRENT_VERSION" | cut -d . -f1)"
MINOR="$(echo "$CURRENT_VERSION" | cut -d . -f2)"
PATCH="$(echo "$CURRENT_VERSION" | cut -d . -f3)"

case "$BUMP_TYPE" in
  major)
    MAJOR=$((MAJOR + 1))
    MINOR=0
    PATCH=0
    ;;
  minor)
    MINOR=$((MINOR + 1))
    PATCH=0
    ;;
  patch)
    PATCH=$((PATCH + 1))
    ;;
  *)
    usage
    ;;
esac

NEW_VERSION="${MAJOR}.${MINOR}.${PATCH}"
echo "Bumping version: ${CURRENT_VERSION} -> ${NEW_VERSION}"

echo "$NEW_VERSION" > "$VERSION_FILE"

git -C "$SCRIPT_DIR" commit --only --gpg-sign --signoff \
  --message "v${NEW_VERSION}" -- VERSION
git -C "$SCRIPT_DIR" tag --sign "v${NEW_VERSION}" --message "v${NEW_VERSION}"
git -C "$SCRIPT_DIR" log -1 --patch
