#!/bin/sh

set -o errexit
set -o nounset

# For better shell expansion in the Dockerfile

exec /usr/bin/sourcemeta-one-server \
  "$SOURCEMETA_ONE_OUTPUT" \
  "$SOURCEMETA_ONE_PORT"
