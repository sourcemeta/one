FROM debian:trixie AS builder

# NodeSource provides npm >= 10, required for the "npm sbom" command
RUN apt-get --yes update && apt-get install --yes --no-install-recommends \
  build-essential ca-certificates cmake ninja-build sassc esbuild shellcheck curl gnupg \
  openssl libssl-dev openssl-provider-fips \
  && curl --fail --silent --show-error --location \
    https://deb.nodesource.com/gpgkey/nodesource-repo.gpg.key \
    | gpg --dearmor -o /usr/share/keyrings/nodesource.gpg \
  && echo "deb [signed-by=/usr/share/keyrings/nodesource.gpg] https://deb.nodesource.com/node_22.x nodistro main" \
    > /etc/apt/sources.list.d/nodesource.list \
  && apt-get --yes update \
  && apt-get install --yes --no-install-recommends nodejs \
  && apt-get clean && rm -rf /var/lib/apt/lists/*

# Enable the OpenSSL FIPS provider by running the module self-tests
# and appending the integrity-checked configuration to our openssl.cnf
RUN openssl fipsinstall -module /usr/lib/*/ossl-modules/fips.so \
  -out /tmp/fipsmodule.cnf
COPY enterprise/openssl.base.cnf /etc/ssl/openssl.cnf
RUN cat /tmp/fipsmodule.cnf >> /etc/ssl/openssl.cnf && cat /etc/ssl/openssl.cnf

COPY package.json /source/package.json
COPY package-lock.json /source/package-lock.json
COPY cmake /source/cmake
COPY src /source/src
COPY contrib /source/contrib
COPY vendor /source/vendor
COPY DEPENDENCIES /source/DEPENDENCIES
COPY CMakeLists.txt /source/CMakeLists.txt
COPY VERSION /source/VERSION
COPY enterprise /source/enterprise

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
  -DONE_ENTERPRISE:BOOL=ON \
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

RUN mkdir -p /usr/share/sourcemeta/one \
  && cd /source && npm sbom --sbom-format spdx --sbom-type library --omit dev \
    > /usr/share/sourcemeta/one/npm-packages.spdx.json \
  && node /source/enterprise/scripts/sbom-vendorpull.js /build/VERSION \
    > /usr/share/sourcemeta/one/vendor-packages.spdx.json

FROM debian:trixie-slim

RUN apt-get --yes update && apt-get install --yes --no-install-recommends \
  openssl-provider-fips \
  && apt-get clean && rm -rf /var/lib/apt/lists/*
COPY --from=builder /etc/ssl/openssl.cnf /etc/ssl/openssl.cnf

# Commercial editions require a paid license
# See https://one.sourcemeta.com/commercial/

# See https://github.com/opencontainers/image-spec/blob/main/annotations.md#pre-defined-annotation-keys
LABEL org.opencontainers.image.url="https://one.sourcemeta.com"
LABEL org.opencontainers.image.documentation="https://one.sourcemeta.com"
LABEL org.opencontainers.image.source="https://github.com/sourcemeta/one"
LABEL org.opencontainers.image.vendor="Sourcemeta"
LABEL org.opencontainers.image.licenses="LicenseRef-Commercial"
LABEL org.opencontainers.image.title="Sourcemeta One Enterprise"
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

# Verify that the index binary uses system OpenSSL for cryptography
RUN ldd /usr/bin/sourcemeta-one-index | grep libcrypto
# Verify that the OpenSSL FIPS provider is configured and present
RUN grep -q 'fips = fips_sect' /etc/ssl/openssl.cnf
RUN test -f /usr/lib/*/ossl-modules/fips.so

# Create the non-root runtime account before the package purge removes
# `passwd` (and with it `useradd`). The user lives in `/etc/passwd`
# after this point and survives the purge. The exact UID sits above
# the typical 1000-range dev user (so host-mounted volumes can be
# mapped unambiguously) and outside the Debian system-user range
# (1-999) reserved for daemons.
ARG SOURCEMETA_ONE_UID=10001
RUN groupadd --system --gid "${SOURCEMETA_ONE_UID}" sourcemeta \
  && useradd --system --uid "${SOURCEMETA_ONE_UID}" \
       --gid "${SOURCEMETA_ONE_UID}" \
       --no-create-home --shell /usr/sbin/nologin sourcemeta

# Remove packages not needed at runtime to reduce the image attack surface.
RUN cp /bin/mount /bin/umount /tmp/ \
  && apt-get --yes update \
  && apt-get --yes purge --allow-remove-essential \
    adduser debconf \
    e2fsprogs gzip hostname init-system-helpers \
    login logsave mount ncurses-base ncurses-bin passwd perl-base \
    sed sysvinit-utils tzdata util-linux util-linux-extra \
  && apt-get --yes autoremove --allow-remove-essential \
  && rm -rf /var/lib/apt/lists/* \
  && mv /tmp/mount /tmp/umount /bin/
RUN dpkg-query --show --showformat='${Package}\t${Version}\t${Homepage}\n' \
  > /usr/share/sourcemeta/one/dpkg-packages

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

# Pre-create the output directory and hand both runtime directories to
# the non-root account so the indexer and server can write through
# them without elevation.
RUN mkdir -p "${SOURCEMETA_ONE_OUTPUT}" \
  && chown -R sourcemeta:sourcemeta \
       "${SOURCEMETA_ONE_WORKDIR}" "${SOURCEMETA_ONE_OUTPUT}"

USER sourcemeta:sourcemeta
# Build-time tripwire: fail the image if the USER directive did not
# actually drop privileges to the non-root account.
RUN test "$(id -u)" -eq "${SOURCEMETA_ONE_UID}" \
  && test "$(id -g)" -eq "${SOURCEMETA_ONE_UID}"

ENV SOURCEMETA_ONE_PORT=8000
HEALTHCHECK --interval=1s --timeout=2s --start-period=1s --retries=10 CMD grep -qE \
  "^\s*[0-9]+:\s+[0-9A-F]+:$(printf '%04X' $SOURCEMETA_ONE_PORT)\s+[0-9A-F:]+\s+0A\s" \
  /proc/net/tcp /proc/net/tcp6
ENTRYPOINT [ "/usr/bin/sourcemeta-server" ]
