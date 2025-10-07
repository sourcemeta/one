#!/bin/sh

set -o errexit
set -o nounset

# This is a wrapper around `sourcemeta-one-index` specifically for the
# Dockerfile, as it reads the environment variables present in the context to
# simplify the interface for generating a One instance

if [ $# -lt 1 ]
then
  echo "Usage: $0 <path/to/one.json>" 1>&2
  exit 1
fi

CONFIGURATION="$1"
shift

case "$(realpath "$CONFIGURATION")" in
  "$SOURCEMETA_ONE_WORKDIR"*)
    /usr/bin/sourcemeta-one-index "$CONFIGURATION" "$SOURCEMETA_ONE_OUTPUT" "$@"
    # Automatically cleanup the source directories
    echo "Deleting $SOURCEMETA_ONE_WORKDIR to keep the image small" 1>&2
    rm -rf "$SOURCEMETA_ONE_WORKDIR"
    ;;
  *)
    echo "error: $1 must be inside the workding directory ($SOURCEMETA_ONE_WORKDIR)" 1>&2
    exit 1
    ;;
esac
