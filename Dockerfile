FROM debian:trixie AS builder

RUN apt-get --yes update && apt-get install --yes --no-install-recommends \
  build-essential ca-certificates cmake ninja-build sassc esbuild shellcheck nodejs npm \
  && apt-get clean && rm -rf /var/lib/apt/lists/*

COPY package.json /source/package.json
COPY package-lock.json /source/package-lock.json
COPY cmake /source/cmake
COPY src /source/src
COPY contrib /source/contrib
COPY vendor /source/vendor
COPY CMakeLists.txt /source/CMakeLists.txt
COPY VERSION /source/VERSION

# For testing
COPY test/cli /source/test/cli
COPY test/unit /source/test/unit
COPY test/js /source/test/js

RUN cd /source && npm ci

ARG SOURCEMETA_ONE_BUILD_TYPE=Release
ARG SOURCEMETA_ONE_PARALLEL=2

RUN	cmake -S /source -B ./build -G Ninja \
  -DCMAKE_BUILD_TYPE:STRING=${SOURCEMETA_ONE_BUILD_TYPE} \
  -DCMAKE_COMPILE_WARNING_AS_ERROR:BOOL=ON \
  -DONE_INDEX:BOOL=ON \
  -DONE_SERVER:BOOL=ON \
  -DONE_TESTS:BOOL=ON \
  -DONE_ENTERPRISE:BOOL=OFF \
  -DBUILD_SHARED_LIBS:BOOL=OFF \
  -DONE_DEBUG_SYMBOLS:BOOL=ON

RUN cmake --build /build \
  --config ${SOURCEMETA_ONE_BUILD_TYPE} \
  --parallel ${SOURCEMETA_ONE_PARALLEL}
RUN cmake --install /build --prefix /usr --verbose \
  --config ${SOURCEMETA_ONE_BUILD_TYPE} \
  --component sourcemeta_one

# Linting
RUN cmake --build /build --config ${SOURCEMETA_ONE_BUILD_TYPE} \
  --target clang_format_test
RUN cmake --build /build --config ${SOURCEMETA_ONE_BUILD_TYPE} \
  --target shellcheck

# Run a few times to rule out any flakiness
RUN ctest --test-dir /build --build-config ${SOURCEMETA_ONE_BUILD_TYPE} \
  --output-on-failure --parallel --repeat-until-fail 5

FROM debian:trixie-slim

# Install `gosu` so the runtime entrypoint can drop from root to the
# unprivileged service account. We deliberately don't set `USER` on
# this image so build-time `RUN` instructions (including those in
# downstream consumer Dockerfiles) keep executing as root, matching
# the well-trodden Postgres/Redis/MySQL pattern.
RUN apt-get --yes update \
  && apt-get install --yes --no-install-recommends gosu \
  && apt-get clean && rm -rf /var/lib/apt/lists/*

# Create the unprivileged service account that the entrypoint will
# `gosu` into. The UID sits above the typical 1000-range dev user
# (so host-mounted volumes can be mapped unambiguously) and outside
# the Debian system-user range (1-999) reserved for daemons.
ARG SOURCEMETA_ONE_UID=10001
RUN groupadd --system --gid "${SOURCEMETA_ONE_UID}" sourcemeta \
  && useradd --system --uid "${SOURCEMETA_ONE_UID}" \
       --gid "${SOURCEMETA_ONE_UID}" \
       --no-create-home --shell /usr/sbin/nologin sourcemeta

# See https://github.com/opencontainers/image-spec/blob/main/annotations.md#pre-defined-annotation-keys
LABEL org.opencontainers.image.url="https://one.sourcemeta.com"
LABEL org.opencontainers.image.documentation="https://one.sourcemeta.com"
LABEL org.opencontainers.image.source="https://github.com/sourcemeta/one"
LABEL org.opencontainers.image.vendor="Sourcemeta"
LABEL org.opencontainers.image.licenses="BUSL-1.1"
LABEL org.opencontainers.image.title="Sourcemeta One"
LABEL org.opencontainers.image.description="A self-hosted platform for publishing, browsing, and governing your JSON Schema collections"
LABEL org.opencontainers.image.authors="Sourcemeta <hello@sourcemeta.com>"

COPY --from=builder /usr/bin/sourcemeta-one-index \
  /usr/bin/sourcemeta-one-index
COPY --from=builder /usr/bin/sourcemeta-one-index.debug \
  /usr/bin/sourcemeta-one-index.debug
COPY --from=builder /usr/bin/sourcemeta-one-server \
  /usr/bin/sourcemeta-one-server
COPY --from=builder /usr/bin/sourcemeta-one-server.debug \
  /usr/bin/sourcemeta-one-server.debug
COPY --from=builder /usr/share/sourcemeta/one \
  /usr/share/sourcemeta/one

# For debugging purposes
RUN ldd /usr/bin/sourcemeta-one-index
RUN ldd /usr/bin/sourcemeta-one-server

# We expect images that extend this one to use this directory
ARG SOURCEMETA_ONE_WORKDIR=/source
ENV SOURCEMETA_ONE_WORKDIR=${SOURCEMETA_ONE_WORKDIR}
WORKDIR ${SOURCEMETA_ONE_WORKDIR}

# To make it easier for the consumer. So they can generate the index
# without caring about output locations at all
ARG SOURCEMETA_ONE_OUTPUT=/sourcemeta
ENV SOURCEMETA_ONE_OUTPUT=${SOURCEMETA_ONE_OUTPUT}
COPY docker/wrapper-index.sh /usr/bin/sourcemeta
COPY docker/wrapper-server.sh /usr/bin/sourcemeta-server
COPY docker/transaction-overlayfs.sh /usr/bin/sourcemeta-transaction-overlayfs

# Pre-create the output directory and hand it to the service account
# so the runtime server can read through it (and the optional
# transactional re-index path can write through it) without
# elevation. The chown is non-recursive to keep blast-radius bounded
# if a build-arg override ever pointed the path at the rootfs root.
RUN test -n "${SOURCEMETA_ONE_OUTPUT}" \
  && test "${SOURCEMETA_ONE_OUTPUT}" != "/" \
  && mkdir -p "${SOURCEMETA_ONE_OUTPUT}" \
  && chown sourcemeta:sourcemeta "${SOURCEMETA_ONE_OUTPUT}"

ENV SOURCEMETA_ONE_PORT=8000
HEALTHCHECK --interval=1s --timeout=2s --start-period=1s --retries=10 CMD grep -qE \
  "^\s*[0-9]+:\s+[0-9A-F]+:$(printf '%04X' $SOURCEMETA_ONE_PORT)\s+[0-9A-F:]+\s+0A\s" \
  /proc/net/tcp /proc/net/tcp6
ENTRYPOINT [ "/usr/bin/sourcemeta-server" ]
