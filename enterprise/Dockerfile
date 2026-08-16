FROM debian:trixie AS builder

# Debian ships npm 9, while the "npm sbom" command below needs 10, so Node
# comes from the distribution its own project publishes rather than from the
# APT repository a third party hosts for it. The archive is checked against
# the checksums that release publishes, which is the integrity that signing
# the repository was there to provide
ARG NODE_VERSION=22.23.1
RUN apt-get --yes update && apt-get install --yes --no-install-recommends \
  build-essential ca-certificates cmake ninja-build sassc esbuild shellcheck curl ccache \
  openssl libssl-dev openssl-provider-fips libcurl4-openssl-dev \
  && apt-get clean && rm -rf /var/lib/apt/lists/* \
  && NODE_ARCH="$(dpkg --print-architecture)" \
  && case "${NODE_ARCH}" in \
    amd64) NODE_ARCH=x64 ;; \
    arm64) NODE_ARCH=arm64 ;; \
    *) echo "Unsupported architecture: ${NODE_ARCH}" >&2; exit 1 ;; \
  esac \
  && NODE_ARCHIVE="node-v${NODE_VERSION}-linux-${NODE_ARCH}.tar.gz" \
  && curl --fail --silent --show-error --location \
    --output "/tmp/${NODE_ARCHIVE}" \
    "https://nodejs.org/dist/v${NODE_VERSION}/${NODE_ARCHIVE}" \
  && curl --fail --silent --show-error --location \
    --output /tmp/SHASUMS256.txt \
    "https://nodejs.org/dist/v${NODE_VERSION}/SHASUMS256.txt" \
  && (cd /tmp && grep " ${NODE_ARCHIVE}$" SHASUMS256.txt \
    | sha256sum --check --strict -) \
  && tar --extract --gzip --file "/tmp/${NODE_ARCHIVE}" --directory /usr/local \
    --strip-components=1 --no-same-owner \
    --exclude "*/CHANGELOG.md" --exclude "*/LICENSE" --exclude "*/README.md" \
  && rm "/tmp/${NODE_ARCHIVE}" /tmp/SHASUMS256.txt \
  && node --version && npm --version

# Enable the OpenSSL FIPS provider by running the module self-tests
# and appending the integrity-checked configuration to our openssl.cnf
RUN openssl fipsinstall -module /usr/lib/*/ossl-modules/fips.so \
  -out /tmp/fipsmodule.cnf
COPY enterprise/openssl.base.cnf /etc/ssl/openssl.cnf
RUN cat /tmp/fipsmodule.cnf >> /etc/ssl/openssl.cnf && cat /etc/ssl/openssl.cnf

COPY package.json /source/package.json
COPY package-lock.json /source/package-lock.json
RUN cd /source && npm ci
COPY cmake /source/cmake
COPY src /source/src
COPY contrib /source/contrib
COPY vendor /source/vendor
COPY DEPENDENCIES /source/DEPENDENCIES
COPY LICENSE-COMMERCIAL /source/LICENSE-COMMERCIAL
COPY CMakeLists.txt /source/CMakeLists.txt
COPY VERSION /source/VERSION
COPY enterprise /source/enterprise

# For testing
COPY test/cli /source/test/cli
COPY test/unit /source/test/unit
COPY test/js /source/test/js


ARG SOURCEMETA_ONE_BUILD_TYPE=Release
ARG SOURCEMETA_ONE_PARALLEL=2

ENV CCACHE_DIR=/ccache
ENV CCACHE_MAXSIZE=1G
RUN	cmake -S /source -B ./build -G Ninja \
  -DCMAKE_C_COMPILER_LAUNCHER:STRING=ccache \
  -DCMAKE_CXX_COMPILER_LAUNCHER:STRING=ccache \
  -DCMAKE_BUILD_TYPE:STRING=${SOURCEMETA_ONE_BUILD_TYPE} \
  -DCMAKE_COMPILE_WARNING_AS_ERROR:BOOL=ON \
  -DONE_INDEX:BOOL=ON \
  -DONE_SERVER:BOOL=ON \
  -DONE_TESTS:BOOL=ON \
  -DONE_ENTERPRISE:BOOL=ON \
  -DBUILD_SHARED_LIBS:BOOL=OFF \
  -DONE_DEBUG_SYMBOLS:BOOL=ON

RUN --mount=type=cache,target=/ccache,sharing=locked cmake --build /build \
  --config ${SOURCEMETA_ONE_BUILD_TYPE} \
  --parallel ${SOURCEMETA_ONE_PARALLEL}
RUN cmake --install /build --prefix /usr --verbose \
  --config ${SOURCEMETA_ONE_BUILD_TYPE} \
  --component sourcemeta_one
RUN cmake --install /build --prefix /usr --verbose \
  --config ${SOURCEMETA_ONE_BUILD_TYPE} \
  --component sourcemeta_jsonschema

# Linting
RUN cmake --build /build --config ${SOURCEMETA_ONE_BUILD_TYPE} \
  --target clang_format_test
RUN cmake --build /build --config ${SOURCEMETA_ONE_BUILD_TYPE} \
  --target shellcheck

# Run a few times to rule out any flakiness
ARG SOURCEMETA_ONE_TEST_REPEAT=1
RUN ctest --test-dir /build --build-config ${SOURCEMETA_ONE_BUILD_TYPE} \
  --output-on-failure --parallel ${SOURCEMETA_ONE_PARALLEL} \
  --repeat-until-fail ${SOURCEMETA_ONE_TEST_REPEAT}

RUN mkdir -p /usr/share/sourcemeta/one \
  && cd /source && npm sbom --sbom-format spdx --sbom-type library --omit dev \
    > /usr/share/sourcemeta/one/npm-packages.spdx.json \
  && node /source/enterprise/scripts/sbom-vendorpull.js /build/VERSION \
    > /usr/share/sourcemeta/one/vendor-packages.spdx.json

FROM debian:trixie-slim

RUN apt-get --yes update && apt-get install --yes --no-install-recommends \
  openssl-provider-fips libcurl4t64 ca-certificates \
  && apt-get clean && rm -rf /var/lib/apt/lists/*
COPY --from=builder /etc/ssl/openssl.cnf /etc/ssl/openssl.cnf

# Production use of the Enterprise edition requires a commercial license.
# See https://one.sourcemeta.com/commercial/

# See https://github.com/opencontainers/image-spec/blob/main/annotations.md#pre-defined-annotation-keys
LABEL org.opencontainers.image.url="https://one.sourcemeta.com"
LABEL org.opencontainers.image.documentation="https://one.sourcemeta.com"
LABEL org.opencontainers.image.source="https://github.com/sourcemeta/one"
LABEL org.opencontainers.image.vendor="Sourcemeta"
LABEL org.opencontainers.image.licenses="LicenseRef-Sourcemeta-Commercial"
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
COPY --from=builder /usr/bin/jsonschema \
  /usr/bin/jsonschema
COPY --from=builder /usr/bin/jsonschema.debug \
  /usr/bin/jsonschema.debug
COPY --from=builder /usr/share/bash-completion/completions/jsonschema \
  /usr/share/bash-completion/completions/jsonschema
COPY --from=builder /usr/share/sourcemeta/one \
  /usr/share/sourcemeta/one
COPY LICENSE /usr/share/doc/sourcemeta-one/LICENSE
COPY LICENSE-COMMERCIAL /usr/share/doc/sourcemeta-one/LICENSE-COMMERCIAL

# For debugging purposes
RUN ldd /usr/bin/sourcemeta-one-index
RUN ldd /usr/bin/sourcemeta-one-server

# Verify that the index binary uses system OpenSSL for cryptography
RUN ldd /usr/bin/sourcemeta-one-index | grep libcrypto
RUN ldd /usr/lib/*/libcurl.so.4 | grep libcrypto
# Verify that the OpenSSL FIPS provider is configured and present
RUN grep -q 'fips = fips_sect' /etc/ssl/openssl.cnf
RUN test -f /usr/lib/*/ossl-modules/fips.so

# Remove packages not needed at runtime to reduce the image attack surface.
# We keep debconf and perl-base because ca-certificates depends on debconf,
# which in turn requires perl-base. Purging either removes ca-certificates and
# wipes the CA bundle that cURL needs to verify TLS certificates. We also keep
# sed because update-ca-certificates needs it, so images that extend this one
# can install custom certificate authorities
RUN cp /bin/mount /bin/umount /tmp/ \
  && apt-get --yes update \
  && apt-get --yes purge --allow-remove-essential \
    adduser \
    e2fsprogs gzip hostname init-system-helpers \
    login logsave mount ncurses-base ncurses-bin passwd \
    sysvinit-utils tzdata util-linux util-linux-extra \
  && apt-get --yes autoremove --allow-remove-essential \
  && rm -rf /var/lib/apt/lists/* \
  && mv /tmp/mount /tmp/umount /bin/
# Verify the CA bundle is present, as cURL needs it to verify TLS certificates
# when making outbound HTTPS requests
RUN test -f /etc/ssl/certs/ca-certificates.crt
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

ENV SOURCEMETA_ONE_PORT=8000
HEALTHCHECK --interval=1s --timeout=2s --start-period=1s --retries=10 CMD grep -qE \
  "^\s*[0-9]+:\s+[0-9A-F]+:$(printf '%04X' $SOURCEMETA_ONE_PORT)\s+[0-9A-F:]+\s+0A\s" \
  /proc/net/tcp /proc/net/tcp6
ENTRYPOINT [ "/usr/bin/sourcemeta-server" ]
