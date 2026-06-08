#!/bin/sh

set -o errexit
set -o nounset

# For better shell expansion in the Dockerfile.
#
# `gosu` drops privileges from root (the image's default execution
# identity, kept that way so build-time `RUN` instructions in
# consumer Dockerfiles are unsurprising) to the unprivileged service
# account before the long-running server starts. The signal-handling
# behaviour of `gosu` is the same as `exec`, so SIGTERM and friends
# reach the server process directly.

exec /usr/sbin/gosu sourcemeta /usr/bin/sourcemeta-one-server \
  "$SOURCEMETA_ONE_OUTPUT" \
  "$SOURCEMETA_ONE_PORT"
