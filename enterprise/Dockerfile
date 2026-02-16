FROM debian:trixie AS builder

RUN apt-get --yes update && apt-get install --yes --no-install-recommends \
  build-essential ca-certificates cmake sassc esbuild shellcheck nodejs npm \
  openssl libssl-dev openssl-provider-fips \
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
COPY collections /source/collections
COPY vendor /source/vendor
COPY CMakeLists.txt /source/CMakeLists.txt
COPY enterprise /source/enterprise

# For testing
COPY test/cli /source/test/cli
COPY test/unit /source/test/unit
COPY test/js /source/test/js

RUN cd /source && npm ci

ARG SOURCEMETA_ONE_BUILD_TYPE=Release
ARG SOURCEMETA_ONE_PARALLEL=2

RUN	cmake -S /source -B ./build \
  -DCMAKE_BUILD_TYPE:STRING=${SOURCEMETA_ONE_BUILD_TYPE} \
  -DCMAKE_COMPILE_WARNING_AS_ERROR:BOOL=ON \
  -DONE_INDEX:BOOL=ON \
  -DONE_SERVER:BOOL=ON \
  -DONE_TESTS:BOOL=ON \
  -DONE_ENTERPRISE:BOOL=ON \
  -DBUILD_SHARED_LIBS:BOOL=OFF

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
RUN cmake --build /build --config ${SOURCEMETA_ONE_BUILD_TYPE} \
  --target jsonschema_fmt_test
RUN cmake --build /build --config ${SOURCEMETA_ONE_BUILD_TYPE} \
  --target jsonschema_metaschema
RUN cmake --build /build --config ${SOURCEMETA_ONE_BUILD_TYPE} \
  --target jsonschema_lint

RUN ctest --test-dir /build --build-config ${SOURCEMETA_ONE_BUILD_TYPE} \
  --output-on-failure --parallel

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
COPY --from=builder /usr/bin/sourcemeta-one-server \
  /usr/bin/sourcemeta-one-server
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

ENV SOURCEMETA_ONE_PORT=8000
ENTRYPOINT [ "/usr/bin/sourcemeta-server" ]
