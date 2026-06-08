#!/bin/sh

set -o errexit
set -o nounset

# For better shell expansion in the Dockerfile.
#
# When the entrypoint runs as root (the image's default execution
# identity, kept that way so build-time `RUN` instructions in consumer
# Dockerfiles are unsurprising), `gosu` drops privileges to the
# unprivileged service account before the long-running server starts.
# When the entrypoint is already running as a non-root account (e.g.
# `docker run --user 1234`, or an orchestrator like OpenShift that
# assigns an arbitrary UID via its restricted SCC), `gosu` would fail
# because privilege drop requires root, so we just exec the server
# directly and trust the caller's choice. Either path uses `exec`
# (and `gosu` itself uses `execve(2)`), so SIGTERM and friends reach
# the server process directly without an intermediate shell.
#
# Binding `SOURCEMETA_ONE_PORT` to a privileged port (below 1024)
# works even after the privilege drop because the server binary
# carries `CAP_NET_BIND_SERVICE` as a file capability, granted at
# image build time.

if [ "$(id -u)" -eq 0 ]
then
  exec /usr/sbin/gosu sourcemeta /usr/bin/sourcemeta-one-server \
    "$SOURCEMETA_ONE_OUTPUT" \
    "$SOURCEMETA_ONE_PORT"
fi

exec /usr/bin/sourcemeta-one-server \
  "$SOURCEMETA_ONE_OUTPUT" \
  "$SOURCEMETA_ONE_PORT"
